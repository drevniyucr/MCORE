/* From goz-shuttle fw/tests (commit 200592d). */
/* Minimal test harness: no dependencies, one executable per module. */
#ifndef MINITEST_H
#define MINITEST_H

#include <math.h>
#include <stdio.h>
#include <string.h>

static int mt_failed, mt_checks;

#define CHECK(cond) do { \
    ++mt_checks; \
    if (!(cond)) { ++mt_failed; printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); } \
} while (0)

#define CHECK_EQ_INT(a, b) do { \
    long long _a = (long long)(a), _b = (long long)(b); ++mt_checks; \
    if (_a != _b) { ++mt_failed; printf("  FAIL %s:%d: %s = %lld, expected %s = %lld\n", \
        __FILE__, __LINE__, #a, _a, #b, _b); } \
} while (0)

#define CHECK_EQ_U32(a, b) do { \
    unsigned long _a = (unsigned long)(a), _b = (unsigned long)(b); ++mt_checks; \
    if (_a != _b) { ++mt_failed; printf("  FAIL %s:%d: %s = 0x%08lX, expected 0x%08lX\n", \
        __FILE__, __LINE__, #a, _a, _b); } \
} while (0)

#define CHECK_NEAR(a, b, eps) do { \
    double _a = (double)(a), _b = (double)(b); ++mt_checks; \
    if (fabs(_a - _b) > (double)(eps)) { ++mt_failed; printf("  FAIL %s:%d: %s = %g, expected %g (+-%g)\n", \
        __FILE__, __LINE__, #a, _a, _b, (double)(eps)); } \
} while (0)

#define CHECK_STR(a, b) do { ++mt_checks; \
    if (strcmp((a), (b)) != 0) { ++mt_failed; printf("  FAIL %s:%d: \"%s\" != \"%s\"\n", \
        __FILE__, __LINE__, (a), (b)); } \
} while (0)

#define RUN(fn) do { printf("- %s\n", #fn); fn(); } while (0)

#define MT_MAIN(body) int main(void) { setvbuf(stdout, NULL, _IONBF, 0); body; \
    printf("%s: %d checks, %d failed\n", __FILE__, mt_checks, mt_failed); \
    return mt_failed ? 1 : 0; }

#endif
