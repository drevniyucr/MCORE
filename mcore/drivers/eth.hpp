#pragma once

#include"core/def.hpp"

constexpr uint32_t ETH_TX_DESC_CNT = 8;
constexpr uint32_t ETH_RX_DESC_CNT = 8;
constexpr uint32_t ETH_MAX_PACKET_SIZE = 1536;

/*DMA TX Descriptor status register`s defenition*/
constexpr uint32_t ETH_DMATXDESC_OWN                     = 0x80000000;  /*!< OWN bit: descriptor is owned by DMA engine */
constexpr uint32_t ETH_DMATXDESC_IC                      = 0x40000000;  /*!< Interrupt on Completion */
constexpr uint32_t ETH_DMATXDESC_LS                      = 0x20000000;  /*!< Last Segment */
constexpr uint32_t ETH_DMATXDESC_FS                      = 0x10000000;  /*!< First Segment */
constexpr uint32_t ETH_DMATXDESC_DC                      = 0x08000000;  /*!< Disable CRC */
constexpr uint32_t ETH_DMATXDESC_DP                      = 0x04000000;  /*!< Disable Padding */
constexpr uint32_t ETH_DMATXDESC_TTSE                    = 0x02000000;  /*!< Transmit Time Stamp Enable */
constexpr uint32_t ETH_DMATXDESC_CIC                     = 0x00C00000;  /*!< Checksum Insertion Control: 4 cases */
constexpr uint32_t ETH_DMATXDESC_CIC_BYPASS              = 0x00000000;  /*!< Do Nothing: Checksum Engine is bypassed */
constexpr uint32_t ETH_DMATXDESC_CIC_IPV4HEADER          = 0x00400000;  /*!< IPV4 header Checksum Insertion */
constexpr uint32_t ETH_DMATXDESC_CIC_TCPUDPICMP_SEGMENT  = 0x00800000;  /*!< TCP/UDP/ICMP Checksum Insertion calculated over segment only */
constexpr uint32_t ETH_DMATXDESC_CIC_TCPUDPICMP_FULL     = 0x00C00000;  /*!< TCP/UDP/ICMP Checksum Insertion fully calculated */
constexpr uint32_t ETH_DMATXDESC_TER                     = 0x00200000;  /*!< Transmit End of Ring */
constexpr uint32_t ETH_DMATXDESC_TCH                     = 0x00100000;  /*!< Second Address Chained */
constexpr uint32_t ETH_DMATXDESC_TTSS                    = 0x00020000;  /*!< Tx Time Stamp Status */
constexpr uint32_t ETH_DMATXDESC_IHE                     = 0x00010000;  /*!< IP Header Error */
constexpr uint32_t ETH_DMATXDESC_ES                      = 0x00008000;  /*!< Error summary: OR of the following bits: UE || ED || EC || LCO || NC || LCA || FF || JT */
constexpr uint32_t ETH_DMATXDESC_JT                      = 0x00004000;  /*!< Jabber Timeout */
constexpr uint32_t ETH_DMATXDESC_FF                      = 0x00002000;  /*!< Frame Flushed: DMA/MTL flushed the frame due to SW flush */
constexpr uint32_t ETH_DMATXDESC_PCE                     = 0x00001000;  /*!< Payload Checksum Error */
constexpr uint32_t ETH_DMATXDESC_LCA                     = 0x00000800;  /*!< Loss of Carrier: carrier lost during transmission */
constexpr uint32_t ETH_DMATXDESC_NC                      = 0x00000400;  /*!< No Carrier: no carrier signal from the transceiver */
constexpr uint32_t ETH_DMATXDESC_LCO                     = 0x00000200;  /*!< Late Collision: transmission aborted due to collision */
constexpr uint32_t ETH_DMATXDESC_EC                      = 0x00000100;  /*!< Excessive Collision: transmission aborted after 16 collisions */
constexpr uint32_t ETH_DMATXDESC_VF                      = 0x00000080;  /*!< VLAN Frame */
constexpr uint32_t ETH_DMATXDESC_CC                      = 0x00000078;  /*!< Collision Count */
constexpr uint32_t ETH_DMATXDESC_ED                      = 0x00000004;  /*!< Excessive Deferral */
constexpr uint32_t ETH_DMATXDESC_UF                      = 0x00000002;  /*!< Underflow Error: late data arrival from the memory */
constexpr uint32_t ETH_DMATXDESC_DB                      = 0x00000001;  /*!< Deferred Bit */


