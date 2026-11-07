/*
 * gcm_hw_config.h — shared between gcm_alt.h and gcm_soft.c (which must not
 * see the ALT context type).
 */
#ifndef MCORE_GCM_HW_CONFIG_H
#define MCORE_GCM_HW_CONFIG_H

/* Room for Mbed TLS's software mbedtls_gcm_context (checked in gcm_soft.c) */
#define MCORE_SW_GCM_CTX_BYTES 512

#endif /* MCORE_GCM_HW_CONFIG_H */
