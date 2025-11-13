/*
* mcore_net.cpp
*
*  Created on: Sep 24, 2025
*      Author: 
*/
#include <cstring>
#include "mcore_tcp.hpp"
#include "mcore_net.hpp"
#include "mcore_eth_utils.hpp"
#include "mcore_eth.hpp"

namespace {
	// Constants for frame processing
	constexpr uint16_t ETH_MIN_FRAME_SIZE = 60;      // Minimum Ethernet frame
	constexpr uint16_t ETH_MAX_FRAME_SIZE = 1518;    // Maximum Ethernet frame
	constexpr uint16_t ETH_HDR_SIZE = 14;            // Ethernet header size
	constexpr uint16_t IP_MIN_HDR_SIZE = 20;         // Minimum IP header size
	constexpr uint16_t ARP_FRAME_SIZE = 42;          // ARP frame size
	constexpr uint16_t ARP_REQUEST_OPCODE = 1;      // ARP request opcode
	constexpr uint16_t ARP_REPLY_OPCODE = 2;         // ARP reply opcode
	constexpr uint8_t ICMP_ECHO_REQUEST = 8;        // ICMP echo request type
	constexpr uint8_t ICMP_ECHO_REPLY = 0;          // ICMP echo reply type
}

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
		IP_ADDR[0], IP_ADDR[1], IP_ADDR[2], IP_ADDR[3], // Src IP = my IP
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

/**
 * @brief Optimized frame processing with early validation and filtering
 * 
 * This function processes received Ethernet frames with the following optimizations:
 * - Early frame size validation
 * - Protocol type filtering before full parsing
 * - Efficient pointer-based frame structure
 * - Hardware checksum validation (already done by MAC)
 */
void NET_ProcessRx(ETH_RxDescListStruct* RxDesc, ETH_TxDescListStruct* TxDesc) {
	// Early validation: minimum frame size check
	if (RxDesc->BuffLen < ETH_HDR_SIZE) {
		return;  // Frame too short
	}

	const uint8_t* const rx_buff = RxDesc->pBuff;
	
	// Extract ethertype early for protocol filtering
	const uint16_t ethertype = static_cast<uint16_t>((rx_buff[12] << 8) | rx_buff[13]);
	
	// Process ARP frames (0x0806)
	if (ethertype == 0x0806) {
		if (RxDesc->BuffLen < ARP_FRAME_SIZE) {
			return;  // ARP frame too short
		}
		
		ipv4_frame ip_frame;
		ip_frame.rx_buff = const_cast<uint8_t*>(rx_buff);
		ip_frame.tx_buff = &(TxDesc->pBuff);
		ip_frame.scr_mac = const_cast<uint8_t*>(&rx_buff[22]);  // ARP sender MAC
		ip_frame.scr_addr = const_cast<uint8_t*>(&rx_buff[28]); // ARP sender IP
		ip_frame.frame_len = static_cast<uint16_t>(RxDesc->BuffLen);
		
		NET_ProcessARP(&ip_frame);
		return;
	}

	// Process IPv4 frames (0x0800)
	if (ethertype != 0x0800) {
		return;  // Not IPv4, ignore
	}

	// Validate minimum IPv4 frame size (Ethernet header + minimum IP header)
	if (RxDesc->BuffLen < (ETH_HDR_SIZE + IP_MIN_HDR_SIZE)) {
		return;
	}

	// Early destination IP check - filter before expensive parsing
	// Use direct memory comparison for better performance
	const uint8_t* const dst_ip = &rx_buff[DST_ADDR_POS];
	if (dst_ip[0] != IP_ADDR[0] || dst_ip[1] != IP_ADDR[1] ||
	    dst_ip[2] != IP_ADDR[2] || dst_ip[3] != IP_ADDR[3]) {
		return;  // Not for us
	}

	// Check hardware-validated IPv4 status from descriptor
	const uint32_t ExtStatus = RxDesc->CurrRxDesc->DESC4;
	
	// Validate IPv4 packet and checksum (hardware already validated)
	if (!(ExtStatus & ETH_DMAPTPRXDESC_IPV4PR) || (ExtStatus & ETH_DMAPTPRXDESC_IPPE)) {
		return;  // Not IPv4 or payload error
	}

	// Parse IP header
	const uint8_t* const ip_hdr = &rx_buff[IP_HDR_POS];
	const uint8_t ip_hdr_len = (ip_hdr[0] & 0x0F) * 4;
	
	// Validate IP header length
	if (ip_hdr_len < IP_MIN_HDR_SIZE || ip_hdr_len > 60) {
		return;  // Invalid IP header length
	}

	const uint16_t ip_len = static_cast<uint16_t>((ip_hdr[2] << 8) | ip_hdr[3]);
	
	// Validate IP length matches frame size
	if (ip_len > (RxDesc->BuffLen - ETH_HDR_SIZE)) {
		return;  // IP length mismatch
	}

	// Prepare frame structure for protocol handlers
	ipv4_frame ip_frame;
	ip_frame.rx_buff = const_cast<uint8_t*>(rx_buff);
	ip_frame.tx_buff = &(TxDesc->pBuff);
	ip_frame.ip_hdr = const_cast<uint8_t*>(ip_hdr);
	ip_frame.ip_hdr_len = ip_hdr_len;
	ip_frame.ip_len = ip_len;
	ip_frame.scr_addr = const_cast<uint8_t*>(&rx_buff[SCR_ADDR_POS]);
	ip_frame.scr_mac = const_cast<uint8_t*>(&rx_buff[6]);

	// Route to protocol handlers based on hardware-detected protocol
	const uint8_t ip_protocol = ExtStatus & ETH_DMAPTPRXDESC_IPPT;
	
	if (ip_protocol == ETH_DMAPTPRXDESC_IPPT_ICMP) {
		NET_ProcessICMP(&ip_frame);
	} else if (ip_protocol == ETH_DMAPTPRXDESC_IPPT_UDP) {
		NET_ProcessUDP(&ip_frame);
	} else if (ip_protocol == ETH_DMAPTPRXDESC_IPPT_TCP) {
		NET_ProcessTCP(&ip_frame);
	}
	// Unknown protocol - silently drop
}