/* Bit definition of TDES1 register*/
constexpr uint32_t ETH_DMATXDESC_TBS2                    = 0x1FFF0000;  /*!< Transmit Buffer2 Size */
constexpr uint32_t ETH_DMATXDESC_TBS1                    = 0x00001FFF;  /*!< Transmit Buffer1 Size */

/*DMA RX Descriptor status register`s defenition*/
constexpr uint32_t ETH_DMARXDESC_OWN         = 0x80000000;  /*!< OWN bit: descriptor is owned by DMA engine  */
constexpr uint32_t ETH_DMARXDESC_AFM         = 0x40000000;  /*!< DA Filter Fail for the rx frame  */
constexpr uint32_t ETH_DMARXDESC_FL          = 0x3FFF0000;  /*!< Receive descriptor frame length  */
constexpr uint32_t ETH_DMARXDESC_ES          = 0x00008000;  /*!< Error summary: OR of the following bits: DE || OE || IPC || LC || RWT || RE || CE */
constexpr uint32_t ETH_DMARXDESC_DE          = 0x00004000;  /*!< Descriptor error: no more descriptors for receive frame  */
constexpr uint32_t ETH_DMARXDESC_SAF         = 0x00002000;  /*!< SA Filter Fail for the received frame */
constexpr uint32_t ETH_DMARXDESC_LE          = 0x00001000;  /*!< Frame size not matching with length field */
constexpr uint32_t ETH_DMARXDESC_OE          = 0x00000800;  /*!< Overflow Error: Frame was damaged due to buffer overflow */
constexpr uint32_t ETH_DMARXDESC_VLAN        = 0x00000400;  /*!< VLAN Tag: received frame is a VLAN frame */
constexpr uint32_t ETH_DMARXDESC_FS          = 0x00000200;  /*!< First descriptor of the frame  */
constexpr uint32_t ETH_DMARXDESC_LS          = 0x00000100;  /*!< Last descriptor of the frame  */
constexpr uint32_t ETH_DMARXDESC_IPV4HCE     = 0x00000080;  /*!< IPC Checksum Error: Rx Ipv4 header checksum error   */
constexpr uint32_t ETH_DMARXDESC_LC          = 0x00000040;  /*!< Late collision occurred during reception   */
constexpr uint32_t ETH_DMARXDESC_FT          = 0x00000020;  /*!< Frame type - Ethernet, otherwise 802.3    */
constexpr uint32_t ETH_DMARXDESC_RWT         = 0x00000010;  /*!< Receive Watchdog Timeout: watchdog timer expired during reception    */
constexpr uint32_t ETH_DMARXDESC_RE          = 0x00000008;  /*!< Receive error: error reported by MII interface  */
constexpr uint32_t ETH_DMARXDESC_DBE         = 0x00000004;  /*!< Dribble bit error: frame contains non int multiple of 8 bits  */
constexpr uint32_t ETH_DMARXDESC_CE          = 0x00000002;  /*!< CRC error */
constexpr uint32_t ETH_DMARXDESC_MAMPCE      = 0x00000001;  /*!< Rx MAC Address/Payload Checksum Error: Rx MAC address matched/ Rx Payload Checksum Error */


 /* Bit definition of RDES1 register*/
