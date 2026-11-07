/*
 * gcm_hw.cpp — MBEDTLS_GCM_ALT on the STM32H7 CRYP accelerator (see gcm_alt.h).
 *
 * AES-GCM per RM0399 (CRYP, GCM mode), 8-bit data swapping so that byte
 * buffers go to DIN/DOUT as little-endian words:
 *   init     key + IV||counter 2 -> CRYPEN; the hardware computes H and clears CRYPEN
 *   header   additional data, last block zero padded
 *   payload  plaintext/ciphertext block by block; for an encryption with a
 *            partial last block NPBLB tells the hash how many bytes are padding
 *   final    ALGODIR = 0, bit lengths of AAD and payload in, tag out
 *
 * The CRYP is used only by the TLS task (no locking). gcm_hw_selftest()
 * compares it with the software GCM at boot; until it has passed, and for
 * anything the hardware path does not cover, the software GCM is used.
 */
#include "mbedtls/build_info.h"

#if defined(MBEDTLS_GCM_ALT)

#include <cstring>

#include "core/def.hpp"
#include "mbedtls/error.h"
#include "mbedtls/gcm.h"
#include "mbedtls/platform_util.h"
#include "net/tls/gcm_hw.hpp"

extern "C" {
// gcm_soft.c: Mbed TLS software GCM under other names (context = void *)
void mcore_sw_gcm_init(void *ctx);
int mcore_sw_gcm_setkey(void *ctx, mbedtls_cipher_id_t cipher, const unsigned char *key, unsigned int keybits);
int mcore_sw_gcm_starts(void *ctx, int mode, const unsigned char *iv, size_t iv_len);
int mcore_sw_gcm_update_ad(void *ctx, const unsigned char *add, size_t add_len);
int mcore_sw_gcm_update(void *ctx, const unsigned char *input, size_t input_length, unsigned char *output,
                        size_t output_size, size_t *output_length);
int mcore_sw_gcm_finish(void *ctx, unsigned char *output, size_t output_size, size_t *output_length,
                        unsigned char *tag, size_t tag_len);
int mcore_sw_gcm_crypt_and_tag(void *ctx, int mode, size_t length, const unsigned char *iv, size_t iv_len,
                               const unsigned char *add, size_t add_len, const unsigned char *input,
                               unsigned char *output, size_t tag_len, unsigned char *tag);
int mcore_sw_gcm_auth_decrypt(void *ctx, size_t length, const unsigned char *iv, size_t iv_len,
                              const unsigned char *add, size_t add_len, const unsigned char *tag, size_t tag_len,
                              const unsigned char *input, unsigned char *output);
void mcore_sw_gcm_free(void *ctx);
}

namespace gcm_hw {
Stats stats = {};
}

