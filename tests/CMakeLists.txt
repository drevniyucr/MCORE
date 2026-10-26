# Host-side unit tests for the MCORE network stack.
# Built with the NATIVE compiler, independently of the firmware cross-build:
#   cmake -S tests -B build-tests -G Ninja
#   cmake --build build-tests
#   ctest --test-dir build-tests --output-on-failure
cmake_minimum_required(VERSION 3.21)
project(mcore_tests C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MCORE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(net_tests
    test_main.cpp
    stubs.cpp
    test_eth_utils.cpp
    test_net.cpp
    test_tcp.cpp
    test_dhcp.cpp
    test_scheduler.cpp
    test_ring_buffer.cpp
    test_arp.cpp
    test_dns.cpp
    test_tcp_connect.cpp
    test_ntp.cpp
    test_rx_lengths.cpp
    test_tcp_buf.cpp
    test_udp.cpp
    # Real stack sources under test (system.cpp/eth.cpp are NOT linked —
    # their symbols are provided by stubs.cpp)
    ${MCORE_ROOT}/mcore/net/eth_utils.cpp
    ${MCORE_ROOT}/mcore/net/net.cpp
    ${MCORE_ROOT}/mcore/net/tcp.cpp
    ${MCORE_ROOT}/mcore/net/tcp_buf.cpp
    ${MCORE_ROOT}/mcore/net/dhcp.cpp
    ${MCORE_ROOT}/mcore/net/arp.cpp
    ${MCORE_ROOT}/mcore/net/dns.cpp
    ${MCORE_ROOT}/mcore/net/ntp.cpp
    ${MCORE_ROOT}/mcore/net/udp.cpp
)

target_include_directories(net_tests PRIVATE
    ${MCORE_ROOT}/mcore
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(NOT MSVC)
    target_compile_options(net_tests PRIVATE -Wall -Wextra)
endif()
if(WIN32)
    # clang on Windows uses the MSVC CRT, which flags strncpy as deprecated
    target_compile_definitions(net_tests PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Self-contained exe on Windows: avoid libstdc++/libgcc/winpthread DLL deps
if(WIN32 AND NOT MSVC)
    target_link_options(net_tests PRIVATE -static)
endif()

# MQTT client shared with goz-shuttle: C11, tested with the shuttle's own
# minitest suite (tests/c), kept identical to the upstream copy
add_executable(mqtt_tests
    c/test_mqtt.c
    ${MCORE_ROOT}/mcore/net/mqtt.c
)
target_include_directories(mqtt_tests PRIVATE ${MCORE_ROOT}/mcore)
set_target_properties(mqtt_tests PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED ON)
if(WIN32)
    # clang on Windows uses the MSVC CRT, which flags strncpy as deprecated
    target_compile_definitions(mqtt_tests PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
if(NOT MSVC)
    target_compile_options(mqtt_tests PRIVATE -Wall -Wextra)
endif()

enable_testing()
add_test(NAME net_tests COMMAND net_tests)
add_test(NAME mqtt_tests COMMAND mqtt_tests)
