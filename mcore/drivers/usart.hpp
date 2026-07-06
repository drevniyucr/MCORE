/*
 * usart.hpp — U(S)ART driver (init, polling and DMA transfers)
 *
 *  Created on: 26 февр. 2026 г.
 *      Author: AkimovMA
 */
#pragma once

#include <cstdint>
#include <cstddef>
#include "core/regs.hpp"
#include "core/rcc.hpp"
#include "core/system.hpp"
#include "core/mcore_config.hpp"
#include "drivers/common.hpp"
#include "drivers/dma.hpp"

constexpr uint8_t USART_BRR_MIN_OVER16 = 0x10;
constexpr uint8_t USART_BRR_MIN_OVER8 = 0x08;
constexpr uint16_t USART_BRR_MAX = 0xFFFF;
constexpr uint8_t USART_TIMEOUT_MS = 100; // Timeout for USART initialization in milliseconds

enum class UartDataBits : uint32_t {
	Bits7 = USART1::_CR1::M1::BitMsk,
	Bits8 = 0b0,
	Bits9 = USART1::_CR1::M0::BitMsk
};

enum class UartStopBits : uint32_t {
	Stop0_5 = 0b01 << USART1::_CR2::STOP::pos, Stop1 = 0b00
	<< USART1::_CR2::STOP::pos, Stop1_5 = 0b11
	<< USART1::_CR2::STOP::pos, Stop2 = 0b10 << USART1::_CR2::STOP::pos
};

enum class UartParityMode : uint32_t {
	None = 0b0 << USART1::_CR1::PCE::pos,
	Even = USART1::_CR1::PCE::BitMsk,
	Odd = USART1::_CR1::PS::BitMsk | USART1::_CR1::PCE::BitMsk
};

enum class UartDirection : uint32_t {
	Rx = USART1::_CR1::RE::BitMsk,
	Tx = USART1::_CR1::TE::BitMsk,
	TxRx = USART1::_CR1::RE::BitMsk | USART1::_CR1::TE::BitMsk
};

enum class UartClkPol : uint32_t {
	IdleLow = 0b0, IdleHigh = USART1::_CR2::CPOL::BitMsk
};
enum class UartClkPh : uint32_t {
	FirstEdge = 0b0, SecondEdge = USART1::_CR2::CPHA::BitMsk
};
enum class UartClkLB : uint32_t {
	Disable = 0b0, Enable = USART1::_CR2::LBCL::BitMsk
};
enum class UartOverSamp : uint32_t {
	Over16 = 0b0, Over8 = USART1::_CR1::OVER8::BitMsk
};
enum class UartClockSrc : uint32_t {
	PCLK = 0b00, SYSCLK = 0b01, HSI = 0b10, LSE = 0b11
};

enum class UartFlowControl : uint32_t {
	None = 0b00,
	RTS = USART1::_CR3::RTSE::BitMsk,
	CTS = USART1::_CR3::CTSE::BitMsk,
	RTS_CTS = USART1::_CR3::RTSE::BitMsk | USART1::_CR3::CTSE::BitMsk
};

enum class UartBitSampling : uint32_t {
	ThreeSample = 0b0, OneSample = USART1::_CR3::ONEBIT::BitMsk
};
enum class UartAutoBaudMode : uint32_t {
	Disabled = 0b00,
	EdgeToEdge = USART1::_CR2::ABRMOD0::BitMsk,
	detect_0x75 = USART1::_CR2::ABRMOD1::BitMsk,
	detect_0x55 = USART1::_CR2::ABRMOD0::BitMsk | USART1::_CR2::ABRMOD1::BitMsk
};

