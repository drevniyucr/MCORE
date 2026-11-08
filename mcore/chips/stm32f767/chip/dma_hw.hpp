#pragma once
// DMA request routing (DmaRoute, DMA_ApplyRoute) (STM32F767).
#include <cstdint>
#include "core/regs.hpp"
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

// Request routing: fixed stream/channel matrix, DMA_SxCR.CHSEL
using DmaRoute = DmaChannel;

template <typename traits, DmaRoute route>
inline void DMA_ApplyRoute() {
	traits::CR::CHSEL::write(static_cast<uint32_t>(route));
}
