/*
 * dma.hpp — DMA stream driver (config, IT transfers, ISR dispatch)
 *
 *   DmaHandle<DmaStream::DMA2_Stream_0> spi4_rx;          // one per stream
 *   DMA_Config<spi4_rx, cfg>();                           // once (or again when idle)
 *   DMA_StartIT<spi4_rx>(src, dst, n);                    // n items of PSIZE
 *   extern "C" void DMA2_Stream0_IRQHandler() { DMA_IRQHandler<spi4_rx>(); }
 *   spi4_rx.on_complete(cb);  spi4_rx.ready();  DMA_Wait<spi4_rx>();
 *   DMA_Stop<spi4_rx>();                                  // abort / leave ERROR
 *
 * State: RESET (not configured) -> READY -> BUSY while the stream runs ->
 * READY at transfer complete, or ERROR on a transfer error (the hardware
 * stops the stream; DMA_Stop or DMA_Config makes it usable again). A circular
 * stream stays BUSY until DMA_Stop: it never completes.
 *
 * D-cache maintenance is done here (see dma_cache_start); a circular receive
 * read between interrupts goes through DMA_InvalidateRx.
 *
 *  Created on: 26 февр. 2026 г.
 *      Author: AkimovMA
 */
#pragma once

#include <cstdint>
#include "core/cache.hpp"
#include "core/regs.hpp"
#include "core/rcc.hpp"
#include "core/system.hpp"
#include "core/mcore_config.hpp"
#include "drivers/common.hpp"


constexpr uint32_t INTRPT_CLR_MASK = 0x3D;
constexpr uint32_t DMA_TIMEOUT_MS = 5;

enum class DmaChannel : uint8_t {
    CHNL_0,
    CHNL_1,
    CHNL_2,
    CHNL_3,
    CHNL_4,
    CHNL_5,
    CHNL_6,
    CHNL_7,
    CHNL_8,
    CHNL_9,
    CHNL_10,
    CHNL_11,
    CHNL_12,
    CHNL_13,
    CHNL_14,
    CHNL_15
};

enum class DmaDir : uint32_t {
    PERIPH_TO_MEM = 0b00,
    MEM_TO_PERIPH = 0b01,
    MEM_MEM = 0b11
};

enum class DmaDataAlign : uint8_t {
    BYTE = 0b00,
    HALFWORD = 0b01,
    WORD = 0b10
};

enum class DmaMode : uint8_t {
    NORMAL = 0b0,
    CIRCULAR = 0b1,
    PERIPH_FLOW_CTRL
};

enum class DmaPrior : uint8_t {
    LOW = 0b00,
    MEDIUM = 0b01,
    HIGH = 0b10,
    VERY_HIGH = 0b11
};

enum class DmaThrhold : uint32_t {
    FULL_1QUATERS = 0b00,
    FULL_HALF = 0b01,
    FULL_3QUATERS = 0b10,
    FULL = 0b11
};

enum class DmaBurst : uint8_t {
    SINGLE = 0b00,
    INC4 = 0b01,
    INC8 = 0b10,
    INC16 = 0b11
};

enum class DmaStream : uint32_t {
    DMA1_Stream_0,
    DMA1_Stream_1,
    DMA1_Stream_2,
    DMA1_Stream_3,
    DMA1_Stream_4,
    DMA1_Stream_5,
    DMA1_Stream_6,
    DMA1_Stream_7,

    DMA2_Stream_0,
    DMA2_Stream_1,
    DMA2_Stream_2,
    DMA2_Stream_3,
    DMA2_Stream_4,
    DMA2_Stream_5,
    DMA2_Stream_6,
    DMA2_Stream_7,
};

// Stream lifecycle STATE (what the handle is doing right now). Operation
// RESULTS are reported separately as DrvStatus. "Half done" is an event
// (on_half), not a state: the stream is still BUSY then.
enum class DmaState : uint8_t {
    RESET,
    READY,
    BUSY,
    ERROR
};

// DmaHandle::errors bits, collected since the last start
namespace dma_err {
constexpr uint8_t Transfer   = 1u << 0;  // bus error: the stream was stopped (state ERROR)
constexpr uint8_t DirectMode = 1u << 1;  // direct mode error: data lost, transfer goes on
constexpr uint8_t Fifo       = 1u << 2;  // FIFO overrun / underrun, transfer goes on
} // namespace dma_err

