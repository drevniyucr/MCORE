/*
 * gcm_soft.c — Mbed TLS's software GCM under the names mcore_sw_gcm_*, next
 * to the CRYP-based MBEDTLS_GCM_ALT (gcm_hw.cpp) as its fallback and as the
 * reference for the boot self-test. Built into the Mbed TLS library of the
 * Cortex-M4 image only.
 */
#define MCORE_GCM_SOFT_TU   /* mbedtls_config_mcore.h: no MBEDTLS_GCM_ALT here */

#define mbedtls_gcm_context       mcore_sw_gcm_context
#define mbedtls_gcm_init          mcore_sw_gcm_init
#define mbedtls_gcm_setkey        mcore_sw_gcm_setkey
#define mbedtls_gcm_starts        mcore_sw_gcm_starts
#define mbedtls_gcm_update_ad     mcore_sw_gcm_update_ad
#define mbedtls_gcm_update        mcore_sw_gcm_update
#define mbedtls_gcm_finish        mcore_sw_gcm_finish
#define mbedtls_gcm_crypt_and_tag mcore_sw_gcm_crypt_and_tag
#define mbedtls_gcm_auth_decrypt  mcore_sw_gcm_auth_decrypt
#define mbedtls_gcm_free          mcore_sw_gcm_free
#define mbedtls_gcm_self_test     mcore_sw_gcm_self_test

#include "../../../third_party/mbedtls/library/gcm.c"

#include "gcm_hw_config.h"

_Static_assert(sizeof(mcore_sw_gcm_context) <= MCORE_SW_GCM_CTX_BYTES,
               "raise MCORE_SW_GCM_CTX_BYTES");