constexpr uint32_t ETH_DMARXDESC_DIC         = 0x80000000;  /*!< Disable Interrupt on Completion */
constexpr uint32_t ETH_DMARXDESC_RBS2        = 0x1FFF0000;  /*!< Receive Buffer2 Size */
constexpr uint32_t ETH_DMARXDESC_RER         = 0x00008000;  /*!< Receive End of Ring */
constexpr uint32_t ETH_DMARXDESC_RCH         = 0x00004000;  /*!< Second Address Chained */
constexpr uint32_t ETH_DMARXDESC_RBS1        = 0x00001FFF;  /*!< Receive Buffer1 Size */

/* Bit definition of RDES4 register */
constexpr uint32_t ETH_DMAPTPRXDESC_PTPV                            = 0x00002000;  /* PTP Version */
constexpr uint32_t ETH_DMAPTPRXDESC_PTPFT                           = 0x00001000;  /* PTP Frame Type */
constexpr uint32_t ETH_DMAPTPRXDESC_PTPMT                           = 0x00000F00;  /* PTP Message Type */
constexpr uint32_t ETH_DMAPTPRXDESC_PTPMT_SYNC                      = 0x00000100;  /* SYNC message(all clock types) */
constexpr uint32_t ETH_DMAPTPRXDESC_PTPMT_FOLLOWUP                  = 0x00000200;  /* FollowUp message (all clock types) */
constexpr uint32_t ETH_DMAPTPRXDESC_PTPMT_DELAYREQ                  = 0x00000300;  /* DelayReq message(all clock types) */
constexpr uint32_t ETH_DMAPTPRXDESC_PTPMT_DELAYRESP                 = 0x00000400;  /* DelayResp message(all clock types) */
constexpr uint32_t ETH_DMAPTPRXDESC_PTPMT_PDELAYREQ_ANNOUNCE        = 0x00000500;  /* PdelayReq message(peer-to-peer transparent clock)or Announce message (Ordinaryor Boundary clock) */
constexpr uint32_t ETH_DMAPTPRXDESC_PTPMT_PDELAYRESP_MANAG          = 0x00000600;  /* PdelayResp message(peer-to-peer transparent clock)or Management message (Ordinaryor Boundary clock)  */
constexpr uint32_t ETH_DMAPTPRXDESC_PTPMT_PDELAYRESPFOLLOWUP_SIGNAL = 0x00000700;  /* PdelayRespFollowUp message (peer-to-peer transparent clock)or Signaling message (Ordinaryor Boundary clock) or Boundary clock) */
constexpr uint32_t ETH_DMAPTPRXDESC_IPV6PR                          = 0x00000080;  /* IPv6 Packet Received */
constexpr uint32_t ETH_DMAPTPRXDESC_IPV4PR                          = 0x00000040;  /* IPv4 Packet Received */
constexpr uint32_t ETH_DMAPTPRXDESC_IPCB                            = 0x00000020;  /* IP Checksum Bypassed */
constexpr uint32_t ETH_DMAPTPRXDESC_IPPE                            = 0x00000010;  /* IP Payload Error */
constexpr uint32_t ETH_DMAPTPRXDESC_IPHE                            = 0x00000008;  /* IP Header Error */
constexpr uint32_t ETH_DMAPTPRXDESC_IPPT                            = 0x00000007;  /* IP Payload Type */
constexpr uint32_t ETH_DMAPTPRXDESC_IPPT_UDP                        = 0x00000001;  /* UDP payload encapsulated in the IP datagram */
constexpr uint32_t ETH_DMAPTPRXDESC_IPPT_TCP                        = 0x00000002;  /* TCP payload encapsulated in the IP datagram */
constexpr uint32_t ETH_DMAPTPRXDESC_IPPT_ICMP                       = 0x00000003;  /* ICMP payload encapsulated in the IP datagram */
constexpr uint32_t ETH_DMATXDESC_CHECKSUMTCPUDPICMPFULL             = 0x00C00000;


constexpr uint32_t HEAP_POINTER = 0x20078000;


constexpr uint32_t PHY_ADDR            = 0x00;
constexpr uint32_t PHY_AUTONEG_EN      = (1U << 12);
constexpr uint32_t PHY_RESTART_AUTONEG = (1U << 9);
constexpr uint32_t PHY_AUTONEG_DONE    = (1U << 5);

