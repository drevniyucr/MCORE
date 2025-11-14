/*
 * mcore_eth.cpp
 *
 *  Created on: Sep 18, 2025
 *      Author: AkimovMA
 */

#include "mcore_eth.hpp"
#include "mcore_conf.hpp"
#include "mcore_system.hpp"
#include "mcore_tcp.hpp"
#include "mcore_net.hpp"
#include "mcore_nvic.hpp"
#include "mcore_rcc.hpp"

namespace {
	// Ethernet DMA descriptor tables (placed in specific memory sections for MPU)
	static ETH_DMADescStruct DMARxDscrTab[ETH_RX_DESC_CNT] 
		__attribute__((section(".RxDecripSection")));
	
	static ETH_DMADescStruct DMATxDscrTab[ETH_TX_DESC_CNT] 
		__attribute__((section(".TxDecripSection")));
	
	// Ethernet receive buffers (placed in specific memory section for MPU)
	static uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE] 
		__attribute__((section(".RxPoolSection")));
	
	// Ethernet transmit buffers (aligned for DMA)
	static uint8_t Tx_Buff[ETH_TX_DESC_CNT][ETH_MAX_PACKET_SIZE] 
		__attribute__((section(".TxPoolSection")));
	
	// Constants
	constexpr uint32_t ETH_RESET_TIMEOUT_MS = 10;
	constexpr uint32_t PHY_RESET_TIMEOUT_MS = 1000;
	constexpr uint32_t PHY_STABILIZATION_DELAY_MS = 40;
	constexpr uint32_t PHY_AUTONEG_TIMEOUT_MS = 5000;
	constexpr uint32_t PHY_LINK_TIMEOUT_MS = 5000;
	constexpr uint32_t PHY_MDIO_TIMEOUT_MS = 50;
	constexpr uint32_t ETH_INIT_DELAY_MS = 1;
	
	// LAN8742A PHY ID values
	constexpr uint32_t PHY_ID1_EXPECTED = 0x0007;
	constexpr uint32_t PHY_ID2_MASK = 0xFFF0;
	constexpr uint32_t PHY_ID2_EXPECTED = 0xC130;
	
	// MAC Address (example - should be configured per device)
	constexpr uint8_t MAC_ADDRESS[6] = {0x02, 0x12, 0x34, 0x56, 0x78, 0x9A};
	
	// Frame size limits
	constexpr uint16_t ETH_MIN_FRAME_SIZE = 60;   // Minimum Ethernet frame (with padding)
	constexpr uint16_t ETH_MAX_FRAME_SIZE = 1518; // Maximum Ethernet frame
	constexpr uint16_t ETH_MIN_PAYLOAD_SIZE = 46; // Minimum payload (without header)
}

// Global descriptor lists
ETH_RxDescListStruct RxDescList;
ETH_TxDescListStruct TxDescList;

// Event flags (set by interrupt, cleared by worker)
volatile uint32_t eth_rx_event = 0;
volatile uint32_t eth_tx_event = 0;

// Statistics (optional - for debugging/monitoring)
namespace {
	volatile uint32_t rx_frames_processed = 0;
	volatile uint32_t rx_frames_dropped = 0;
	volatile uint32_t tx_frames_sent = 0;
	volatile uint32_t tx_frames_dropped = 0;
}

