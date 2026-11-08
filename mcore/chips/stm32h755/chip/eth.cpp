/*
 * STM32H755 Ethernet driver (Synopsys Ethernet QoS MAC + DMA, RMII, LAN8742A).
 *
 * Exposes exactly the API of the STM32F7 driver (drivers/eth.hpp), so the
 * network stack is shared. Differences handled here:
 *   - the H7 uses 4-word descriptors with separate "read" and "write-back"
 *     layouts and tail-pointer driven DMA rings;
 *   - the RX checksum/protocol status (RDES1) is translated into the F7
 *     RDES4 layout (ETH_DMAPTPRXDESC_*) the stack expects;
 *   - descriptors and buffers live in D2 SRAM3 (0x30040000): the Ethernet DMA
 *     cannot reach DTCM. The linker script and the MPU region must agree.
 */

#include "drivers/eth.hpp"
#include "config.hpp"
#include "core/system.hpp"
#include "core/nvic.hpp"
#include "core/rcc.hpp"
#include "net/tcp.hpp"
#include "net/net.hpp"

namespace {
// --- Hardware descriptor (normal descriptor, DMACCR.DSL = 0 -> 16 bytes) ---
struct EthDesc {
	volatile uint32_t DES0;
	volatile uint32_t DES1;
	volatile uint32_t DES2;
	volatile uint32_t DES3;
};

// RX descriptor, read format (owned by DMA)
constexpr uint32_t RDES3_OWN   = 1U << 31;
constexpr uint32_t RDES3_IOC   = 1U << 30;
constexpr uint32_t RDES3_BUF1V = 1U << 24;
// RX descriptor, write-back format
constexpr uint32_t RDES3_CTXT  = 1U << 30;
constexpr uint32_t RDES3_FD    = 1U << 29;
constexpr uint32_t RDES3_LD    = 1U << 28;
constexpr uint32_t RDES3_RS1V  = 1U << 26;   // RDES1 holds valid status
constexpr uint32_t RDES3_ES    = 1U << 15;
constexpr uint32_t RDES3_PL    = 0x7FFFU;
constexpr uint32_t RDES1_PT    = 0x7U;       // payload type: 1 UDP, 2 TCP, 3 ICMP
constexpr uint32_t RDES1_IPHE  = 1U << 3;
constexpr uint32_t RDES1_IPV4  = 1U << 4;
constexpr uint32_t RDES1_IPV6  = 1U << 5;
constexpr uint32_t RDES1_IPCB  = 1U << 6;
constexpr uint32_t RDES1_IPCE  = 1U << 7;
// TX descriptor, read format
constexpr uint32_t TDES2_IOC   = 1U << 31;
constexpr uint32_t TDES2_B1L   = 0x3FFFU;
constexpr uint32_t TDES3_OWN   = 1U << 31;
constexpr uint32_t TDES3_FD    = 1U << 29;
constexpr uint32_t TDES3_LD    = 1U << 28;
constexpr uint32_t TDES3_CIC_FULL = 3U << 16; // IP header + payload checksum, pseudo-header in HW
constexpr uint32_t TDES3_FL    = 0x7FFFU;

// MDIO
constexpr uint32_t MDIO_GOC_WRITE = 0b01;
constexpr uint32_t MDIO_GOC_READ  = 0b11;
// SYSCFG_PMCR.EPIS: Ethernet PHY interface
constexpr uint32_t EPIS_RMII = 0b100;
constexpr uint32_t DMA_PBL_32 = 32;

alignas(32) EthDesc DMARxDscrTab[ETH_RX_DESC_CNT] __attribute__((section(".RxDecripSection")));
alignas(32) EthDesc DMATxDscrTab[ETH_TX_DESC_CNT] __attribute__((section(".TxDecripSection")));
alignas(32) uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE] __attribute__((section(".RxPoolSection")));
alignas(32) uint8_t Tx_Buff[ETH_TX_DESC_CNT][ETH_MAX_PACKET_SIZE] __attribute__((section(".TxPoolSection")));

// Shadow descriptors handed to the stack: DESC4 carries the RX status in the
// F7 enhanced-descriptor layout (ETH_DMAPTPRXDESC_*).
ETH_DMADescStruct RxShadow;
ETH_DMADescStruct TxShadow[ETH_TX_DESC_CNT];

