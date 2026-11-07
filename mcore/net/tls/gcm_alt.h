/*
 * gcm_alt.h — MBEDTLS_GCM_ALT context for the STM32H7 CRYP accelerator.
 *
 * One-shot GCM (mbedtls_gcm_crypt_and_tag / mbedtls_gcm_auth_decrypt, the
 * calls TLS 1.2 records use) runs on the CRYP peripheral; everything else
 * (streaming API, IV lengths other than 12 bytes, a failed self-test) runs on
 * Mbed TLS's software GCM, compiled a second time under other names
 * (gcm_soft.c). See gcm_hw.cpp.
 */
#ifndef MCORE_GCM_ALT_H
#define MCORE_GCM_ALT_H

#include <stdint.h>

#include "gcm_hw_config.h"

typedef struct mbedtls_gcm_context {
	unsigned char key[32];
	unsigned int keybits;
	int hw_key;   /* AES key the CRYP can use */
	union {
		unsigned char bytes[MCORE_SW_GCM_CTX_BYTES];
		uint64_t align;
	} sw;         /* software GCM context (mcore_sw_gcm_*) */
} mbedtls_gcm_context;

#endif /* MCORE_GCM_ALT_H */
