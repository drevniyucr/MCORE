# STM32F767 (NUCLEO-F767ZI): one Cortex-M7 image with the network stack.
#   cmake -S projects/stm32f767 -B build-f767 && cmake --build build-f767
cmake_minimum_required(VERSION 3.21)
get_filename_component(MCORE_ROOT ${CMAKE_CURRENT_LIST_DIR}/../.. ABSOLUTE)
include(${MCORE_ROOT}/cmake/arm-none-eabi.cmake)

project(STM32F767 LANGUAGES C CXX ASM)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(${MCORE_ROOT}/cmake/mcore.cmake)

mcore_sources(stm32f767)
file(GLOB APP_SRC ${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp)

mcore_firmware(STM32F767
    CPU      -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard
    LINKER   ${CMAKE_CURRENT_SOURCE_DIR}/STM32F767ZITX_FLASH.ld
    INCLUDES ${MCORE_CHIP_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/app
    SOURCES  ${MCORE_CORE_SRC} ${MCORE_CHIP_SRC} ${MCORE_CHIP_ETH_SRC} ${MCORE_NET_SRC}
             ${APP_SRC} ${CMAKE_CURRENT_SOURCE_DIR}/startup_stm32f767zitx.s)
