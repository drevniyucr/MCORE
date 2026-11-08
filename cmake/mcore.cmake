# MCORE build helpers shared by the projects (projects/<chip>/CMakeLists.txt).
#
# A project selects its chip by the chip directory it passes in INCLUDES
# (mcore/chips/<chip>): the framework includes "chip/regs.hpp", "chip/rcc.hpp",
# ... and gets that chip's files. There are no chip defines.

get_filename_component(MCORE_ROOT ${CMAKE_CURRENT_LIST_DIR}/.. ABSOLUTE)

# mcore_sources(<chip>) sets the source groups of the framework:
#   MCORE_CORE_SRC      core, scheduler, newlib stubs
#   MCORE_CHIP_SRC      chip layer without the Ethernet driver
#   MCORE_CHIP_ETH_SRC  chip Ethernet driver (images with a network stack)
#   MCORE_NET_SRC       IP stack, DHCP, SNTP, MQTT
#   MCORE_TLS_SRC       TLS client on Mbed TLS (see mcore_mbedtls)
#   MCORE_VDA_SRC       VDA 5050 client
macro(mcore_sources CHIP)
    set(MCORE_CHIP_DIR ${MCORE_ROOT}/mcore/chips/${CHIP})
    file(GLOB MCORE_CORE_SRC ${MCORE_ROOT}/mcore/core/*.cpp)
    list(APPEND MCORE_CORE_SRC
        ${MCORE_ROOT}/mcore/core/syscalls.c
        ${MCORE_ROOT}/mcore/sched/sched.cpp)          # port_host.cpp is for tests only
    file(GLOB MCORE_CHIP_SRC ${MCORE_CHIP_DIR}/chip/*.cpp)
    set(MCORE_CHIP_ETH_SRC ${MCORE_CHIP_DIR}/chip/eth.cpp)
    list(REMOVE_ITEM MCORE_CHIP_SRC ${MCORE_CHIP_ETH_SRC})
    file(GLOB MCORE_NET_SRC ${MCORE_ROOT}/mcore/net/*.cpp)
    file(GLOB MCORE_TLS_SRC ${MCORE_ROOT}/mcore/net/tls/*.cpp)
    file(GLOB MCORE_VDA_SRC ${MCORE_ROOT}/mcore/vda5050/*.cpp)
endmacro()

# mcore_firmware(<target> CPU <flags> LINKER <script> INCLUDES <dirs>
#                [DEFINES <defs>] SOURCES <files>)
# Builds one ELF plus <target>.bin and prints its size. INCLUDES: the chip
# directory (mcore/chips/<chip>) and the application directories.
function(mcore_firmware NAME)
    cmake_parse_arguments(FW "" "LINKER" "CPU;DEFINES;INCLUDES;SOURCES" ${ARGN})

    set(C_FLAGS ${FW_CPU} -Os -ffunction-sections -fdata-sections -Wall -Wextra)
    set(CXX_FLAGS ${C_FLAGS} -fno-rtti -fno-exceptions -std=c++20)

    add_executable(${NAME} ${FW_SOURCES})
    target_include_directories(${NAME} PRIVATE ${MCORE_ROOT}/mcore ${FW_INCLUDES})
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

# mcore_mbedtls(<target> CPU <flags> [DEFINES <defs>] [SOURCES <extra files>])
# Mbed TLS (third_party/mbedtls) as a static library with the MCORE
# configuration (mcore/net/tls/mbedtls_config_mcore.h). DEFINES are PUBLIC:
# they select features of that configuration for the library and its users.
function(mcore_mbedtls NAME)
    cmake_parse_arguments(TLS "" "" "CPU;DEFINES;SOURCES" ${ARGN})
    set(MBEDTLS_DIR ${MCORE_ROOT}/third_party/mbedtls)
    file(GLOB MBEDTLS_SRC ${MBEDTLS_DIR}/library/*.c)
    add_library(${NAME} STATIC ${MBEDTLS_SRC} ${TLS_SOURCES})
    target_include_directories(${NAME} PUBLIC ${MBEDTLS_DIR}/include ${MCORE_ROOT}/mcore/net/tls)
    target_compile_definitions(${NAME} PUBLIC
        MBEDTLS_CONFIG_FILE="mbedtls_config_mcore.h" MCORE_TLS_MEMORY_DEBUG ${TLS_DEFINES})
    target_compile_options(${NAME} PRIVATE ${TLS_CPU} -Os -ffunction-sections -fdata-sections)
endfunction()

# mcore_embed_pem(<pem file> <C name> <output header>): a PEM file as a C string
function(mcore_embed_pem PEM_FILE PEM_NAME OUT)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${PEM_FILE})
    file(READ ${PEM_FILE} PEM_TEXT)
    configure_file(${MCORE_ROOT}/cmake/pem.h.in ${OUT} @ONLY)
endfunction()
