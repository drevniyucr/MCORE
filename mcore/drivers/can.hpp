/*
 * can.hpp — bxCAN driver (init + polling TX/RX)
 *
 *  Created on: 26 февр. 2026 г.
 *      Author: AkimovMA
 */
#pragma once

#include <cstdint>
#include "core/regs.hpp"
#include "core/rcc.hpp"
#include "core/system.hpp"
#include "core/mcore_config.hpp"
#include "drivers/common.hpp"

constexpr uint8_t CAN_TIMEOUT_MS = 10; // Timeout for CAN initialization in milliseconds


enum class CanMode : uint32_t {
	Normal         = 0,
	Loopback       = CAN1::_BTR::LBKM::BitMsk,
	Silent         = CAN1::_BTR::SILM::BitMsk,
	SilentLoopback = CAN1::_BTR::LBKM::BitMsk | CAN1::_BTR::SILM::BitMsk
};


struct CanHwConfig {
    uint32_t Prescaler = 6;
    uint32_t SyncJumpWidth = 1;
    uint32_t TimeSeg1 = 15;
    uint32_t TimeSeg2 = 2;
    CanMode Mode = CanMode::Normal;
    bool TimeTriggerMode = false;
    bool AutoBusOff = true;
    bool AutoWakeUp = true;
    bool AutoRetransmission = true;
    bool FifoLock = false;
    bool FifoPriority = false;
};

// Clock enable: unified RccEnable<CANx> map in core/rcc.hpp.

template<typename CANx, CanHwConfig cfg>
static inline DrvStatus CAN_Init() {

    uint32_t tickstart;
    uint32_t temp = 0;

	/* 0. Clock enable */
	RccEnable<CANx>::enable();

	/* 1. Enter init mode */
	CANx::_MCR::INRQ::set();

    tickstart = get_tick();

    while (!CANx::_MSR::INAK::is_set()) {
        if ((get_tick() - tickstart) > CAN_TIMEOUT_MS) {
            return DrvStatus::Timeout; // Handle error: CAN initialization timeout
        }
    }

    /* 2. Exit from sleep mode */
	CANx::_MCR::SLEEP::clear();
    
    tickstart = get_tick();

    while (CANx::_MSR::SLAK::is_set()) {
        if ((get_tick() - tickstart) > CAN_TIMEOUT_MS) {
            return DrvStatus::Timeout; // Handle error: CAN initialization timeout
        }
    }
    /* Time-triggered communication mode, automatic bus-off management, 
    automatic wake-up mode, non-automatic retransmission, 
    receive FIFO locked mode, 
    and transmit FIFO priority are configured in the CAN_MCR register.*/
    
    temp = CANx::_MCR::read();

    if constexpr (cfg.TimeTriggerMode) {
          temp |=  CANx::_MCR::TTCM::BitMsk;} 
    else {temp &= ~CANx::_MCR::TTCM::BitMsk;}

    if constexpr (cfg.AutoBusOff) {
          temp |=  CANx::_MCR::ABOM::BitMsk;} 
    else {temp &= ~CANx::_MCR::ABOM::BitMsk;}

    if constexpr (cfg.AutoWakeUp) {
          temp |=  CANx::_MCR::AWUM::BitMsk;} 
    else {temp &= ~CANx::_MCR::AWUM::BitMsk;}

    if constexpr (cfg.AutoRetransmission) {
          temp &= ~CANx::_MCR::NART::BitMsk;} 
    else {temp |=  CANx::_MCR::NART::BitMsk;}

    if constexpr (cfg.FifoLock) {
          temp |=  CANx::_MCR::RFLM::BitMsk;} 
    else {temp &= ~CANx::_MCR::RFLM::BitMsk;}

    if constexpr (cfg.FifoPriority) {
          temp |=  CANx::_MCR::TXFP::BitMsk;} 
    else {temp &= ~CANx::_MCR::TXFP::BitMsk;}
    
    CANx::_MCR::overwrite(temp);

    /* 3. timing*/
    CANx::_BTR::overwrite(
        ( static_cast<uint32_t>(cfg.SyncJumpWidth - 1) << CANx::_BTR::SJW::pos) 
        |(static_cast<uint32_t>(cfg.Prescaler - 1)     << CANx::_BTR::BRP::pos) 
        |(static_cast<uint32_t>(cfg.TimeSeg1 - 1)      << CANx::_BTR::TS1::pos) 
        |(static_cast<uint32_t>(cfg.TimeSeg2 - 1)      << CANx::_BTR::TS2::pos) 
        |static_cast<uint32_t>(cfg.Mode)
    );

    CANx::_MCR::INRQ::clear();
    tickstart = get_tick();

	while (CANx::_MSR::INAK::is_set()) {
        if ((get_tick() - tickstart) > CAN_TIMEOUT_MS) {
            return DrvStatus::Timeout; // Handle error: CAN initialization timeout
        }
    }

    return DrvStatus::Ok;
}


