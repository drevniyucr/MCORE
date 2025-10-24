cmake_minimum_required(VERSION 3.19)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/stm32-cmake/cmake/stm32_gcc.cmake)
include(tools/fetch_svd.cmake)

# Prefer local CMSIS (Drivers/CMSIS) and avoid fetching from network when possible
set(FETCHCONTENT_SOURCE_DIR_STM32-CMSIS "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS")
set(FETCHCONTENT_SOURCE_DIR_STM32-CMSIS-F7 "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS")

# Select MCU model:
set(MCU STM32F767)

# Set project name to match directory name. If set othervise tasks.json and launch.json should be updated
get_filename_component(BASE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(PROJECT_NAME ${BASE_FOLDER})

project(${PROJECT_NAME} C CXX ASM)


# MCU familiy from MCU
string(SUBSTRING ${MCU} 5 2 MCU_FAMILY)
string(SUBSTRING ${MCU} 5 6 MCU_MODEL)

# Fetch CMSIS-SVD only (use local CMSIS in Drivers/)
fetch_svd(${MCU})
# Use local CMSIS from Drivers/CMSIS; avoid linking fetched CMSIS sources to prevent duplicates
stm32_fetch_cmsis(${MCU_FAMILY})
update_launch_json(${MCU})

# NOTE: we prefer using local CMSIS headers under Drivers/CMSIS and local startup/linker scripts.
# Do not use the CMSIS:: targets provided by fetched content to avoid double-definitions of
# startup/system sources.


file(GLOB_RECURSE PROJECT_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Startup/*.s"
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# Use local CMSIS headers from Drivers/CMSIS
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F7xx/Include
)

# Force using local linker script from repo root
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/STM32F767ZITX_FLASH.ld")
    message(STATUS "Using linker script: ${CMAKE_CURRENT_SOURCE_DIR}/STM32F767ZITX_FLASH.ld")
    # pass -T to linker
    target_link_options(${PROJECT_NAME} PRIVATE "-T${CMAKE_CURRENT_SOURCE_DIR}/STM32F767ZITX_FLASH.ld")
endif()
target_include_directories(${PROJECT_NAME} PRIVATE 
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc 
${CMAKE_CURRENT_SOURCE_DIR}/Core/Src
)

# Link required libraries
target_link_libraries(${PROJECT_NAME}
   # HAL::STM32::${MCU_FAMILY}::RCC
   # HAL::STM32::${MCU_FAMILY}::GPIO
   # HAL::STM32::${MCU_FAMILY}::CORTEX
    # Use local CMSIS headers instead of the CMSIS:: targets to avoid duplicate startup/system sources
    # CMSIS::STM32::${MCU_MODEL}
    STM32::Nano # use newlib nano to reduce code size.
    STM32::NoSys
)
target_compile_options(${PROJECT_NAME} 
PUBLIC
-DSTM32F767xx 
#-Os 
-fno-exceptions 
-fno-rtti
)

# Ensure ARM/Thumb and FPU/compiler options are set for GCC toolchain
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mthumb -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -Wall -ffunction-sections -fdata-sections")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mthumb -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -Wall -ffunction-sections -fdata-sections")
    set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mthumb -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard")
endif()

set(CMAKE_CXX_STANDARD 20)

stm32_print_size_of_target(${PROJECT_NAME})
stm32_generate_elf_bin(${PROJECT_NAME})
stm32_generate_elf_hex(${PROJECT_NAME})