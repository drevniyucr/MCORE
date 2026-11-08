/*
 * newlib time hook for the Cortex-M4 image: time()/gettimeofday() report the
 * SNTP wall clock (0 = 1970 until the first sync). Mbed TLS references time()
 * as its default clock before tls::platform_init() installs the SNTP source.
 */
#include <sys/time.h>

#include "net/sntp.hpp"

extern "C" int _gettimeofday(struct timeval *tv, void *) {
	if (tv != nullptr) {
		const uint64_t ms = NET_SNTP_UnixMs();
		tv->tv_sec = static_cast<time_t>(ms / 1000U);
		tv->tv_usec = static_cast<suseconds_t>((ms % 1000U) * 1000U);
	}
	return 0;
}