/**
 * @brief Process UDP packets
 * 
 * Validates destination port and routes to appropriate handler
 */
void NET_ProcessUDP(ipv4_frame* frame) {
	// Calculate UDP header offset
	const uint8_t* const udp_hdr = &frame->ip_hdr[frame->ip_hdr_len];
	
	// Extract destination port (network byte order)
	const uint16_t dst_port = static_cast<uint16_t>((udp_hdr[2] << 8) | udp_hdr[3]);
	
	if (dst_port != UDP_DST_PORT) {
		// Port not for us - send ICMP unreachable
		NET_SendICMP_Unreachable(frame);
		return;
	}

	// TODO: Route to UDP handler based on port
	// send_UDP_echo(RxDesc);
}

/**
 * @brief Send ICMP Port Unreachable message
 * 
 * Constructs and sends ICMP destination unreachable (port unreachable) message
 * according to RFC 792
 */
void NET_SendICMP_Unreachable(ipv4_frame* frame) {
	uint8_t *tx_buffer = *frame->tx_buff;
	
	// Copy template
	memcpy(tx_buffer, icmp_port_unrchble_template, ICMP_UNRCHBLE_FRAME_LEN);
	
	// Update Ethernet destination MAC (source MAC from received frame)
	memcpy(tx_buffer, frame->scr_mac, MAC_ADDR_LEN);
	
	// Update IP destination address (source IP from received frame)
	memcpy(&tx_buffer[DST_ADDR_POS], frame->scr_addr, IP_ADDR_LEN);
	
	// ICMP Data: Include original IP header + first 8 bytes of payload (RFC 792)
	const uint16_t icmp_data_len = frame->ip_hdr_len + 8;
	memcpy(&tx_buffer[ETH_HDR_LEN + IP_HDR_LEN + 8], frame->ip_hdr, icmp_data_len);
	
	// Update IP total length
	const uint16_t new_ip_total_len = IP_HDR_LEN + 8 + icmp_data_len;
	tx_buffer[IP_HDR_POS + 2] = static_cast<uint8_t>(new_ip_total_len >> 8);
	tx_buffer[IP_HDR_POS + 3] = static_cast<uint8_t>(new_ip_total_len & 0xFF);
	
	// Calculate and set IP header checksum
	uint16_t ip_csum = checksum(&tx_buffer[IP_HDR_POS], IP_HDR_LEN);
	tx_buffer[IP_HDR_POS + 10] = static_cast<uint8_t>(ip_csum >> 8);
	tx_buffer[IP_HDR_POS + 11] = static_cast<uint8_t>(ip_csum & 0xFF);
	
	// Calculate and set ICMP checksum
	const uint16_t icmp_len = 8 + icmp_data_len;
	uint16_t icmp_csum = checksum(&tx_buffer[ETH_HDR_LEN + IP_HDR_LEN], icmp_len);
	tx_buffer[ETH_HDR_LEN + IP_HDR_LEN + 2] = static_cast<uint8_t>(icmp_csum >> 8);
	tx_buffer[ETH_HDR_LEN + IP_HDR_LEN + 3] = static_cast<uint8_t>(icmp_csum & 0xFF);
	
	// Send frame
	ETH_SendFrame(ETH_HDR_LEN + new_ip_total_len);
}

