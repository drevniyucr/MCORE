# mcore.cmake — use MCORE from another CMake project (arm-none-eabi cross build)
#
#   include(path/to/MCORE/mcore.cmake)
#   mcore_add_library(mcore)                    # the framework as a target
#   mcore_add_library(mcore TLS path/to/my_mbedtls_config.h)   # ... with TLS
#   add_executable(app.elf main.cpp)
#   target_link_libraries(app.elf PRIVATE mcore)
#   target_link_options(app.elf PRIVATE ${MCORE_LINK_OPTIONS})
#
# The target is an OBJECT library on purpose: the interrupt handlers MCORE
# defines (fault.cpp, eth.cpp, system.cpp) only override the weak ones of the
# startup file if their objects are linked unconditionally; from a static
# archive they would be pulled in only when something else references them.
# --gc-sections drops whatever the application does not use.
#
# Without TLS, NetClient is built for plain TCP (MCORE_NET_TLS=0) and the
# mbedTLS glue (net/tls.cpp) is left out. With TLS <config>, mbedTLS
# ${MCORE_MBEDTLS_VERSION} is downloaded (checksum verified; offline: set
# MBEDTLS_DIR to an unpacked release) and built with that config header; the
# config must keep what net/tls.cpp uses (see mcore/net/tls/mcore_mbedtls_config.h:
# HAVE_TIME + PLATFORM_MS_TIME_ALT, ENTROPY_HARDWARE_ALT, MEMORY_BUFFER_ALLOC_C,
# ECP_RESTARTABLE, SSL_MAX_FRAGMENT_LENGTH).
#
# The C MQTT client (net/mqtt.c) is not included: the application can take it
# from its own copy. Sizes can be set on the target, PUBLIC:
#   MCORE_NETCLIENT_TX_LEN=<bytes>   NetClient TX queue (default 2048)
#   MCORE_TLS_ARENA_SIZE=<bytes>     mbedTLS heap (default 32 KB)
#   MCORE_TLS_ECP_MAX_OPS=<n>        ECC work per handshake step (default 400)
# Per-instance TX queues: NetClientBuf<N> instead of NetClient.
#
# The application may provide "config.hpp" on its include path to override
# the clock values of core/mcore_config.hpp.

set(MCORE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(MCORE_STARTUP ${MCORE_DIR}/platform/startup_stm32f767zitx.s)
set(MCORE_LINKER_SCRIPT ${MCORE_DIR}/platform/STM32F767ZITX_FLASH.ld)
set(MCORE_CPU_FLAGS -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard)
# The C library (e.g. -specs=nano.specs -specs=nosys.specs) is the
# application's choice: GCC refuses the same specs file twice.
set(MCORE_LINK_OPTIONS ${MCORE_CPU_FLAGS} -T${MCORE_LINKER_SCRIPT} -Wl,--gc-sections)

set(MCORE_MBEDTLS_VERSION 3.6.7)
# Optimisation of the mbedTLS library: the handshake is bignum arithmetic,
# -O2 makes it ~20% faster than -Os for ~12 KB more flash (measured on F767)
set(MCORE_MBEDTLS_OPT -O2 CACHE STRING "compiler optimisation for mbedTLS (-O2 / -Os)")
set(MCORE_MBEDTLS_SHA256 a7e8bcbec0e6f761b4af24f25677626b35f762f68eef79c08677a363212d11f6)

# mbedTLS sources (downloaded once per build tree) as a static library
function(_mcore_mbedtls target config_header)
  set(MBEDTLS_DIR "" CACHE PATH "unpacked mbedtls-${MCORE_MBEDTLS_VERSION} (empty: download)")
  if(NOT MBEDTLS_DIR)
    include(FetchContent)
    FetchContent_Declare(mbedtls
      URL https://github.com/Mbed-TLS/mbedtls/releases/download/mbedtls-${MCORE_MBEDTLS_VERSION}/mbedtls-${MCORE_MBEDTLS_VERSION}.tar.bz2
      URL_HASH SHA256=${MCORE_MBEDTLS_SHA256}
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
      SOURCE_SUBDIR do-not-add)            # sources only, its own CMake project is not used
    FetchContent_MakeAvailable(mbedtls)
    set(src_dir ${mbedtls_SOURCE_DIR})
  else()
    set(src_dir ${MBEDTLS_DIR})
  endif()
  get_filename_component(config_dir ${config_header} DIRECTORY)
  get_filename_component(config_name ${config_header} NAME)
  file(GLOB lib_src ${src_dir}/library/*.c)
  add_library(${target} STATIC ${lib_src})
  target_include_directories(${target} SYSTEM PUBLIC ${src_dir}/include)
  target_include_directories(${target} PUBLIC ${config_dir} PRIVATE ${src_dir}/library)
  target_compile_definitions(${target} PUBLIC "MBEDTLS_CONFIG_FILE=\"${config_name}\"")
  target_compile_options(${target} PRIVATE ${MCORE_CPU_FLAGS} ${MCORE_MBEDTLS_OPT} -ffunction-sections -fdata-sections -w)
endfunction()

function(mcore_add_library name)
  cmake_parse_arguments(ARG "" "TLS" "" ${ARGN})
  enable_language(CXX ASM)
  file(GLOB_RECURSE src CONFIGURE_DEPENDS ${MCORE_DIR}/mcore/*.cpp)
  if(NOT ARG_TLS)
    list(FILTER src EXCLUDE REGEX "/net/tls\\.cpp$")
  endif()
  add_library(${name} OBJECT ${src} ${MCORE_STARTUP})
  target_include_directories(${name} PUBLIC ${MCORE_DIR}/mcore)
  target_compile_options(${name} PRIVATE
    ${MCORE_CPU_FLAGS} -Os -ffunction-sections -fdata-sections -Wall -Wextra
    $<$<COMPILE_LANGUAGE:CXX>:-std=c++20 -fno-rtti -fno-exceptions>)
  if(ARG_TLS)
    _mcore_mbedtls(${name}_mbedtls ${ARG_TLS})
    target_link_libraries(${name} PUBLIC ${name}_mbedtls)
    target_compile_definitions(${name} PUBLIC MCORE_NET_TLS=1)
  else()
    target_compile_definitions(${name} PUBLIC MCORE_NET_TLS=0)
  endif()
endfunction()
