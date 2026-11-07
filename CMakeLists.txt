cmake_minimum_required(VERSION 3.21)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)


if(NOT STM32_TOOLCHAIN_PATH)
    if(DEFINED ENV{STM32_TOOLCHAIN_PATH})
        message(STATUS "Detected toolchain path STM32_TOOLCHAIN_PATH in environmental variables: ")
        message(STATUS "$ENV{STM32_TOOLCHAIN_PATH}")
        set(STM32_TOOLCHAIN_PATH $ENV{STM32_TOOLCHAIN_PATH})
    else()
        if(NOT CMAKE_C_COMPILER)
            set(STM32_TOOLCHAIN_PATH "/usr")
            message(STATUS "No STM32_TOOLCHAIN_PATH specified, using default: " ${STM32_TOOLCHAIN_PATH})
        else()
            # keep only directory of compiler
            get_filename_component(STM32_TOOLCHAIN_PATH ${CMAKE_C_COMPILER} DIRECTORY)
            # remove the last /bin directory
            get_filename_component(STM32_TOOLCHAIN_PATH ${STM32_TOOLCHAIN_PATH} DIRECTORY)
        endif()
    endif()
    file(TO_CMAKE_PATH "${STM32_TOOLCHAIN_PATH}" STM32_TOOLCHAIN_PATH)
endif()


set(STM32_TARGET_TRIPLET "arm-none-eabi")

set(TOOLCHAIN_SYSROOT  "${STM32_TOOLCHAIN_PATH}/${STM32_TARGET_TRIPLET}")
set(TOOLCHAIN_BIN_PATH "${STM32_TOOLCHAIN_PATH}/bin")
set(TOOLCHAIN_INC_PATH "${STM32_TOOLCHAIN_PATH}/${STM32_TARGET_TRIPLET}/include")
set(TOOLCHAIN_LIB_PATH "${STM32_TOOLCHAIN_PATH}/${STM32_TARGET_TRIPLET}/lib")

find_program(CMAKE_OBJCOPY NAMES ${STM32_TARGET_TRIPLET}-objcopy HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_OBJDUMP NAMES ${STM32_TARGET_TRIPLET}-objdump HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_SIZE NAMES ${STM32_TARGET_TRIPLET}-size HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_DEBUGGER NAMES ${STM32_TARGET_TRIPLET}-gdb HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_CPPFILT NAMES ${STM32_TARGET_TRIPLET}-c++filt HINTS ${TOOLCHAIN_BIN_PATH})

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

find_program(CMAKE_C_COMPILER   NAMES ${STM32_TARGET_TRIPLET}-gcc HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_CXX_COMPILER NAMES ${STM32_TARGET_TRIPLET}-g++ HINTS ${TOOLCHAIN_BIN_PATH})
find_program(CMAKE_ASM_COMPILER NAMES ${STM32_TARGET_TRIPLET}-gcc HINTS ${TOOLCHAIN_BIN_PATH})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Target chip. The firmware target is named after it:
#   cmake -B build                          -> target STM32F767 (default)
#   cmake -B build-h7 -DMCORE_CHIP=STM32H755 -> targets STM32H755 (Cortex-M7,
#                                              real time) and STM32H755_CM4
#                                              (Cortex-M4, network)
set(MCORE_CHIP "STM32F767" CACHE STRING "Target MCU")
set_property(CACHE MCORE_CHIP PROPERTY STRINGS STM32F767 STM32H755)

project(${MCORE_CHIP} LANGUAGES C CXX ASM)

# Export compile commands for clangd and other tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Per-chip settings
if(MCORE_CHIP STREQUAL "STM32F767")
    set(CHIP_DIR      stm32f767)
    set(CPU_FLAGS     -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard)
    set(CHIP_DEFINES  MCORE_STM32F767)
    set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/platform/stm32f767/STM32F767ZITX_FLASH.ld)
    set(STARTUP       platform/stm32f767/startup_stm32f767zitx.s)
