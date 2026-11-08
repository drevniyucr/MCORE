#pragma once
// ---------------------------------------------------------------------------
// Framework configuration contract.
//
// The framework consumes a small set of oscillator/clock values (below). The
// application supplies board specifics and overrides in its own "config.hpp"
// (MAC/PHY, a custom HSE crystal, timeouts, ...). This header pulls that app
// config if it is on the include path, then fills in defaults for anything the
// application did not define.
//
// Net effect:
//   - The framework no longer *hard-requires* an application config.hpp; it
//     compiles standalone (e.g. host unit tests, where app/ is not on -I).
//   - The application still overrides any value simply by defining it (its
//     config.hpp already uses `#if !defined` guards, and is included first).
//
// Override a value by defining it before the framework is included, or in the
// application's config.hpp.
// ---------------------------------------------------------------------------

#if defined(__has_include)
  #if __has_include("config.hpp")
    #include "config.hpp"   // application overrides + board configuration
  #endif
#endif

// --- Oscillator / clock contract (defaults; Hz) ---
#ifndef HSE_VALUE
  #define HSE_VALUE 8000000U    // high-speed external crystal
#endif
#ifndef HSI_VALUE
  #define HSI_VALUE 16000000U   // high-speed internal RC
#endif
#ifndef LSE_VALUE
  #define LSE_VALUE 32768U      // low-speed external crystal
#endif
#ifndef LSI_VALUE
  #define LSI_VALUE 32000U      // low-speed internal RC
#endif