struct CanFrame {
	uint32_t id;      // 11-bit
	uint8_t  dlc;     // 0..8
	uint8_t  data[8];
};


template<typename CANx>
struct CanHandle {

	/** Send a single frame (polling, millisecond timeout) */
	static inline DrvStatus CAN_Send(const CanFrame& f, uint32_t timeout_ms = 100) {

        uint32_t tickstart = get_tick();

		while (!(CANx::_TSR::TME0::is_set())) {
			if ((get_tick() - tickstart) > timeout_ms) return DrvStatus::Timeout;
		}
	 	
        /* 2. Записываем ID (Standard ID, IDE = 0, RTR = 0) */
	    CANx::_TI0R::overwrite(
            CANx::_TI0R::STID::Val2Mask(f.id & 0x7FF)
	    );
                                                               
	    /* 3. DLC */
	    CANx::_TDT0R::overwrite(
            CANx::_TDT0R::DLC::Val2Mask(f.dlc & 0x0F)
	    );

	    /* 4. DATA low */
	    CANx::_TDL0R::overwrite(
            CANx::_TDL0R::DATA0::Val2Mask(f.data[0])
		  | CANx::_TDL0R::DATA1::Val2Mask(f.data[1])
		  | CANx::_TDL0R::DATA2::Val2Mask(f.data[2])
		  | CANx::_TDL0R::DATA3::Val2Mask(f.data[3])
	    );

	    /* 5. DATA high */
	    CANx::_TDH0R::overwrite(
            CANx::_TDH0R::DATA4::Val2Mask(f.data[4])
		  | CANx::_TDH0R::DATA5::Val2Mask(f.data[5])
		  | CANx::_TDH0R::DATA6::Val2Mask(f.data[6])
		  | CANx::_TDH0R::DATA7::Val2Mask(f.data[7])
	    );

	    /* 6. Запрос на передачу */
	    CANx::_TI0R::TXRQ::set();

	return DrvStatus::Ok;
	}

	/** Receive a single frame (polling, millisecond timeout) */
	static inline DrvStatus CAN_Recv(CanFrame& f, uint32_t timeout_ms = 100) {

        uint32_t tickstart = get_tick();

        while (!CANx::_RF0R::FMP0::is_set()) {
            if ((get_tick() - tickstart) > timeout_ms) return DrvStatus::Timeout;
        }
        
        f.id = CANx::_RI0R::STID::read();

        f.dlc = CANx::_RDT0R::DLC::read();

        uint32_t low  = CANx::_RDL0R::read();
        uint32_t high = CANx::_RDH0R::read();

	    f.data[0] = low;
	    f.data[1] = low >> 8;
	    f.data[2] = low >> 16;
	    f.data[3] = low >> 24;
	    f.data[4] = high;
	    f.data[5] = high >> 8;
	    f.data[6] = high >> 16;
	    f.data[7] = high >> 24;

	    CANx::_RF0R::RFOM0::set(); // release FIFO
		return DrvStatus::Ok;
	}
};