elseif(MCORE_CHIP STREQUAL "STM32H755")
    # Dual core. Cortex-M7 (double-precision FPU): real-time application.
    set(CHIP_DIR      stm32h755)
    set(CPU_FLAGS     -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard)
    set(CHIP_DEFINES  MCORE_STM32H755 MCORE_DUAL_CORE CORE_CM7)
    set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/platform/stm32h755/STM32H755ZITX_FLASH_CM7.ld)
    set(STARTUP       platform/stm32h755/startup_stm32h755xx.s)
    # Cortex-M4 (single-precision FPU): network application (app_cm4/).
    # Same startup file: its ExitRun0Mode() is empty on the M4 (clock.cpp).
    set(MCORE_DUAL_CORE ON)
    set(CM4_CPU_FLAGS     -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard)
    # TCP slots: MQTT + diagnostics port 5001 (each slot is ~8.8 KB of RAM)
    set(CM4_DEFINES       MCORE_STM32H755 MCORE_DUAL_CORE CORE_CM4 TCP_MAX_CONNECTIONS=2)
    set(CM4_LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/platform/stm32h755/STM32H755ZITX_FLASH_CM4.ld)
else()
    message(FATAL_ERROR "Unsupported MCORE_CHIP='${MCORE_CHIP}' (use STM32F767 or STM32H755)")
endif()
message(STATUS "MCORE chip: ${MCORE_CHIP}")

