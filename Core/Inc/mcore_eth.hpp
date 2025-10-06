/*
 * mcore_eth.hpp
 *
 *  Created on: Sep 18, 2025
 *      Author: AkimovMA
 */
#pragma once

#include"mcore_def.hpp"

#define ETH_TX_DESC_CNT 4U
#define ETH_RX_DESC_CNT 8U
#define ETH_MAX_PACKET_SIZE 1536U

/*DMA TX Descriptor status register`s defenition*/
#define ETH_DMATXDESC_OWN                     0x80000000U  /*!< OWN bit: descriptor is owned by DMA engine */
#define ETH_DMATXDESC_IC                      0x40000000U  /*!< Interrupt on Completion */
#define ETH_DMATXDESC_LS                      0x20000000U  /*!< Last Segment */
#define ETH_DMATXDESC_FS                      0x10000000U  /*!< First Segment */
#define ETH_DMATXDESC_DC                      0x08000000U  /*!< Disable CRC */
#define ETH_DMATXDESC_DP                      0x04000000U  /*!< Disable Padding */
#define ETH_DMATXDESC_TTSE                    0x02000000U  /*!< Transmit Time Stamp Enable */
#define ETH_DMATXDESC_CIC                     0x00C00000U  /*!< Checksum Insertion Control: 4 cases */
#define ETH_DMATXDESC_CIC_BYPASS              0x00000000U  /*!< Do Nothing: Checksum Engine is bypassed */
#define ETH_DMATXDESC_CIC_IPV4HEADER          0x00400000U  /*!< IPV4 header Checksum Insertion */
#define ETH_DMATXDESC_CIC_TCPUDPICMP_SEGMENT  0x00800000U  /*!< TCP/UDP/ICMP Checksum Insertion calculated over segment only */
#define ETH_DMATXDESC_CIC_TCPUDPICMP_FULL     0x00C00000U  /*!< TCP/UDP/ICMP Checksum Insertion fully calculated */
#define ETH_DMATXDESC_TER                     0x00200000U  /*!< Transmit End of Ring */
#define ETH_DMATXDESC_TCH                     0x00100000U  /*!< Second Address Chained */
#define ETH_DMATXDESC_TTSS                    0x00020000U  /*!< Tx Time Stamp Status */
#define ETH_DMATXDESC_IHE                     0x00010000U  /*!< IP Header Error */
#define ETH_DMATXDESC_ES                      0x00008000U  /*!< Error summary: OR of the following bits: UE || ED || EC || LCO || NC || LCA || FF || JT */
#define ETH_DMATXDESC_JT                      0x00004000U  /*!< Jabber Timeout */
#define ETH_DMATXDESC_FF                      0x00002000U  /*!< Frame Flushed: DMA/MTL flushed the frame due to SW flush */
#define ETH_DMATXDESC_PCE                     0x00001000U  /*!< Payload Checksum Error */
#define ETH_DMATXDESC_LCA                     0x00000800U  /*!< Loss of Carrier: carrier lost during transmission */
#define ETH_DMATXDESC_NC                      0x00000400U  /*!< No Carrier: no carrier signal from the transceiver */
#define ETH_DMATXDESC_LCO                     0x00000200U  /*!< Late Collision: transmission aborted due to collision */
#define ETH_DMATXDESC_EC                      0x00000100U  /*!< Excessive Collision: transmission aborted after 16 collisions */
#define ETH_DMATXDESC_VF                      0x00000080U  /*!< VLAN Frame */
#define ETH_DMATXDESC_CC                      0x00000078U  /*!< Collision Count */
#define ETH_DMATXDESC_ED                      0x00000004U  /*!< Excessive Deferral */
#define ETH_DMATXDESC_UF                      0x00000002U  /*!< Underflow Error: late data arrival from the memory */
#define ETH_DMATXDESC_DB                      0x00000001U  /*!< Deferred Bit */


/* Bit definition of TDES1 register*/
#define ETH_DMATXDESC_TBS2                    0x1FFF0000U  /*!< Transmit Buffer2 Size */
#define ETH_DMATXDESC_TBS1                    0x00001FFFU  /*!< Transmit Buffer1 Size */

/*DMA RX Descriptor status register`s defenition*/
#define ETH_DMARXDESC_OWN         0x80000000U  /*!< OWN bit: descriptor is owned by DMA engine  */
#define ETH_DMARXDESC_AFM         0x40000000U  /*!< DA Filter Fail for the rx frame  */
#define ETH_DMARXDESC_FL          0x3FFF0000U  /*!< Receive descriptor frame length  */
#define ETH_DMARXDESC_ES          0x00008000U  /*!< Error summary: OR of the following bits: DE || OE || IPC || LC || RWT || RE || CE */
#define ETH_DMARXDESC_DE          0x00004000U  /*!< Descriptor error: no more descriptors for receive frame  */
#define ETH_DMARXDESC_SAF         0x00002000U  /*!< SA Filter Fail for the received frame */
#define ETH_DMARXDESC_LE          0x00001000U  /*!< Frame size not matching with length field */
#define ETH_DMARXDESC_OE          0x00000800U  /*!< Overflow Error: Frame was damaged due to buffer overflow */
#define ETH_DMARXDESC_VLAN        0x00000400U  /*!< VLAN Tag: received frame is a VLAN frame */
#define ETH_DMARXDESC_FS          0x00000200U  /*!< First descriptor of the frame  */
#define ETH_DMARXDESC_LS          0x00000100U  /*!< Last descriptor of the frame  */
#define ETH_DMARXDESC_IPV4HCE     0x00000080U  /*!< IPC Checksum Error: Rx Ipv4 header checksum error   */
#define ETH_DMARXDESC_LC          0x00000040U  /*!< Late collision occurred during reception   */
#define ETH_DMARXDESC_FT          0x00000020U  /*!< Frame type - Ethernet, otherwise 802.3    */
#define ETH_DMARXDESC_RWT         0x00000010U  /*!< Receive Watchdog Timeout: watchdog timer expired during reception    */
#define ETH_DMARXDESC_RE          0x00000008U  /*!< Receive error: error reported by MII interface  */
#define ETH_DMARXDESC_DBE         0x00000004U  /*!< Dribble bit error: frame contains non int multiple of 8 bits  */
#define ETH_DMARXDESC_CE          0x00000002U  /*!< CRC error */
#define ETH_DMARXDESC_MAMPCE      0x00000001U  /*!< Rx MAC Address/Payload Checksum Error: Rx MAC address matched/ Rx Payload Checksum Error */


 /* Bit definition of RDES1 register*/
