# mcore.cmake — use MCORE from another CMake project (arm-none-eabi cross build)
#
#   include(path/to/MCORE/mcore.cmake)
#   mcore_add_library(mcore)                    # the framework as a target
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
# Not included: the mbedTLS glue (net/tls.cpp, net/netclient.cpp: they need
# mbedTLS and its config) and the C MQTT client (net/mqtt.c), which the
# application can take from its own copy.
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

function(mcore_add_library name)
  enable_language(CXX ASM)
  file(GLOB_RECURSE src CONFIGURE_DEPENDS ${MCORE_DIR}/mcore/*.cpp)
  list(FILTER src EXCLUDE REGEX "/net/(tls|netclient)\\.cpp$")
  add_library(${name} OBJECT ${src} ${MCORE_STARTUP})
  target_include_directories(${name} PUBLIC ${MCORE_DIR}/mcore)
  target_compile_options(${name} PRIVATE
    ${MCORE_CPU_FLAGS} -Os -ffunction-sections -fdata-sections -Wall -Wextra
    $<$<COMPILE_LANGUAGE:CXX>:-std=c++20 -fno-rtti -fno-exceptions>)
endfunction()
