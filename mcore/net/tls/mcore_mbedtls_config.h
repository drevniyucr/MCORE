/* mbedTLS 3.6 configuration of MCORE: a TLS 1.2 client, nothing else.
 * Started from the goz-shuttle config (same chip, same broker setup):
 *
 *   - one cipher suite, ECDHE-ECDSA-AES128-GCM-SHA256, with P-256 keys:
 *     small and fast on the Cortex-M7 without crypto hardware;
 *   - the broker certificate is always verified against the given CA and
 *     the configured name or IP address (netclient.cpp sets VERIFY_REQUIRED);
 *   - entropy from the RNG peripheral (mbedtls_hardware_poll in tls.cpp);
 *   - memory from a static arena (memory_buffer_alloc): the TLS link is its
 *     only user; the rest of MCORE stays heap-free.
 *
 * Differences from the shuttle:
 *   - ECP_RESTARTABLE: the superloop has no tasks, so the elliptic-curve math
 *     of the handshake runs in slices (mbedtls_ecp_set_max_ops) instead of
 *     blocking the loop (and the 1 s watchdog) for hundreds of milliseconds;
 *   - HAVE_TIME_DATE: certificate validity dates ARE checked, against the RTC;
 *   - MAX_FRAGMENT_LENGTH: the client asks for 4 KB records. Mosquitto
 *     (OpenSSL) ignores the request, so the input buffer stays 16 KB.
 * Credentials are DER, so neither PEM nor base64 is built. */
#ifndef MCORE_MBEDTLS_CONFIG_H
#define MCORE_MBEDTLS_CONFIG_H

/* platform */
#define MBEDTLS_HAVE_ASM
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_MEMORY_BUFFER_ALLOC_C
#define MBEDTLS_MEMORY_DEBUG                 /* peak arena use: tls_heap_peak() */
#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_ENTROPY_HARDWARE_ALT
#define MBEDTLS_DEPRECATED_REMOVED

/* time: libc time() = the RTC (app/syscalls.c _gettimeofday), milliseconds
 * from SysTick (mbedtls_ms_time in tls.cpp) */
#define MBEDTLS_HAVE_TIME
#define MBEDTLS_HAVE_TIME_DATE
#define MBEDTLS_PLATFORM_MS_TIME_ALT

/* random numbers */
#define MBEDTLS_ENTROPY_C
#define MBEDTLS_ENTROPY_FORCE_SHA256
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_HMAC_DRBG_C
#define MBEDTLS_ECDSA_DETERMINISTIC

/* symmetric crypto and hashes */
#define MBEDTLS_AES_C
#define MBEDTLS_AES_ROM_TABLES               /* const tables in flash: -8 KB of RAM, no init pass */
#define MBEDTLS_GCM_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_MD_C
#define MBEDTLS_SHA224_C
#define MBEDTLS_SHA256_C

/* public key */
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_ECP_C
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_NIST_OPTIM
#define MBEDTLS_ECP_RESTARTABLE
#define MBEDTLS_ECDH_C
#define MBEDTLS_ECDSA_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_OID_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_MPI_MAX_SIZE 64

/* certificates */
#define MBEDTLS_X509_USE_C
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_X509_CHECK_KEY_USAGE
#define MBEDTLS_X509_CHECK_EXTENDED_KEY_USAGE
#define MBEDTLS_X509_MAX_INTERMEDIATE_CA 2

/* TLS */
#define MBEDTLS_SSL_TLS_C
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_PROTO_TLS1_2
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#define MBEDTLS_SSL_SERVER_NAME_INDICATION
#define MBEDTLS_SSL_EXTENDED_MASTER_SECRET
#define MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
#define MBEDTLS_SSL_ALL_ALERT_MESSAGES
#define MBEDTLS_SSL_CIPHERSUITES MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
#define MBEDTLS_SSL_IN_CONTENT_LEN  16384    /* full records: MFL is not honoured by OpenSSL servers */
#define MBEDTLS_SSL_OUT_CONTENT_LEN 2048

#define MBEDTLS_ERROR_C                      /* readable reasons in NetClient::error() */

#endif /* MCORE_MBEDTLS_CONFIG_H */