ETH_Status ETH_Init() {
	uint32_t tickstart;
	
	// Enable Ethernet interface clock
	enableEthInterface();
	delay_ms(ETH_INIT_DELAY_MS);
	
	// Enable SYSCFG clock for RMII selection
	RCC::_APB2ENR::SYSCFGEN::set();
	RCC::_APB2ENR::SYSCFGEN::read();  // Read to ensure write completed
	
	// Select RMII interface (required for external PHY)
	SYSCFG::_PMC::MII_RMII_SEL::set();
	SYSCFG::_PMC::MII_RMII_SEL::read();  // Read to ensure write completed
	
	// Ethernet Software reset
	Ethernet_DMA::_DMABMR::SR::set();
	tickstart = get_tick();
	
	while (Ethernet_DMA::_DMABMR::SR::read()) {
		if ((get_tick() - tickstart) > ETH_RESET_TIMEOUT_MS) {
			return ETH_Status::RESET_FAULT;
		}
	}
	
	// Configure MDIO clock based on system clock
	PHY_SetMDIOClock();
	
	// Initialize PHY (autonegotiation, link detection)
	ETH_Status status = PHY_Init();
	if (status != ETH_Status::OK) {
		return status;
	}
	
	// MAC Control Register configuration
	Ethernet_MAC::_MACCR::overwrite(
		Ethernet_MAC::_MACCR::CSTF::BitMsk |  // Strip CRC from Type frames
		(static_cast<uint32_t>(MACCR_IFG::_96bit) 
		 << Ethernet_MAC::_MACCR::IFG::pos) |  // 96-bit interframe gap (Ethernet standard)
		Ethernet_MAC::_MACCR::FES::BitMsk |   // Fast Ethernet 100 Mbps
		Ethernet_MAC::_MACCR::DM::BitMsk |    // Full-duplex mode
		Ethernet_MAC::_MACCR::IPCO::BitMsk |  // Hardware IPv4 checksum offload
		Ethernet_MAC::_MACCR::RD::BitMsk      // Retry disable (safe in full-duplex)
	);
	
	delay_ms(ETH_INIT_DELAY_MS);
	
	// MAC Flow Control Register configuration
	Ethernet_MAC::_MACFCR::overwrite(
		Ethernet_MAC::_MACFCR::ZQPD::BitMsk |  // Ignore PAUSE frames with zero pause
		(static_cast<uint32_t>(MACFCR_PLT::Minus4) 
		 << Ethernet_MAC::_MACFCR::PLT::pos)   // Pause time threshold
	);
	
	delay_ms(ETH_INIT_DELAY_MS);
	
	// DMA Operation Mode Register configuration
	Ethernet_DMA::_DMAOMR::bitSet<
		Ethernet_DMA::_DMAOMR::RSF,  // Receive Store and Forward
		Ethernet_DMA::_DMAOMR::TSF,  // Transmit Store and Forward
		Ethernet_DMA::_DMAOMR::FUGF, // Forward undersized good frames
		Ethernet_DMA::_DMAOMR::OSF   // Operate on second frame
	>();
	
	delay_ms(ETH_INIT_DELAY_MS);
	
	// MAC Address configuration
	// Pack MAC address into high and low registers
	const uint32_t MAC_ADDR_HR = (MAC_ADDRESS[5] << 8) | MAC_ADDRESS[4];
	const uint32_t MAC_ADDR_LR = (MAC_ADDRESS[3] << 24) | (MAC_ADDRESS[2] << 16) |
	                             (MAC_ADDRESS[1] << 8)  | MAC_ADDRESS[0];
	
	Ethernet_MAC::_MACA0HR::MACA0H::write(MAC_ADDR_HR);
	Ethernet_MAC::_MACA0LR::MACA0L::write(MAC_ADDR_LR);
	
	// DMA Bus Mode Register configuration
	Ethernet_DMA::_DMABMR::setMask(
		Ethernet_DMA::_DMABMR::AAB::BitMsk |   // Address-aligned beats
		Ethernet_DMA::_DMABMR::FB::BitMsk |    // Fixed burst
		(static_cast<uint32_t>(DMABMR_PBL::_32Beats) 
		 << Ethernet_DMA::_DMABMR::RDP::pos) | // Rx DMA PBL
		(static_cast<uint32_t>(DMABMR_PBL::_32Beats) 
		 << Ethernet_DMA::_DMABMR::PBL::pos) | // Programmable burst length
		Ethernet_DMA::_DMABMR::EDFE::BitMsk |  // Enhanced descriptor format enable
		Ethernet_DMA::_DMABMR::DA::BitMsk |    // Descriptor skip length
		Ethernet_DMA::_DMABMR::USP::BitMsk     // Use separate PBL
	);
	
	delay_ms(ETH_INIT_DELAY_MS);
	
	// Initialize descriptor chains
	ETH_RxDescInit();
	ETH_TxDescInit();
	
	// Disable MMC (Management Counters) interrupts
	Ethernet_MAC::_MACIMR::bitSet<
		Ethernet_MAC::_MACIMR::TSTIM,  // Time stamp trigger interrupt mask
		Ethernet_MAC::_MACIMR::PMTIM   // PMT interrupt mask
	>();
	
	// Disable Rx MMC interrupts
	Ethernet_MMC::_MMCRIMR::bitSet<
		Ethernet_MMC::_MMCRIMR::RGUFM,  // Rx good unicast frames mask
		Ethernet_MMC::_MMCRIMR::RFAEM,  // Rx alignment error mask
		Ethernet_MMC::_MMCRIMR::RFCEM   // Rx CRC error mask
	>();
	
	// Disable Tx MMC interrupts
	Ethernet_MMC::_MMCTIMR::bitSet<
		Ethernet_MMC::_MMCTIMR::TGFM,     // Tx good frames mask
		Ethernet_MMC::_MMCTIMR::TGFMSCM,  // Tx good frames more than single collision mask
		Ethernet_MMC::_MMCTIMR::TGFSCM   // Tx good frames single collision mask
	>();
	
	// Enable DMA interrupts (Normal interrupt summary, Rx, Tx)
	Ethernet_DMA::_DMAIER::bitSet<
		Ethernet_DMA::_DMAIER::NISE,  // Normal interrupt summary enable
		Ethernet_DMA::_DMAIER::RIE,   // Receive interrupt enable
		Ethernet_DMA::_DMAIER::TIE    // Transmit interrupt enable
	>();
	
	delay_ms(ETH_INIT_DELAY_MS);
	
	// Enable MAC transmitter and receiver
	Ethernet_MAC::_MACCR::bitSet<
		Ethernet_MAC::_MACCR::TE,  // Transmit enable
		Ethernet_MAC::_MACCR::RE    // Receive enable
	>();
	
	delay_ms(ETH_INIT_DELAY_MS);
	
	// Flush transmit FIFO
	Ethernet_DMA::_DMAOMR::FTF::set();
	delay_ms(ETH_INIT_DELAY_MS);
	Ethernet_DMA::_DMAOMR::FTF::clear();
	
	// Enable DMA transmission and reception
	Ethernet_DMA::_DMAOMR::ST::set();  // Start transmission
	Ethernet_DMA::_DMAOMR::SR::set();  // Start reception
	
	// Configure and enable Ethernet interrupt
	NVIC_API::SetPriority<IRQn_Type::ETH_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::ETH_IRQn>();
	
	return ETH_Status::OK;
}