struct UartHwConfig {
	uint32_t Baud = 115200;               								// 9600, 115200, ...
	UartDataBits DataBits = UartDataBits::Bits8;           				// 7, 8, 9 bits
	UartStopBits StopBits = UartStopBits::Stop1;           				// 1, 2
	UartParityMode Parity = UartParityMode::None;             			// none, even, odd
	UartDirection Direction = UartDirection::TxRx;          			// tx, rx, tx_rx
	UartClkPol CLKPolarity = UartClkPol::IdleLow;        				// idle low/high
	UartClkPh CLKPhase = UartClkPh::FirstEdge;           				// first/second edge
	UartClkLB CLKLastBit = UartClkLB::Disable;         					// disable/enable
	UartOverSamp OverSampling = UartOverSamp::Over16;        			// oversampling by 16 or 8
	UartClockSrc ClockSource = UartClockSrc::PCLK;         				// PCLK, SYSCLK, HSI, LSE
	UartFlowControl FlowControl = UartFlowControl::None;                // none, RTS, CTS, RTS/CTS
	UartBitSampling BitSamplingMethod = UartBitSampling::ThreeSample;   // 1 sample bit, 3 sample bits
	UartAutoBaudMode AutoBaudMode = UartAutoBaudMode::Disabled;         // auto baud rate mode
	uint8_t AssertTime = 0;          // DE assertion time (0-31)
	uint8_t DeAssertTime = 0;        // DE de-assertion time (0-31)
	bool UseDMARx = false;
	bool UseDMATx = false;
	bool EnableDE = false; 		      // whether to enable Driver Enable mode
	bool DEPolarityHigh = false;      // DE polarity: true for active high, false for active low
};

// Kernel clock SOURCE selection (DKCFGR2). Bus clock enable is the unified
// RccEnable<USARTx> map in core/rcc.hpp.
template<typename USARTx>
static inline void USART_ClkSrcSel(UartClockSrc src);
template <>
inline void USART_ClkSrcSel<USART1>(UartClockSrc src) {
	RCC::_DKCFGR2::USART1SEL::write(static_cast<uint32_t>(src));
}
template <>
inline void USART_ClkSrcSel<USART2>(UartClockSrc src) {
	RCC::_DKCFGR2::USART2SEL::write(static_cast<uint32_t>(src));
}
template <>
inline void USART_ClkSrcSel<USART3>(UartClockSrc src) {
	RCC::_DKCFGR2::USART3SEL::write(static_cast<uint32_t>(src));
}
template <>
inline void USART_ClkSrcSel<USART6>(UartClockSrc src) {
	RCC::_DKCFGR2::USART6SEL::write(static_cast<uint32_t>(src));
}
template <>
inline void USART_ClkSrcSel<UART4>(UartClockSrc src) {
	RCC::_DKCFGR2::UART4SEL::write(static_cast<uint32_t>(src));
}
template <>
inline void USART_ClkSrcSel<UART5>(UartClockSrc src) {
	RCC::_DKCFGR2::UART5SEL::write(static_cast<uint32_t>(src));
}
template <>
inline void USART_ClkSrcSel<UART7>(UartClockSrc src) {
	RCC::_DKCFGR2::UART7SEL::write(static_cast<uint32_t>(src));
}
template <>
inline void USART_ClkSrcSel<UART8>(UartClockSrc src) {
	RCC::_DKCFGR2::UART8SEL::write(static_cast<uint32_t>(src));
}

template <typename T>
struct is_full_usart : std::false_type {};

template <> struct is_full_usart<USART1> : std::true_type {};
template <> struct is_full_usart<USART2> : std::true_type {};
template <> struct is_full_usart<USART3> : std::true_type {};
template <> struct is_full_usart<USART6> : std::true_type {};

template <typename T>
struct usart_bus;

template <> struct usart_bus<USART1> { static constexpr bool apb2 = true; };
template <> struct usart_bus<USART6> { static constexpr bool apb2 = true; };

template <> struct usart_bus<USART2> { static constexpr bool apb2 = false; };
template <> struct usart_bus<USART3> { static constexpr bool apb2 = false; };
template <> struct usart_bus<UART4> { static constexpr bool apb2 = false; };
template <> struct usart_bus<UART5> { static constexpr bool apb2 = false; };
template <> struct usart_bus<UART7> { static constexpr bool apb2 = false; };
template <> struct usart_bus<UART8> { static constexpr bool apb2 = false; };