namespace {

using gcm_hw::stats;

constexpr uint32_t TIMEOUT = 1000000;   // polling iterations (~ms at 200 MHz)

// CR field values
constexpr uint32_t CR_ALGODIR = 1U << 2;
constexpr uint32_t CR_GCM = 1U << 19;              // ALGOMODE = 1000 (ALGOMODE[2:0] = 0)
constexpr uint32_t CR_DATATYPE_8 = 2U << 6;
constexpr uint32_t CR_FFLUSH = 1U << 14;
constexpr uint32_t CR_CRYPEN = 1U << 15;
constexpr uint32_t CR_PHASE_POS = 16;
constexpr uint32_t CR_NPBLB_POS = 20;
constexpr uint32_t PH_INIT = 0, PH_HEADER = 1, PH_PAYLOAD = 2, PH_FINAL = 3;

uint32_t be32(const unsigned char *p) {
	return (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) | (uint32_t(p[2]) << 8) | uint32_t(p[3]);
}

uint32_t load32(const unsigned char *p) {
	uint32_t w;
	memcpy(&w, p, 4);
	return w;
}

bool wait_sr(uint32_t mask, bool set) {
	for (uint32_t i = 0; i < TIMEOUT; ++i) {
		if (((CRYP::_SR::read() & mask) != 0U) == set) {
			return true;
		}
	}
	return false;
}

bool wait_idle() {
	return wait_sr(CRYP::_SR::IFEM::BitMsk, true) && wait_sr(CRYP::_SR::BUSY::BitMsk, false);
}

void cr(uint32_t v) {
	CRYP::_CR::overwrite(v);
}

// One block in, optionally one block out
bool block(const unsigned char in[16], unsigned char *out) {
	if (!wait_sr(CRYP::_SR::IFNF::BitMsk, true)) {
		return false;
	}
	for (int i = 0; i < 4; ++i) {
		CRYP::_DIN::overwrite(load32(in + 4 * i));
	}
	if (out == nullptr) {
		return true;
	}
	unsigned char tmp[16];
	for (int i = 0; i < 4; ++i) {
		if (!wait_sr(CRYP::_SR::OFNE::BitMsk, true)) {
			return false;
		}
		const uint32_t w = CRYP::_DOUT::read();
		memcpy(tmp + 4 * i, &w, 4);
	}
	memcpy(out, tmp, 16);
	return true;
}

// Full GCM on the CRYP. out may be null for length 0. tag gets 16 bytes.
int run(const mbedtls_gcm_context *ctx, bool decrypt, size_t len, const unsigned char iv[12],
        const unsigned char *add, size_t add_len, const unsigned char *in, unsigned char *out,
        unsigned char tag[16]) {
	const uint32_t keysize = ctx->keybits == 128 ? 0U : (ctx->keybits == 192 ? 1U : 2U);
	const uint32_t base = CR_GCM | CR_DATATYPE_8 | (keysize << 8) | (decrypt ? CR_ALGODIR : 0U);
	int ret = MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;

	cr(base);
	cr(base | CR_FFLUSH);
	// Keys end at K3RR: 128 bit -> K2LR..K3RR, 192 -> K1LR.., 256 -> K0LR..
	uint32_t k[8] = {};
	const unsigned nk = ctx->keybits / 32U;
	for (unsigned i = 0; i < nk; ++i) {
		k[8 - nk + i] = be32(ctx->key + 4 * i);
	}
	CRYP::_K0LR::overwrite(k[0]);
	CRYP::_K0RR::overwrite(k[1]);
	CRYP::_K1LR::overwrite(k[2]);
	CRYP::_K1RR::overwrite(k[3]);
	CRYP::_K2LR::overwrite(k[4]);
	CRYP::_K2RR::overwrite(k[5]);
	CRYP::_K3LR::overwrite(k[6]);
	CRYP::_K3RR::overwrite(k[7]);
	mbedtls_platform_zeroize(k, sizeof(k));
	CRYP::_IV0LR::overwrite(be32(iv));
	CRYP::_IV0RR::overwrite(be32(iv + 4));
	CRYP::_IV1LR::overwrite(be32(iv + 8));
	CRYP::_IV1RR::overwrite(2U);   // counter 1 is for the tag (J0)

	unsigned char pad[16];
	do {
		// init: the hardware computes the hash subkey and clears CRYPEN
		cr(base | (PH_INIT << CR_PHASE_POS) | CR_CRYPEN);
		uint32_t i = 0;
		while ((CRYP::_CR::read() & CR_CRYPEN) != 0U && ++i < TIMEOUT) {
		}
		if (i >= TIMEOUT) {
			break;
		}
		// header
		if (add_len > 0U) {
			cr(base | (PH_HEADER << CR_PHASE_POS) | CR_CRYPEN);
			size_t n = 0;
			bool ok = true;
			for (; ok && n + 16U <= add_len; n += 16U) {
				ok = block(add + n, nullptr);
			}
			if (ok && n < add_len) {
				memset(pad, 0, sizeof(pad));
				memcpy(pad, add + n, add_len - n);
				ok = block(pad, nullptr);
			}
			if (!ok || !wait_idle()) {
				break;
			}
			cr(base | (PH_HEADER << CR_PHASE_POS));
		}
		// payload
		if (len > 0U) {
			cr(base | (PH_PAYLOAD << CR_PHASE_POS) | CR_CRYPEN);
			size_t n = 0;
			bool ok = true;
			for (; ok && n + 16U <= len; n += 16U) {
				ok = block(in + n, out + n);
			}
			if (ok && n < len) {
				const size_t rest = len - n;
				if (!decrypt) {
					// tell the hash which bytes of the last block are padding
					if (!wait_idle()) {
						break;
					}
					cr(base | (PH_PAYLOAD << CR_PHASE_POS));
					cr(base | (PH_PAYLOAD << CR_PHASE_POS) | ((16U - rest) << CR_NPBLB_POS) | CR_CRYPEN);
				}
				memset(pad, 0, sizeof(pad));
				memcpy(pad, in + n, rest);
				ok = block(pad, pad);
				if (ok) {
					memcpy(out + n, pad, rest);
				}
			}
			if (!ok || !wait_idle()) {
				break;
			}
			cr(base | (PH_PAYLOAD << CR_PHASE_POS));
		}
		// final: ALGODIR must be 0
		const uint32_t fin = (base & ~CR_ALGODIR) | (PH_FINAL << CR_PHASE_POS);
		cr(fin | CR_CRYPEN);
		// Bit lengths as plain 32-bit values: the data swapping does not apply
		// to them (found by the self-test: swapped lengths gave wrong tags)
		CRYP::_DIN::overwrite(0U);
		CRYP::_DIN::overwrite(static_cast<uint32_t>(add_len * 8U));
		CRYP::_DIN::overwrite(0U);
		CRYP::_DIN::overwrite(static_cast<uint32_t>(len * 8U));
		bool ok = true;
		for (int w = 0; ok && w < 4; ++w) {
			ok = wait_sr(CRYP::_SR::OFNE::BitMsk, true);
			if (ok) {
				const uint32_t v = CRYP::_DOUT::read();
				memcpy(tag + 4 * w, &v, 4);
			}
		}
		if (ok) {
			ret = 0;
		}
	} while (false);

	cr(0);
	mbedtls_platform_zeroize(pad, sizeof(pad));
	if (ret != 0) {
		stats.hw_errors += 1;
	}
	return ret;
}

bool hw_usable(const mbedtls_gcm_context *ctx, size_t iv_len, size_t tag_len) {
	return stats.hw_ok != 0U && ctx->hw_key != 0 && iv_len == 12U && tag_len >= 4U && tag_len <= 16U;
}

} // namespace

