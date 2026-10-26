/*
 * mcore_eth.cpp
 *
 *  Created on: Sep 18, 2025
 *      Author: AkimovMA
 */

#include "drivers/eth.hpp"
#include "core/mcore_config.hpp"
#include "core/system.hpp"
#include "net/tcp.hpp"
#include "net/net.hpp"
#include "core/nvic.hpp"
#include "core/rcc.hpp"

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

	// Link, followed by link_poll() from ETH_RxWorker
	bool link_up = false;
	ETH_LinkState link_mode = {0, 0};
	uint32_t link_changes = 0;
	uint32_t link_polled_at = 0;

	// Speed and duplex into MACCR / MACFCR; the MAC must be stopped (TE/RE off)
	void mac_configure(const ETH_LinkState &link) {
		uint32_t DM = 0, FES = 0, JD = 0, CSD = 0, RD = 0, DC = 0, FCB_BPA = 0;
		if (link.speed) {             // 100 Mbit/s, else 10
			FES = 1;
		}
		if (link.duplex) {            // full duplex
			DM  = 1;
			CSD = 1;
			JD  = 1;
			RD  = 1;
		} else {                      // half duplex
			FCB_BPA = 1;
			DC = 1;
			JD = 1;
		}

		Ethernet_MAC::_MACCR::overwrite(
		Ethernet_MAC::_MACCR::CSTF::BitMsk      |  // Strip CRC from Type frames
		(static_cast<uint32_t>(MACCR_IFG::_96bit)
		<< Ethernet_MAC::_MACCR::IFG::pos)      |  // 96-bit interframe gap
		(FES << Ethernet_MAC::_MACCR::FES::pos) |  // Fast Ethernet 10/100 Mbps
		(DM  << Ethernet_MAC::_MACCR::DM::pos)  |  // Duplex mode
		(JD  << Ethernet_MAC::_MACCR::JD::pos)  |  // Jabber disable
		(CSD << Ethernet_MAC::_MACCR::CSD::pos) |  // Carrier sense disable
		(RD  << Ethernet_MAC::_MACCR::RD::pos)  |  // Retry disable
		(DC  << Ethernet_MAC::_MACCR::DC::pos)  |  // Deferral check
		Ethernet_MAC::_MACCR::IPCO::BitMsk      |  // Hardware IPv4 checksum offload
		Ethernet_MAC::_MACCR::APCS::BitMsk		   //Automatic pad/CRC stripping
		);

		Ethernet_MAC::_MACFCR::overwrite(
		Ethernet_MAC::_MACFCR::ZQPD::BitMsk        |  // Ignore PAUSE frames with zero pause
		(static_cast<uint32_t>(MACFCR_PLT::Minus4)
		<< Ethernet_MAC::_MACFCR::PLT::pos)        |
		Ethernet_MAC::_MACFCR::RFCE::BitMsk        |
		Ethernet_MAC::_MACFCR::TFCE::BitMsk        |
		(FCB_BPA << Ethernet_MAC::_MACFCR::FCB::pos)  //Flow Control Busy / Back Pressure Activate
		);
	}

	// Speed / duplex resolved by autonegotiation (LAN8742A special status)
	bool phy_link_mode(ETH_LinkState *lnk) {
		uint32_t sr;
		if (!PHY_Read(PHY_ADDR, PHY_SC_SR, &sr)) return false;
		if ((sr & PHY_FULLDUPLEX_100M) == PHY_FULLDUPLEX_100M) {
			*lnk = {1, 1};
		} else if ((sr & PHY_HALFDUPLEX_100M) == PHY_HALFDUPLEX_100M) {
			*lnk = {1, 0};
		} else if ((sr & PHY_FULLDUPLEX_10M) == PHY_FULLDUPLEX_10M) {
			*lnk = {0, 1};
		} else if ((sr & PHY_HALFDUPLEX_10M) == PHY_HALFDUPLEX_10M) {
			*lnk = {0, 0};
		} else {
			return false;
		}
		return true;
	}

	// Follow the cable: start the MAC on link up, stop it on link down
	void link_poll() {
		const uint32_t now = get_tick();
		if (now - link_polled_at < ETH_LINK_POLL_MS) return;
		link_polled_at = now;

		uint32_t bsr;
		// The link bit latches low: the first read reports a drop since the last one
		if (!PHY_Read(PHY_ADDR, PHY_BSR, &bsr) || !PHY_Read(PHY_ADDR, PHY_BSR, &bsr)) return;
		const bool up = (bsr & PHY_LINKED_STATUS) && (bsr & PHY_AUTONEG_DONE);

		if (up && !link_up) {
			ETH_LinkState mode;
			if (!phy_link_mode(&mode)) return;           // next poll
			link_mode = mode;
			mac_configure(mode);
			Ethernet_MAC::_MACCR::bitSet<Ethernet_MAC::_MACCR::TE, Ethernet_MAC::_MACCR::RE>();
			link_up = true;
			link_changes++;
		} else if (!up && link_up) {
			Ethernet_MAC::_MACCR::TE::clear();
			Ethernet_MAC::_MACCR::RE::clear();
			link_up = false;
			link_changes++;
		}
	}
}

