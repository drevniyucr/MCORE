// Shared declarations between stubs.cpp and the test files.
#pragma once

#include <cstdint>
#include <vector>

struct CapturedFrame {
	std::vector<uint8_t> data;
};

// Frames "transmitted" through the ETH_SendFrame stub, in order.
extern std::vector<CapturedFrame> g_tx_frames;

// Controllable clock returned by the get_tick() stub (ms).
extern uint32_t g_test_now;

// Deterministic value returned by the NET_Entropy() stub (TCP ISN).
constexpr uint32_t TEST_ISN = 0x11223344;

// Clears captured frames, resets the clock and re-points the TX buffer.
void test_reset(uint32_t now = 1000);