// =============================================================================
// Mbed TLS GCM API
// =============================================================================
extern "C" {

void mbedtls_gcm_init(mbedtls_gcm_context *ctx) {
	memset(ctx, 0, sizeof(*ctx));
	mcore_sw_gcm_init(ctx->sw.bytes);
}

int mbedtls_gcm_setkey(mbedtls_gcm_context *ctx, mbedtls_cipher_id_t cipher, const unsigned char *key,
                       unsigned int keybits) {
	const int ret = mcore_sw_gcm_setkey(ctx->sw.bytes, cipher, key, keybits);
	if (ret != 0) {
		return ret;
	}
	ctx->hw_key = (cipher == MBEDTLS_CIPHER_ID_AES && (keybits == 128 || keybits == 192 || keybits == 256)) ? 1 : 0;
	if (ctx->hw_key != 0) {
		memcpy(ctx->key, key, keybits / 8U);
		ctx->keybits = keybits;
	}
	return 0;
}

int mbedtls_gcm_starts(mbedtls_gcm_context *ctx, int mode, const unsigned char *iv, size_t iv_len) {
	return mcore_sw_gcm_starts(ctx->sw.bytes, mode, iv, iv_len);
}

int mbedtls_gcm_update_ad(mbedtls_gcm_context *ctx, const unsigned char *add, size_t add_len) {
	return mcore_sw_gcm_update_ad(ctx->sw.bytes, add, add_len);
}

int mbedtls_gcm_update(mbedtls_gcm_context *ctx, const unsigned char *input, size_t input_length,
                       unsigned char *output, size_t output_size, size_t *output_length) {
	return mcore_sw_gcm_update(ctx->sw.bytes, input, input_length, output, output_size, output_length);
}

int mbedtls_gcm_finish(mbedtls_gcm_context *ctx, unsigned char *output, size_t output_size,
                       size_t *output_length, unsigned char *tag, size_t tag_len) {
	return mcore_sw_gcm_finish(ctx->sw.bytes, output, output_size, output_length, tag, tag_len);
}

int mbedtls_gcm_crypt_and_tag(mbedtls_gcm_context *ctx, int mode, size_t length, const unsigned char *iv,
                              size_t iv_len, const unsigned char *add, size_t add_len,
                              const unsigned char *input, unsigned char *output, size_t tag_len,
                              unsigned char *tag) {
	if (hw_usable(ctx, iv_len, tag_len)) {
		unsigned char full[16];
		const int ret = run(ctx, mode == MBEDTLS_GCM_DECRYPT, length, iv, add, add_len, input, output, full);
		if (ret == 0) {
			memcpy(tag, full, tag_len);
			stats.hw_ops += 1;
			stats.hw_bytes += length;
			return 0;
		}
	}
	stats.sw_ops += 1;
	return mcore_sw_gcm_crypt_and_tag(ctx->sw.bytes, mode, length, iv, iv_len, add, add_len, input, output,
	                                  tag_len, tag);
}

int mbedtls_gcm_auth_decrypt(mbedtls_gcm_context *ctx, size_t length, const unsigned char *iv, size_t iv_len,
                             const unsigned char *add, size_t add_len, const unsigned char *tag, size_t tag_len,
                             const unsigned char *input, unsigned char *output) {
	if (hw_usable(ctx, iv_len, tag_len)) {
		unsigned char full[16];
		if (run(ctx, true, length, iv, add, add_len, input, output, full) == 0) {
			stats.hw_ops += 1;
			stats.hw_bytes += length;
			unsigned char diff = 0;
			for (size_t i = 0; i < tag_len; ++i) {
				diff |= static_cast<unsigned char>(tag[i] ^ full[i]);
			}
			if (diff != 0U) {
				mbedtls_platform_zeroize(output, length);
				return MBEDTLS_ERR_GCM_AUTH_FAILED;
			}
			return 0;
		}
	}
	stats.sw_ops += 1;
	return mcore_sw_gcm_auth_decrypt(ctx->sw.bytes, length, iv, iv_len, add, add_len, tag, tag_len, input,
	                                 output);
}

void mbedtls_gcm_free(mbedtls_gcm_context *ctx) {
	if (ctx == nullptr) {
		return;
	}
	mcore_sw_gcm_free(ctx->sw.bytes);
	mbedtls_platform_zeroize(ctx, sizeof(*ctx));
}

} // extern "C"

