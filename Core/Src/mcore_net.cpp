/*
 * mcore_net.c
 *
 *  Created on: Sep 24, 2025
 *      Author: AkimovMA
 */
#include <cstring>
#include "mcore_tcp.hpp"
#include "mcore_net.hpp"
#include "mcore_eth_utils.hpp"
#include "mcore_eth.hpp"
//#include "mcore_temp_frame.hpp"

uint8_t MAC_ADDR[MAC_ADDR_LEN] = { 0x02, 0x12, 0x34, 0x56, 0x78, 0x9A };
uint8_t IP_ADDR[IP_ADDR_LEN] = { 192, 168, 0, 123 };

uint8_t icmp_port_unrchble_template[ICMP_UNRCHBLE_FRAME_LEN] = {
		// Ethernet Header (14B)
		0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, // dst MAC (будет заменено)
		MAC_ADDR[0], MAC_ADDR[1], MAC_ADDR[2], MAC_ADDR[3], MAC_ADDR[4],
		MAC_ADDR[5],   // src MAC = my MAC
		0x08, 0x00,                         // Ethertype = IPv4

		// IP Header (20B)
		0x45, 0x00,                         // Version/IHL, TOS
		0x00, 0x2E,               // Total Length = 52B (20 IP + 8 ICMP + 8 UDP)
		0x00, 0x00,                         // Identification
		0x00, 0x00,                         // Flags + Fragment
		0x40,                               // TTL = 64
		0x01,                               // Protocol = ICMP
		0x00, 0x00,               // Header checksum (вычислять перед отправкой)
		IP_ADDR[0], IP_ADDR[1], IP_ADDR[2], IP_ADDR[3], // Src IP = my IP (192.168.1.50)
		0x00, 0x00, 0x00, 0x00, // Dst IP (будет заменено на src IP отправителя)

		// ICMP Header (8B)
		0x03, 0x03,                         // Type=3, Code=3 (Port Unreachable)
		0x00, 0x00,                      // Checksum (вычислять перед отправкой)
		0x00, 0x00, 0x00, 0x00,             // Unused

		// ICMP Data (первые 8 байт оригинального UDP пакета)
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

uint8_t UDPsendFrameTemplate[UDP_TEMPLATE_FRAME_LEN] = {
		// Ethernet Header (14B)
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,         // dst MAC (будет заменено)
		MAC_ADDR[0], MAC_ADDR[1], MAC_ADDR[2],
		MAC_ADDR[3], MAC_ADDR[4], MAC_ADDR[5],   // src MAC = my MAC
		0x08, 0x00,                             // Ethertype = IPv4

		// IP Header (20B)
		0x45, 0x00,                            // Version/IHL, TOS
		0x00, 0x00,   // Total Length (IP+UDP header+payload, будет обновляться)
		0x00, 0x00,                            // Identification
		0x00, 0x00,                            // Flags + Fragment
		0x40,                                  // TTL
		0x11,                                  // Protocol = UDP
		0x00, 0x00,                     // IP checksum (обнулять перед расчетом)
		IP_ADDR[0], IP_ADDR[1], IP_ADDR[2], IP_ADDR[3], // Src IP
		0x00, 0x00, 0x00, 0x00,                   // Dst IP (будет заменено)

		// UDP Header (8B)
		0x13, 0x88,                             // Src port (5000, можно менять)
		0x00, 0x00,                             // Dst port (будет заменено)
		0x00, 0x00,                  // UDP length (header+payload, обновляется)
		0x00, 0x00,                            // UDP checksum (можно обнулить)
		// Payload (заполняется динамически, здесь пока нули)
		};

void NET_ProcessRx(ETH_RxDescListStruct* RxDesc, ETH_TxDescListStruct* TxDesc) {

	if (RxDesc->BuffLen < 14)
		return;

	uint16_t ethertype = (RxDesc->pBuff[12] << 8) | RxDesc->pBuff[13];
	ipv4_frame ip_frame;

	if (ethertype == 0x0806) {
		ip_frame.rx_buff    =  RxDesc->pBuff;
		ip_frame.tx_buff    = &(TxDesc->pBuff);
		ip_frame.scr_mac    = &RxDesc->pBuff[22];
		ip_frame.scr_addr   = &RxDesc->pBuff[28];
		ip_frame.frame_len = (uint16_t) RxDesc->BuffLen;
		NET_ProcessARP(&ip_frame);
		return;
	}

	if (RxDesc->BuffLen < 34)
		return; // IP + ICMP minimal

	// destination ip check
	if (memcmp(&(RxDesc->pBuff[DST_ADDR_POS]), IP_ADDR, 4) != 0)
		return;

	uint32_t ExtStatus = RxDesc->CurrRxDesc->DESC4;
	//check IPv4 and checksum
	if ((ExtStatus & ETH_DMAPTPRXDESC_IPV4PR)
			&& !(ExtStatus & ETH_DMAPTPRXDESC_IPPE)) {
		ip_frame.rx_buff    =  RxDesc->pBuff;
		ip_frame.tx_buff    = &(TxDesc->pBuff);
		ip_frame.ip_hdr     = &RxDesc->pBuff[IP_HDR_POS];
		ip_frame.ip_hdr_len = (ip_frame.ip_hdr[0] & 0x0F) * 4;
		ip_frame.ip_len = (ip_frame.ip_hdr[2]<<8|ip_frame.ip_hdr[3]);
		ip_frame.scr_addr   = &RxDesc->pBuff[SCR_ADDR_POS];
		ip_frame.scr_mac    = &RxDesc->pBuff[6];
		// check ICMP
		if ((ExtStatus & ETH_DMAPTPRXDESC_IPPT_ICMP)
				== ETH_DMAPTPRXDESC_IPPT_ICMP) {
			NET_ProcessICMP(&ip_frame);
			return;
		}
		// check UDP
		if ((ExtStatus & ETH_DMAPTPRXDESC_IPPT_UDP) == ETH_DMAPTPRXDESC_IPPT_UDP) {
			NET_ProcessUDP(&ip_frame);
			return;
		}
		// check TCP
		if ((ExtStatus & ETH_DMAPTPRXDESC_IPPT_TCP) == ETH_DMAPTPRXDESC_IPPT_TCP) {
			NET_ProcessTCP(&ip_frame); // когда нибудь
			return;
		}
	}
}


void NET_ProcessUDP(ipv4_frame* frame) {
	uint16_t dst_port = (frame->ip_hdr[frame->ip_hdr_len + 2] << 8) | frame->ip_hdr[frame->ip_hdr_len + 3];
	if (dst_port != UDP_DST_PORT) {
		NET_SendICMP_Unreachable(frame);
		return; // не наш порт
	}

	//send_UDP_echo(RxDesc);
}
void NET_SendICMP_Unreachable(ipv4_frame* frame) {

	uint8_t *tx_buffer = *frame->tx_buff;
	// Скопировать шаблон Ethernet + IP + ICMP
	memcpy(tx_buffer, icmp_port_unrchble_template, ICMP_UNRCHBLE_FRAME_LEN);
	// Ethernet: заменить MAC назначения
	memcpy(tx_buffer, frame->scr_mac, 6);       // MAC источника → dst
	// IP: заменить адреса
	memcpy(&tx_buffer[DST_ADDR_POS],frame->scr_addr, 4); // dst IP = src IP из входящего пакета
	// --- ICMP DATA ---
	// RFC 792: вложить IP заголовок исходного пакета + первые 8 байт данных
	uint16_t icmp_data_len = frame->ip_hdr_len + 8;
	memcpy(&tx_buffer[14 + 20 + 8], frame->ip_hdr, icmp_data_len);
	// --- Пересчитать длины ---
	uint16_t new_ip_total_len = 20 /*IP hdr*/+ 8 /*ICMP hdr*/+ icmp_data_len;
	tx_buffer[16] = (uint8_t)(new_ip_total_len >> 8);
	tx_buffer[17] = (uint8_t)(new_ip_total_len & 0xFF);
	// --- Пересчитать контрольные суммы ---
	// IP header checksum
//	memset(&tx_buffer[24], 0, 2);
	uint16_t ip_csum = checksum(&tx_buffer[IP_HDR_POS], IP_HDR_LEN);
	tx_buffer[24] = (uint8_t)(ip_csum >> 8);
	tx_buffer[25] = (uint8_t)(ip_csum & 0xFF);
	// ICMP checksum
	//memset(&tx_buffer[14 + 20 + 2], 0, 2);
	uint16_t icmp_csum = checksum(&tx_buffer[14 + 20], 8 + icmp_data_len);
	tx_buffer[14 + 20 + 2] = (uint8_t)(icmp_csum >> 8);
	tx_buffer[14 + 20 + 3] = (uint8_t)(icmp_csum & 0xFF);
	// Отправка
	ETH_SendFrame(14 + new_ip_total_len);
}

void NET_SendUDP(UDP_SendFrameStruct SendFrame) {

	uint8_t *txBuf = TxDescList.pBuff;
	uint16_t udp_len = SendFrame.DataBuffLen + 8;
	//Insert Template
	memcpy(txBuf, UDPsendFrameTemplate, UDP_TEMPLATE_FRAME_LEN);
	//Insert destination MAC
	memcpy(txBuf, SendFrame.DstMAC, 6);
	//Insert destination IP
	memcpy(&txBuf[30], SendFrame.DstIP, 4);
	//Insert checksum IP
	uint8_t *pos = &txBuf[IP_HDR_POS];
	pos[2] = (uint8_t)(((IP_HDR_LEN + udp_len) >> 8));
	pos[3] = ((IP_HDR_LEN + udp_len) & 0xFF);
	uint16_t ip_csum = checksum(pos, IP_HDR_LEN);
	pos[10] = (uint8_t) (ip_csum >> 8);
	pos[11] = (uint8_t) (ip_csum & 0xFF);
	// UDP header
	pos = &txBuf[UDP_HDR_POS];
	//source port
	pos[0] = (uint8_t)(SendFrame.SrcPort >> 8);
	pos[1] = (uint8_t)(SendFrame.SrcPort & 0xFF);
	//destination port
	pos[2] = (uint8_t)(SendFrame.DstPort >> 8);
	pos[3] = (uint8_t)(SendFrame.DstPort & 0xFF);
	//lenght udp frame
	pos[4] = (uint8_t)(udp_len >> 8);
	pos[5] = (uint8_t)(udp_len & 0xFF);
	// UDP checksum можно обнулить
	memcpy(&pos[8], SendFrame.pDataBuff, SendFrame.DataBuffLen);
	udp_checksum(IP_ADDR, SendFrame.DstIP, pos, &pos[8], udp_len);
	// Отправляем
	ETH_SendFrame(SendFrame.DataBuffLen + UDP_TEMPLATE_FRAME_LEN);
}

void NET_ProcessICMP(ipv4_frame* frame) {

	uint8_t *icmp = &frame->ip_hdr[frame->ip_hdr_len];
	if (icmp[0] != 8) return; // not Echo request
	uint8_t *txBuf = *frame->tx_buff;
	// Готовим ответ
	memcpy(&txBuf[IP_HDR_POS], frame->ip_hdr, frame->ip_len);
	// Ethernet swap
	memcpy(txBuf, &frame->scr_mac, 6);       // dst = src
	memcpy(&txBuf[6], MAC_ADDR, 6);    // src = my MAC
	//IPv4
	txBuf[12] = 0x08;
	txBuf[13] = 0x00;
	// IP swap
	memcpy(&txBuf[SCR_ADDR_POS], IP_ADDR, 4);    // src = my IP
	memcpy(&txBuf[DST_ADDR_POS], frame->scr_addr, 4); // dst = src IP
	// Обновляем checksum IP
	uint8_t* ip = &txBuf[IP_HDR_POS];
	ip[10] = 0;
	ip[11] = 0;
	uint16_t ip_csum = checksum(ip, frame->ip_hdr_len);
	ip[10] = (uint8_t) (ip_csum >> 8);
	ip[11] = (uint8_t) (ip_csum & 0xFF);

	// ICMP: Echo reply
	icmp = &ip[frame->ip_hdr_len];
	icmp[0] = 0; // Echo Reply
	icmp[2] = 0;
	icmp[3] = 0;
	uint16_t icmp_len = frame->ip_len - frame->ip_hdr_len;
	uint16_t icmp_csum = checksum(icmp, icmp_len);
	icmp[2] = (uint8_t)(icmp_csum >> 8);
	icmp[3] = (uint8_t)(icmp_csum & 0xFF);
	// Отправляем
	ETH_SendFrame(frame->ip_len+ETH_HDR_LEN);
}

void NET_ProcessARP(ipv4_frame* frame) {
	// Минимальная проверка
	if (frame->frame_len < 42)return; // слишком короткий
	// Разбираем ARP-запрос
	if (frame->rx_buff[ETH_HDR_LEN+7] != 1)return; // не ARP-request
	// Проверяем, что спрашивают именно наш IP
	if (memcmp(&frame->rx_buff[ETH_HDR_LEN+24], IP_ADDR, 4) != 0)return;
	// Готовим ARP-ответ (42 байта)
	uint8_t *txBuf = *frame->tx_buff;
	// Ethernet заголовок
	memcpy(&txBuf[12], &frame->rx_buff[12], frame->frame_len - 12); // dst = src запроса
	memcpy(&txBuf[0],   frame->scr_mac, 6);   // dst = src запроса
	memcpy(&txBuf[6],   MAC_ADDR, 6);       // src = мой MAC
	txBuf[20] = 0x00;
	txBuf[21] = 0x02; // Opcode = reply (2)
	memcpy(&txBuf[22], MAC_ADDR, 6);      // sender MAC
	memcpy(&txBuf[28], IP_ADDR, 4);       // sender IP
	memcpy(&txBuf[32], frame->scr_mac, 6); // target MAC = sender из запроса
	memcpy(&txBuf[38], frame->scr_addr, 4); // target IP = sender IP
	// Передаём через DMA (один дескриптор)
	ETH_SendFrame(frame->frame_len);

}