constexpr uint32_t ETH_RESET_TIMEOUT_MS = 10;
constexpr uint32_t PHY_RESET_TIMEOUT_MS = 1000;
constexpr uint32_t PHY_STABILIZATION_DELAY_MS = 40;
constexpr uint32_t PHY_AUTONEG_TIMEOUT_MS = 5000;
constexpr uint32_t PHY_LINK_TIMEOUT_MS = 5000;
constexpr uint32_t PHY_MDIO_TIMEOUT_MS = 50;

// LAN8742A PHY ID values
constexpr uint32_t PHY_ID1_EXPECTED = 0x0007;
constexpr uint32_t PHY_ID2_MASK = 0xFFF0;
constexpr uint32_t PHY_ID2_EXPECTED = 0xC130;

constexpr uint16_t ETH_MIN_FRAME_SIZE = 60;
constexpr uint16_t ETH_MAX_FRAME_SIZE = 1518;

volatile uint32_t rx_frames_processed = 0;
volatile uint32_t rx_frames_dropped = 0;
volatile uint32_t tx_frames_sent = 0;
volatile uint32_t tx_frames_dropped = 0;

inline uint32_t addr_of(const volatile void *p) {
	return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(const_cast<void*>(p)));
}

// Hand an RX descriptor (back) to the DMA.
inline void rx_arm(uint32_t i) {
	DMARxDscrTab[i].DES0 = addr_of(&Rx_Buff[i][0]);
	DMARxDscrTab[i].DES1 = 0U;
	DMARxDscrTab[i].DES2 = 0U;
	__DMB();
	DMARxDscrTab[i].DES3 = RDES3_OWN | RDES3_IOC | RDES3_BUF1V;
}

// RDES1 (H7) -> RDES4 (F7) checksum / protocol status
constexpr uint32_t rx_status_f7(uint32_t des1, uint32_t des3) {
	if ((des3 & RDES3_RS1V) == 0U) {
		return 0U;
	}
	uint32_t st = des1 & RDES1_PT;   // ETH_DMAPTPRXDESC_IPPT uses the same codes
	if (des1 & RDES1_IPHE) st |= ETH_DMAPTPRXDESC_IPHE;
	if (des1 & RDES1_IPCE) st |= ETH_DMAPTPRXDESC_IPPE;
	if (des1 & RDES1_IPCB) st |= ETH_DMAPTPRXDESC_IPCB;
	if (des1 & RDES1_IPV4) st |= ETH_DMAPTPRXDESC_IPV4PR;
	if (des1 & RDES1_IPV6) st |= ETH_DMAPTPRXDESC_IPV6PR;
	return st;
}
// The net stack routes on these exact values (see NET_ProcessRx).
static_assert(rx_status_f7(RDES1_IPV4 | 2U, RDES3_RS1V) == (ETH_DMAPTPRXDESC_IPV4PR | ETH_DMAPTPRXDESC_IPPT_TCP));
static_assert(rx_status_f7(RDES1_IPV4 | 1U, RDES3_RS1V) == (ETH_DMAPTPRXDESC_IPV4PR | ETH_DMAPTPRXDESC_IPPT_UDP));
static_assert(rx_status_f7(RDES1_IPV4 | 3U, RDES3_RS1V) == (ETH_DMAPTPRXDESC_IPV4PR | ETH_DMAPTPRXDESC_IPPT_ICMP));
static_assert(rx_status_f7(RDES1_IPV4 | RDES1_IPCE | 2U, RDES3_RS1V) & ETH_DMAPTPRXDESC_IPPE);
static_assert(rx_status_f7(RDES1_IPV4 | 2U, 0U) == 0U, "RDES1 ignored unless RS1V");
} // namespace

// Global descriptor lists (shared API with the F7 driver / net stack)
ETH_RxDescListStruct RxDescList;
ETH_TxDescListStruct TxDescList;

volatile uint32_t eth_rx_event = 0;
volatile uint32_t eth_tx_event = 0;