template<DmaStream S>
struct DmaHandle
{
    static inline volatile DmaState state = DmaState::RESET;
    static inline volatile uint8_t errors = 0;   // dma_err bits since the last start
    static inline const DmaStream Stream = S;
    static inline uint32_t src;
    static inline uint32_t dst;
    static inline uint16_t len;

    static inline void (*tc_callback)() = nullptr;
    static inline void (*ht_callback)() = nullptr;
    static inline void (*err_callback)() = nullptr;

    // --- Async completion API -------------------------------------------
    // Register callbacks fired from the DMA ISR (see DMA_IRQHandler): on
    // transfer-complete (every lap for a circular stream), half-transfer, and
    // transfer error (the stream has stopped; FIFO / direct mode errors only
    // set bits in `errors`). Received data is already visible to the CPU
    // (D-cache invalidated) when they run. Interrupt context: keep them short.
    static void on_complete(void (*cb)()) noexcept { tc_callback = cb; }
    static void on_half(void (*cb)())     noexcept { ht_callback = cb; }
    static void on_error(void (*cb)())    noexcept { err_callback = cb; }

    // --- State queries (so callers need not compare the raw DmaState) ---
    static bool ready()  noexcept { return state == DmaState::READY; }
    static bool busy()   noexcept { return state == DmaState::BUSY; }
    static bool failed() noexcept { return state == DmaState::ERROR; }
};


struct DmaHwConfig {
    DmaChannel Channel;
    DmaDir Direction;
    bool PeriphIncEnable;
    bool MemIncEnable;
    DmaDataAlign PeriphDataAlignment;
    DmaDataAlign MemDataAlignment;
    DmaMode Mode = DmaMode::NORMAL;
    DmaPrior Priority = DmaPrior::LOW;
    bool FIFOModeEnable = false;
    DmaThrhold FIFOThreshold = DmaThrhold::FULL_HALF;
    DmaBurst MemBurst = DmaBurst::SINGLE;
    DmaBurst PeriphBurst = DmaBurst::SINGLE;
};


// Clock enable: unified RccEnable<DMAx> map in core/rcc.hpp.



template<typename DMAx, uint8_t N>
struct StreamRegs;

