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


static ETH_DMADescStruct DMARxDscrTab[ETH_RX_DESC_CNT] __attribute__((section(".RxDecripSection")));

static ETH_DMADescStruct DMATxDscrTab[ETH_TX_DESC_CNT] __attribute__((section(".TxDecripSection")));

static uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE] __attribute__((section(".RxPoolSection")));

static uint8_t Tx_Buff[ETH_TX_DESC_CNT][ETH_MAX_PACKET_SIZE] __attribute__((aligned(4)));

ETH_RxDescListStruct RxDescList;
ETH_TxDescListStruct TxDescList;

volatile uint32_t eth_rx_event = 0;
volatile uint32_t eth_tx_event = 0;

ETH_Status ETH_Init(void) {

	uint32_t tickstart;
	enableEthInterface();
	delay_ms(1);

	RCC::_APB2ENR::SYSCFGEN::set();
	RCC::_APB2ENR::SYSCFGEN::read();
	
	/* RMII select */
	SYSCFG::_PMC::MII_RMII_SEL::set();
	SYSCFG::_PMC::MII_RMII_SEL::read();
	
	/* Ethernet Software reset */
	Ethernet_DMA::_DMABMR::SR::set();
	tickstart = get_tick();

	while (Ethernet_DMA::_DMABMR::SR::read()) {
		if (((get_tick() - tickstart) > 10)) {
			return ETH_Status::RESET_FAULT; // get status
		}
	}
	

	PHY_SetMDIOClock();

	ETH_Status status = PHY_Init();
	if (status != ETH_Status::OK) {
		return status;
	}

	/*MACCR cofig */
	Ethernet_MAC::_MACCR::overwrite(
	Ethernet_MAC::_MACCR::CSTF::BitMsk|// Удалять CRC у Type-фреймов (удобно для TCP/IP стека)
	(static_cast<uint32_t>(MACCR_IFG::_96bit)
	 << Ethernet_MAC::_MACCR::IFG::pos)|  // GAP = 96 бит → стандарт Ethernet (12 байт между кадрами)
	Ethernet_MAC::_MACCR::FES::BitMsk  |  // Fast Ethernet 100 Мбит/с (RMII не поддерживает 1 Гбит)
	Ethernet_MAC::_MACCR::DM::BitMsk   |  // Full-duplex (обязательно для TCP/IP)
	Ethernet_MAC::_MACCR::IPCO::BitMsk |  // Аппаратная проверка IPv4 checksum (ускоряет TCP/UDP/IP стек)
	Ethernet_MAC::_MACCR::RD::BitMsk// Retry disable (актуально только для half-duplex, тут безопасно)
	//Ethernet_MAC::_MACCR::BL,// Back-off limit = 10 (стандарт Ethernet, в full-duplex неважно)
	);

	delay_ms(1);

	/*MACFCR cofig */
	Ethernet_MAC::_MACFCR::overwrite(
	Ethernet_MAC::_MACFCR::ZQPD::BitMsk | // ZQPD = 1 → игнорировать PAUSE кадры с нулевой паузой
	(static_cast<uint32_t>(MACFCR_PLT::Minus4) 
	<< Ethernet_MAC::_MACFCR::PLT::pos) // PLT = pause time / 4 → порог возобновления передачи
	);
	
	delay_ms(1);

	/*DMAOMR cofig */
	Ethernet_DMA::_DMAOMR::bitSet<
	Ethernet_DMA::_DMAOMR::RSF, // Bit 25: Receive Store and Forward
	Ethernet_DMA::_DMAOMR::TSF, //Bit 21: Transmit Store and Forward
	Ethernet_DMA::_DMAOMR::FUGF,// Bit 6: Forward undersized good frames
	Ethernet_DMA::_DMAOMR::OSF  // Bit 2: Operate on second frame
	>();
	
	delay_ms(1);

	/*MAC addr cofig */
	// Пример MAC: 02:12:34:56:78:9A
	uint8_t MAC_addr[6] = {0x02, 0x12, 0x34, 0x56, 0x78, 0x9A};
	uint32_t MAC_ADDR_HR = (MAC_addr[5] << 8)  | MAC_addr[4];

	uint32_t MAC_ADDR_LR = (MAC_addr[3] << 24) |(MAC_addr[2] << 16) |
						   (MAC_addr[1] << 8)  | MAC_addr[0];

	Ethernet_MAC::_MACA0HR::MACA0H::write(MAC_ADDR_HR);
	Ethernet_MAC::_MACA0LR::MACA0L::write(MAC_ADDR_LR);

	/*DMAOMR cofig */
	Ethernet_DMA::_DMABMR::setMask(
	Ethernet_DMA::_DMABMR::AAB::BitMsk |
	Ethernet_DMA::_DMABMR::FB::BitMsk |
	(static_cast<uint32_t>(DMABMR_PBL::_32Beats)
	<<Ethernet_DMA::_DMABMR::RDP::pos) | 
	(static_cast<uint32_t>(DMABMR_PBL::_32Beats)
	<<Ethernet_DMA::_DMABMR::PBL::pos) | 
	Ethernet_DMA::_DMABMR::EDFE::BitMsk |
	Ethernet_DMA::_DMABMR::DA::BitMsk |
	Ethernet_DMA::_DMABMR::USP::BitMsk
);

	delay_ms(1);

	/*ETH_Init cofig */
	ETH_RxDescInit();
	ETH_TxDescInit();

	/* Disable MMC Interrupts */
	Ethernet_MAC::_MACIMR::bitSet<
	Ethernet_MAC::_MACIMR::TSTIM,
	Ethernet_MAC::_MACIMR::PMTIM
	>();
	/* Disable Rx MMC Interrupts */
	Ethernet_MMC::_MMCRIMR::bitSet<
	Ethernet_MMC::_MMCRIMR::RGUFM,
	Ethernet_MMC::_MMCRIMR::RFAEM,
	Ethernet_MMC::_MMCRIMR::RFCEM
	>();
	/* Disable Tx MMC Interrupts */
	Ethernet_MMC::_MMCTIMR::bitSet<
	Ethernet_MMC::_MMCTIMR::TGFM,
	Ethernet_MMC::_MMCTIMR::TGFMSCM,
	Ethernet_MMC::_MMCTIMR::TGFSCM
	>();
	/* Enable Rx Tx interrupt */
	Ethernet_DMA::_DMAIER::bitSet<
	Ethernet_DMA::_DMAIER::NISE,
	Ethernet_DMA::_DMAIER::RIE,
	Ethernet_DMA::_DMAIER::TIE
	>();

	delay_ms(1);
	
	Ethernet_MAC::_MACCR::bitSet<
	Ethernet_MAC::_MACCR::TE,
	Ethernet_MAC::_MACCR::RE
	>();

	delay_ms(1);

	/* Set the Flush Transmit FIFO bit */

	Ethernet_DMA::_DMAOMR::FTF::set();
	delay_ms(1);
	Ethernet_DMA::_DMAOMR::FTF::clear();

	/* Enable the DMA transmission */
	Ethernet_DMA::_DMAOMR::ST::set();
	/* Enable the DMA reception */
	Ethernet_DMA::_DMAOMR::SR::set();

	NVIC_API::SetPriority<IRQn_Type::ETH_IRQn,0>();
    NVIC_API::EnableIRQ<IRQn_Type::ETH_IRQn>();

    return ETH_Status::OK;
}