/**
 * @brief Send UDP frame
 * 
 * Constructs and sends a UDP frame using the template
 */
void NET_SendUDP(UDP_SendFrameStruct SendFrame) {
	uint8_t *txBuf = TxDescList.pBuff;
	const uint16_t udp_len = SendFrame.DataBuffLen + UDP_HDR_LEN;
	
	// Copy template
	memcpy(txBuf, UDPsendFrameTemplate, UDP_TEMPLATE_FRAME_LEN);
	
	// Update destination MAC
	memcpy(txBuf, SendFrame.DstMAC, MAC_ADDR_LEN);
	
	// Update destination IP
	memcpy(&txBuf[DST_ADDR_POS], SendFrame.DstIP, IP_ADDR_LEN);
	
	// Update IP header
	uint8_t *ip_hdr = &txBuf[IP_HDR_POS];
	const uint16_t ip_total_len = IP_HDR_LEN + udp_len;
	ip_hdr[2] = static_cast<uint8_t>(ip_total_len >> 8);
	ip_hdr[3] = static_cast<uint8_t>(ip_total_len & 0xFF);
	
	// Calculate and set IP checksum
	uint16_t ip_csum = checksum(ip_hdr, IP_HDR_LEN);
	ip_hdr[10] = static_cast<uint8_t>(ip_csum >> 8);
	ip_hdr[11] = static_cast<uint8_t>(ip_csum & 0xFF);
	
	// Update UDP header
	uint8_t *udp_hdr = &txBuf[UDP_HDR_POS];
	udp_hdr[0] = static_cast<uint8_t>(SendFrame.SrcPort >> 8);
	udp_hdr[1] = static_cast<uint8_t>(SendFrame.SrcPort & 0xFF);
	udp_hdr[2] = static_cast<uint8_t>(SendFrame.DstPort >> 8);
	udp_hdr[3] = static_cast<uint8_t>(SendFrame.DstPort & 0xFF);
	udp_hdr[4] = static_cast<uint8_t>(udp_len >> 8);
	udp_hdr[5] = static_cast<uint8_t>(udp_len & 0xFF);
	
	// Copy payload
	memcpy(&udp_hdr[UDP_HDR_LEN], SendFrame.pDataBuff, SendFrame.DataBuffLen);
	
	// Calculate UDP checksum
	udp_checksum(IP_ADDR, SendFrame.DstIP, udp_hdr, &udp_hdr[UDP_HDR_LEN], udp_len);
	
	// Send frame
	ETH_SendFrame(SendFrame.DataBuffLen + UDP_TEMPLATE_FRAME_LEN);
}

/**
 * @brief Process ICMP Echo Request and send Echo Reply
 * 
 * Handles ICMP echo requests (ping) and responds with echo reply
 */