template<typename DMAx>
struct StreamRegs<DMAx, 0> {
    using CR = typename DMAx::_S0CR;
    using NDTR = typename DMAx::_S0NDTR;
    using PAR = typename DMAx::_S0PAR;
    using M0AR = typename DMAx::_S0M0AR;
    using M1AR = typename DMAx::_S0M1AR;
    using FCR = typename DMAx::_S0FCR;
    using TC = typename DMAx::_LISR::TCIF0;
    using HT = typename DMAx::_LISR::HTIF0;
    using TE = typename DMAx::_LISR::TEIF0;
    using DE = typename DMAx::_LISR::DMEIF0;
    using FE = typename DMAx::_LISR::FEIF0;
};
template<typename DMAx>
struct StreamRegs<DMAx, 1> {
    using CR = typename DMAx::_S1CR;
    using NDTR = typename DMAx::_S1NDTR;
    using PAR = typename DMAx::_S1PAR;
    using M0AR = typename DMAx::_S1M0AR;
    using M1AR = typename DMAx::_S1M1AR;
    using FCR = typename DMAx::_S1FCR;
    using TC = typename DMAx::_LISR::TCIF1;
    using HT = typename DMAx::_LISR::HTIF1;
    using TE = typename DMAx::_LISR::TEIF1;
    using DE = typename DMAx::_LISR::DMEIF1;
    using FE = typename DMAx::_LISR::FEIF1;

};
template<typename DMAx>
struct StreamRegs<DMAx, 2> {
    using CR = typename DMAx::_S2CR;
    using NDTR = typename DMAx::_S2NDTR;
    using PAR = typename DMAx::_S2PAR;
    using M0AR = typename DMAx::_S2M0AR;
    using M1AR = typename DMAx::_S2M1AR;
    using FCR = typename DMAx::_S2FCR;
    using TC = typename DMAx::_LISR::TCIF2;
    using HT = typename DMAx::_LISR::HTIF2;
    using TE = typename DMAx::_LISR::TEIF2;
    using DE = typename DMAx::_LISR::DMEIF2;
    using FE = typename DMAx::_LISR::FEIF2;
};
template<typename DMAx>
struct StreamRegs<DMAx, 3> {
    using CR = typename DMAx::_S3CR;
    using NDTR = typename DMAx::_S3NDTR;
    using PAR = typename DMAx::_S3PAR;
    using M0AR = typename DMAx::_S3M0AR;
    using M1AR = typename DMAx::_S3M1AR;
    using FCR = typename DMAx::_S3FCR;
    using TC = typename DMAx::_LISR::TCIF3;
    using HT = typename DMAx::_LISR::HTIF3;
    using TE = typename DMAx::_LISR::TEIF3;
    using DE = typename DMAx::_LISR::DMEIF3;
    using FE = typename DMAx::_LISR::FEIF3;
};
template<typename DMAx>
struct StreamRegs<DMAx, 4> {
    using CR = typename DMAx::_S4CR;
    using NDTR = typename DMAx::_S4NDTR;
    using PAR = typename DMAx::_S4PAR;
    using M0AR = typename DMAx::_S4M0AR;
    using M1AR = typename DMAx::_S4M1AR;
    using FCR = typename DMAx::_S4FCR;
    using TC = typename DMAx::_HISR::TCIF4;
    using HT = typename DMAx::_HISR::HTIF4;
    using TE = typename DMAx::_HISR::TEIF4;
    using DE = typename DMAx::_HISR::DMEIF4;
    using FE = typename DMAx::_HISR::FEIF4;
};
template<typename DMAx>
struct StreamRegs<DMAx, 5> {
    using CR = typename DMAx::_S5CR;
    using NDTR = typename DMAx::_S5NDTR;
    using PAR = typename DMAx::_S5PAR;
    using M0AR = typename DMAx::_S5M0AR;
    using M1AR = typename DMAx::_S5M1AR;
    using FCR = typename DMAx::_S5FCR;
    using TC = typename DMAx::_HISR::TCIF5;
    using HT = typename DMAx::_HISR::HTIF5;
    using TE = typename DMAx::_HISR::TEIF5;
    using DE = typename DMAx::_HISR::DMEIF5;
    using FE = typename DMAx::_HISR::FEIF5;
};
template<typename DMAx>
struct StreamRegs<DMAx, 6> {
    using CR = typename DMAx::_S6CR;
    using NDTR = typename DMAx::_S6NDTR;
    using PAR = typename DMAx::_S6PAR;
    using M0AR = typename DMAx::_S6M0AR;
    using M1AR = typename DMAx::_S6M1AR;
    using FCR = typename DMAx::_S6FCR;
    using TC = typename DMAx::_HISR::TCIF6;
    using HT = typename DMAx::_HISR::HTIF6;
    using TE = typename DMAx::_HISR::TEIF6;
    using DE = typename DMAx::_HISR::DMEIF6;
    using FE = typename DMAx::_HISR::FEIF6;
};

template<typename DMAx>
struct StreamRegs<DMAx, 7> {
    using CR = typename DMAx::_S7CR;
    using NDTR = typename DMAx::_S7NDTR;
    using PAR = typename DMAx::_S7PAR;
    using M0AR = typename DMAx::_S7M0AR;
    using M1AR = typename DMAx::_S7M1AR;
    using FCR = typename DMAx::_S7FCR;
    using TC = typename DMAx::_HISR::TCIF7;
    using HT = typename DMAx::_HISR::HTIF7;
    using TE = typename DMAx::_HISR::TEIF7;
    using DE = typename DMAx::_HISR::DMEIF7;
    using FE = typename DMAx::_HISR::FEIF7;
};



template<DmaStream S>
struct DmaTraits
{
    static constexpr uint32_t raw = static_cast<uint32_t>(S);

    static constexpr uint32_t dma_index = raw / 8;
    static constexpr uint32_t stream_index = raw % 8;

    using DMAx = std::conditional_t<dma_index == 0, DMA1, DMA2>;