void ETH_RxDescInit(void) {
	for (uint32_t i = 0; i < ETH_RX_DESC_CNT; i++) {
		DMARxDscrTab[i].DESC0 = ETH_DMARXDESC_OWN;
		DMARxDscrTab[i].DESC1 = ETH_MAX_PACKET_SIZE | ETH_DMARXDESC_RCH;
		DMARxDscrTab[i].DESC2 = (uint32_t) &Rx_Buff[i][0];
		DMARxDscrTab[i].DESC3 = (uint32_t) &DMARxDscrTab[(i + 1)
				% ETH_RX_DESC_CNT];
		RxDescList.RxDesc[i] = &DMARxDscrTab[i];
	}
	RxDescList.CurrDescNum = 0;
	RxDescList.CurrRxDesc = &DMARxDscrTab[0];
	RxDescList.pBuff = &Rx_Buff[0][0];
	Ethernet_DMA::_DMARDLAR::overwrite(
		reinterpret_cast<uint32_t>(&DMARxDscrTab[0]));
	
}

void ETH_TxDescInit(void) {

	for (uint32_t i = 0; i < ETH_TX_DESC_CNT; i++) {
		DMATxDscrTab[i].DESC0 = ETH_DMATXDESC_TCH;
		DMATxDscrTab[i].DESC1 = 0;
		DMATxDscrTab[i].DESC2 = (uint32_t) &Tx_Buff[i][0];
		DMATxDscrTab[i].DESC3 = (uint32_t) &DMATxDscrTab[(i + 1)
				% ETH_TX_DESC_CNT];
		TxDescList.TxDesc[i] = &DMATxDscrTab[i];
	}
	TxDescList.CurrHeadNum = 0;
	TxDescList.CurrTailNum = 0;
	TxDescList.CurrTxDesc = &DMATxDscrTab[0];
	TxDescList.pBuff = &Tx_Buff[0][0];
	Ethernet_DMA::_DMATDLAR::overwrite(
		reinterpret_cast<uint32_t>(&DMATxDscrTab[0]));
}