ETH_Status ETH_Init() {
	// 1. RMII must be selected while the MAC is still unclocked / in reset
	RCC::_APB4ENR::SYSCFGEN::set();
	(void) RCC::_APB4ENR::read();
	SYSCFG::_PMCR::EPIS::write(EPIS_RMII);
	(void) SYSCFG::_PMCR::read();

	// 2. D2 SRAM (buffers) + MAC/TX/RX clocks
	enableD2SRAM();
	RCC::_AHB1ENR::setMask<RCC::_AHB1ENR::ETH1MACEN::BitMsk |
	                       RCC::_AHB1ENR::ETH1TXEN::BitMsk |
	                       RCC::_AHB1ENR::ETH1RXEN::BitMsk>();
	(void) RCC::_AHB1ENR::read();
	delay_ms(1);

	// 3. Software reset (completes only when the PHY provides REF_CLK)
	Ethernet_MAC::_DMAMR::SWR::set();
	const uint32_t tickstart = get_tick();
	while (Ethernet_MAC::_DMAMR::SWR::read()) {
		if ((get_tick() - tickstart) > ETH_RESET_TIMEOUT_MS) {
			return ETH_Status::RESET_FAULT;
		}
	}

	// 4. 1 us tick for the MAC (LPI/PMT timers), MDIO clock
	Ethernet_MAC::_MAC1USTCR::overwrite((AHBClock / 1000000U) - 1U);
	PHY_SetMDIOClock();

	// 5. PHY: autonegotiation, link
	ETH_LinkState link = {0, 0};
	const ETH_Status status = PHY_Init(&link);
	if (status != ETH_Status::OK) {
		return status;
	}

	// 6. MAC configuration
	Ethernet_MAC::_MACCR::overwrite(
		  Ethernet_MAC::_MACCR::IPC::BitMsk          // RX checksum offload (fills RDES1)
		| Ethernet_MAC::_MACCR::CST::BitMsk          // strip CRC of type frames
		| Ethernet_MAC::_MACCR::ACS::BitMsk          // strip pad/CRC of length frames
		| Ethernet_MAC::_MACCR::JD::BitMsk           // jabber timer off
		| (link.speed  ? Ethernet_MAC::_MACCR::FES::BitMsk : 0U)
		| (link.duplex ? Ethernet_MAC::_MACCR::DM::BitMsk  : 0U));

	// Unicast to our MAC + broadcast; no promiscuous/multicast
	Ethernet_MAC::_MACPFR::overwrite(0U);
	// Honour received PAUSE frames
	Ethernet_MAC::_MACRxFCR::RFE::set();

	// MAC address (MACA0) from the network stack
	Ethernet_MAC::_MACA0HR::overwrite(
		  (static_cast<uint32_t>(MAC_ADDR[5]) << 8) | MAC_ADDR[4]);
	Ethernet_MAC::_MACA0LR::overwrite(
		  (static_cast<uint32_t>(MAC_ADDR[3]) << 24) | (static_cast<uint32_t>(MAC_ADDR[2]) << 16)
		| (static_cast<uint32_t>(MAC_ADDR[1]) << 8)  |  MAC_ADDR[0]);

	// 7. MTL: store-and-forward both ways (required for TX checksum insertion)
	Ethernet_MAC::_MTLTxQOMR::TSF::set();
	Ethernet_MAC::_MTLRxQOMR::RSF::set();

	// 8. DMA: fixed bursts, address-aligned beats, 16-byte descriptors
	Ethernet_MAC::_DMASBMR::overwrite(Ethernet_MAC::_DMASBMR::AAL::BitMsk |
	                                  Ethernet_MAC::_DMASBMR::FB::BitMsk);
	Ethernet_MAC::_DMACCR::overwrite(0U);   // DSL = 0, PBLx8 = 0
	Ethernet_MAC::_DMACTxCR::overwrite(
		  (DMA_PBL_32 << Ethernet_MAC::_DMACTxCR::TXPBL::pos)
		| Ethernet_MAC::_DMACTxCR::OSF::BitMsk);
	Ethernet_MAC::_DMACRxCR::overwrite(
		  (DMA_PBL_32 << Ethernet_MAC::_DMACRxCR::RXPBL::pos)
		| (ETH_MAX_PACKET_SIZE << Ethernet_MAC::_DMACRxCR::RBSZ::pos));

	// 9. Descriptor rings
	ETH_RxDescInit();
	ETH_TxDescInit();

	// 10. Mask all MMC counter interrupts
	Ethernet_MAC::_MMC_RX_INTERRUPT_MASK::setMask(
		  Ethernet_MAC::_MMC_RX_INTERRUPT_MASK::RXCRCERPIM::BitMsk
		| Ethernet_MAC::_MMC_RX_INTERRUPT_MASK::RXALGNERPIM::BitMsk
		| Ethernet_MAC::_MMC_RX_INTERRUPT_MASK::RXUCGPIM::BitMsk
		| Ethernet_MAC::_MMC_RX_INTERRUPT_MASK::RXLPIUSCIM::BitMsk
		| Ethernet_MAC::_MMC_RX_INTERRUPT_MASK::RXLPITRCIM::BitMsk);
	Ethernet_MAC::_MMC_TX_INTERRUPT_MASK::setMask(
		  Ethernet_MAC::_MMC_TX_INTERRUPT_MASK::TXSCOLGPIM::BitMsk
		| Ethernet_MAC::_MMC_TX_INTERRUPT_MASK::TXMCOLGPIM::BitMsk
		| Ethernet_MAC::_MMC_TX_INTERRUPT_MASK::TXGPKTIM::BitMsk
		| Ethernet_MAC::_MMC_TX_INTERRUPT_MASK::TXLPIUSCIM::BitMsk
		| Ethernet_MAC::_MMC_TX_INTERRUPT_MASK::TXLPITRCIM::BitMsk);

	// 11. DMA interrupts: TX/RX complete + RX buffer unavailable
	Ethernet_MAC::_DMACIER::overwrite(
		  Ethernet_MAC::_DMACIER::NIE::BitMsk
		| Ethernet_MAC::_DMACIER::RIE::BitMsk
		| Ethernet_MAC::_DMACIER::TIE::BitMsk
		| Ethernet_MAC::_DMACIER::AIE::BitMsk
		| Ethernet_MAC::_DMACIER::RBUE::BitMsk);

	// 12. Start: MAC TX/RX, flush TX FIFO, DMA TX/RX
	Ethernet_MAC::_MACCR::bitSet<Ethernet_MAC::_MACCR::TE, Ethernet_MAC::_MACCR::RE>();
	Ethernet_MAC::_MTLTxQOMR::FTQ::set();
	const uint32_t flushstart = get_tick();
	while (Ethernet_MAC::_MTLTxQOMR::FTQ::read()) {
		if ((get_tick() - flushstart) > ETH_RESET_TIMEOUT_MS) {
			return ETH_Status::RESET_FAULT;
		}
	}
	Ethernet_MAC::_DMACTxCR::ST::set();
	Ethernet_MAC::_DMACRxCR::SR::set();
	Ethernet_MAC::_DMACSR::overwrite(Ethernet_MAC::_DMACSR::TPS::BitMsk |
	                                 Ethernet_MAC::_DMACSR::RPS::BitMsk);

	NVIC_API::SetPriority<IRQn_Type::ETH_IRQn, 0>();
	NVIC_API::EnableIRQ<IRQn_Type::ETH_IRQn>();
	return ETH_Status::OK;
}