template<typename USARTx, UartHwConfig cfg>
static inline DrvStatus USART_Init() {
	/* 0. Clock enable + kernel clock source */
	RccEnable<USARTx>::enable();
	USART_ClkSrcSel<USARTx>(cfg.ClockSource);

	/* 1. Disable USART */
	USARTx::_CR1::UE::clear();
	/* 2. CR1: word length, parity, oversampling */
	USARTx::_CR1::overwrite(
		static_cast<uint32_t>(cfg.OverSampling) |
		static_cast<uint32_t>(cfg.DataBits) |
		static_cast<uint32_t>(cfg.Parity) |
		(static_cast<uint32_t>(cfg.AssertTime & 0x1F) << USARTx::_CR1::DEAT0::pos) |
		(static_cast<uint32_t>(cfg.DeAssertTime & 0x1F) << USARTx::_CR1::DEDT0::pos)
	);

	/* 3. CR2: CPOL,CPHA,LBCL,STOP bits */
	USARTx::_CR2::clear();

	if constexpr (is_full_usart<USARTx>::value) {
		USARTx::_CR2::overwrite(
			static_cast<uint32_t>(cfg.AutoBaudMode) |
			static_cast<uint32_t>(cfg.CLKPolarity) |
			static_cast<uint32_t>(cfg.CLKLastBit) |
			static_cast<uint32_t>(cfg.CLKPhase) |
			static_cast<uint32_t>(cfg.StopBits)
		);
	}
	else {
		USARTx::_CR2::overwrite(
			static_cast<uint32_t>(cfg.AutoBaudMode) |
			static_cast<uint32_t>(cfg.StopBits));
	}

	if (cfg.AutoBaudMode != UartAutoBaudMode::Disabled) {
		USARTx::_CR2::ABREN::set();
	}
	/* 4. CR3:HardWare flow control, bit sampling */
	USARTx::_CR3::clear();
	USARTx::_CR3::overwrite(
		(static_cast<uint32_t>(cfg.DEPolarityHigh) << USARTx::_CR3::DEP::pos) |
		(static_cast<uint32_t>(cfg.EnableDE) << USARTx::_CR3::DEM::pos) |
		(static_cast<uint32_t>(cfg.UseDMATx) << USARTx::_CR3::DMAT::pos) |
		(static_cast<uint32_t>(cfg.UseDMARx) << USARTx::_CR3::DMAR::pos) |
		static_cast<uint32_t>(cfg.BitSamplingMethod) |
		static_cast<uint32_t>(cfg.FlowControl));

	/* 5. BRR:*/
	uint32_t UsartDiv = 0;
	uint16_t brrtemp = 0;
	uint32_t FreqMult = 1;
	uint32_t pclk = 0;

	if constexpr (usart_bus<USARTx>::apb2) {
		pclk = APB2Clock;
	}
	else {
		pclk = APB1Clock;
	}

	FreqMult = (cfg.OverSampling == UartOverSamp::Over8) ? 2U : 1U;

	switch (cfg.ClockSource) {
	case UartClockSrc::PCLK:
		UsartDiv = (pclk * FreqMult + cfg.Baud / 2) / cfg.Baud;
		break;
	case UartClockSrc::SYSCLK:
		UsartDiv = (SystemCoreClock * FreqMult + cfg.Baud / 2) / cfg.Baud;
		break;
	case UartClockSrc::HSI:
		UsartDiv = (HSI_VALUE * FreqMult + cfg.Baud / 2) / cfg.Baud;
		break;
	case UartClockSrc::LSE:
		UsartDiv = (LSE_VALUE * FreqMult + cfg.Baud / 2) / cfg.Baud;
		break;
	default:
		break;
	}

	if (UsartDiv <= USART_BRR_MAX) {
		if ((cfg.OverSampling == UartOverSamp::Over8) && (UsartDiv >= USART_BRR_MIN_OVER8)) {
			brrtemp = static_cast <uint16_t>(UsartDiv & 0xFFF0U);
			brrtemp |= static_cast<uint16_t>((UsartDiv & 0x000FU) >> 1U);
			USARTx::_BRR::overwrite(brrtemp);
		}
		else if ((cfg.OverSampling == UartOverSamp::Over16) && (UsartDiv >= USART_BRR_MIN_OVER16)) {
			USARTx::_BRR::overwrite(static_cast<uint16_t>(UsartDiv));
		}
		else {
			return DrvStatus::InvalidArg;	// Handle error: baud rate out of range
		}
	}
	else {
		return DrvStatus::InvalidArg;	// Handle error: baud rate out of range
	}
	/* 6. Enable USART */
	USARTx::_CR1::UE::set();
	USARTx::_CR1::TE::set();
	USARTx::_CR1::RE::set();
	uint32_t tickstart = get_tick();
	while (!USARTx::_ISR::TEACK::is_set()) {
		if ((get_tick() - tickstart) > USART_TIMEOUT_MS) {
			return DrvStatus::Timeout; // Handle error: USART initialization timeout
		}
	}
	tickstart = get_tick();
	while (!USARTx::_ISR::REACK::is_set()) {
		if ((get_tick() - tickstart) > USART_TIMEOUT_MS) {
			return DrvStatus::Timeout; // Handle error: USART initialization timeout
		}
	}

	return DrvStatus::Ok;
}