extern "C" void ETH_IRQHandler(void) {

	uint32_t DMASR =  Ethernet_DMA::_DMASR::read();
	uint32_t DMAIER = Ethernet_DMA::_DMAIER::read();
	constexpr uint32_t RS = Ethernet_DMA::_DMASR::RS::BitMsk;
	constexpr uint32_t RIE = Ethernet_DMA::_DMAIER::RIE::BitMsk;
	constexpr uint32_t TS = Ethernet_DMA::_DMASR::TS::BitMsk;
	constexpr uint32_t TIE = Ethernet_DMA::_DMAIER::TIE::BitMsk;
	constexpr uint32_t NIS = Ethernet_DMA::_DMASR::NIS::BitMsk;

	/*RX interrupt*/
	if ((DMASR & RS) && (DMAIER & RIE)) {
		Ethernet_DMA::_DMASR::setMask(RS|NIS);
		eth_rx_event = 1;
	}
	/*TX interrupt*/
	if ((DMASR & TS) && (DMAIER & TIE)) {
		Ethernet_DMA::_DMASR::setMask(TS|NIS);

		TxDescList.CurrTailNum = (TxDescList.CurrTailNum + 1) % ETH_TX_DESC_CNT;
//		TxDescList.pBuff = &Tx_Buff[TxDescList.CurrDescNum][0];
		TxDescList.CurrTxDesc = TxDescList.TxDesc[TxDescList.CurrTailNum];
		eth_tx_event = 1;
	}
}

void ETH_RxWorker(void) {

	// начинаем с индекса последнего обработанного дескриптора
	if(eth_rx_event != 0){
	eth_rx_event = 0;
	uint32_t idx = RxDescList.CurrDescNum;

	while (1) {
		uint32_t desc0 = DMARxDscrTab[idx].DESC0;

		if (desc0 & ETH_DMARXDESC_OWN)
			break;
		// проверим, что это полный кадр в одном дескрипторе (FS+LS)
		if ((desc0 & (ETH_DMARXDESC_FS | ETH_DMARXDESC_LS))
				== (ETH_DMARXDESC_FS | ETH_DMARXDESC_LS) && !(desc0 & ETH_DMARXDESC_ES)) {

			RxDescList.BuffLen = (uint16_t) ((desc0 & ETH_DMARXDESC_FL) >> 16);
			RxDescList.pBuff = (uint8_t*) DMARxDscrTab[idx].DESC2;
			RxDescList.CurrRxDesc = &DMARxDscrTab[idx];
			// обработка пакета (передаём указатель и длину)

			NET_ProcessRx(&RxDescList, &TxDescList);
		} else {
			// поддержка мульти-сегментных фреймов здесь при необходимости
			// для простоты — бросаем или собираем в буфер
		}
		// возвращаем дескриптор DMA
		DMARxDscrTab[idx].DESC0 = ETH_DMARXDESC_OWN;
		__DSB(); // ensure descriptor written before moving on
		Ethernet_DMA::_DMARPDR::overwrite(0U);
		idx = (idx + 1) % ETH_RX_DESC_CNT;
		}
	RxDescList.CurrDescNum = idx;
	}
}

void ETH_SendFrame(uint32_t len) {
	TxDescList.TxDesc[TxDescList.CurrHeadNum]->DESC1 =
			(len & ETH_DMATXDESC_TBS1); // first address chained

	TxDescList.TxDesc[TxDescList.CurrHeadNum]->DESC0 = ETH_DMATXDESC_OWN
			| ETH_DMATXDESC_TCH | ETH_DMATXDESC_FS | ETH_DMATXDESC_LS | ETH_DMATXDESC_CIC_TCPUDPICMP_FULL
			| ETH_DMATXDESC_IC; // Owned by DMA
	__DMB();
	Ethernet_DMA::_DMATPDR::overwrite(0U);
	TxDescList.CurrHeadNum = ((TxDescList.CurrHeadNum + 1) % ETH_TX_DESC_CNT);
	TxDescList.pBuff = &Tx_Buff[TxDescList.CurrHeadNum][0];
}

