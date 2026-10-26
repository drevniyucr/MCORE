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

# Export compile commands for clangd and other tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



# CPU / компилятор
set(CPU_FLAGS -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard)
# C flags
set(C_FLAGS ${CPU_FLAGS} -Os -ffunction-sections -fdata-sections -Wall -Wextra)
# C++ flags
set(CXX_FLAGS ${C_FLAGS} -fno-rtti -fno-exceptions -std=c++20)

# Linker
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/platform/STM32F767ZITX_FLASH.ld)

# Исходники
file(GLOB_RECURSE SRC
    mcore/*.c
    mcore/*.cpp
    app/*.c
    app/*.cpp
)
list(APPEND SRC platform/startup_stm32f767zitx.s)

add_executable(${PROJECT_NAME} ${SRC})

# Lower bound for the RTC at boot, in UTC (__DATE__/__TIME__ are the build
# PC's local time). Taken at configure time; only main.cpp sees it, so a
# reconfigure does not rebuild everything.
string(TIMESTAMP MCORE_BUILD_UNIX "%s" UTC)
set_source_files_properties(app/main.cpp PROPERTIES COMPILE_DEFINITIONS "MCORE_BUILD_UNIX=${MCORE_BUILD_UNIX}LL")

target_include_directories(${PROJECT_NAME} PRIVATE
    mcore
    app
)

# ---- mbedTLS: TLS for net/netclient (same release and setup as goz-shuttle) ----
# 3.6 LTS, downloaded at configure time and checked against the release
# checksum; for an offline build point MBEDTLS_DIR at an unpacked release.
# Only its sources are used, its own CMake project is not.
set(MBEDTLS_VERSION 3.6.7)
set(MBEDTLS_SHA256 a7e8bcbec0e6f761b4af24f25677626b35f762f68eef79c08677a363212d11f6)
set(MBEDTLS_DIR "" CACHE PATH "unpacked mbedtls-${MBEDTLS_VERSION} (empty: download)")
if(NOT MBEDTLS_DIR)
    include(FetchContent)
    FetchContent_Declare(mbedtls
        URL https://github.com/Mbed-TLS/mbedtls/releases/download/mbedtls-${MBEDTLS_VERSION}/mbedtls-${MBEDTLS_VERSION}.tar.bz2
        URL_HASH SHA256=${MBEDTLS_SHA256}
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        SOURCE_SUBDIR do-not-add)
    FetchContent_MakeAvailable(mbedtls)
    set(MBEDTLS_SRC_DIR ${mbedtls_SOURCE_DIR})
else()
    set(MBEDTLS_SRC_DIR ${MBEDTLS_DIR})
endif()
file(GLOB MBEDTLS_SOURCES ${MBEDTLS_SRC_DIR}/library/*.c)
add_library(mcore_mbedtls STATIC ${MBEDTLS_SOURCES})
target_include_directories(mcore_mbedtls SYSTEM PUBLIC ${MBEDTLS_SRC_DIR}/include)
target_include_directories(mcore_mbedtls PUBLIC mcore/net/tls PRIVATE ${MBEDTLS_SRC_DIR}/library)
target_compile_definitions(mcore_mbedtls PUBLIC "MBEDTLS_CONFIG_FILE=\"mcore_mbedtls_config.h\"")
target_compile_options(mcore_mbedtls PRIVATE ${C_FLAGS} -w)
target_link_libraries(${PROJECT_NAME} PRIVATE mcore_mbedtls)

set(LINKER_MAP_FILE "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map")


target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:C>:${C_FLAGS}>)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${CXX_FLAGS}>)


target_link_options(${PROJECT_NAME} PRIVATE
    ${CPU_FLAGS}
    -T${LINKER_SCRIPT}
    -Wl,-Map=${LINKER_MAP_FILE}
    -Wl,--gc-sections
    -specs=nano.specs
    -lc -lnosys
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
    COMMENT "Generating .bin and showing firmware size"
)
