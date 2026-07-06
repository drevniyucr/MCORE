/*
 * port.cpp — hardware-backed implementations of net-stack seams.
 * Host tests link their own stubs instead of this file.
 */
#include "net/net.hpp"

uint32_t NET_Entropy() {
	return SysTick::_STCVR::read();
}