void ETH_RxDescInit() {
	for (uint32_t i = 0; i < ETH_RX_DESC_CNT; i++) {
		// Initialize descriptor: owned by DMA, chained, buffer size
		DMARxDscrTab[i].DESC0 = ETH_DMARXDESC_OWN;
		DMARxDscrTab[i].DESC1 = ETH_MAX_PACKET_SIZE | ETH_DMARXDESC_RCH;
		DMARxDscrTab[i].DESC2 = reinterpret_cast<uint32_t>(&Rx_Buff[i][0]);
		// Chain to next descriptor (circular)
		DMARxDscrTab[i].DESC3 = reinterpret_cast<uint32_t>(
			&DMARxDscrTab[(i + 1) % ETH_RX_DESC_CNT]);
		RxDescList.RxDesc[i] = &DMARxDscrTab[i];
	}
	
	// Initialize descriptor list state
	RxDescList.CurrDescNum = 0;
	RxDescList.CurrRxDesc = &DMARxDscrTab[0];
	RxDescList.pBuff = &Rx_Buff[0][0];
	
	// Set DMA receive descriptor list address register
	Ethernet_DMA::_DMARDLAR::overwrite(
		reinterpret_cast<uint32_t>(&DMARxDscrTab[0]));
}

void ETH_TxDescInit() {
	for (uint32_t i = 0; i < ETH_TX_DESC_CNT; i++) {
		// Initialize descriptor: chained, no buffer size yet
		DMATxDscrTab[i].DESC0 = ETH_DMATXDESC_TCH;
		DMATxDscrTab[i].DESC1 = 0;
		DMATxDscrTab[i].DESC2 = reinterpret_cast<uint32_t>(&Tx_Buff[i][0]);
		// Chain to next descriptor (circular)
		DMATxDscrTab[i].DESC3 = reinterpret_cast<uint32_t>(
			&DMATxDscrTab[(i + 1) % ETH_TX_DESC_CNT]);
		TxDescList.TxDesc[i] = &DMATxDscrTab[i];
	}
	
	// Initialize descriptor list state
	TxDescList.CurrHeadNum = 0;
	TxDescList.CurrTailNum = 0;
	TxDescList.CurrTxDesc = &DMATxDscrTab[0];
	TxDescList.pBuff = &Tx_Buff[0][0];
	
	// Set DMA transmit descriptor list address register
	Ethernet_DMA::_DMATDLAR::overwrite(
		reinterpret_cast<uint32_t>(&DMATxDscrTab[0]));
}

