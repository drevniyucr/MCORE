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

__attribute__((section(".RxDecripSection"))) ETH_DMADescStruct DMARxDscrTab[ETH_RX_DESC_CNT];

__attribute__((section(".TxDecripSection"))) ETH_DMADescStruct DMATxDscrTab[ETH_TX_DESC_CNT];

__attribute__((section(".RxPoolSection"))) uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE];

__attribute__((aligned(32))) uint8_t Tx_Buff[ETH_TX_DESC_CNT][ETH_MAX_PACKET_SIZE] __attribute__((aligned(32)));

ETH_RxDescListStruct RxDescList;
ETH_TxDescListStruct TxDescList;

volatile uint32_t eth_rx_event = 0;
volatile uint32_t eth_tx_event = 0;

void ETH_Init(void) {

	int32_t tmpreg;
	uint32_t tickstart;

	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	(void) (RCC->APB2ENR & RCC_APB2ENR_SYSCFGEN);

	/* RMII select */
	SYSCFG->PMC |= SYSCFG_PMC_MII_RMII_SEL;
	(void) SYSCFG->PMC;

	/* Ethernet Software reset */
	ETH->DMABMR |= ETH_DMABMR_SR;
	tickstart = get_tick();
	while (ETH->DMABMR & ETH_DMABMR_SR) {
		if (((get_tick() - tickstart) > 10)) {
			return; // get status
		}
	}
	/*MAC cofig */
	tmpreg =
	ETH_MACCR_CSTF // Удалять CRC у Type-фреймов (удобно для TCP/IP стека)
	| ETH_MACCR_IFG_96Bit // GAP = 96 бит → стандарт Ethernet (12 байт между кадрами)
			| ETH_MACCR_FES // Fast Ethernet 100 Мбит/с (RMII не поддерживает 1 Гбит)
			| ETH_MACCR_DM          // Full-duplex (обязательно для TCP/IP)
			| ETH_MACCR_IPCO // Аппаратная проверка IPv4 checksum (ускоряет TCP/UDP/IP стек)
			| ETH_MACCR_RD // Retry disable (актуально только для half-duplex, тут безопасно)
			| ETH_MACCR_BL_10 // Back-off limit = 10 (стандарт Ethernet, в full-duplex неважно)
			| ETH_MACCR_TE          // Включаем Transmitter
			| ETH_MACCR_RE;         // Включаем Receiver

	ETH->MACCR = tmpreg;
	tmpreg = ETH->MACCR;
	delay_ms(1);
	ETH->MACCR = tmpreg;
	/*MACFCR cofig */
	tmpreg =
	ETH_MACFCR_ZQPD // ZQPD = 1 → игнорировать PAUSE кадры с нулевой паузой
	| ETH_MACFCR_PLT_Minus4; // PLT = pause time / 4 → порог возобновления передачи

	ETH->MACFCR = tmpreg;
	tmpreg = ETH->MACFCR;
	delay_ms(1);
	ETH->MACFCR = tmpreg;
	/*DMAOMR cofig */
	tmpreg =
	ETH_DMAOMR_RSF 		// Bit 25: Receive Store and Forward
	| ETH_DMAOMR_TSF        //Bit 21: Transmit Store and Forward
			| ETH_DMAOMR_FUGF;		// Bit 6: Forward undersized good frames
	//| ETH_DMAOMR_OSF;		// Bit 2: Operate on second frame

	ETH->DMAOMR = tmpreg;
	tmpreg = ETH->DMAOMR;
	delay_ms(1);
	ETH->DMAOMR = tmpreg;
	/*MAC addr cofig */
	// Пример MAC: 02:12:34:56:78:9A
	uint8_t MAC_addr[6];
	MAC_addr[0] = 0x02;
	MAC_addr[1] = 0x12;
	MAC_addr[2] = 0x34;
	MAC_addr[3] = 0x56;
	MAC_addr[4] = 0x78;
	MAC_addr[5] = 0x9A;
	ETH->MACA0HR = (MAC_addr[5] << 8) | MAC_addr[4];       // MAC[5]<<8 | MAC[4]
	ETH->MACA0LR = (MAC_addr[3] << 24) | (MAC_addr[2] << 16)
			| (MAC_addr[1] << 8) | MAC_addr[0];

	/*DMAOMR cofig */
	tmpreg =
	ETH_DMABMR_AAB | ETH_DMABMR_FB | ETH_DMABMR_RDP_32Beat
			| ETH_DMABMR_PBL_32Beat | ETH_DMABMR_EDE | ETH_DMABMR_DA
			| ETH_DMABMR_USP;

	ETH->DMABMR = tmpreg;
	tmpreg = ETH->DMABMR;
	delay_ms(1);
	ETH->DMABMR = tmpreg;

	ETH_RxDescInit();
	ETH_TxDescInit();

	/* Disable MMC Interrupts */
	ETH->MACIMR |= ETH_MACIMR_TSTIM | ETH_MACIMR_PMTIM;
	/* Disable Rx MMC Interrupts */
	ETH->MMCRIMR |= ETH_MMCRIMR_RGUFM | ETH_MMCRIMR_RFAEM | ETH_MMCRIMR_RFCEM;
	/* Disable Tx MMC Interrupts */
	ETH->MMCTIMR |= ETH_MMCTIMR_TGFM | ETH_MMCTIMR_TGFMSCM | ETH_MMCTIMR_TGFSCM;
	/* Enable Rx Tx interrupt */
	ETH->DMAIER |= ETH_DMAIER_NISE | ETH_DMAIER_RIE | ETH_DMAIER_TIE;
	tmpreg = ETH->DMAIER;
	PHY_Init();

	ETH->MACCR |= ETH_MACCR_TE;
	tmpreg = ETH->MACCR;
	delay_ms(1);
	ETH->MACCR = tmpreg;

	ETH->MACCR |= ETH_MACCR_RE;
	tmpreg = ETH->MACCR;
	delay_ms(1);
	ETH->MACCR = tmpreg;

	/* Set the Flush Transmit FIFO bit */
	ETH->DMAOMR |= ETH_DMAOMR_FTF;
	delay_ms(1);
	ETH->DMAOMR &= ~ETH_DMAOMR_FTF;

	/* Enable the DMA transmission */
	ETH->DMAOMR |= ETH_DMAOMR_ST;
	/* Enable the DMA reception */
	ETH->DMAOMR |= ETH_DMAOMR_SR;

	__NVIC_EnableIRQ(ETH_IRQn);
	__NVIC_SetPriority(ETH_IRQn, 0);
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

	ETH->DMARDLAR = (uint32_t) &DMARxDscrTab[0];
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
	ETH->DMATDLAR = (uint32_t) &DMATxDscrTab[0];
}