bool ETH_LinkUp() {
	return link_up;
}

ETH_LinkState ETH_LinkMode() {
	return link_mode;
}

uint32_t ETH_LinkChanges() {
	return link_changes;
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
	
	// PHY: identity, reset, autonegotiation started (not waited for: the
	// link is followed by link_poll(), which also sets speed and duplex)
	const ETH_Status status = PHY_Init();
	if (status != ETH_Status::OK) {
		return status;
	}
	mac_configure({1, 1});   // placeholder until the link reports its mode; MAC stays off
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

	// The MAC transmitter and receiver are enabled by link_poll() on link up

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

	link_up = false;
	link_changes = 0;
	link_polled_at = get_tick() - ETH_LINK_POLL_MS;   // first poll right away
	link_poll();
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
	
	// DMASR is write-1-to-clear: plain stores only. A read-modify-write
	// (setMask) wrote back every pending bit and cleared RS while handling TS,
	// so a frame received meanwhile waited for the next RX interrupt.
	const uint32_t sr = Ethernet_DMA::_DMASR::read();

	// Descriptors the DMA has sent are free again (here, not only on RX:
	// a transmit burst must not wait for incoming traffic to reclaim them)
	auto reclaim_tx = [] {
		uint8_t tail = TxDescList.CurrTailNum;
		const uint8_t head = TxDescList.CurrHeadNum;
		while (tail != head && !(TxDescList.TxDesc[tail]->DESC0 & ETH_DMATXDESC_OWN)) {
			tail = (tail + 1) % ETH_TX_DESC_CNT;
		}
		TxDescList.CurrTailNum = tail;
	};

	// TX interrupt handling
	if (sr & TS) {
		Ethernet_DMA::_DMASR::overwrite(TS | NIS);
		reclaim_tx();
		eth_tx_event = 1;  // Signal TX completion
	}
	// RX interrupt handling
	if (sr & RS) {
		Ethernet_DMA::_DMASR::overwrite(RS | NIS);
		reclaim_tx();
		eth_rx_event = 1;
	}
}

void ETH_RxWorker() {
	link_poll();

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
	// No link: the MAC transmitter is off, a queued frame would only wait
	if (!link_up) {
		return false;
	}
	// Check if there's space in TX queue
	// Head pointer advances when we send, tail advances when DMA completes
	const uint8_t next_head = (TxDescList.CurrHeadNum + 1) % ETH_TX_DESC_CNT;
	return (next_head != TxDescList.CurrTailNum);
}

bool ETH_SendFrame(uint32_t len) {
	// Validate frame length
	if (len > ETH_MAX_FRAME_SIZE) {
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
		ETH_DMATXDESC_FS  |                     // First segment
		ETH_DMATXDESC_LS  |                     // Last segment
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
	uint32_t bcr, id1, id2, tickstart;
	
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
	
	// Start autonegotiation; the PHY keeps renegotiating whenever a cable
	// is plugged in, link_poll() picks up the result
	if (!PHY_Write(PHY_ADDR, PHY_BCR, PHY_AUTONEG_EN | PHY_RESTART_AUTONEG)) {
		return ETH_Status::UNEXPECT_FAULT;
	}
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