// =============================================================================
// Boot self-test: hardware against software on pseudo-random vectors
// =============================================================================
namespace gcm_hw {

namespace {
uint32_t lcg = 0x12345678U;
unsigned char rnd() {
	lcg = lcg * 1664525U + 1013904223U;
	return static_cast<unsigned char>(lcg >> 24);
}
void fill(unsigned char *p, size_t n) {
	for (size_t i = 0; i < n; ++i) p[i] = rnd();
}
} // namespace

bool selftest() {
	RCC::_AHB2ENR::CRYPTEN::set();
	(void) RCC::_AHB2ENR::read();
	stats.hw_ok = 1;   // let the API take the hardware path for the test

	static unsigned char pt[1200], hw_ct[1200], sw_ct[1200], back[1200];
	const size_t lengths[] = { 0, 1, 15, 16, 17, 31, 48, 60, 64, 255, 1024, 1199 };
	const size_t aads[] = { 0, 13, 20, 33 };
	const unsigned keys[] = { 128, 256, 192 };
	bool pass = true;
	uint32_t cases = 0;

	for (unsigned kb : keys) {
		for (size_t L : lengths) {
			for (size_t A : aads) {
				unsigned char key[32], iv[12], aad[40], hw_tag[16], sw_tag[16];
				fill(key, sizeof(key));
				fill(iv, sizeof(iv));
				fill(aad, sizeof(aad));
				fill(pt, L);
				mbedtls_gcm_context c;
				mbedtls_gcm_init(&c);
				uint32_t stage = 0;   // 0 = ok, else the check that failed
				if (mbedtls_gcm_setkey(&c, MBEDTLS_CIPHER_ID_AES, key, kb) != 0) stage = 1;
				else if (run(&c, false, L, iv, aad, A, pt, hw_ct, hw_tag) != 0) stage = 2;
				else if (mcore_sw_gcm_crypt_and_tag(c.sw.bytes, MBEDTLS_GCM_ENCRYPT, L, iv, 12, aad, A, pt,
				                                    sw_ct, 16, sw_tag) != 0) stage = 3;
				else if (memcmp(hw_ct, sw_ct, L) != 0) stage = 4;
				else if (memcmp(hw_tag, sw_tag, 16) != 0) stage = 5;
				// decrypt through the API (hardware) and a forged tag
				else if (mbedtls_gcm_auth_decrypt(&c, L, iv, 12, aad, A, sw_tag, 16, sw_ct, back) != 0 ||
				         memcmp(back, pt, L) != 0) stage = 6;
				else {
					sw_tag[0] ^= 1U;
					if (mbedtls_gcm_auth_decrypt(&c, L, iv, 12, aad, A, sw_tag, 16, sw_ct, back) !=
					    MBEDTLS_ERR_GCM_AUTH_FAILED) stage = 7;
				}
				const bool ok = stage == 0;
				if (!ok && stats.first_failure == 0U) {
					stats.failure_stage = stage;
					memcpy(stats.hw_tag, hw_tag, 16);
					memcpy(stats.sw_tag, sw_tag, 16);
				}
				mbedtls_gcm_free(&c);
				cases += 1;
				if (!ok) {
					pass = false;
					if (stats.first_failure == 0U) {
						stats.first_failure = (kb << 20) | (static_cast<uint32_t>(L) << 8) | static_cast<uint32_t>(A);
					}
				}
			}
		}
	}
	stats.selftest_cases = cases;
	stats.hw_ok = pass ? 1U : 0U;
	stats.hw_ops = stats.hw_bytes = stats.sw_ops = 0;
	return pass;
}

} // namespace gcm_hw

#endif /* MBEDTLS_GCM_ALT */