ETH_Status PHY_Init(void)
{
    uint32_t bcr, bsr, id1, id2, tickstart;

    // --- Проверяем PHY ID ---
    PHY_Read(PHY_ADDR, PHY_ID1, &id1);
    PHY_Read(PHY_ADDR, PHY_ID2, &id2);
    if ((id1 != 0x0007) || ((id2 & 0xFFF0) != 0xC130))
        return ETH_Status::PHY_ID_FAULT;

    // --- Сброс PHY ---
    PHY_Write(PHY_ADDR, PHY_BCR, PHY_RESET);
    tickstart = get_tick();
    do {
        PHY_Read(PHY_ADDR, PHY_BCR, &bcr);
        if ((get_tick() - tickstart) > 1000)
            return ETH_Status::PHY_RESET_FAULT;
    } while (bcr & PHY_RESET);

    delay_ms(40); // время стабилизации LAN8742 после reset

    // --- Запускаем автосогласование ---
    PHY_Write(PHY_ADDR, PHY_BCR, PHY_AUTONEG_EN | PHY_RESTART_AUTONEG);
    tickstart = get_tick();
    do {
        PHY_Read(PHY_ADDR, PHY_BSR, &bsr);
        PHY_Read(PHY_ADDR, PHY_BSR, &bsr); // двойное чтение!
        if ((get_tick() - tickstart) > 5000)
            return ETH_Status::PHY_AUTONEG_FAULT;
    } while (!(bsr & PHY_AUTONEG_DONE));

    // --- Ждём LINK ---
    tickstart = get_tick();
    do {
        PHY_Read(PHY_ADDR, PHY_BSR, &bsr);
        PHY_Read(PHY_ADDR, PHY_BSR, &bsr);
        if ((get_tick() - tickstart) > 5000)
            return ETH_Status::PHY_LINK_DOWN;
    } while (!(bsr & PHY_LINKED_STATUS));

    return ETH_Status::OK;
}

void PHY_SetMDIOClock(void) {

	uint32_t hclk;
	uint32_t tmpreg;

	tmpreg = Ethernet_MAC::_MACMIIAR::read();
	tmpreg &= ~Ethernet_MAC::_MACMIIAR::CR::BitMsk;
	hclk = SystemCoreClock;

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

	uint32_t tmpreg;
	uint32_t tickstart;
	/* Get the ETHERNET MACMIIAR value */
	tmpreg = Ethernet_MAC::_MACMIIAR::read();
	tmpreg &= ~(Ethernet_MAC::_MACMIIAR::PA::BitMsk |
	            Ethernet_MAC::_MACMIIAR::MR::BitMsk |
	            Ethernet_MAC::_MACMIIAR::MW::BitMsk |
	            Ethernet_MAC::_MACMIIAR::MB::BitMsk);
	/* Prepare the MII address register value */
	tmpreg |= ((phy_addr << Ethernet_MAC::_MACMIIAR::PA::pos) & Ethernet_MAC::_MACMIIAR::PA::BitMsk);
	tmpreg |= ((phy_reg << Ethernet_MAC::_MACMIIAR::MR::pos)  & Ethernet_MAC::_MACMIIAR::MR::BitMsk);
	tmpreg &= ~Ethernet_MAC::_MACMIIAR::MW::BitMsk; /* Set the read mode            */
	tmpreg |= Ethernet_MAC::_MACMIIAR::MB::BitMsk; /* Set the MII Busy bit         */
	/* Write the result value into the MII Address register */
	Ethernet_MAC::_MACMIIAR::overwrite(tmpreg);
	/* Check for the Busy flag */
	tickstart = get_tick();
	while (Ethernet_MAC::_MACMIIAR::MB::read()) {
		if ((get_tick() - tickstart) > 50)
			return false;
	}
	*pRegValue = Ethernet_MAC::_MACMIIDR::TD::read();
	return true;
}

bool PHY_Write(uint32_t phy_addr, uint32_t phy_reg, uint32_t RegValue) {

	uint32_t tmpreg;
	uint32_t tickstart;
	/* Get the ETHERNET MACMIIAR value */
	tmpreg = Ethernet_MAC::_MACMIIAR::read();
	tmpreg &= ~(Ethernet_MAC::_MACMIIAR::PA::BitMsk |
		        Ethernet_MAC::_MACMIIAR::MR::BitMsk |
		        Ethernet_MAC::_MACMIIAR::MW::BitMsk |
		        Ethernet_MAC::_MACMIIAR::MB::BitMsk);

	/* Prepare the MII register address value */
	tmpreg |= ((phy_addr << Ethernet_MAC::_MACMIIAR::PA::pos) & Ethernet_MAC::_MACMIIAR::PA::BitMsk); /* Set the PHY device address */
	tmpreg |= ((phy_reg <<  Ethernet_MAC::_MACMIIAR::MR::pos) & Ethernet_MAC::_MACMIIAR::MR::BitMsk); /* Set the PHY register address */
	tmpreg |= Ethernet_MAC::_MACMIIAR::MW::BitMsk; /* Set the write mode */
	tmpreg |= Ethernet_MAC::_MACMIIAR::MB::BitMsk; /* Set the MII Busy bit */
	/* Give the value to the MII data register */
	Ethernet_MAC::_MACMIIDR::TD::write(static_cast<uint16_t>(RegValue));
	/* Write the result value into the MII Address register */
	Ethernet_MAC::_MACMIIAR::overwrite(tmpreg);
	/* Check for the Busy flag */
	tickstart = get_tick();
	while (Ethernet_MAC::_MACMIIAR::MB::read()) {
		if ((get_tick() - tickstart) > 50)
			return false;
	}
	return true;
}
