/*
 * spi.hpp — SPI master driver (init, polling and DMA full-duplex transfers)
 *
 * Layout follows the other drivers:
 *   - SPI_Init<SPIx, cfg>()      one-shot init, config as NTTP
 *   - SpiHandle<SPIx, Word>      data path (transfer / write / read / DMA)
 *   - SpiSelect<CsPin>           RAII chip select for polling transfers
 *
 * Chip select is software-driven (SSM = SSI = 1): any GPIO can be CS and
 * several devices can share one bus. Slave mode, hardware NSS, CRC, TI
 * frame format and I2S are not covered.
 *
 * F7 SPI has a 32-bit RX/TX FIFO with data packing: a 16-bit access to DR
 * with an 8-bit frame size moves TWO frames. The data path therefore touches
 * DR through a pointer of exactly the frame width (Word) — never the generic
 * 32-bit Register::overwrite/read.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include "core/regs.hpp"
#include "core/rcc.hpp"
#include "core/system.hpp"
#include "drivers/common.hpp"
#include "drivers/dma.hpp"

constexpr uint8_t SPI_TIMEOUT_MS = 100;  // default whole-transfer timeout
constexpr uint8_t SPI_PRESCALER_STEPS = 8;  // BR = 0..7 -> PCLK / 2..256

// Frames written but not yet read back. Keeps the TX FIFO fed while the RX
// FIFO (4 bytes / 2 halfwords) can never overrun.
constexpr size_t SPI_MAX_IN_FLIGHT = 2;


enum class SpiMode : uint8_t {   // (CPOL, CPHA)
    Mode0 = 0b00,                // idle low,  sample on rising edge
    Mode1 = 0b01,                // idle low,  sample on falling edge
    Mode2 = 0b10,                // idle high, sample on falling edge
    Mode3 = 0b11                 // idle high, sample on rising edge
};

enum class SpiBitOrder : uint8_t { MsbFirst, LsbFirst };

struct SpiHwConfig {
    uint32_t MaxClockHz = 1'000'000;  // SCK <= this; picks the smallest PCLK/2..256 that fits
    SpiMode Mode = SpiMode::Mode0;
    SpiBitOrder BitOrder = SpiBitOrder::MsbFirst;
    uint8_t DataBits = 8;             // 4..16; use SpiHandle<SPIx, uint16_t> above 8
};

// SPI1/4/5/6 hang off APB2, SPI2/3 off APB1.
template<typename SPIx>
inline constexpr bool spi_on_apb2 = std::is_same_v<SPIx, SPI1> || std::is_same_v<SPIx, SPI4>
                                 || std::is_same_v<SPIx, SPI5> || std::is_same_v<SPIx, SPI6>;


template<typename SPIx, SpiHwConfig cfg>
static inline DrvStatus SPI_Init() {
    static_assert(cfg.DataBits >= 4 && cfg.DataBits <= 16, "SPI: DataBits must be 4..16");
    static_assert(cfg.MaxClockHz > 0, "SPI: MaxClockHz must be non-zero");

    /* 0. Clock enable */
    RccEnable<SPIx>::enable();

    /* 1. Disable SPI: CR1/CR2 are configured with SPE = 0 */
    SPIx::_CR1::SPE::clear();

    /* 2. Baud rate: PCLK is a runtime value (RCC may be reconfigured) */
    const uint32_t pclk = spi_on_apb2<SPIx> ? APB2Clock : APB1Clock;
    uint32_t br = 0;
    while (br < SPI_PRESCALER_STEPS && (pclk >> (br + 1)) > cfg.MaxClockHz) ++br;
    if (br == SPI_PRESCALER_STEPS) return DrvStatus::InvalidArg;  // even PCLK/256 is too fast

    /* 3. CR1: master, software NSS held high, clock mode, bit order */
    using CR1 = typename SPIx::_CR1;
    constexpr uint32_t mode = static_cast<uint32_t>(cfg.Mode);
    constexpr uint32_t cr1 = CR1::MSTR::BitMsk | CR1::SSM::BitMsk | CR1::SSI::BitMsk
                           | ((mode >> 1) << CR1::CPOL::pos)
                           | ((mode & 1U) << CR1::CPHA::pos)
                           | (cfg.BitOrder == SpiBitOrder::LsbFirst ? CR1::LSBFIRST::BitMsk : 0U);
    CR1::overwrite(cr1 | (br << CR1::BR::pos));

    /* 4. CR2: frame size; RXNE at 1 byte for frames <= 8 bits (FRXTH) */
    using CR2 = typename SPIx::_CR2;
    constexpr uint32_t cr2 = ((cfg.DataBits - 1U) << CR2::DS::pos)
                           | (cfg.DataBits <= 8 ? CR2::FRXTH::BitMsk : 0U);
    CR2::template overwrite<cr2>();

    /* 5. Enable */
    SPIx::_CR1::SPE::set();

    return DrvStatus::Ok;
}


// RAII chip select around a polling transfer (CS is active low):
//   { SpiSelect<flash_cs> cs; SpiHandle<SPI4>::write(cmd, 4); }
// For DMA transfers drive CS by hand and release it in the on_complete callback.
template<typename CsPin>
struct SpiSelect {
    SpiSelect() noexcept { CsPin::reset(); }
    ~SpiSelect() { CsPin::set(); }
    SpiSelect(const SpiSelect&) = delete;
    SpiSelect& operator=(const SpiSelect&) = delete;
};