void ETH_RxDescInit() {
	for (uint32_t i = 0; i < ETH_RX_DESC_CNT; i++) {
		rx_arm(i);
	}
	RxShadow = {};
	RxDescList.CurrDescNum = 0;
	RxDescList.CurrRxDesc = &RxShadow;
	RxDescList.pBuff = &Rx_Buff[0][0];
	RxDescList.BuffLen = 0;
	for (uint32_t i = 0; i < ETH_RX_DESC_CNT; i++) {
		RxDescList.RxDesc[i] = nullptr;   // hardware descriptors are private here
	}

	Ethernet_MAC::_DMACRxRLR::overwrite(ETH_RX_DESC_CNT - 1U);
	Ethernet_MAC::_DMACRxDLAR::overwrite(addr_of(&DMARxDscrTab[0]));
	__DSB();
	Ethernet_MAC::_DMACRxDTPR::overwrite(addr_of(&DMARxDscrTab[ETH_RX_DESC_CNT - 1U]));
}

void ETH_TxDescInit() {
	for (uint32_t i = 0; i < ETH_TX_DESC_CNT; i++) {
		DMATxDscrTab[i].DES0 = addr_of(&Tx_Buff[i][0]);
		DMATxDscrTab[i].DES1 = 0U;
		DMATxDscrTab[i].DES2 = 0U;
		DMATxDscrTab[i].DES3 = 0U;
		TxShadow[i] = {};
		TxDescList.TxDesc[i] = &TxShadow[i];
	}
	TxDescList.CurrHeadNum = 0;
	TxDescList.CurrTailNum = 0;
	TxDescList.CurrTxDesc = &TxShadow[0];
	TxDescList.pBuff = &Tx_Buff[0][0];
	TxDescList.BuffLen = 0;

	Ethernet_MAC::_DMACTxRLR::overwrite(ETH_TX_DESC_CNT - 1U);
	Ethernet_MAC::_DMACTxDLAR::overwrite(addr_of(&DMATxDscrTab[0]));
	Ethernet_MAC::_DMACTxDTPR::overwrite(addr_of(&DMATxDscrTab[0]));
}

