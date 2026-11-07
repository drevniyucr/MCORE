/*
 * Mbed TLS 3.6 configuration for MCORE (selected with
 * -DMBEDTLS_CONFIG_FILE="mbedtls_config_mcore.h").
 *
 * Scope: a TLS 1.2 CLIENT for an MQTT broker (VDA 5050), on a Cortex-M4
 * without an OS:
 *   - key exchange ECDHE-ECDSA and ECDHE-RSA, AES-128/256-GCM, SHA-256/384
 *   - server certificate chains with RSA or ECDSA keys (P-256, P-384)
 *   - optional client certificate (mutual TLS) with an RSA or ECDSA key
 *   - SNI, max_fragment_length, extended master secret
 * Platform hooks (mcore/net/tls/tls_platform.cpp):
 *   - memory: static arena (MEMORY_BUFFER_ALLOC), no malloc
 *   - entropy: mbedtls_hardware_poll() = STM32H7 RNG (host tests: a stub)
 *   - time: wall clock from SNTP, for certificate validity checks
 *
 * TLS 1.3 is left out on purpose: it needs the PSA crypto core (more flash
 * and RAM) and mosquitto/EMQX/HiveMQ all still accept TLS 1.2.
 */
#ifndef MBEDTLS_CONFIG_MCORE_H
#define MBEDTLS_CONFIG_MCORE_H

/* ---- Platform -------------------------------------------------------- */
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_MEMORY_BUFFER_ALLOC_C
#define MBEDTLS_HAVE_TIME
#define MBEDTLS_HAVE_TIME_DATE
#define MBEDTLS_PLATFORM_TIME_ALT          /* mbedtls_platform_set_time() */
#define MBEDTLS_PLATFORM_MS_TIME_ALT       /* mbedtls_ms_time() in tls_platform.cpp */
#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_ENTROPY_HARDWARE_ALT       /* mbedtls_hardware_poll() */
#define MBEDTLS_DEPRECATED_REMOVED

/* ---- Random numbers -------------------------------------------------- */
#define MBEDTLS_ENTROPY_C
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_HMAC_DRBG_C                /* deterministic ECDSA */

/* ---- Symmetric crypto and hashes ------------------------------------- */
#define MBEDTLS_AES_C
/* AES tables in RAM (8 KB, built at start): the Cortex-M4 reads data from
 * flash bank 2 slowly, and AES-GCM looks them up for every byte */
#define MBEDTLS_GCM_C
/* STM32H7 CRYP for AES-GCM records (mcore/net/tls/gcm_hw.cpp); gcm_soft.c
 * compiles the software GCM next to it as fallback and test reference */
#if defined(MCORE_HW_GCM) && !defined(MCORE_GCM_SOFT_TU)
#define MBEDTLS_GCM_ALT
#endif
#define MBEDTLS_CIPHER_C
#define MBEDTLS_MD_C
#define MBEDTLS_SHA224_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA384_C
#define MBEDTLS_SHA512_C
#define MBEDTLS_SHA1_C                     /* only for legacy CA signatures */

/* ---- Public key crypto ----------------------------------------------- */
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_ECP_C
#define MBEDTLS_ECP_NIST_OPTIM
/* ECC in bounded slices (mbedtls_ecp_set_max_ops): the ECDHE-ECDSA handshake
 * returns MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS instead of computing for ~1.8 s
 * in one call. Only short Weierstrass curves restart (not X25519). */
#define MBEDTLS_ECP_RESTARTABLE
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
#define MBEDTLS_ECP_DP_CURVE25519_ENABLED
#define MBEDTLS_ECDH_C
#define MBEDTLS_ECDSA_C
#define MBEDTLS_ECDSA_DETERMINISTIC
#define MBEDTLS_RSA_C
#define MBEDTLS_PKCS1_V15
#define MBEDTLS_PKCS1_V21
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_OID_C

/* ---- Certificates ---------------------------------------------------- */
#define MBEDTLS_X509_USE_C
#define MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_PEM_PARSE_C
#define MBEDTLS_BASE64_C

/* ---- TLS ------------------------------------------------------------- */
#define MBEDTLS_SSL_TLS_C
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_PROTO_TLS1_2
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#define MBEDTLS_SSL_SERVER_NAME_INDICATION
#define MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
#define MBEDTLS_SSL_EXTENDED_MASTER_SECRET
/* Session resumption with tickets (RFC 5077): a reconnect skips the
 * certificate check and the ECDHE (milliseconds instead of 0.65 s) */
#define MBEDTLS_SSL_SESSION_TICKETS
/* MBEDTLS_SSL_KEEP_PEER_CERTIFICATE stays off: saves the peer chain's RAM */

/* A full 16 KB record must fit on receive (servers rarely honour
 * max_fragment_length); what we send is small. */
#define MBEDTLS_SSL_IN_CONTENT_LEN   16384
#define MBEDTLS_SSL_OUT_CONTENT_LEN  4096

/* Strings for mbedtls_strerror() in diagnostics */
#define MBEDTLS_ERROR_C

/* Arena usage statistics (tls::heap_used/heap_peak) for tests/self-test */
#if defined(MCORE_TLS_MEMORY_DEBUG)
#define MBEDTLS_MEMORY_DEBUG
#endif

/* Host tests also need a TLS server for the in-memory handshake test and
 * the on-target self-test (MCORE_TLS_SELFTEST). */
#if defined(MCORE_TLS_TEST_SERVER)
#define MBEDTLS_SSL_SRV_C
#define MBEDTLS_SSL_TICKET_C               /* test server issues tickets */
/* the self-test image (client + server arenas) has no room for RAM tables */
#define MBEDTLS_AES_ROM_TABLES
#endif

#endif /* MBEDTLS_CONFIG_MCORE_H */
