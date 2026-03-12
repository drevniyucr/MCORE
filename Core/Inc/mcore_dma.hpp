/*
 * mcore_usart.hpp
 *
 *  Created on: 26 февр. 2026 г.
 *      Author: AkimovMA
 */
#pragma once

#include <stdint.h>
#include "mcore_regs.hpp"
#include "mcore_system.hpp"
#include "mcore_conf.hpp"


constexpr uint32_t INTRPT_CLR_MASK = 0x3D;
constexpr uint32_t DMA_TIMEOUT = 5;

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

enum class DmaState : uint8_t {
    RESET,
    READY,
    BUSY,
    HALF,
    COMPLETE,
    ERROR
};

template<DmaStream S>
struct DmaHandle
{
    static inline volatile DmaState state = DmaState::RESET;
    static inline const DmaStream Stream = S;
    static inline uint32_t src;
    static inline uint32_t dst;
    static inline uint16_t len;

    static inline void (*tc_callback)() = nullptr;
    static inline void (*ht_callback)() = nullptr;
    static inline void (*err_callback)() = nullptr;
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


template<typename DMA>
inline void DMA_ClkEnable();

template<>
inline void DMA_ClkEnable<DMA1>() { RCC::_AHB1ENR::DMA1EN::set(); }
template<>
inline void DMA_ClkEnable<DMA2>() { RCC::_AHB1ENR::DMA2EN::set(); }



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

template<DmaStream Stream>
static inline bool DMA_IsEnabled()
{
    using traits = DmaTraits<Stream>;
    return traits::CR::EN::is_set();
}

template<DmaStream Stream>
static inline uint16_t DMA_Remaining()
{
    using traits = DmaTraits<Stream>;
    return traits::NDTR::read();
}

template<DmaStream Stream>
static inline DmaState DMA_Wait()
{
    uint32_t starttick = get_tick();
    while (DmaHandle<Stream>::state == DmaState::BUSY) {
        if (get_tick() - starttick > DMA_TIMEOUT) {
            return DmaState::ERROR;
        };
    }
    return DmaHandle<Stream>::state;
}

template<DmaStream Stream>
static inline DmaState DMA_Stop()
{
    DmaTraits<Stream>::CR::EN::clear();
    uint32_t starttick = get_tick();
    while (DmaTraits<Stream>::CR::EN::is_set()) {
        if (get_tick() - starttick > DMA_TIMEOUT) {
            return DmaState::ERROR;
        };
    }
    DmaHandle<Stream>::state = DmaState::RESET;
    return DmaState::COMPLETE;
}


template<auto& Handle, DmaHwConfig cfg>
DmaState DMA_Config()
{
    auto& h = Handle;
    using traits = DmaTraits<h.Stream>;

    if (h.state != DmaState::RESET) return DmaState::ERROR;


    DMA_ClkEnable<typename traits::DMAx>();

    traits::CR::EN::clear();
    uint32_t starttick = get_tick();

    while (traits::CR::EN::is_set()) {
        if (get_tick() - starttick > DMA_TIMEOUT) {
            return DmaState::ERROR;
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
                    if constexpr (cfg.MemBurst > DmaBurst::INC4) return DmaState::ERROR;
                    break;
                case  DmaThrhold::FULL_HALF:
                    if constexpr (cfg.MemBurst == DmaBurst::INC16) return DmaState::ERROR;
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
                    return DmaState::ERROR;
                    break;
                case DmaThrhold::FULL_HALF:
                    if constexpr (cfg.MemBurst > DmaBurst::INC4) return DmaState::ERROR;
                    break;
                case DmaThrhold::FULL:
                    if constexpr (cfg.MemBurst == DmaBurst::INC16) return DmaState::ERROR;
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
                    return DmaState::ERROR;
                    break;
                case  DmaThrhold::FULL:
                    if constexpr (cfg.MemBurst > DmaBurst::INC4) return DmaState::ERROR;
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

    traits::IFCR::setMask(INTRPT_CLR_MASK << traits::intrpt_idx);

    h.state = DmaState::READY;

    return DmaState::COMPLETE;
}

template<auto& Handle>
DmaState DMA_SetTransfer(uint32_t src, uint32_t dst, uint16_t len)
{
    auto& h = Handle;
    using traits = DmaTraits<h.Stream>;

    if (h.state != DmaState::READY) return DmaState::ERROR;

    h.state = DmaState::BUSY;

    traits::CR::DBM::clear();

    traits::NDTR::NDT::write(len);

    if (traits::CR::DIR::read() == static_cast<uint32_t>(DmaDir::MEM_TO_PERIPH))
    {
        traits::PAR::PA::write(dst);
        traits::M0AR::M0A::write(src);
    }
    else
    {
        traits::PAR::PA::write(src);
        traits::M0AR::M0A::write(dst);
    }
    return DmaState::COMPLETE;
}


template<auto& Handle>
DmaState DMA_StartIT(uint32_t ScrAddr, uint32_t DstAddr, uint16_t DataLen) {

    auto& h = Handle;
    using traits = DmaTraits<h.Stream>;

    traits::IFCR::setMask(INTRPT_CLR_MASK << traits::intrpt_idx);

    if (DMA_SetTransfer<Handle>(ScrAddr, DstAddr, DataLen) == DmaState::ERROR)
        return DmaState::ERROR;

    h.src = ScrAddr;
    h.dst = DstAddr;
    h.len = DataLen;

    traits::CR::setMask(
        traits::CR::TCIE::BitMsk
        | traits::CR::HTIE::BitMsk
        | traits::CR::TEIE::BitMsk
        | traits::CR::DMEIE::BitMsk
    );

    traits::FCR::FEIE::set();

    h.state = DmaState::BUSY;

    __DSB();
    traits::CR::EN::set();

    return DmaState::COMPLETE;
}


template<auto& Handle>
void DMA_IRQHandler()
{
    auto& h = Handle;

    using traits = DmaTraits<h.Stream>;

    auto flags = traits::ISR::read();

    traits::IFCR::setMask(INTRPT_CLR_MASK << traits::intrpt_idx);

    if (flags & traits::TxError::BitMsk)
    {
        h.state = DmaState::ERROR;

        if (h.err_callback)
            h.err_callback();
        return;
    }
    if (flags & traits::TxHalf::BitMsk)
    {
        h.state = DmaState::HALF;

        if (h.ht_callback)
            h.ht_callback();
    }
    if (flags & traits::TxComplete::BitMsk)
    {
        h.state = DmaState::READY;

        if (h.tc_callback)
            h.tc_callback();
    }
}