    using ISR = std::conditional_t<
        (stream_index < 4),
        typename DMAx::_LISR,
        typename DMAx::_HISR
        >;

    using IFCR = std::conditional_t<
        (stream_index < 4),
        typename DMAx::_LIFCR,
        typename DMAx::_HIFCR
        >;

    using regs = StreamRegs<DMAx, stream_index>;

    using CR = typename regs::CR;
    using NDTR = typename regs::NDTR;
    using PAR = typename regs::PAR;
    using M0AR = typename regs::M0AR;
    using M1AR = typename regs::M1AR;
    using FCR = typename regs::FCR;
    using TxComplete = typename regs::TC;
    using TxHalf = typename regs::HT;
    using TxError = typename regs::TE;
    using DmError = typename regs::DE;
    using FifoError = typename regs::FE;




    static constexpr uint8_t offset_table[4] = { 0,6,16,22 };
    static constexpr uint8_t intrpt_idx = offset_table[stream_index % 4];
};

// D-cache: memory the stream reads is cleaned before the start; memory it
// writes is cleaned + invalidated before (no dirty line may later be evicted
// over received data) and invalidated again at half / full completion (the
// CPU may have pulled lines in speculatively meanwhile). Nothing happens with
// the cache off or for DTCM. Bytes = NDTR x PSIZE in every direction.
// Receive buffers: alignas(32), see core/cache.hpp.
template<typename traits>
inline void dma_cache_start(uint32_t src, uint32_t dst, uint16_t len)
{
    const uint32_t n = uint32_t{len} << traits::CR::PSIZE::read();
    switch (static_cast<DmaDir>(traits::CR::DIR::read())) {
    case DmaDir::MEM_TO_PERIPH: cache::clean(src, n); break;
    case DmaDir::PERIPH_TO_MEM: cache::clean_invalidate(dst, n); break;
    case DmaDir::MEM_MEM:       cache::clean(src, n); cache::clean_invalidate(dst, n); break;
    }
}

template<typename traits>
inline void dma_cache_received(uint32_t dst, uint16_t len)
{
    if (traits::CR::DIR::read() != static_cast<uint32_t>(DmaDir::MEM_TO_PERIPH)) {
        cache::invalidate(dst, uint32_t{len} << traits::CR::PSIZE::read());
    }
}

template<auto& Handle>
inline bool DMA_IsEnabled()
{
    return DmaTraits<Handle.Stream>::CR::EN::is_set();
}

// Items still to transfer in this pass (NDTR)
template<auto& Handle>
inline uint16_t DMA_Remaining()
{
    return DmaTraits<Handle.Stream>::NDTR::read();
}

// Items transferred so far in this pass; for a circular receive, the index
// the stream writes next
template<auto& Handle>
inline uint16_t DMA_Received()
{
    const uint16_t left = DMA_Remaining<Handle>();
    return left <= Handle.len ? static_cast<uint16_t>(Handle.len - left) : 0;
}

// Make `count` received items from index `first` visible to the CPU (D-cache
// invalidate; wraps around the end of the buffer). Needed only for data read
// before the half / complete interrupt, e.g. a circular UART receive polled
// with DMA_Received(); the interrupt handler does the rest.
template<auto& Handle>
inline void DMA_InvalidateRx(uint16_t first, uint16_t count)
{
    using traits = DmaTraits<Handle.Stream>;
    const uint16_t len = Handle.len;
    if (len == 0 || count == 0) return;
    if (count > len) count = len;
    first %= len;
    const uint32_t shift = traits::CR::PSIZE::read();
    const uint16_t first_part = (first + count > len) ? static_cast<uint16_t>(len - first) : count;
    cache::invalidate(Handle.dst + (uint32_t{first} << shift), uint32_t{first_part} << shift);
    if (first_part < count) cache::invalidate(Handle.dst, uint32_t(count - first_part) << shift);
}

// Wait until the transfer ends: Ok when complete, Error after a transfer
// error, Timeout otherwise (always for a circular stream)
template<auto& Handle>
inline DrvStatus DMA_Wait(uint32_t timeout_ms = DMA_TIMEOUT_MS)
{
    const uint32_t starttick = get_tick();
    while (Handle.state == DmaState::BUSY) {
        if (get_tick() - starttick > timeout_ms) return DrvStatus::Timeout;
    }
    return Handle.state == DmaState::READY ? DrvStatus::Ok : DrvStatus::Error;
}