// Called at the end of every ETH interrupt (RX/TX done): lets an application
// wake the task that runs ETH_RxWorker. Empty by default.
extern "C" __attribute__((weak)) void ETH_EventHook(void) {
}

extern "C" void ETH_IRQHandler(void) {
	const uint32_t sr = Ethernet_MAC::_DMACSR::read();

	if (sr & Ethernet_MAC::_DMACSR::TI::BitMsk) {
		Ethernet_MAC::_DMACSR::overwrite(Ethernet_MAC::_DMACSR::TI::BitMsk |
		                                 Ethernet_MAC::_DMACSR::NIS::BitMsk);
		// Reclaim descriptors the DMA has finished with
		uint8_t tail = TxDescList.CurrTailNum;
		const uint8_t head = TxDescList.CurrHeadNum;
		while (tail != head && !(DMATxDscrTab[tail].DES3 & TDES3_OWN)) {
			tail = static_cast<uint8_t>((tail + 1U) % ETH_TX_DESC_CNT);
		}
		TxDescList.CurrTailNum = tail;
		eth_tx_event = 1;
	}

	if (sr & Ethernet_MAC::_DMACSR::RI::BitMsk) {
		Ethernet_MAC::_DMACSR::overwrite(Ethernet_MAC::_DMACSR::RI::BitMsk |
		                                 Ethernet_MAC::_DMACSR::NIS::BitMsk);
		eth_rx_event = 1;
	}

	if (sr & Ethernet_MAC::_DMACSR::RBU::BitMsk) {
		// Ring was full: the worker re-arms descriptors; restart the RX DMA.
		Ethernet_MAC::_DMACSR::overwrite(Ethernet_MAC::_DMACSR::RBU::BitMsk |
		                                 Ethernet_MAC::_DMACSR::AIS::BitMsk);
		eth_rx_event = 1;
	}

	// Other abnormal events (bus error, descriptor error, watchdog): acknowledge
	// them so the abnormal summary cannot keep the IRQ pending forever.
	constexpr uint32_t ABNORMAL = Ethernet_MAC::_DMACSR::FBE::BitMsk
	                            | Ethernet_MAC::_DMACSR::CDE::BitMsk
	                            | Ethernet_MAC::_DMACSR::RWT::BitMsk
	                            | Ethernet_MAC::_DMACSR::RPS::BitMsk
	                            | Ethernet_MAC::_DMACSR::TPS::BitMsk;
	if (sr & ABNORMAL) {
		Ethernet_MAC::_DMACSR::overwrite((sr & ABNORMAL) | Ethernet_MAC::_DMACSR::AIS::BitMsk);
	}
	ETH_EventHook();
}

