// Host-side stand-ins for the hardware symbols the net stack links against.
// The firmware implementations live in mcore/drivers/eth.cpp,
// mcore/core/system.cpp and mcore/net/port.cpp — none of which are compiled
// into the test binary.
#include <cstdint>
#include <cstring>

#include "drivers/eth.hpp"
#include "core/system.hpp"
#include "net/tcp.hpp"
#include "test_support.hpp"

std::vector<CapturedFrame> g_tx_frames;
uint32_t g_test_now = 0;

static uint8_t tx_buffer[ETH_MAX_PACKET_SIZE];

// Real one lives in eth.cpp; the stack writes outgoing frames into
// TxDescList.pBuff and then calls ETH_SendFrame(len).
ETH_TxDescListStruct TxDescList = {};

bool ETH_SendFrame(uint32_t len) {
	CapturedFrame f;
	f.data.assign(TxDescList.pBuff, TxDescList.pBuff + len);
	g_tx_frames.push_back(std::move(f));
	return true;
}

bool ETH_IsTxBufferAvailable() {
	return true;
}

uint32_t get_tick() {
	return g_test_now;
}

void print(const char *, ...) {
	// silence NET_TCP_ClientAdd/Remove logging
}

uint32_t NET_Entropy() {
	return TEST_ISN;
}

void test_reset(uint32_t now) {
	TxDescList.pBuff = tx_buffer;
	g_tx_frames.clear();
	g_test_now = now;
}