void NET_ProcessICMP(ipv4_frame* frame) {
	const uint8_t *icmp = &frame->ip_hdr[frame->ip_hdr_len];
	
	// Only process echo requests
	if (icmp[0] != ICMP_ECHO_REQUEST) {
		return;
	}
	
	uint8_t *txBuf = *frame->tx_buff;
	
	// Copy entire IP packet (will modify headers)
	memcpy(&txBuf[IP_HDR_POS], frame->ip_hdr, frame->ip_len);
	
	// Swap Ethernet addresses
	memcpy(txBuf, frame->scr_mac, MAC_ADDR_LEN);        // dst = src
	memcpy(&txBuf[6], MAC_ADDR, MAC_ADDR_LEN);          // src = my MAC
	
	// Set ethertype
	txBuf[12] = 0x08;
	txBuf[13] = 0x00;
	
	// Swap IP addresses
	memcpy(&txBuf[SCR_ADDR_POS], IP_ADDR, IP_ADDR_LEN);      // src = my IP
	memcpy(&txBuf[DST_ADDR_POS], frame->scr_addr, IP_ADDR_LEN); // dst = src IP
	
	// Update IP header checksum (zero checksum field first)
	uint8_t* ip = &txBuf[IP_HDR_POS];
	ip[10] = 0;
	ip[11] = 0;
	// Note: Hardware can calculate IP checksum, but we'll do it here for now
	// uint16_t ip_csum = checksum(ip, frame->ip_hdr_len);
	// ip[10] = static_cast<uint8_t>(ip_csum >> 8);
	// ip[11] = static_cast<uint8_t>(ip_csum & 0xFF);

	// Update ICMP header: change type to Echo Reply
	uint8_t* reply_icmp = &ip[frame->ip_hdr_len];
	reply_icmp[0] = ICMP_ECHO_REPLY;  // Echo Reply
	reply_icmp[2] = 0;
	reply_icmp[3] = 0;
	// Note: Hardware can calculate ICMP checksum, but we'll do it here for now
	// const uint16_t icmp_len = frame->ip_len - frame->ip_hdr_len;
	// uint16_t icmp_csum = checksum(reply_icmp, icmp_len);
	// reply_icmp[2] = static_cast<uint8_t>(icmp_csum >> 8);
	// reply_icmp[3] = static_cast<uint8_t>(icmp_csum & 0xFF);
	
	// Send frame
	ETH_SendFrame(frame->ip_len + ETH_HDR_LEN);
}

/**
 * @brief Process ARP requests and send ARP replies
 * 
 * Handles ARP requests for our IP address and responds with ARP reply
 */
void NET_ProcessARP(ipv4_frame* frame) {
	// Validate minimum ARP frame size
	if (frame->frame_len < ARP_FRAME_SIZE) {
		return;
	}
	
	// Check if it's an ARP request (opcode = 1)
	if (frame->rx_buff[ETH_HDR_LEN + 7] != ARP_REQUEST_OPCODE) {
		return;  // Not an ARP request
	}
	
	// Check if they're asking for our IP address
	if (memcmp(&frame->rx_buff[ETH_HDR_LEN + 24], IP_ADDR, IP_ADDR_LEN) != 0) {
		return;  // Not asking for our IP
	}
	
	// Prepare ARP reply
	uint8_t *txBuf = *frame->tx_buff;
	
	// Copy ethertype and ARP header from request
	memcpy(&txBuf[12], &frame->rx_buff[12], frame->frame_len - 12);
	
	// Update Ethernet header: swap MAC addresses
	memcpy(txBuf, frame->scr_mac, MAC_ADDR_LEN);        // dst = src MAC
	memcpy(&txBuf[6], MAC_ADDR, MAC_ADDR_LEN);           // src = my MAC
	
	// Update ARP opcode to reply (2)
	txBuf[ETH_HDR_LEN + 6] = 0x00;
	txBuf[ETH_HDR_LEN + 7] = ARP_REPLY_OPCODE;
	
	// Update ARP sender fields (our MAC and IP)
	memcpy(&txBuf[ETH_HDR_LEN + 8], MAC_ADDR, MAC_ADDR_LEN);  // sender MAC = my MAC
	memcpy(&txBuf[ETH_HDR_LEN + 14], IP_ADDR, IP_ADDR_LEN);  // sender IP = my IP
	
	// Update ARP target fields (request sender's MAC and IP)
	memcpy(&txBuf[ETH_HDR_LEN + 18], frame->scr_mac, MAC_ADDR_LEN);  // target MAC = sender MAC
	memcpy(&txBuf[ETH_HDR_LEN + 24], frame->scr_addr, IP_ADDR_LEN);   // target IP = sender IP
	
	// Send ARP reply
	ETH_SendFrame(frame->frame_len);
}