void ETH_RxWorker() {
	if (eth_rx_event == 0) {
		return;
	}
	eth_rx_event = 0;

	uint32_t idx = RxDescList.CurrDescNum;
	for (uint32_t n = 0; n < ETH_RX_DESC_CNT; n++) {
		const uint32_t des3 = DMARxDscrTab[idx].DES3;
		if (des3 & RDES3_OWN) {
			break;   // DMA still owns it: nothing more received
		}

		const uint16_t frame_len = static_cast<uint16_t>(des3 & RDES3_PL);
		const bool complete = (des3 & (RDES3_FD | RDES3_LD)) == (RDES3_FD | RDES3_LD);
		const bool good = !(des3 & (RDES3_ES | RDES3_CTXT));

		if (complete && good && frame_len >= ETH_MIN_FRAME_SIZE && frame_len <= ETH_MAX_FRAME_SIZE) {
			RxShadow.DESC0 = des3;
			RxShadow.DESC4 = rx_status_f7(DMARxDscrTab[idx].DES1, des3);
			RxDescList.BuffLen = frame_len;
			RxDescList.pBuff = &Rx_Buff[idx][0];
			RxDescList.CurrRxDesc = &RxShadow;
			NET_ProcessRx(&RxDescList, &TxDescList);
			rx_frames_processed += 1;
		} else {
			rx_frames_dropped += 1;   // multi-buffer frames, errors, context descriptors
		}

		rx_arm(idx);
		idx = (idx + 1U) % ETH_RX_DESC_CNT;
	}
	RxDescList.CurrDescNum = idx;

	// Writing the tail pointer also resumes the DMA after "buffer unavailable"
	__DSB();
	Ethernet_MAC::_DMACRxDTPR::overwrite(addr_of(&DMARxDscrTab[ETH_RX_DESC_CNT - 1U]));
}

bool ETH_IsTxBufferAvailable() {
	const uint8_t next_head = static_cast<uint8_t>((TxDescList.CurrHeadNum + 1U) % ETH_TX_DESC_CNT);
	return next_head != TxDescList.CurrTailNum;
}

bool ETH_SendFrame(uint32_t len) {
	if (len > ETH_MAX_FRAME_SIZE) {
		return false;
	}
	if (!ETH_IsTxBufferAvailable()) {
		tx_frames_dropped += 1;
		return false;
	}

	const uint32_t head = TxDescList.CurrHeadNum;
	EthDesc &d = DMATxDscrTab[head];
	d.DES0 = addr_of(&Tx_Buff[head][0]);
	d.DES1 = 0U;
	d.DES2 = TDES2_IOC | (len & TDES2_B1L);
	__DMB();
	d.DES3 = TDES3_OWN | TDES3_FD | TDES3_LD | TDES3_CIC_FULL | (len & TDES3_FL);

	const uint32_t next = (head + 1U) % ETH_TX_DESC_CNT;
	TxDescList.CurrHeadNum = static_cast<uint8_t>(next);
	TxDescList.CurrTxDesc = &TxShadow[next];
	TxDescList.pBuff = &Tx_Buff[next][0];

	// Tail = first descriptor the DMA must NOT process yet; this kicks the DMA.
	__DSB();
	Ethernet_MAC::_DMACSR::overwrite(Ethernet_MAC::_DMACSR::TBU::BitMsk |
	                                 Ethernet_MAC::_DMACSR::TPS::BitMsk);
	Ethernet_MAC::_DMACTxDTPR::overwrite(addr_of(&DMATxDscrTab[next]));

	tx_frames_sent += 1;
	return true;
}