template<typename SPIx, typename Word = uint8_t>
struct SpiHandle {
    static_assert(std::is_same_v<Word, uint8_t> || std::is_same_v<Word, uint16_t>,
                  "SPI: Word must be uint8_t (4..8-bit frames) or uint16_t (9..16-bit frames)");

    // Shifted out when there is nothing to send (reads): MOSI stays high.
    static constexpr Word FILL = static_cast<Word>(~Word{0});

    /**
     * Full-duplex polling transfer of len frames.
     * tx == nullptr sends FILL, rx == nullptr discards what comes back.
     * Returns once the bus is idle (BSY = 0), so CS may be released right after.
     */
    static DrvStatus transfer(const Word* tx, Word* rx, size_t len,
                              uint32_t timeout_ms = SPI_TIMEOUT_MS) {
        using SR = typename SPIx::_SR;
        stop_dma_requests();

        const uint32_t tickstart = get_tick();
        size_t sent = 0;
        size_t received = 0;

        while (received < len) {
            const uint32_t sr = SR::read();
            if (sent < len && sent - received < SPI_MAX_IN_FLIGHT && (sr & SR::TXE::BitMsk)) {
                dr_write(tx ? tx[sent] : FILL);
                ++sent;
            }
            if (sr & SR::RXNE::BitMsk) {
                const Word w = dr_read();
                if (rx) rx[received] = w;
                ++received;
                continue;
            }
            if ((get_tick() - tickstart) > timeout_ms) {
                flush_rx();
                return DrvStatus::Timeout;
            }
        }
        return wait_idle(timeout_ms);
    }

    /** Send len frames, discarding the input. */
    static DrvStatus write(const Word* tx, size_t len, uint32_t timeout_ms = SPI_TIMEOUT_MS) {
        return transfer(tx, nullptr, len, timeout_ms);
    }

    /** Receive len frames while clocking out FILL. */
    static DrvStatus read(Word* rx, size_t len, uint32_t timeout_ms = SPI_TIMEOUT_MS) {
        return transfer(nullptr, rx, len, timeout_ms);
    }

    /** One frame each way. */
    static DrvStatus exchange(Word out, Word& in, uint32_t timeout_ms = SPI_TIMEOUT_MS) {
        return transfer(&out, &in, 1, timeout_ms);
    }

    /**
     * Full-duplex DMA transfer of len frames; returns immediately.
     * Completion = the RX stream finishing (the last frame received is the
     * last frame sent): poll hrx.ready() or use hrx.on_complete(cb).
     *
     * Streams are configured by the application with DMA_Config: hrx as
     * PERIPH_TO_MEM, htx as MEM_TO_PERIPH, both without peripheral increment
     * and with PSIZE matching Word. tx and rx may be the same buffer
     * (in-place exchange: each RX write lands behind the TX read).
     * With the D-cache on, buffers must be cleaned / invalidated by the caller.
     */
    template<auto& hrx, auto& htx>
    static DrvStatus transfer_dma(const Word* tx, Word* rx, uint16_t len) {
        if (tx == nullptr || rx == nullptr || len == 0) return DrvStatus::InvalidArg;
        if (!hrx.ready() || !htx.ready()) return DrvStatus::Busy;

        // RM0410 order: RXDMAEN, start both streams, then TXDMAEN
        stop_dma_requests();
        SPIx::_CR2::RXDMAEN::set();

        const uint32_t dr = SPIx::_DR::address;
        DrvStatus st = DMA_StartIT<hrx>(dr, reinterpret_cast<uint32_t>(rx), len);
        if (st != DrvStatus::Ok) return st;

        st = DMA_StartIT<htx>(reinterpret_cast<uint32_t>(tx), dr, len);
        if (st != DrvStatus::Ok) {
            (void)DMA_Stop<hrx.Stream>();
            hrx.state = DmaState::READY;
            return st;
        }

        SPIx::_CR2::TXDMAEN::set();
        return DrvStatus::Ok;
    }

    /** True while a frame is being shifted or the TX FIFO is not empty. */
    static bool busy() noexcept { return SPIx::_SR::BSY::is_set(); }

private:
    [[gnu::always_inline]] static volatile Word* dr() noexcept {
        return reinterpret_cast<volatile Word*>(SPIx::_DR::address);
    }
    [[gnu::always_inline]] static void dr_write(Word w) noexcept { *dr() = w; }
    [[gnu::always_inline]] static Word dr_read() noexcept { return *dr(); }

    static void stop_dma_requests() noexcept {
        SPIx::_CR2::template clearMask<SPIx::_CR2::RXDMAEN::BitMsk | SPIx::_CR2::TXDMAEN::BitMsk>();
    }

    // Drop stale frames (and with them a pending OVR: read DR, then SR).
    static void flush_rx() noexcept {
        while (SPIx::_SR::FRLVL::read() != 0 || SPIx::_SR::RXNE::is_set()) (void)dr_read();
        (void)SPIx::_SR::read();
    }

    static DrvStatus wait_idle(uint32_t timeout_ms) {
        const uint32_t tickstart = get_tick();
        while (SPIx::_SR::FTLVL::read() != 0 || SPIx::_SR::BSY::is_set()) {
            if ((get_tick() - tickstart) > timeout_ms) return DrvStatus::Timeout;
        }
        return DrvStatus::Ok;
    }
};
