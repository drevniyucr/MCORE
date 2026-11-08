#pragma once
// DMA request routing (DmaRoute, DMA_ApplyRoute) (STM32H755).
#include <cstdint>
#include "core/regs.hpp"
#include "chip/dma_request.hpp"

// Request routing: any stream, the request is picked by DMAMUX1
using DmaRoute = DmaRequest;

template <typename traits, DmaRoute route>
inline void DMA_ApplyRoute() {
	// DMAMUX1 channel n feeds DMA1 stream n (n = 0..7) / DMA2 stream n-8.
	constexpr uintptr_t dmamux_ccr = DMAMUX1::DMAMUX1_BASE + 4U * traits::raw;
	*reinterpret_cast<volatile uint32_t*>(dmamux_ccr) = static_cast<uint32_t>(route);

	// UART/USART streams must use bufferable transfers (DMA_SxCR.TRBUFF).
	constexpr uint32_t DMA_SxCR_TRBUFF = 1U << 20;
	if constexpr (dma_request_is_uart(route)) {
		traits::CR::setMask(DMA_SxCR_TRBUFF);
	} else {
		traits::CR::clearMask(DMA_SxCR_TRBUFF);
	}
}