// Stop the stream (abort a transfer, end a circular one, or leave ERROR) and
// make it READY for the next start with the same configuration
template<auto& Handle>
inline DrvStatus DMA_Stop()
{
    auto& h = Handle;
    using traits = DmaTraits<h.Stream>;

    // interrupts off first: disabling the stream raises TCIF, which must not
    // reach on_complete
    traits::CR::template clearMask<traits::CR::TCIE::BitMsk | traits::CR::HTIE::BitMsk |
                                   traits::CR::TEIE::BitMsk | traits::CR::DMEIE::BitMsk>();
    traits::FCR::FEIE::clear();
    traits::CR::EN::clear();
    const uint32_t starttick = get_tick();
    while (traits::CR::EN::is_set()) {
        if (get_tick() - starttick > DMA_TIMEOUT_MS) return DrvStatus::Timeout;
    }
    traits::IFCR::overwrite(INTRPT_CLR_MASK << traits::intrpt_idx);
    if (h.state != DmaState::RESET) h.state = DmaState::READY;
    return DrvStatus::Ok;
}


template<auto& Handle, DmaHwConfig cfg>
DrvStatus DMA_Config()
{
    auto& h = Handle;
    using traits = DmaTraits<h.Stream>;

    if (h.state == DmaState::BUSY) return DrvStatus::Busy;   // DMA_Stop first


    RccEnable<typename traits::DMAx>::enable();

    traits::CR::EN::clear();
    uint32_t starttick = get_tick();

    while (traits::CR::EN::is_set()) {
        if (get_tick() - starttick > DMA_TIMEOUT_MS) {
            return DrvStatus::Timeout;
        }
    };

    traits::CR::CHSEL::write(static_cast<uint32_t>(cfg.Channel));

    traits::CR::DIR::write(static_cast<uint32_t>(cfg.Direction));
    traits::CR::PL::write(static_cast<uint32_t>(cfg.Priority));

    traits::CR::MINC::write(cfg.MemIncEnable ? 1u : 0u);
    traits::CR::PINC::write(cfg.PeriphIncEnable ? 1u : 0u);

    traits::CR::MSIZE::write(static_cast<uint32_t>(cfg.MemDataAlignment));
    traits::CR::PSIZE::write(static_cast<uint32_t>(cfg.PeriphDataAlignment));

    if constexpr (cfg.Mode != DmaMode::PERIPH_FLOW_CTRL) {
        traits::CR::PFCTRL::clear();
        traits::CR::CIRC::write(cfg.Mode);
    }
    else {
        traits::CR::CIRC::clear();
        traits::CR::PFCTRL::set();
    }

    traits::FCR::DMDIS::write(cfg.FIFOModeEnable ? 1 : 0);

    if constexpr (cfg.FIFOModeEnable) {
        // check fifo param
        if constexpr (cfg.MemBurst != DmaBurst::SINGLE) {
            if constexpr (cfg.MemDataAlignment == DmaDataAlign::BYTE) {
                switch (cfg.FIFOThreshold) {
                case  DmaThrhold::FULL_1QUATERS:
                case  DmaThrhold::FULL_3QUATERS:
                    if constexpr (cfg.MemBurst > DmaBurst::INC4) return DrvStatus::InvalidArg;
                    break;
                case  DmaThrhold::FULL_HALF:
                    if constexpr (cfg.MemBurst == DmaBurst::INC16) return DrvStatus::InvalidArg;
                    break;
                case  DmaThrhold::FULL:
                default:
                    break;
                }
            }

            if constexpr (cfg.MemDataAlignment == DmaDataAlign::HALFWORD) {
                switch (cfg.FIFOThreshold) {
                case DmaThrhold::FULL_1QUATERS:
                case DmaThrhold::FULL_3QUATERS:
                    return DrvStatus::InvalidArg;
                    break;
                case DmaThrhold::FULL_HALF:
                    if constexpr (cfg.MemBurst > DmaBurst::INC4) return DrvStatus::InvalidArg;
                    break;
                case DmaThrhold::FULL:
                    if constexpr (cfg.MemBurst == DmaBurst::INC16) return DrvStatus::InvalidArg;
                    break;
                default:
                    break;
                }
            }

            if constexpr (cfg.MemDataAlignment == DmaDataAlign::WORD) {
                switch (cfg.FIFOThreshold) {
                case DmaThrhold::FULL_1QUATERS:
                case DmaThrhold::FULL_HALF:
                case DmaThrhold::FULL_3QUATERS:
                    return DrvStatus::InvalidArg;
                    break;
                case  DmaThrhold::FULL:
                    if constexpr (cfg.MemBurst > DmaBurst::INC4) return DrvStatus::InvalidArg;
                    break;
                default:
                    break;
                }
            }
        }
        traits::CR::PBURST::write(static_cast<uint32_t>(cfg.PeriphBurst));
        traits::CR::MBURST::write(static_cast<uint32_t>(cfg.MemBurst));
        traits::FCR::FTH::write(static_cast<uint32_t>(cfg.FIFOThreshold));
    }

    traits::IFCR::overwrite(INTRPT_CLR_MASK << traits::intrpt_idx);   // write-only, w1c

    h.state = DmaState::READY;

    return DrvStatus::Ok;
}