extern "C" void ETH_IRQHandler(void) {
	//const uint32_t DMASR = Ethernet_DMA::_DMASR::read();
	//const uint32_t DMAIER = Ethernet_DMA::_DMAIER::read();
	
	// Bit masks for interrupt checking
	constexpr uint32_t RS = Ethernet_DMA::_DMASR::RS::BitMsk;   // Receive status
	//constexpr uint32_t RIE = Ethernet_DMA::_DMAIER::RIE::BitMsk; // Receive interrupt enable
	constexpr uint32_t TS = Ethernet_DMA::_DMASR::TS::BitMsk;   // Transmit status
	//constexpr uint32_t TIE = Ethernet_DMA::_DMAIER::TIE::BitMsk; // Transmit interrupt enable
	constexpr uint32_t NIS = Ethernet_DMA::_DMASR::NIS::BitMsk; // Normal interrupt summary
	
	// TX interrupt handling
	if (Ethernet_DMA::_DMASR::TS::is_set()) {
		Ethernet_DMA::_DMASR::setMask(TS | NIS);  // Clear status bits

		eth_tx_event = 1;  // Signal TX completion
	}
	// RX interrupt handling
	if (Ethernet_DMA::_DMASR::RS::is_set()) {
		Ethernet_DMA::_DMASR::setMask(RS | NIS);  // Clear status bits
		 // Продвигаем tail безопасно
		uint8_t tail = TxDescList.CurrTailNum;
		uint8_t head = TxDescList.CurrHeadNum;
        // Продвигаем tail пока OWN=0 и tail != head
		while (tail != head && !(TxDescList.TxDesc[tail]->DESC0 & ETH_DMATXDESC_OWN)) {
			tail = (tail + 1) % ETH_TX_DESC_CNT;
		}
		TxDescList.CurrTailNum = tail;
		eth_tx_event = 1;
	}
}

void ETH_RxWorker() {
	
	if (eth_rx_event == 0) {
			return;  // No RX event to process
		}

	eth_rx_event = 0;  // Clear event flag
	uint32_t idx = RxDescList.CurrDescNum;
	uint32_t frames_processed = 0;
	constexpr uint32_t MAX_FRAMES_PER_CALL = ETH_RX_DESC_CNT;  // Process all available
	
	// Process all available received frames (limit to prevent starvation)
	while (frames_processed < MAX_FRAMES_PER_CALL) {
		const uint32_t desc0 = DMARxDscrTab[idx].DESC0;
		
		// If descriptor is still owned by DMA, we've processed all available frames
		if (desc0 & ETH_DMARXDESC_OWN) {
			break;
		}
		
		// Extract frame length early for validation
		const uint16_t frame_len = static_cast<uint16_t>((desc0 & ETH_DMARXDESC_FL) >> 16);
		
		// Validate frame length
		if (frame_len < ETH_MIN_FRAME_SIZE || frame_len > ETH_MAX_FRAME_SIZE) {
			// Invalid frame size - drop it
			rx_frames_dropped += 1;
			DMARxDscrTab[idx].DESC0 = ETH_DMARXDESC_OWN;
			__DSB();
			Ethernet_DMA::_DMARPDR::overwrite(0U);
			idx = (idx + 1) % ETH_RX_DESC_CNT;
			frames_processed++;
			continue;
		}
		
		// Check if this is a complete frame in a single descriptor (First + Last segment)
		// and has no errors
		const bool is_complete_frame = 
			(desc0 & (ETH_DMARXDESC_FS | ETH_DMARXDESC_LS)) == 
			(ETH_DMARXDESC_FS | ETH_DMARXDESC_LS);
		const bool has_no_errors = !(desc0 & ETH_DMARXDESC_ES);
		
		if (is_complete_frame && has_no_errors) {
			// Extract frame length and buffer pointer
			RxDescList.BuffLen = frame_len;
			RxDescList.pBuff = reinterpret_cast<uint8_t*>(DMARxDscrTab[idx].DESC2);
			RxDescList.CurrRxDesc = &DMARxDscrTab[idx];
			
			// Process the received packet
			NET_ProcessRx(&RxDescList, &TxDescList);
			rx_frames_processed += 1;
		} else {
			// Multi-segment frames or errors - drop for now
			// Could be extended to handle fragmented frames if needed
			rx_frames_dropped += 1;
		}
		
		// Return descriptor to DMA
		DMARxDscrTab[idx].DESC0 = ETH_DMARXDESC_OWN;
		__DSB();  // Ensure descriptor write completes before DMA access
		Ethernet_DMA::_DMARPDR::overwrite(0U);  // Trigger DMA poll
		
		// Move to next descriptor (circular)
		idx = (idx + 1) % ETH_RX_DESC_CNT;
		frames_processed++;
	}
	
	RxDescList.CurrDescNum = idx;
}

