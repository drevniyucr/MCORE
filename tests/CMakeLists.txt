# Host-side unit tests for the MCORE network stack.
# Built with the NATIVE compiler, independently of the firmware cross-build:
#   cmake -S tests -B build-tests -G Ninja
#   cmake --build build-tests
#   ctest --test-dir build-tests --output-on-failure
cmake_minimum_required(VERSION 3.21)
project(mcore_tests CXX)

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
    # Real stack sources under test (system.cpp/eth.cpp are NOT linked —
    # their symbols are provided by stubs.cpp)
    ${MCORE_ROOT}/mcore/net/eth_utils.cpp
    ${MCORE_ROOT}/mcore/net/net.cpp
    ${MCORE_ROOT}/mcore/net/tcp.cpp
    ${MCORE_ROOT}/mcore/net/dhcp.cpp
)

target_include_directories(net_tests PRIVATE
    ${MCORE_ROOT}/mcore
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(NOT MSVC)
    target_compile_options(net_tests PRIVATE -Wall -Wextra)
endif()

# Self-contained exe on Windows: avoid libstdc++/libgcc/winpthread DLL deps
if(WIN32 AND NOT MSVC)
    target_link_options(net_tests PRIVATE -static)
endif()

enable_testing()
add_test(NAME net_tests COMMAND net_tests)