// Start a transfer of len items (of PSIZE) with interrupts. src / dst are
// the addresses in transfer direction: for PERIPH_TO_MEM src is the
// peripheral register. Busy while the stream runs, Error after a transfer
// error until DMA_Stop.
template<auto& Handle>
DrvStatus DMA_StartIT(uint32_t src, uint32_t dst, uint16_t len)
{
    auto& h = Handle;
    using traits = DmaTraits<h.Stream>;

    if (len == 0) return DrvStatus::InvalidArg;
    if (h.state == DmaState::ERROR) return DrvStatus::Error;
    if (h.state != DmaState::READY) return DrvStatus::Busy;

    traits::IFCR::overwrite(INTRPT_CLR_MASK << traits::intrpt_idx);

    traits::CR::DBM::clear();
    traits::NDTR::NDT::write(len);
    if (traits::CR::DIR::read() == static_cast<uint32_t>(DmaDir::MEM_TO_PERIPH)) {
        traits::PAR::PA::write(dst);
        traits::M0AR::M0A::write(src);
    } else {                                      // PAR = source for MEM_MEM too
        traits::PAR::PA::write(src);
        traits::M0AR::M0A::write(dst);
    }
    h.src = src;
    h.dst = dst;
    h.len = len;
    h.errors = 0;

    traits::CR::setMask(
        traits::CR::TCIE::BitMsk
        | traits::CR::HTIE::BitMsk
        | traits::CR::TEIE::BitMsk
        | traits::CR::DMEIE::BitMsk
    );
    traits::FCR::FEIE::set();

    dma_cache_start<traits>(src, dst, len);

    h.state = DmaState::BUSY;

    __DSB();
    traits::CR::EN::set();

    return DrvStatus::Ok;
}


template<auto& Handle>
void DMA_IRQHandler()
{
    auto& h = Handle;
    using traits = DmaTraits<h.Stream>;

    const uint32_t flags = traits::ISR::read() & (INTRPT_CLR_MASK << traits::intrpt_idx);
    traits::IFCR::overwrite(flags);

    if (flags & traits::DmError::BitMsk) h.errors |= dma_err::DirectMode;
    if (flags & traits::FifoError::BitMsk) h.errors |= dma_err::Fifo;

    if (flags & traits::TxError::BitMsk)
    {
        h.errors |= dma_err::Transfer;            // the hardware has cleared EN
        h.state = DmaState::ERROR;
        if (h.err_callback)
            h.err_callback();
        return;
    }

    if (flags & (traits::TxHalf::BitMsk | traits::TxComplete::BitMsk))
        dma_cache_received<traits>(h.dst, h.len);

    if (flags & traits::TxHalf::BitMsk)
    {
        if (h.ht_callback)
            h.ht_callback();
    }
    if (flags & traits::TxComplete::BitMsk)
    {
        if (!traits::CR::CIRC::is_set())          // a circular stream runs on
            h.state = DmaState::READY;
        if (h.tc_callback)
            h.tc_callback();
    }
}
