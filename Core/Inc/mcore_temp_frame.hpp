<<<<<<< HEAD
/*
 * mcore_temp_frame.hpp
 *
 *  Created on: Sep 27, 2025
 *      Author: AkimovMA
 */
#pragma once

#include "mcore_tcp.hpp"


uint8_t TCPsendFrameTemplate[TCP_TEMPLATE_FRAME_LEN] = {
		// === Ethernet Header (14B) ===
		0x00, 0x00, 0x00, 0x00, 0x00,
		0x00,        // Dst MAC (будет подставлен)
		MAC_ADDR[0], MAC_ADDR[1], MAC_ADDR[2], MAC_ADDR[3], MAC_ADDR[4],
		MAC_ADDR[5], // Src MAC = мой MAC
		0x08, 0x00,  // Ethertype = IPv4

		// === IPv4 Header (20B) ===
		0x45,// Version=4, IHL=5 (20 байт)
		0x00,        // TOS
		0x00, 0x00, // Total Length (20 IP + 20 TCP + payload, будет обновляться)
		0x00, 0x00,  // Identification
		0x00, 0x00,  // Flags + Fragment offset
		0x40,        // TTL
		0x06,        // Protocol = TCP (0x06)
		0x00, 0x00,  // Header checksum (обнулять перед расчетом)
		IP_ADDR[0], IP_ADDR[1], IP_ADDR[2], IP_ADDR[3],  // Src IP
		0x00, 0x00, 0x00, 0x00,  // Dst IP (будет подставлен)

		// === TCP Header (20B, без опций) ===
		0x13, 0x88,  // Src Port = 5000 (0x1388) (можно менять)
		0x00, 0x00,  // Dst Port (будет подставлен)
		0x00, 0x00, 0x00, 0x00,  // Seq Num (будет подставлен)
		0x00, 0x00, 0x00, 0x00,  // Ack Num (будет подставлен)
		0x50,        // Data Offset=5 (20 байт), Reserved=0
		0x02,        // Flags = SYN (0x02), можно менять на ACK/PSH/FIN
		0x04, 0x00,  // Window size = 1024 (0x0400)
		0x00, 0x00,  // TCP checksum (обнулять перед расчетом)
		0x00, 0x00   // Urgent Pointer = 0
		// Payload идёт сразу после TCP заголовка
		};


=======
/*
 * mcore_temp_frame.hpp
 *
 *  Created on: Sep 27, 2025
 *      Author: AkimovMA
 */
#pragma once

#include "mcore_tcp.hpp"


uint8_t TCPsendFrameTemplate[TCP_TEMPLATE_FRAME_LEN] = {
		// === Ethernet Header (14B) ===
		0x00, 0x00, 0x00, 0x00, 0x00,
		0x00,        // Dst MAC (будет подставлен)
		MAC_ADDR[0], MAC_ADDR[1], MAC_ADDR[2], MAC_ADDR[3], MAC_ADDR[4],
		MAC_ADDR[5], // Src MAC = мой MAC
		0x08, 0x00,  // Ethertype = IPv4

		// === IPv4 Header (20B) ===
		0x45,// Version=4, IHL=5 (20 байт)
		0x00,        // TOS
		0x00, 0x00, // Total Length (20 IP + 20 TCP + payload, будет обновляться)
		0x00, 0x00,  // Identification
		0x00, 0x00,  // Flags + Fragment offset
		0x40,        // TTL
		0x06,        // Protocol = TCP (0x06)
		0x00, 0x00,  // Header checksum (обнулять перед расчетом)
		IP_ADDR[0], IP_ADDR[1], IP_ADDR[2], IP_ADDR[3],  // Src IP
		0x00, 0x00, 0x00, 0x00,  // Dst IP (будет подставлен)

		// === TCP Header (20B, без опций) ===
		0x13, 0x88,  // Src Port = 5000 (0x1388) (можно менять)
		0x00, 0x00,  // Dst Port (будет подставлен)
		0x00, 0x00, 0x00, 0x00,  // Seq Num (будет подставлен)
		0x00, 0x00, 0x00, 0x00,  // Ack Num (будет подставлен)
		0x50,        // Data Offset=5 (20 байт), Reserved=0
		0x02,        // Flags = SYN (0x02), можно менять на ACK/PSH/FIN
		0x04, 0x00,  // Window size = 1024 (0x0400)
		0x00, 0x00,  // TCP checksum (обнулять перед расчетом)
		0x00, 0x00   // Urgent Pointer = 0
		// Payload идёт сразу после TCP заголовка
		};


>>>>>>> 7688bfa (update)