#define ETH_DMARXDESC_DIC         0x80000000U  /*!< Disable Interrupt on Completion */
#define ETH_DMARXDESC_RBS2        0x1FFF0000U  /*!< Receive Buffer2 Size */
#define ETH_DMARXDESC_RER         0x00008000U  /*!< Receive End of Ring */
#define ETH_DMARXDESC_RCH         0x00004000U  /*!< Second Address Chained */
#define ETH_DMARXDESC_RBS1        0x00001FFFU  /*!< Receive Buffer1 Size */

/* Bit definition of RDES4 register */
#define ETH_DMAPTPRXDESC_PTPV                            0x00002000U  /* PTP Version */
#define ETH_DMAPTPRXDESC_PTPFT                           0x00001000U  /* PTP Frame Type */
#define ETH_DMAPTPRXDESC_PTPMT                           0x00000F00U  /* PTP Message Type */
#define ETH_DMAPTPRXDESC_PTPMT_SYNC                      0x00000100U  /* SYNC message
                                                                                   (all clock types) */
#define ETH_DMAPTPRXDESC_PTPMT_FOLLOWUP                  0x00000200U  /* FollowUp message
                                                                                   (all clock types) */
#define ETH_DMAPTPRXDESC_PTPMT_DELAYREQ                  0x00000300U  /* DelayReq message
                                                                                   (all clock types) */
#define ETH_DMAPTPRXDESC_PTPMT_DELAYRESP                 0x00000400U  /* DelayResp message
                                                                                   (all clock types) */
#define ETH_DMAPTPRXDESC_PTPMT_PDELAYREQ_ANNOUNCE        0x00000500U  /* PdelayReq message
                                                                                   (peer-to-peer transparent clock)
                                                                                    or Announce message (Ordinary
                                                                                    or Boundary clock) */
#define ETH_DMAPTPRXDESC_PTPMT_PDELAYRESP_MANAG          0x00000600U  /* PdelayResp message
                                                                                   (peer-to-peer transparent clock)
                                                                                    or Management message (Ordinary
                                                                                    or Boundary clock)  */
#define ETH_DMAPTPRXDESC_PTPMT_PDELAYRESPFOLLOWUP_SIGNAL 0x00000700U  /* PdelayRespFollowUp message
                                                                                  (peer-to-peer transparent clock)
                                                                                   or Signaling message (Ordinary
                                                                                   or Boundary clock) */
#define ETH_DMAPTPRXDESC_IPV6PR                          0x00000080U  /* IPv6 Packet Received */
#define ETH_DMAPTPRXDESC_IPV4PR                          0x00000040U  /* IPv4 Packet Received */
#define ETH_DMAPTPRXDESC_IPCB                            0x00000020U  /* IP Checksum Bypassed */
#define ETH_DMAPTPRXDESC_IPPE                            0x00000010U  /* IP Payload Error */
#define ETH_DMAPTPRXDESC_IPHE                            0x00000008U  /* IP Header Error */
#define ETH_DMAPTPRXDESC_IPPT                            0x00000007U  /* IP Payload Type */
#define ETH_DMAPTPRXDESC_IPPT_UDP                        0x00000001U  /* UDP payload encapsulated in
                                                                                   the IP datagram */
#define ETH_DMAPTPRXDESC_IPPT_TCP                        0x00000002U  /* TCP payload encapsulated in
                                                                                   the IP datagram */
#define ETH_DMAPTPRXDESC_IPPT_ICMP                       0x00000003U  /* ICMP payload encapsulated in
                                                                                     the IP datagram */

#define ETH_DMATXDESC_CHECKSUMTCPUDPICMPFULL     0x00C00000U


#define HEAP_POINTER 0x20078000


#define PHY_ADDR        0x00U
#define PHY_AUTONEG_EN  (1U << 12)
#define PHY_AUTONEG_DONE (1U << 5)


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




extern ETH_TxDescListStruct TxDescList;


/* Глобальные Rx/Tx дескрипторы */
void ETH_Init(void);
void ETH_RxDescInit(void);
void ETH_TxDescInit(void);
void ETH_RxWorker(void);
void ETH_SendFrame(uint32_t len);

void PHY_Init(void);
void PHY_Write(uint32_t phy_addr, uint32_t phy_reg, uint32_t RegValue);
void PHY_Read(uint32_t phy_addr, uint32_t phy_reg, uint32_t *pRegValue);
void PHY_SetMDIOClock(void);