bool ETH_IsTxBufferAvailable() {
	// Check if there's space in TX queue
	// Head pointer advances when we send, tail advances when DMA completes
	const uint8_t next_head = (TxDescList.CurrHeadNum + 1) % ETH_TX_DESC_CNT;
	return (next_head != TxDescList.CurrTailNum);
}

bool ETH_SendFrame(uint32_t len) {
	// Validate frame length
	if (len < ETH_MIN_FRAME_SIZE || len > ETH_MAX_FRAME_SIZE) {
		return false;  // Invalid frame size
	}
	
	// Check if TX buffer is available
	if (!ETH_IsTxBufferAvailable()) {
		tx_frames_dropped += 1;
	return false;  // TX queue full
	}
	
	const uint32_t head_idx = TxDescList.CurrHeadNum;
	
	// Set frame length in descriptor
	TxDescList.TxDesc[head_idx]->DESC1 = (len & ETH_DMATXDESC_TBS1);
	
	// Configure descriptor: owned by DMA, chained, complete frame, checksum offload
	TxDescList.TxDesc[head_idx]->DESC0 = 
		ETH_DMATXDESC_OWN |                    // Owned by DMA
		ETH_DMATXDESC_TCH |                    // Second address chained
		ETH_DMATXDESC_FS |                     // First segment
		ETH_DMATXDESC_LS |                     // Last segment
		ETH_DMATXDESC_CIC_TCPUDPICMP_FULL |    // Full checksum insertion
		ETH_DMATXDESC_IC;                      // Interrupt on completion
	
	__DMB();  // Ensure descriptor writes complete before DMA access
	Ethernet_DMA::_DMATPDR::overwrite(0U);  // Trigger DMA poll
	
	// Update head pointer and buffer pointer
	TxDescList.CurrHeadNum = (TxDescList.CurrHeadNum + 1) % ETH_TX_DESC_CNT;
	TxDescList.pBuff = &Tx_Buff[TxDescList.CurrHeadNum][0];
	
	tx_frames_sent += 1;
	return true;
}

ETH_Status PHY_Init() {
	uint32_t bcr, bsr, id1, id2, tickstart;
	
	// Verify PHY ID (LAN8742A)
	PHY_Read(PHY_ADDR, PHY_ID1, &id1);
	PHY_Read(PHY_ADDR, PHY_ID2, &id2);
	if ((id1 != PHY_ID1_EXPECTED) || ((id2 & PHY_ID2_MASK) != PHY_ID2_EXPECTED)) {
		return ETH_Status::PHY_ID_FAULT;
	}
	
	// Reset PHY
	PHY_Write(PHY_ADDR, PHY_BCR, PHY_RESET);
	tickstart = get_tick();
	do {
		PHY_Read(PHY_ADDR, PHY_BCR, &bcr);
		if ((get_tick() - tickstart) > PHY_RESET_TIMEOUT_MS) {
			return ETH_Status::PHY_RESET_FAULT;
		}
	} while (bcr & PHY_RESET);
	
	// Wait for PHY stabilization after reset
	delay_ms(PHY_STABILIZATION_DELAY_MS);
	
	// Start autonegotiation
	PHY_Write(PHY_ADDR, PHY_BCR, PHY_AUTONEG_EN | PHY_RESTART_AUTONEG);
	tickstart = get_tick();
	do {
		PHY_Read(PHY_ADDR, PHY_BSR, &bsr);
		PHY_Read(PHY_ADDR, PHY_BSR, &bsr);  // Double read (PHY requirement)
		if ((get_tick() - tickstart) > PHY_AUTONEG_TIMEOUT_MS) {
			return ETH_Status::PHY_AUTONEG_FAULT;
		}
	} while (!(bsr & PHY_AUTONEG_DONE));
	
	// Wait for link establishment
	tickstart = get_tick();
	do {
		PHY_Read(PHY_ADDR, PHY_BSR, &bsr);
		PHY_Read(PHY_ADDR, PHY_BSR, &bsr);  // Double read (PHY requirement)
		if ((get_tick() - tickstart) > PHY_LINK_TIMEOUT_MS) {
			return ETH_Status::PHY_LINK_DOWN;
		}
	} while (!(bsr & PHY_LINKED_STATUS));
	
	return ETH_Status::OK;
}