extern "C" void ETH_IRQHandler(void) {

	uint32_t DMASR = ETH->DMASR;
	uint32_t DMAIER = ETH->DMAIER;

	/*RX interrupt*/
	if ((DMASR & ETH_DMASR_RS) && (DMAIER & ETH_DMAIER_RIE)) {
		ETH->DMASR = ETH_DMASR_RS | ETH_DMASR_NIS;
		eth_rx_event = 1;
	}
	/*TX interrupt*/
	if ((DMASR & ETH_DMASR_TS) && (DMAIER & ETH_DMAIER_TIE)) {
		ETH->DMASR = ETH_DMASR_TS | ETH_DMASR_NIS;

		TxDescList.CurrTailNum = (TxDescList.CurrTailNum + 1) % ETH_TX_DESC_CNT;
//		TxDescList.pBuff = &Tx_Buff[TxDescList.CurrDescNum][0];
		TxDescList.CurrTxDesc = TxDescList.TxDesc[TxDescList.CurrTailNum];
		eth_tx_event = 1;
	}
}

void ETH_RxWorker(void) {

	static uint32_t start_tick = get_tick();
	if((get_tick()-start_tick) >= 100) {
		NET_TCP_Timers();
		start_tick = get_tick();
	}
	// начинаем с индекса последнего обработанного дескриптора
	eth_rx_event = 0;
	uint32_t idx = RxDescList.CurrDescNum;

	while (1) {
		uint32_t desc0 = DMARxDscrTab[idx].DESC0;

		if (desc0 & ETH_DMARXDESC_OWN)
			break;
		// проверим, что это полный кадр в одном дескрипторе (FS+LS)
		if ((desc0 & (ETH_DMARXDESC_FS | ETH_DMARXDESC_LS))
				== (ETH_DMARXDESC_FS | ETH_DMARXDESC_LS)) {

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

		idx = (idx + 1) % ETH_RX_DESC_CNT;
	}
	RxDescList.CurrDescNum = idx;

}

void ETH_SendFrame(uint32_t len) {
	TxDescList.TxDesc[TxDescList.CurrHeadNum]->DESC1 =
			(len & ETH_DMATXDESC_TBS1); // first address chained

	TxDescList.TxDesc[TxDescList.CurrHeadNum]->DESC0 = ETH_DMATXDESC_OWN
			| ETH_DMATXDESC_TCH | ETH_DMATXDESC_FS | ETH_DMATXDESC_LS
			| ETH_DMATXDESC_IC; // Owned by DMA
	__DMB();
	ETH->DMATPDR = 0; // запускаем DMA
	TxDescList.CurrHeadNum = ((TxDescList.CurrHeadNum + 1) % ETH_TX_DESC_CNT);
	TxDescList.pBuff = &Tx_Buff[TxDescList.CurrHeadNum][0];
}

void PHY_Init(void) {

	uint32_t bcr = 0;
	uint32_t bsr = 0;
	uint32_t regval = 0;
	uint32_t tickstart = 0;
	/* Reset PHY */
	PHY_Write(PHY_ADDR, PHY_BCR, PHY_RESET);
	tickstart = get_tick();

	while (bcr & (PHY_RESET)) {
		if ((get_tick() - tickstart) > 5000) {
			return;
		}
		PHY_Read(PHY_ADDR, PHY_BCR, &bcr);
	}
	/* Enable autoneg */
	PHY_Write(PHY_ADDR, PHY_BCR, PHY_AUTONEG_EN);
	tickstart = get_tick();

	while (!(bsr & (PHY_AUTONEG_DONE))) {
		if ((get_tick() - tickstart) > 50000) {
			return;
		}
		PHY_Read(PHY_ADDR, PHY_BSR, &bsr);
	}
	/* Wait LINK Status */
	tickstart = get_tick();
	while ((regval & PHY_LINKED_STATUS) != PHY_LINKED_STATUS) {
		if ((get_tick() - tickstart) > 5000) {
			return;
		}
		PHY_Read(PHY_ADDR, PHY_BSR, &regval);
	}
}

void PHY_SetMDIOClock(void) {

	uint32_t hclk;
	uint32_t tmpreg;

	tmpreg = ETH->MACMIIAR;
	tmpreg &= ~ETH_MACMIIAR_CR_Msk;
	hclk = SystemCoreClock;

	if (hclk < 35000000U) {
		tmpreg |= ETH_MACMIIAR_CR_Div16;
	} else if (hclk < 60000000U) {
		tmpreg |= ETH_MACMIIAR_CR_Div26;
	} else if (hclk < 100000000U) {
		tmpreg |= ETH_MACMIIAR_CR_Div42;
	} else if (hclk < 150000000U) {
		tmpreg |= ETH_MACMIIAR_CR_Div62;
	} else {
		tmpreg |= ETH_MACMIIAR_CR_Div102;
	}

	ETH->MACMIIAR = tmpreg;
}

void PHY_Read(uint32_t phy_addr, uint32_t phy_reg, uint32_t *pRegValue) {

	int32_t tmpreg;
	int32_t tickstart;
	/* Get the ETHERNET MACMIIAR value */
	tmpreg = ETH->MACMIIAR & ETH_MACMIIAR_CR_Msk;
	/* Prepare the MII address register value */
	tmpreg |= ((phy_addr << 11U) & ETH_MACMIIAR_PA); /* Set the PHY device address   */
	tmpreg |= ((phy_reg << 6U) & ETH_MACMIIAR_MR); /* Set the PHY register address */
	tmpreg &= ~ETH_MACMIIAR_MW; /* Set the read mode            */
	tmpreg |= ETH_MACMIIAR_MB; /* Set the MII Busy bit         */
	/* Write the result value into the MII Address register */
	ETH->MACMIIAR = tmpreg;
	/* Check for the Busy flag */
	tickstart = get_tick();
	while ((tmpreg & ETH_MACMIIAR_MB) == ETH_MACMIIAR_MB) {
		tmpreg = ETH->MACMIIAR;
		if ((get_tick() - tickstart) > 5000)
			return;
	}
	*pRegValue = (uint16_t) (ETH->MACMIIDR & 0xFFFFU);
}

void PHY_Write(uint32_t phy_addr, uint32_t phy_reg, uint32_t RegValue) {

	int32_t tmpreg;
	int32_t tickstart;
	/* Get the ETHERNET MACMIIAR value */
	tmpreg = ETH->MACMIIAR & ETH_MACMIIAR_CR_Msk;
	/* Prepare the MII register address value */
	tmpreg |= ((phy_addr << 11U) & ETH_MACMIIAR_PA); /* Set the PHY device address */
	tmpreg |= (((uint32_t) phy_reg << 6U) & ETH_MACMIIAR_MR); /* Set the PHY register address */
	tmpreg |= ETH_MACMIIAR_MW; /* Set the write mode */
	tmpreg |= ETH_MACMIIAR_MB; /* Set the MII Busy bit */
	/* Give the value to the MII data register */
	ETH->MACMIIDR = (uint16_t) RegValue;
	/* Write the result value into the MII Address register */
	ETH->MACMIIAR = tmpreg;
	/* Check for the Busy flag */
	tickstart = get_tick();
	while ((tmpreg & ETH_MACMIIAR_MB) == ETH_MACMIIAR_MB) {
		tmpreg = ETH->MACMIIAR;
		if ((get_tick() - tickstart) > 5000)
			return;
	}

}