template<typename USARTx>
struct UartHandle {

	template<auto& hdma>
	static inline DrvStatus UartTxDMA(uint8_t* pData, uint16_t Size) {

		auto& h = hdma;
		if (h.state != DmaState::READY)
			return DrvStatus::Busy;

		return DMA_StartIT<hdma>(reinterpret_cast<uint32_t>(pData), USARTx::_TDR::address , Size);
	}

	template<auto& hdma>
	static inline DrvStatus UartRxDMA(uint8_t* pData, uint16_t Size) {

		auto& h = hdma;
		if (h.state != DmaState::READY)
			return DrvStatus::Busy;

		return DMA_StartIT<hdma>(USARTx::_RDR::address, reinterpret_cast<uint32_t>(pData), Size);
	}

	/** Send a single byte (polling, millisecond timeout) */
	static inline DrvStatus sendByte(uint8_t byte, uint32_t timeout_ms = USART_TIMEOUT_MS) {
		uint32_t tickstart = get_tick();
		while (!USARTx::_ISR::TXE::is_set()) {
			if ((get_tick() - tickstart) > timeout_ms) return DrvStatus::Timeout;
			if (checkErrors()) return DrvStatus::Error;
		}
		USARTx::_TDR::overwrite(byte);
		return DrvStatus::Ok;
	}

	/** Send a buffer (polling, per-byte millisecond timeout) */
	static inline DrvStatus sendBuffer(const uint8_t* buf, size_t len, uint32_t timeout_ms = USART_TIMEOUT_MS) {
		for (size_t i = 0; i < len; ++i) {
			auto status = sendByte(buf[i], timeout_ms);
			if (status != DrvStatus::Ok) return status;
		}
		return DrvStatus::Ok;
	}

	/** Receive a single byte (polling, millisecond timeout) */
	static inline DrvStatus recvByte(uint8_t& byte, uint32_t timeout_ms = USART_TIMEOUT_MS) {
		uint32_t tickstart = get_tick();
		while (!USARTx::_ISR::RXNE::is_set()) {
			if ((get_tick() - tickstart) > timeout_ms) return DrvStatus::Timeout;
			if (checkErrors()) return DrvStatus::Error;
		}
		byte = static_cast<uint8_t>(USARTx::_RDR::read());
		return DrvStatus::Ok;
	}

	/** Receive a buffer (polling, per-byte millisecond timeout) */
	static inline DrvStatus recvBuffer(uint8_t* buf, size_t len, uint32_t timeout_ms = USART_TIMEOUT_MS) {
		for (size_t i = 0; i < len; ++i) {
			auto status = recvByte(buf[i], timeout_ms);
			if (status != DrvStatus::Ok) return status;
		}
		return DrvStatus::Ok;
	}

private:
	/** Check hardware errors (ORE, FE, PE) */
	static inline bool checkErrors() {
		bool error = false;
		if (USARTx::_ISR::ORE::is_set()) {
			USARTx::_ICR::ORECF::set(); // clear overrun
			error = true;
		}
		if (USARTx::_ISR::FE::is_set()) {
			USARTx::_ICR::FECF::set();  // clear framing
			error = true;
		}
		if (USARTx::_ISR::PE::is_set()) {
			USARTx::_ICR::PECF::set();  // clear parity
			error = true;
		}
		return error;
	}
};
