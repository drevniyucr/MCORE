# STM32H755 (NUCLEO-H755ZI-Q), dual core:
#   STM32H755      Cortex-M7, bank 1: real time (cm7/)
#   STM32H755_CM4  Cortex-M4, bank 2: Ethernet, TCP/IP, TLS, MQTT, VDA 5050 (cm4/)
# Both include shared/ (inter-core protocol, board configuration).
#   cmake -S projects/stm32h755 -B build-h7 && cmake --build build-h7
cmake_minimum_required(VERSION 3.21)
get_filename_component(MCORE_ROOT ${CMAKE_CURRENT_LIST_DIR}/../.. ABSOLUTE)
include(${MCORE_ROOT}/cmake/arm-none-eabi.cmake)

project(STM32H755 LANGUAGES C CXX ASM)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(${MCORE_ROOT}/cmake/mcore.cmake)

mcore_sources(stm32h755)
set(STARTUP ${CMAKE_CURRENT_SOURCE_DIR}/startup_stm32h755xx.s)   # both cores
set(SHARED  ${CMAKE_CURRENT_SOURCE_DIR}/shared)

# --- Cortex-M7 (double-precision FPU): real time, no network stack -------------------
file(GLOB CM7_SRC ${CMAKE_CURRENT_SOURCE_DIR}/cm7/*.cpp)
mcore_firmware(STM32H755
    CPU      -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard
    LINKER   ${CMAKE_CURRENT_SOURCE_DIR}/STM32H755ZITX_FLASH_CM7.ld
    INCLUDES ${MCORE_CHIP_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cm7 ${SHARED}
    SOURCES  ${MCORE_CORE_SRC} ${MCORE_CHIP_SRC} ${CM7_SRC} ${STARTUP})

# --- Cortex-M4 (single-precision FPU): network ------------------------------------
set(CM4_CPU -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard)
option(MCORE_TLS_SELFTEST "M4: run an in-memory TLS handshake at boot (timing/RAM report)" OFF)

# MCORE_HW_GCM: AES-GCM on the CRYP (mcore/net/tls/gcm_hw.cpp); gcm_soft.c is
# Mbed TLS's software GCM under other names (fallback, self-test reference)
set(TLS_DEFINES MCORE_HW_GCM)
if(MCORE_TLS_SELFTEST)
    list(APPEND TLS_DEFINES MCORE_TLS_TEST_SERVER MCORE_TLS_SELFTEST)
endif()
mcore_mbedtls(mbedtls_cm4 CPU ${CM4_CPU} DEFINES ${TLS_DEFINES}
    SOURCES ${MCORE_ROOT}/mcore/net/tls/gcm_soft.c)

# Broker CA (PEM) embedded as a string; replace the file for production.
set(BROKER_CA_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cm4/certs/broker_ca.pem)
file(READ ${BROKER_CA_FILE} PEM_TEXT)
if(PEM_TEXT MATCHES "MCORE TEST CA")
    message(WARNING "cm4/certs/broker_ca.pem is the MCORE TEST CA - replace it for production")
endif()
mcore_embed_pem(${BROKER_CA_FILE} BROKER_CA_PEM ${CMAKE_BINARY_DIR}/generated/broker_ca.h)
if(MCORE_TLS_SELFTEST)
    foreach(item ca.crt:SELFTEST_CA_PEM server.crt:SELFTEST_SERVER_CRT server.key:SELFTEST_SERVER_KEY)
        string(REPLACE ":" ";" pair ${item})
        list(GET pair 0 file)
        list(GET pair 1 name)
        mcore_embed_pem(${MCORE_ROOT}/tests/certs/${file} ${name} ${CMAKE_BINARY_DIR}/generated/selftest_${name}.h)
    endforeach()
    string(TIMESTAMP MCORE_BUILD_UNIX_TIME "%s" UTC)
endif()

file(GLOB CM4_SRC ${CMAKE_CURRENT_SOURCE_DIR}/cm4/*.cpp)
mcore_firmware(STM32H755_CM4
    CPU      ${CM4_CPU}
    LINKER   ${CMAKE_CURRENT_SOURCE_DIR}/STM32H755ZITX_FLASH_CM4.ld
    INCLUDES ${MCORE_CHIP_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cm4 ${SHARED} ${CMAKE_BINARY_DIR}/generated
    # TCP slots: MQTT + diagnostics port 5001 (each slot is ~8.8 KB of RAM)
    DEFINES  TCP_MAX_CONNECTIONS=2
    SOURCES  ${MCORE_CORE_SRC} ${MCORE_CHIP_SRC} ${MCORE_CHIP_ETH_SRC} ${MCORE_NET_SRC}
             ${MCORE_TLS_SRC} ${MCORE_VDA_SRC} ${CM4_SRC} ${STARTUP})
target_link_libraries(STM32H755_CM4 PRIVATE mbedtls_cm4)
if(MCORE_TLS_SELFTEST)
    target_compile_definitions(STM32H755_CM4 PRIVATE MCORE_BUILD_UNIX_TIME=${MCORE_BUILD_UNIX_TIME}ULL)
endif()