# Sources. Each firmware image picks from these groups.
file(GLOB CORE_SRC ${CMAKE_SOURCE_DIR}/mcore/core/*.cpp)
file(GLOB NET_SRC  ${CMAKE_SOURCE_DIR}/mcore/net/*.cpp)
file(GLOB CHIP_SRC ${CMAKE_SOURCE_DIR}/mcore/chip/${CHIP_DIR}/*.cpp)
set(CHIP_ETH_SRC ${CMAKE_SOURCE_DIR}/mcore/chip/${CHIP_DIR}/eth.cpp)
list(REMOVE_ITEM CHIP_SRC ${CHIP_ETH_SRC})
file(GLOB APP_SRC ${CMAKE_SOURCE_DIR}/app/*.c ${CMAKE_SOURCE_DIR}/app/*.cpp)
set(SCHED_SRC ${CMAKE_SOURCE_DIR}/mcore/sched/sched.cpp)   # port_host.cpp is for tests only

# mcore_firmware(<target> CPU <flags> DEFINES <defs> LINKER <script> SOURCES <files>)
# Builds one ELF plus <target>.bin and prints its size.
function(mcore_firmware NAME)
    cmake_parse_arguments(FW "" "LINKER" "CPU;DEFINES;SOURCES" ${ARGN})

    set(C_FLAGS ${FW_CPU} -Os -ffunction-sections -fdata-sections -Wall -Wextra)
    set(CXX_FLAGS ${C_FLAGS} -fno-rtti -fno-exceptions -std=c++20)

    add_executable(${NAME} ${FW_SOURCES})
    target_include_directories(${NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/mcore
        ${CMAKE_SOURCE_DIR}/app
    )
    target_compile_definitions(${NAME} PRIVATE ${FW_DEFINES})
    target_compile_options(${NAME} PRIVATE $<$<COMPILE_LANGUAGE:C>:${C_FLAGS}>)
    target_compile_options(${NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_FLAGS}>)
    set_property(TARGET ${NAME} APPEND PROPERTY LINK_DEPENDS ${FW_LINKER})

    target_link_options(${NAME} PRIVATE
        ${FW_CPU}
        -T${FW_LINKER}
        -Wl,-Map=${CMAKE_BINARY_DIR}/${NAME}.map
        -Wl,--gc-sections
        -specs=nano.specs
        -lc -lnosys
    )

    add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${NAME}> $<TARGET_FILE_BASE_NAME:${NAME}>.bin
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${NAME}>
        COMMENT "Generating ${NAME}.bin and showing firmware size"
    )
endfunction()

if(MCORE_DUAL_CORE)
    # Cortex-M7: real time, no network stack
    mcore_firmware(${PROJECT_NAME}
        CPU     ${CPU_FLAGS}
        DEFINES ${CHIP_DEFINES}
        LINKER  ${LINKER_SCRIPT}
        SOURCES ${CORE_SRC} ${SCHED_SRC} ${CHIP_SRC} ${APP_SRC} ${STARTUP})

    # Cortex-M4: Ethernet + TCP/IP + TLS (Mbed TLS) + MQTT
    option(MCORE_TLS_SELFTEST "M4: run an in-memory TLS handshake at boot (timing/RAM report)" OFF)

    set(MBEDTLS_DIR ${CMAKE_SOURCE_DIR}/third_party/mbedtls)
    set(TLS_DEFINES MBEDTLS_CONFIG_FILE="mbedtls_config_mcore.h" MCORE_TLS_MEMORY_DEBUG MCORE_HW_GCM)
    if(MCORE_TLS_SELFTEST)
        list(APPEND TLS_DEFINES MCORE_TLS_TEST_SERVER MCORE_TLS_SELFTEST)
    endif()
    file(GLOB MBEDTLS_SRC ${MBEDTLS_DIR}/library/*.c)
    # gcm_soft.c: software GCM under other names, fallback of the CRYP GCM
    add_library(mbedtls_cm4 STATIC ${MBEDTLS_SRC} ${CMAKE_SOURCE_DIR}/mcore/net/tls/gcm_soft.c)
    target_include_directories(mbedtls_cm4 PUBLIC
        ${MBEDTLS_DIR}/include ${CMAKE_SOURCE_DIR}/mcore/net/tls)
    target_compile_definitions(mbedtls_cm4 PUBLIC ${TLS_DEFINES})
    target_compile_options(mbedtls_cm4 PRIVATE ${CM4_CPU_FLAGS} -Os -ffunction-sections -fdata-sections)

    # Broker CA (PEM) embedded as a string; replace the file for production.
    set(BROKER_CA_FILE ${CMAKE_SOURCE_DIR}/app_cm4/certs/broker_ca.pem)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${BROKER_CA_FILE})
    file(READ ${BROKER_CA_FILE} PEM_TEXT)
    if(PEM_TEXT MATCHES "MCORE TEST CA")
        message(WARNING "app_cm4/certs/broker_ca.pem is the MCORE TEST CA - replace it for production")
    endif()
    set(PEM_NAME BROKER_CA_PEM)
    configure_file(${CMAKE_SOURCE_DIR}/app_cm4/certs/pem.h.in ${CMAKE_BINARY_DIR}/generated/broker_ca.h @ONLY)
    if(MCORE_TLS_SELFTEST)
        foreach(item ca.crt:SELFTEST_CA_PEM server.crt:SELFTEST_SERVER_CRT server.key:SELFTEST_SERVER_KEY)
            string(REPLACE ":" ";" pair ${item})
            list(GET pair 0 file)
            list(GET pair 1 PEM_NAME)
            file(READ ${CMAKE_SOURCE_DIR}/tests/certs/${file} PEM_TEXT)
            configure_file(${CMAKE_SOURCE_DIR}/app_cm4/certs/pem.h.in
                ${CMAKE_BINARY_DIR}/generated/selftest_${PEM_NAME}.h @ONLY)
        endforeach()
        string(TIMESTAMP MCORE_BUILD_UNIX_TIME "%s" UTC)
    endif()

    file(GLOB APP_CM4_SRC ${CMAKE_SOURCE_DIR}/app_cm4/*.cpp)
    file(GLOB TLS_SRC ${CMAKE_SOURCE_DIR}/mcore/net/tls/*.cpp)
    file(GLOB VDA_SRC ${CMAKE_SOURCE_DIR}/mcore/vda5050/*.cpp)
    mcore_firmware(${PROJECT_NAME}_CM4
        CPU     ${CM4_CPU_FLAGS}
        DEFINES ${CM4_DEFINES}
        LINKER  ${CM4_LINKER_SCRIPT}
        SOURCES ${CORE_SRC} ${SCHED_SRC} ${CHIP_SRC} ${CHIP_ETH_SRC} ${NET_SRC} ${TLS_SRC} ${VDA_SRC}
                ${APP_CM4_SRC} ${CMAKE_SOURCE_DIR}/app/syscalls.c ${STARTUP})
    target_link_libraries(${PROJECT_NAME}_CM4 PRIVATE mbedtls_cm4)
    target_include_directories(${PROJECT_NAME}_CM4 PRIVATE ${CMAKE_BINARY_DIR}/generated)
    if(MCORE_TLS_SELFTEST)
        target_compile_definitions(${PROJECT_NAME}_CM4 PRIVATE MCORE_BUILD_UNIX_TIME=${MCORE_BUILD_UNIX_TIME}ULL)
    endif()
else()
    mcore_firmware(${PROJECT_NAME}
        CPU     ${CPU_FLAGS}
        DEFINES ${CHIP_DEFINES}
        LINKER  ${LINKER_SCRIPT}
        SOURCES ${CORE_SRC} ${SCHED_SRC} ${CHIP_SRC} ${CHIP_ETH_SRC} ${NET_SRC} ${APP_SRC} ${STARTUP})
endif()
