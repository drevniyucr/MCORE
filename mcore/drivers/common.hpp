#pragma once
#include <cstdint>

// ---------------------------------------------------------------------------
// DrvStatus — the unified result code for all peripheral drivers (init and
// polling data paths). Replaces the per-driver copies (USARTStatus, UartStatus,
// I2CStatus, I2CPollStatus, CANStatus, CanPollStatus) that all spelled the
// same four values.
//
// Convention: 0 == Ok, so `if (st != DrvStatus::Ok)` is the error check.
// Driver-specific conditions get their own enumerator (Nack) instead of a
// separate enum type.
// ---------------------------------------------------------------------------
enum class DrvStatus : uint8_t {
    Ok = 0,
    Error,       // hardware error flag raised (bus error, overrun, ...)
    Timeout,     // deadline elapsed before the hardware became ready
    InvalidArg,  // config/argument out of range (e.g. baud rate, FIFO combo)
    Nack,        // I2C: address or data byte not acknowledged
    Busy,        // resource not READY (e.g. DMA stream occupied)
};