void PHY_SetMDIOClock() {
	uint32_t tmpreg = Ethernet_MAC::_MACMIIAR::read();
	tmpreg &= ~Ethernet_MAC::_MACMIIAR::CR::BitMsk;
	
	const uint32_t hclk = SystemCoreClock;
	
	// Select MDIO clock divider based on HCLK frequency
	// MDC must be between 1-2.5 MHz (IEEE 802.3 requirement)
	if (hclk < 35000000U) {
		tmpreg |= static_cast<uint32_t>(MACMIIAR_CR::Div16);
	} else if (hclk < 60000000U) {
		tmpreg |= static_cast<uint32_t>(MACMIIAR_CR::Div26);
	} else if (hclk < 100000000U) {
		tmpreg |= static_cast<uint32_t>(MACMIIAR_CR::Div42);
	} else if (hclk < 150000000U) {
		tmpreg |= static_cast<uint32_t>(MACMIIAR_CR::Div62);
	} else {
		tmpreg |= static_cast<uint32_t>(MACMIIAR_CR::Div102);
	}
	
	Ethernet_MAC::_MACMIIAR::overwrite(tmpreg);
}

bool PHY_Read(uint32_t phy_addr, uint32_t phy_reg, uint32_t *pRegValue) {
	uint32_t tmpreg = Ethernet_MAC::_MACMIIAR::read();
	
	// Clear address and register fields
	tmpreg &= ~(Ethernet_MAC::_MACMIIAR::PA::BitMsk |
	            Ethernet_MAC::_MACMIIAR::MR::BitMsk |
	            Ethernet_MAC::_MACMIIAR::MW::BitMsk |
	            Ethernet_MAC::_MACMIIAR::MB::BitMsk);
	
	// Set PHY address and register
	tmpreg |= ((phy_addr << Ethernet_MAC::_MACMIIAR::PA::pos) 
	           & Ethernet_MAC::_MACMIIAR::PA::BitMsk);
	tmpreg |= ((phy_reg << Ethernet_MAC::_MACMIIAR::MR::pos) 
	           & Ethernet_MAC::_MACMIIAR::MR::BitMsk);
	
	// Set read mode and start operation
	tmpreg &= ~Ethernet_MAC::_MACMIIAR::MW::BitMsk;  // Read mode
	tmpreg |= Ethernet_MAC::_MACMIIAR::MB::BitMsk;   // Set busy bit
	
	Ethernet_MAC::_MACMIIAR::overwrite(tmpreg);
	
	// Wait for operation to complete
	uint32_t tickstart = get_tick();
	while (Ethernet_MAC::_MACMIIAR::MB::read()) {
		if ((get_tick() - tickstart) > PHY_MDIO_TIMEOUT_MS) {
			return false;
		}
	}
	
	// Read result
	*pRegValue = Ethernet_MAC::_MACMIIDR::TD::read();
	return true;
}

bool PHY_Write(uint32_t phy_addr, uint32_t phy_reg, uint32_t RegValue) {
	uint32_t tmpreg = Ethernet_MAC::_MACMIIAR::read();
	
	// Clear address and register fields
	tmpreg &= ~(Ethernet_MAC::_MACMIIAR::PA::BitMsk |
	            Ethernet_MAC::_MACMIIAR::MR::BitMsk |
	            Ethernet_MAC::_MACMIIAR::MW::BitMsk |
	            Ethernet_MAC::_MACMIIAR::MB::BitMsk);
	
	// Set PHY address and register
	tmpreg |= ((phy_addr << Ethernet_MAC::_MACMIIAR::PA::pos) 
	           & Ethernet_MAC::_MACMIIAR::PA::BitMsk);
	tmpreg |= ((phy_reg << Ethernet_MAC::_MACMIIAR::MR::pos) 
	           & Ethernet_MAC::_MACMIIAR::MR::BitMsk);
	
	// Set write mode and start operation
	tmpreg |= Ethernet_MAC::_MACMIIAR::MW::BitMsk;  // Write mode
	tmpreg |= Ethernet_MAC::_MACMIIAR::MB::BitMsk;  // Set busy bit
	
	// Write data first
	Ethernet_MAC::_MACMIIDR::TD::write(static_cast<uint16_t>(RegValue));
	
	// Then start the operation
	Ethernet_MAC::_MACMIIAR::overwrite(tmpreg);
	
	// Wait for operation to complete
	uint32_t tickstart = get_tick();
	while (Ethernet_MAC::_MACMIIAR::MB::read()) {
		if ((get_tick() - tickstart) > PHY_MDIO_TIMEOUT_MS) {
			return false;
		}
	}
	
	return true;
}