enum class MACMIIAR_CR : uint8_t {
	Div42  = 0b000, /* HCLK:60-100 MHz; MDC clock= HCLK/42 */
	Div62  = 0b001, /* HCLK:100-150 MHz; MDC clock= HCLK/62 */
	Div16  = 0b010, /* HCLK:20-35 MHz; MDC clock= HCLK/16 */
	Div26  = 0b011, /* HCLK:35-60 MHz; MDC clock= HCLK/26 */
	Div102 = 0b100  /* HCLK:150-168 MHz; MDC clock= HCLK/102 */
};

enum class DMABMR_PBL : uint16_t {
	_1Beat   = 0x01,
	_2Beats  = 0x02,
	_4Beats  = 0x04,
	_8Beats  = 0x08,
	_16Beats = 0x10,
	_32Beats = 0x20
};

enum class MACFCR_PLT : uint8_t {
	Minus4   = 0b00, /* Pause time minus 4 slot times */
	Minus28  = 0b01, /* Pause time minus 28 slot times */
	Minus144 = 0b10, /* Pause time minus 144 slot times */
	Minus246 = 0b11  /* Pause time minus 246 slot times */
};
enum class MACCR_IFG : uint8_t {
	_96bit = 0b000,/* interframe gap 96 bit times */
	_88bit = 0b001,/* interframe gap 88 bit times */
	_80bit = 0b010,/* interframe gap 80 bit times */
	_72bit = 0b011,/* interframe gap 72 bit times */
	_64bit = 0b100,/* interframe gap 64 bit times */
	_56bit = 0b101,/* interframe gap 56 bit times */
	_48bit = 0b110,/* interframe gap 48 bit times */
	_40bit = 0b111 /* interframe gap 40 bit times */
};

enum class ETH_Status:uint8_t {
	OK = 0,
	RESET_FAULT = 1,
	PHY_RESET_FAULT = 2,
	PHY_AUTONEG_FAULT = 3,
	PHY_LINK_DOWN = 4,
	PHY_ID_FAULT = 5,
	UNEXPECT_FAULT = 6
};

struct ETH_DMADescStruct{
	volatile uint32_t DESC0; /* Status */
	volatile uint32_t DESC1; /* Control */
	volatile uint32_t DESC2; /* Buffer1 Addr */
	volatile uint32_t DESC3; /* Buffer2/Next Desc */
	volatile uint32_t DESC4;
	volatile uint32_t DESC5;
	volatile uint32_t DESC6;
	volatile uint32_t DESC7;
};

struct ETH_TxDescListStruct{
	ETH_DMADescStruct *TxDesc[ETH_TX_DESC_CNT];
	ETH_DMADescStruct *CurrTxDesc;
	uint32_t BuffLen;
	uint8_t CurrHeadNum;
	uint8_t CurrTailNum;
	uint8_t* pBuff;

};

struct ETH_RxDescListStruct{
	ETH_DMADescStruct *RxDesc[ETH_RX_DESC_CNT];
	ETH_DMADescStruct *CurrRxDesc;
	uint32_t CurrDescNum;
	uint8_t* pBuff;
	uint32_t BuffLen;
};

struct ETH_LinkState {
	    uint8_t speed;     // 0=10M,1=100M
	    uint8_t duplex;    // 0=half,1=full
};


extern ETH_TxDescListStruct TxDescList;


/* Глобальные Rx/Tx дескрипторы */
ETH_Status ETH_Init(void);
void ETH_RxDescInit(void);
void ETH_TxDescInit(void);
void ETH_RxWorker(void);
bool ETH_SendFrame(uint32_t len);
bool ETH_IsTxBufferAvailable(void);

ETH_Status PHY_Init(ETH_LinkState *lnk);
bool PHY_Write(uint32_t phy_addr, uint32_t phy_reg, uint32_t RegValue);
bool PHY_Read(uint32_t phy_addr, uint32_t phy_reg, uint32_t *pRegValue);
void PHY_SetMDIOClock(void);