ETH_Status PHY_Init(ETH_LinkState *lnk) {
	uint32_t bcr = 0, bsr = 0, id1 = 0, id2 = 0, tickstart;

	PHY_Read(PHY_ADDR, PHY_ID1, &id1);
	PHY_Read(PHY_ADDR, PHY_ID2, &id2);
	if ((id1 != PHY_ID1_EXPECTED) || ((id2 & PHY_ID2_MASK) != PHY_ID2_EXPECTED)) {
		return ETH_Status::PHY_ID_FAULT;
	}

	PHY_Write(PHY_ADDR, PHY_BCR, PHY_RESET);
	tickstart = get_tick();
	do {
		PHY_Read(PHY_ADDR, PHY_BCR, &bcr);
		if ((get_tick() - tickstart) > PHY_RESET_TIMEOUT_MS) {
			return ETH_Status::PHY_RESET_FAULT;
		}
	} while (bcr & PHY_RESET);

	delay_ms(PHY_STABILIZATION_DELAY_MS);

	PHY_Write(PHY_ADDR, PHY_BCR, PHY_AUTONEG_EN | PHY_RESTART_AUTONEG);
	tickstart = get_tick();
	do {
		PHY_Read(PHY_ADDR, PHY_BSR, &bsr);
		PHY_Read(PHY_ADDR, PHY_BSR, &bsr);   // latched bits: read twice
		if ((get_tick() - tickstart) > PHY_AUTONEG_TIMEOUT_MS) {
			return ETH_Status::PHY_AUTONEG_FAULT;
		}
	} while (!(bsr & PHY_AUTONEG_DONE));

	tickstart = get_tick();
	do {
		PHY_Read(PHY_ADDR, PHY_BSR, &bsr);
		PHY_Read(PHY_ADDR, PHY_BSR, &bsr);
		if ((get_tick() - tickstart) > PHY_LINK_TIMEOUT_MS) {
			return ETH_Status::PHY_LINK_DOWN;
		}
	} while (!(bsr & PHY_LINKED_STATUS));

	PHY_Read(PHY_ADDR, PHY_SC_SR, &bsr);
	if ((bsr & PHY_FULLDUPLEX_100M) == PHY_FULLDUPLEX_100M) {
		lnk->duplex = 1; lnk->speed = 1;
	} else if ((bsr & PHY_HALFDUPLEX_100M) == PHY_HALFDUPLEX_100M) {
		lnk->duplex = 0; lnk->speed = 1;
	} else if ((bsr & PHY_FULLDUPLEX_10M) == PHY_FULLDUPLEX_10M) {
		lnk->duplex = 1; lnk->speed = 0;
	} else if ((bsr & PHY_HALFDUPLEX_10M) == PHY_HALFDUPLEX_10M) {
		lnk->duplex = 0; lnk->speed = 0;
	} else {
		return ETH_Status::UNEXPECT_FAULT;
	}
	return ETH_Status::OK;
}

// MDC = hclk / CR divider, must stay <= 2.5 MHz (RM0399, ETH_MACMDIOAR.CR)
void PHY_SetMDIOClock() {
	const uint32_t hclk = AHBClock;
	uint32_t cr;
	if (hclk < 35000000U)       cr = 0b0010;  // /16
	else if (hclk < 60000000U)  cr = 0b0011;  // /26
	else if (hclk < 100000000U) cr = 0b0000;  // /42
	else if (hclk < 150000000U) cr = 0b0001;  // /62
	else if (hclk < 250000000U) cr = 0b0100;  // /102
	else                        cr = 0b0101;  // /124
	Ethernet_MAC::_MACMDIOAR::CR::write(cr);
}

namespace {
bool mdio_xfer(uint32_t phy_addr, uint32_t phy_reg, uint32_t goc) {
	uint32_t ar = Ethernet_MAC::_MACMDIOAR::read();
	ar &= Ethernet_MAC::_MACMDIOAR::CR::BitMsk;   // keep only the clock range
	ar |= ((phy_addr << Ethernet_MAC::_MACMDIOAR::PA::pos)  & Ethernet_MAC::_MACMDIOAR::PA::BitMsk)
	    | ((phy_reg  << Ethernet_MAC::_MACMDIOAR::RDA::pos) & Ethernet_MAC::_MACMDIOAR::RDA::BitMsk)
	    | (goc << Ethernet_MAC::_MACMDIOAR::GOC::pos)
	    | Ethernet_MAC::_MACMDIOAR::MB::BitMsk;
	Ethernet_MAC::_MACMDIOAR::overwrite(ar);

	const uint32_t tickstart = get_tick();
	while (Ethernet_MAC::_MACMDIOAR::MB::read()) {
		if ((get_tick() - tickstart) > PHY_MDIO_TIMEOUT_MS) {
			return false;
		}
	}
	return true;
}
} // namespace

bool PHY_Read(uint32_t phy_addr, uint32_t phy_reg, uint32_t *pRegValue) {
	if (Ethernet_MAC::_MACMDIOAR::MB::read()) {
		return false;
	}
	if (!mdio_xfer(phy_addr, phy_reg, MDIO_GOC_READ)) {
		return false;
	}
	*pRegValue = Ethernet_MAC::_MACMDIODR::MD::read();
	return true;
}

bool PHY_Write(uint32_t phy_addr, uint32_t phy_reg, uint32_t RegValue) {
	if (Ethernet_MAC::_MACMDIOAR::MB::read()) {
		return false;
	}
	Ethernet_MAC::_MACMDIODR::overwrite(RegValue & 0xFFFFU);
	return mdio_xfer(phy_addr, phy_reg, MDIO_GOC_WRITE);
}
