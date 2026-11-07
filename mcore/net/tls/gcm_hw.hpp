#pragma once
/*
 * gcm_hw — AES-GCM on the STM32H7 CRYP for Mbed TLS (MBEDTLS_GCM_ALT, build
 * option MCORE_HW_GCM). Call selftest() once at boot, before the first TLS
 * connection: it enables the CRYP clock, compares the hardware with the
 * software GCM and enables the hardware path only if every case matches.
 */
#include <cstdint>

namespace gcm_hw {

struct Stats {
	uint32_t hw_ok;            // 1: self-test passed, hardware in use
	uint32_t selftest_cases;
	uint32_t first_failure;    // keybits << 20 | length << 8 | aad length
	uint32_t hw_ops;           // records en/decrypted by the CRYP
	uint32_t hw_bytes;
	uint32_t sw_ops;           // calls that took the software path
	uint32_t hw_errors;        // CRYP timeouts (then software)
	uint32_t failure_stage;    // self-test check that failed first
	uint8_t hw_tag[16], sw_tag[16];
};
extern Stats stats;

bool selftest();

} // namespace gcm_hw
