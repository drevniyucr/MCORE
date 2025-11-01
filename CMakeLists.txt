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

project(STM32F767 LANGUAGES C CXX ASM)



# CPU / компилятор
set(CPU_FLAGS -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard)
# C flags
set(C_FLAGS ${CPU_FLAGS} -Os -ffunction-sections -fdata-sections -Wall -Wextra)
# C++ flags
set(CXX_FLAGS ${C_FLAGS} -fno-rtti -fno-exceptions -std=c++20)

# Linker
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F767ZITX_FLASH.ld)

# Исходники
file(GLOB SRC
   # Core/Src/*.c
   # Core/Src/*.cpp
    Core/Startup/startup_stm32f767zitx.s
    Core/Src/main.cpp
    Core/Src/mcore_mpu.cpp
    Core/Src/mcore_rcc.cpp
    Core/Src/stm32f7xx_it.c
)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE STM32F767xx
    Core/Inc
  # Drivers/CMSIS/Device/ST/STM32F7xx/Include
   # Drivers/CMSIS/Include
)


target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:C>:${C_FLAGS}>)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_FLAGS}>)
target_link_options(${PROJECT_NAME} PRIVATE
    ${CPU_FLAGS}
    -T${LINKER_SCRIPT}
    -Wl,--gc-sections
    -specs=nano.specs
    -lc -lnosys
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
    COMMENT "Generating .bin and showing firmware size"
)
