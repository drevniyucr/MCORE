<<<<<<< HEAD
/*
 * mcore_tcp.cpp
 *
 *  Created on: Sep 28, 2025
 *      Author: AkimovMA
 */
#include <cstddef>
#include <cstring>
#include "mcore_tcp.hpp"
#include "mcore_net.hpp"
#include "mcore_system.hpp"
#include "mcore_eth_utils.hpp"
#include "mcore_eth.hpp"
#include "mcore_temp_frame.hpp"

tcp_conn_t tcp_clients[TCP_MAX_CONNECTIONS] = { };

uint8_t active_list[TCP_MAX_CONNECTIONS] = { };
uint8_t active_conn = 0;

uint8_t free_list[TCP_MAX_CONNECTIONS];
uint8_t free_top = 0;

uint8_t **ppTxBuff = NULL;

void NET_TCP_Init(void) {
	memset(tcp_clients, 0, sizeof(tcp_clients));
	free_top = 0;
	active_conn = 0;
	ppTxBuff = &TxDescList.pBuff;
	for (uint8_t i = 0; i < TCP_MAX_CONNECTIONS; i++) {
		free_list[free_top++] = i;
	}
}

int NET_TCP_ClientAdd(void) {
	if (free_top != 0 && active_conn < TCP_MAX_CONNECTIONS){
	uint8_t i = free_list[--free_top];
	active_list[active_conn++] = i; // добавляем в список активных
	return i;
	}
	return -1;
}

void NET_TCP_ClientRemove(uint8_t idx) {
	// удаляем из active_list
	for (int j = 0; j < active_conn; j++) {
		if (active_list[j] == idx) {
			if (j != active_conn - 1) {
				active_list[j] = active_list[--active_conn];
			} else {
				active_conn--;
			}
			memset(&tcp_clients[idx], 0, sizeof(tcp_conn_t));
			free_list[free_top++] = idx;
			break;
		}
	}
}

//ранний RST без создания сокета tcp_conn_t
void NET_SendTCP_RST(ipv4_frame *frame, uint16_t src_port, uint16_t dst_port,
		uint32_t seq, uint32_t ack, uint8_t flags) {

	eth_hdr_t *eth = reinterpret_cast<eth_hdr_t*>(*ppTxBuff);
	ip_hdr_t *ip = reinterpret_cast<ip_hdr_t*>((*ppTxBuff + ETH_HDR_LEN));
	tcp_hdr_t *tcp = reinterpret_cast<tcp_hdr_t*>((*ppTxBuff + ETH_HDR_LEN
			+ IP_HDR_LEN));

	memcpy(eth, TCPsendFrameTemplate, TCP_TEMPLATE_FRAME_LEN);
	// Ethernet
	memcpy(eth->dst, frame->scr_mac, MAC_ADDR_LEN);
	// IP
	ip->tot_len = bswap16(IP_HDR_LEN + TCP_HDR_LEN);
	ip->dst_ip = bswap32(BuffU8ToU32(frame->scr_addr));
	ip->checksum = bswap16(
			checksum(reinterpret_cast<uint8_t*>(ip), IP_HDR_LEN));
	// TCP
	tcp->src_port = bswap16(src_port);
	tcp->dst_port = bswap16(dst_port);
	tcp->seq_num = bswap32(seq);
	tcp->ack_num = bswap32(ack);
	tcp->flags = flags;
	tcp->checksum = bswap16(
			tcp_checksum(IP_ADDR, frame->scr_addr,
					reinterpret_cast<uint8_t*>(tcp),
					static_cast<uint16_t>(TCP_HDR_LEN)));

	ETH_SendFrame(
	ETH_HDR_LEN + IP_HDR_LEN + TCP_HDR_LEN);

}

void NET_SendRetransmitTCP(tcp_conn_t *conn) {

	eth_hdr_t *eth = reinterpret_cast<eth_hdr_t*>(*ppTxBuff);
	ip_hdr_t *ip = reinterpret_cast<ip_hdr_t*>((*ppTxBuff + ETH_HDR_LEN));
	tcp_hdr_t *tcp = reinterpret_cast<tcp_hdr_t*>((*ppTxBuff + ETH_HDR_LEN
			+ IP_HDR_LEN));

	memcpy(eth, TCPsendFrameTemplate, ETH_HDR_LEN + IP_HDR_LEN);
	// Ethernet
	memcpy(eth->dst, conn->client_mac, MAC_ADDR_LEN);
	// IP
	ip->tot_len = bswap16(IP_HDR_LEN + TCP_HDR_LEN + conn->soc_tx_buff_pos);
	ip->dst_ip = bswap32(conn->client_ip);
	ip->checksum = bswap16(
			checksum(reinterpret_cast<uint8_t*>(ip), IP_HDR_LEN));
	// TCP
	memcpy(tcp, conn->socket_tx_buff, conn->soc_tx_buff_pos);

	ETH_SendFrame(
	ETH_HDR_LEN + IP_HDR_LEN + conn->soc_tx_buff_pos);
}

// отправка frame на созданном сокете
void NET_SendTCP(tcp_conn_t *conn, uint32_t seq, uint32_t ack, uint8_t flags,
		uint8_t retransmit, const uint8_t *data, uint16_t data_len) {

	eth_hdr_t *eth = reinterpret_cast<eth_hdr_t*>(*ppTxBuff);
	ip_hdr_t *ip = reinterpret_cast<ip_hdr_t*>((*ppTxBuff + ETH_HDR_LEN));
	tcp_hdr_t *tcp = reinterpret_cast<tcp_hdr_t*>((*ppTxBuff + ETH_HDR_LEN
			+ IP_HDR_LEN));

	memcpy(eth, TCPsendFrameTemplate, TCP_TEMPLATE_FRAME_LEN);
	// Ethernet
	memcpy(eth->dst, conn->client_mac, MAC_ADDR_LEN);
	// IP
	ip->tot_len = bswap16(IP_HDR_LEN + TCP_HDR_LEN + data_len);
	ip->dst_ip = bswap32(conn->client_ip);
	ip->checksum = bswap16(
			checksum(reinterpret_cast<uint8_t*>(ip), IP_HDR_LEN));
	// TCP
	tcp->src_port = bswap16(conn->server_port);
	tcp->dst_port = bswap16(conn->client_port);
	tcp->seq_num = bswap32(seq);
	tcp->ack_num = bswap32(ack);
	tcp->window = bswap16(conn->window_size);
	tcp->flags = flags;

	// копируем данные (если есть)
	if (data_len > 0) {
		memcpy(reinterpret_cast<uint8_t*>(tcp) + TCP_HDR_LEN, data, data_len);
	}
	uint8_t client_ip[4];
	U32toBuffU8(client_ip, conn->client_ip);

	tcp->checksum = bswap16(
			tcp_checksum(IP_ADDR, client_ip, reinterpret_cast<uint8_t*>(tcp),
					static_cast<uint16_t>(TCP_HDR_LEN + data_len)));

	if (retransmit == 1) {
		memcpy(conn->socket_tx_buff, (uint8_t*) tcp, TCP_HDR_LEN + data_len);
		conn->soc_tx_buff_pos = TCP_HDR_LEN + data_len;
		conn->retransmit_timer = get_tick();
		conn->retransmit_count = 0;
	}
	ETH_SendFrame(
	ETH_HDR_LEN + IP_HDR_LEN + TCP_HDR_LEN + data_len);
}

// ==========================
// Обработка TCP пакета
// ==========================

void NET_TCP_Timers(void) {

	uint8_t idx;
	tcp_conn_t *conn;
	for (int i = active_conn - 1; i >= 0; i--) {
		idx = active_list[i];
		conn = &tcp_clients[idx];

		if (conn->retransmit_timer
				&& (get_tick() - conn->retransmit_timer > TCP_RETRANSMIT_TIMEOUT)) {

			if (conn->retransmit_count < TCP_RETRANSMISSION_MAX_COUNT) {

				NET_SendRetransmitTCP(conn);
				conn->retransmit_timer = get_tick();
				conn->retransmit_count++;
			} else {
				NET_TCP_ClientRemove(idx);
				continue;
			}
		}

		if (((get_tick() - conn->last_activity) > TCP_KEEPALIVE_TIMEOUT)
				&& (conn->state == tcp_state_t::TCP_ESTABLISHED)) {

			if ((get_tick() - conn->last_keepalive) >= TCP_KEEPALIVE_INTERVAL) {

				if (conn->keep_alive_count >= TCP_KEEPALIVE_MAX_COUNT) {

					NET_TCP_ClientRemove(idx);
					continue;
				}

				NET_SendTCP(conn, conn->tcp_my_seq - 1, conn->tcp_client_seq,
				NOT_SAVE_FOR_RETRANSMIT, TCP_ACK, NULL, 0);

				conn->keep_alive_count++;
				conn->last_keepalive = get_tick();
			}
		}
	}
}

void NET_ProcessTCP(ipv4_frame *frame) {

tcp_hdr_t *tcp =
		reinterpret_cast<tcp_hdr_t*>((&frame->ip_hdr[frame->ip_hdr_len]));

uint8_t client_flags = tcp->flags;

uint16_t client_window = bswap16(tcp->window);
uint16_t client_port = bswap16(tcp->src_port);
uint16_t server_port = bswap16(tcp->dst_port);
uint32_t client_ip = BuffU8ToU32(frame->scr_addr);
uint32_t client_seq = bswap32(tcp->seq_num);
uint32_t client_ack = bswap32(tcp->ack_num);

tcp_conn_t *conn = NULL;

// Ищем сессию по IP-порту + выкидываем RST если подключение к занятому порту
for (int i = 0; i < active_conn; i++) {
	uint8_t idx = active_list[i];
	if (tcp_clients[idx].client_ip == client_ip
			&& tcp_clients[idx].client_port == client_port
			&& tcp_clients[idx].server_port == server_port) {
		conn = &tcp_clients[idx];
		if (client_flags & TCP_RST) {
			//опционально
			//NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK,NULL, 0);
			NET_TCP_ClientRemove(idx);
			return;
		}
		break;

	} else if (tcp_clients[active_list[i]].server_port == server_port) {
		NET_SendTCP_RST(frame, server_port, client_port, 0, client_seq + 1,
		TCP_RST | TCP_ACK);
		return;
	}
}
//выкидываем RST если подлкючений слишком много или создаем новое подлючение
if (!conn && (client_flags & TCP_SYN)) {
	int num = NET_TCP_ClientAdd();
	if (num == -1) {
		NET_SendTCP_RST(frame, server_port, client_port, 0, client_seq + 1,
		TCP_RST | TCP_ACK);
	} else {
		conn = &tcp_clients[num];
		conn->socket_tag = static_cast<uint8_t>(num);
		conn->client_ip = client_ip;
		conn->last_activity = get_tick();
		memcpy(conn->client_mac, frame->scr_mac, MAC_ADDR_LEN);
		conn->client_port = client_port;
		conn->server_port = server_port;
		conn->tcp_client_seq = client_seq;
		conn->rcv_next = client_seq + 1;		//syn +1 my_seq
		conn->client_window = client_window;
		conn->tcp_my_seq = 0;
		conn->state = tcp_state_t::TCP_SYN_RCVD;
		NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
		SAVE_FOR_RETRANSMIT,
		TCP_SYN | TCP_ACK, NULL, 0);
		conn->tcp_my_seq += 1;
	}
}
// нет сессии для обработки
if (!conn)
	return;
// Далее обработка FSM
switch (conn->state) {
case tcp_state_t::TCP_SYN_RCVD:
	if ((client_flags & TCP_ACK) && (client_ack == conn->tcp_my_seq)
			&& (client_seq == conn->rcv_next)) {
		// если получили ACK на SYN+ACK переходим дальше
		conn->last_activity = get_tick();
		conn->keep_alive_count = 0;
		conn->retransmit_timer = 0;
		conn->retransmit_count = 0;
		conn->soc_tx_buff_pos = 0;
		conn->state = tcp_state_t::TCP_ESTABLISHED;
	}
	break;

case tcp_state_t::TCP_ESTABLISHED:
	if ((client_ack == conn->tcp_my_seq) && (client_seq == conn->rcv_next)) {
		conn->last_activity = get_tick();
		conn->keep_alive_count = 0;
		if (client_flags & TCP_FIN) {
			//если получили FIN закрывыем соединение graceful close
			conn->rcv_next += 1;
			NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK,
			NOT_SAVE_FOR_RETRANSMIT,
			NULL, 0);
			NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
			SAVE_FOR_RETRANSMIT,
			TCP_FIN | TCP_ACK, NULL, 0);
			// FIN тоже занимает 1 байт
			conn->tcp_my_seq += 1;
			//ждем финальный ACK
			conn->state = tcp_state_t::TCP_LAST_ACK;

		} else if (client_flags & TCP_ACK) {
			conn->soc_tx_buff_pos = 0;
			conn->retransmit_timer = 0;
			conn->retransmit_count = 0;
			// считаем data len и отправляем ACK + сохраняем в буффер(потом)
			uint16_t hdr_len = (tcp->offset_reserved >> 4) * 4;
			uint16_t data_len = static_cast<uint16_t>((frame->ip_len
					- frame->ip_hdr_len - hdr_len));

			if (data_len > 0) {
				uint8_t *payload = reinterpret_cast<uint8_t*>(tcp) + hdr_len;

				if (conn->soc_rx_buff_pos + data_len
						<= sizeof(conn->socket_rx_buff)) {
					memcpy(&conn->socket_rx_buff[conn->soc_rx_buff_pos],
							payload, data_len);
					conn->soc_rx_buff_pos += data_len;
					conn->rcv_next += data_len;
					// Обновляем окно
					conn->window_size = SOCKET_RX_BUFF_LEN
							- conn->soc_rx_buff_pos;
				} else {
					uint16_t len = sizeof(conn->socket_rx_buff)
							- conn->soc_rx_buff_pos;
					memcpy(&conn->socket_rx_buff[conn->soc_rx_buff_pos],
							payload, len);
					conn->soc_rx_buff_pos = sizeof(conn->socket_rx_buff);
					conn->rcv_next += len;
					conn->window_size = 0;

				}
			}

			NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
			NOT_SAVE_FOR_RETRANSMIT, TCP_ACK, NULL, 0);
		}
	}

	break;

case tcp_state_t::TCP_FIN_WAIT_1:
	if ((client_flags & TCP_ACK) && (client_ack == conn->tcp_my_seq)
			&& (client_seq == conn->rcv_next)) {
		conn->last_activity = get_tick();
		conn->keep_alive_count = 0;
		conn->soc_tx_buff_pos = 0;
		conn->retransmit_timer = 0;
		conn->retransmit_count = 0;
		conn->state = tcp_state_t::TCP_FIN_WAIT_2;
	}
	break;

case tcp_state_t::TCP_FIN_WAIT_2:
	if ((client_flags & (TCP_ACK | TCP_FIN)) && (client_ack == conn->tcp_my_seq)
			&& (client_seq == conn->rcv_next)) {
		NET_SendTCP(conn, conn->tcp_my_seq, client_seq, TCP_ACK,
		NOT_SAVE_FOR_RETRANSMIT, NULL, 0);
		NET_TCP_ClientRemove(conn->socket_tag);
		return;

	}
	break;

case tcp_state_t::TCP_LAST_ACK:
	if ((client_flags & TCP_ACK) && (client_ack == conn->tcp_my_seq)
			&& (client_seq == conn->rcv_next)) {
		NET_TCP_ClientRemove(conn->socket_tag);
		return;
	}
	break;

default:
	break;
}
}

void NET_TCP_Close(tcp_conn_t *conn) {
	if (conn->state == tcp_state_t::TCP_ESTABLISHED) {
		NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_FIN,
				SAVE_FOR_RETRANSMIT, NULL, 0);
		conn->tcp_my_seq += 1;
		conn->state = tcp_state_t::TCP_FIN_WAIT_1;
	}
}

int NET_TCP_SendUser(tcp_conn_t *conn, const uint8_t *data, uint16_t len) {

	if (conn->state != tcp_state_t::TCP_ESTABLISHED) return -1;
// Проверка окна
	if (len > conn->window_size) return -1;

	NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK | TCP_PSH,
			SAVE_FOR_RETRANSMIT, data, len);
	conn->tcp_my_seq += len;
	return 0;
}

=======
/*
 * mcore_tcp.cpp
 *
 *  Created on: Sep 28, 2025
 *      Author: AkimovMA
 */
#include <cstddef>
#include <cstring>
#include "mcore_tcp.hpp"
#include "mcore_net.hpp"
#include "mcore_system.hpp"
#include "mcore_eth_utils.hpp"
#include "mcore_eth.hpp"
#include "mcore_temp_frame.hpp"

tcp_conn_t tcp_clients[TCP_MAX_CONNECTIONS] = { };
uint8_t active_list[TCP_MAX_CONNECTIONS] = { };
uint8_t active_conn = 0;

uint8_t free_list[TCP_MAX_CONNECTIONS];
uint8_t free_top = 0;

uint8_t **ppTxBuff = NULL;

void NET_TCP_Init(void) {
	memset(tcp_clients, 0, sizeof(tcp_clients));
	free_top = 0;
	active_conn = 0;
	ppTxBuff = &TxDescList.pBuff;
	for (uint8_t i = 0; i < TCP_MAX_CONNECTIONS; i++) {
		free_list[free_top++] = i;
	}
}

int NET_TCP_ClientAdd(void) {
	if (free_top != 0 && active_conn < TCP_MAX_CONNECTIONS) {
		uint8_t i = free_list[--free_top];
		active_list[active_conn++] = i; // добавляем в список активных
		return i;
	}
	return -1;
}

void NET_TCP_ClientRemove(uint8_t idx) {
	// удаляем из active_list
	for (int j = 0; j < active_conn; j++) {
		if (active_list[j] == idx) {
			if (j != active_conn - 1) {
				active_list[j] = active_list[--active_conn];
			} else {
				active_conn--;
			}
			memset(&tcp_clients[idx], 0, sizeof(tcp_conn_t));
			free_list[free_top++] = idx;
			break;
		}
	}
}

//ранний RST без создания сокета tcp_conn_t
void NET_SendTCP_RST(ipv4_frame *frame, uint16_t src_port, uint16_t dst_port,
		uint32_t seq, uint32_t ack, uint8_t flags) {

	eth_hdr_t *eth = reinterpret_cast<eth_hdr_t*>(*ppTxBuff);
	ip_hdr_t *ip = reinterpret_cast<ip_hdr_t*>((*ppTxBuff + ETH_HDR_LEN));
	tcp_hdr_t *tcp = reinterpret_cast<tcp_hdr_t*>((*ppTxBuff + ETH_HDR_LEN
			+ IP_HDR_LEN));

	memcpy(eth, TCPsendFrameTemplate, TCP_TEMPLATE_FRAME_LEN);
	// Ethernet
	memcpy(eth->dst, frame->scr_mac, MAC_ADDR_LEN);
	// IP
	ip->tot_len = bswap16(IP_HDR_LEN + TCP_HDR_LEN);
	ip->dst_ip = bswap32(BuffU8ToU32(frame->scr_addr));
	ip->checksum = bswap16(
			checksum(reinterpret_cast<uint8_t*>(ip), IP_HDR_LEN));
	// TCP
	tcp->src_port = bswap16(src_port);
	tcp->dst_port = bswap16(dst_port);
	tcp->seq_num = bswap32(seq);
	tcp->ack_num = bswap32(ack);
	tcp->flags = flags;
	tcp->checksum = bswap16(
			tcp_checksum(IP_ADDR, frame->scr_addr,
					reinterpret_cast<uint8_t*>(tcp),
					static_cast<uint16_t>(TCP_HDR_LEN)));

	ETH_SendFrame(
	ETH_HDR_LEN + IP_HDR_LEN + TCP_HDR_LEN);

}

void NET_SendRetransmitTCP(tcp_conn_t *conn) {

	eth_hdr_t *eth = reinterpret_cast<eth_hdr_t*>(*ppTxBuff);
	ip_hdr_t *ip = reinterpret_cast<ip_hdr_t*>((*ppTxBuff + ETH_HDR_LEN));
	tcp_hdr_t *tcp = reinterpret_cast<tcp_hdr_t*>((*ppTxBuff + ETH_HDR_LEN
			+ IP_HDR_LEN));

	memcpy(eth, TCPsendFrameTemplate, ETH_HDR_LEN + IP_HDR_LEN);
	// Ethernet
	memcpy(eth->dst, conn->client_mac, MAC_ADDR_LEN);
	// IP
	ip->tot_len = bswap16(IP_HDR_LEN + TCP_HDR_LEN + conn->soc_tx_buff_pos);
	ip->dst_ip = bswap32(conn->client_ip);
	ip->checksum = bswap16(
			checksum(reinterpret_cast<uint8_t*>(ip), IP_HDR_LEN));
	// TCP
	memcpy(tcp, conn->socket_tx_buff, conn->soc_tx_buff_pos);

	ETH_SendFrame(
	ETH_HDR_LEN + IP_HDR_LEN + conn->soc_tx_buff_pos);
}

// отправка frame на созданном сокете
void NET_SendTCP(tcp_conn_t *conn, uint32_t seq, uint32_t ack, uint8_t flags,
		uint8_t retransmit, const uint8_t *data, uint16_t data_len) {

	eth_hdr_t *eth = reinterpret_cast<eth_hdr_t*>(*ppTxBuff);
	ip_hdr_t *ip = reinterpret_cast<ip_hdr_t*>((*ppTxBuff + ETH_HDR_LEN));
	tcp_hdr_t *tcp = reinterpret_cast<tcp_hdr_t*>((*ppTxBuff + ETH_HDR_LEN
			+ IP_HDR_LEN));

	memcpy(eth, TCPsendFrameTemplate, TCP_TEMPLATE_FRAME_LEN);
	// Ethernet
	memcpy(eth->dst, conn->client_mac, MAC_ADDR_LEN);
	// IP
	ip->tot_len = bswap16(IP_HDR_LEN + TCP_HDR_LEN + data_len);
	ip->dst_ip = bswap32(conn->client_ip);
	ip->checksum = bswap16(
			checksum(reinterpret_cast<uint8_t*>(ip), IP_HDR_LEN));
	// TCP
	tcp->src_port = bswap16(conn->server_port);
	tcp->dst_port = bswap16(conn->client_port);
	tcp->seq_num = bswap32(seq);
	tcp->ack_num = bswap32(ack);
	tcp->window = bswap16(conn->window_size);
	tcp->flags = flags;

	// копируем данные (если есть)
	if (data_len > 0) {
		memcpy(reinterpret_cast<uint8_t*>(tcp) + TCP_HDR_LEN, data, data_len);
	}
	uint8_t client_ip[4];
	U32toBuffU8(client_ip, conn->client_ip);

	tcp->checksum = bswap16(
			tcp_checksum(IP_ADDR, client_ip, reinterpret_cast<uint8_t*>(tcp),
					static_cast<uint16_t>(TCP_HDR_LEN + data_len)));

	if (retransmit == 1) {
		memcpy(conn->socket_tx_buff, (uint8_t*) tcp, TCP_HDR_LEN + data_len);
		conn->soc_tx_buff_pos = TCP_HDR_LEN + data_len;
		conn->retransmit_timer = get_tick();
		conn->retransmit_count = 0;
	}
	ETH_SendFrame(
	ETH_HDR_LEN + IP_HDR_LEN + TCP_HDR_LEN + data_len);
}

// ==========================
// Обработка TCP пакета
// ==========================

void NET_TCP_Timers(void) {

	uint8_t idx;
	tcp_conn_t *conn;
	for (int i = active_conn - 1; i >= 0; i--) {
		idx = active_list[i];
		conn = &tcp_clients[idx];

		if (conn->retransmit_timer
				&& (get_tick() - conn->retransmit_timer > TCP_RETRANSMIT_TIMEOUT)) {

			if (conn->retransmit_count < TCP_RETRANSMISSION_MAX_COUNT) {

				NET_SendRetransmitTCP(conn);
				conn->retransmit_timer = get_tick();
				conn->retransmit_count++;
			} else {
				NET_TCP_ClientRemove(idx);
				continue;
			}
		}

		if (((get_tick() - conn->last_activity) > TCP_KEEPALIVE_TIMEOUT)
				&& (conn->state == tcp_state_t::TCP_ESTABLISHED)) {

			if ((get_tick() - conn->last_keepalive) >= TCP_KEEPALIVE_INTERVAL) {

				if (conn->keep_alive_count >= TCP_KEEPALIVE_MAX_COUNT) {

					NET_TCP_ClientRemove(idx);
					continue;
				}

				NET_SendTCP(conn, conn->tcp_my_seq - 1, conn->tcp_client_seq,
				NOT_SAVE_FOR_RETRANSMIT, TCP_ACK, NULL, 0);

				conn->keep_alive_count++;
				conn->last_keepalive = get_tick();
			}
		}
	}
}

void NET_ProcessTCP(ipv4_frame *frame) {

	tcp_hdr_t *tcp =
			reinterpret_cast<tcp_hdr_t*>((&frame->ip_hdr[frame->ip_hdr_len]));

	uint8_t client_flags = tcp->flags;

	uint16_t client_window = bswap16(tcp->window);
	uint16_t client_port = bswap16(tcp->src_port);
	uint16_t server_port = bswap16(tcp->dst_port);
	uint32_t client_ip = BuffU8ToU32(frame->scr_addr);
	uint32_t client_seq = bswap32(tcp->seq_num);
	uint32_t client_ack = bswap32(tcp->ack_num);

	tcp_conn_t *conn = NULL;

// Ищем сессию по IP-порту + выкидываем RST если подключение к занятому порту
	for (int i = 0; i < active_conn; i++) {
		uint8_t idx = active_list[i];
		if (tcp_clients[idx].client_ip == client_ip
				&& tcp_clients[idx].client_port == client_port
				&& tcp_clients[idx].server_port == server_port) {
			conn = &tcp_clients[idx];
			//	conn->tcp_client_seq = client_seq;
			conn->client_window = client_window;
			if (client_flags & TCP_RST) {
				//опционально
				//NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK,NULL, 0);
				NET_TCP_ClientRemove(idx);
				return;
			}
			break;

		} else if (tcp_clients[idx].server_port == server_port) {
			NET_SendTCP_RST(frame, server_port, client_port, 0, client_seq + 1,
			TCP_RST);
			return;
		}
	}
//выкидываем RST если подлкючений слишком много или создаем новое подлючение
	if (!conn && (client_flags & TCP_SYN)) {
		int num = NET_TCP_ClientAdd();
		if (num == -1) {
			NET_SendTCP_RST(frame, server_port, client_port, 0, client_seq + 1,
			TCP_RST | TCP_ACK);
		} else {
			conn = &tcp_clients[num];
			conn->socket_tag = static_cast<uint8_t>(num);
			conn->client_ip = client_ip;
			conn->last_activity = get_tick();
			memcpy(conn->client_mac, frame->scr_mac, MAC_ADDR_LEN);
			conn->client_port = client_port;
			conn->server_port = server_port;
			conn->tcp_client_seq = client_seq;
			conn->rcv_next = client_seq + 1;		//syn +1 my_seq
			conn->client_window = client_window;
			conn->window_size = SOCKET_RX_BUFF_LEN;
			conn->tcp_my_seq = SysTick->VAL; // @suppress("Field cannot be resolved")
			conn->snd_unack = conn->tcp_my_seq;
			conn->state = tcp_state_t::TCP_SYN_RCVD;
			NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
			TCP_SYN | TCP_ACK, SAVE_FOR_RETRANSMIT, NULL, 0);
			conn->tcp_my_seq += 1;
			return;
		}
	}
// нет сессии для обработки
	if (!conn)
		return;
// Далее обработка FSM
	switch (conn->state) {
	case tcp_state_t::TCP_SYN_RCVD:

		if (client_flags & TCP_ACK) {

			if ((client_ack == conn->tcp_my_seq)
					&& (client_seq == conn->rcv_next)) {
				// если получили ACK на SYN+ACK переходим дальше
				conn->last_activity = get_tick();
				conn->snd_unack = client_ack;
				conn->keep_alive_count = 0;
				conn->retransmit_timer = 0;
				conn->retransmit_count = 0;
				conn->soc_tx_buff_pos = 0;
				conn->state = tcp_state_t::TCP_ESTABLISHED;

			} else {
//				NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_RST,
//					NOT_SAVE_FOR_RETRANSMIT, NULL, 0);
				return;
			}
		}

		if ((client_flags & TCP_SYN) && client_seq + 1 == conn->rcv_next)
			NET_SendRetransmitTCP(conn);

	break;

	case tcp_state_t::TCP_ESTABLISHED:

	conn->last_activity = get_tick();
	conn->keep_alive_count = 0;

	if ((client_ack <= conn->tcp_my_seq) && (client_seq == conn->rcv_next)) {

		if (client_flags & TCP_FIN) {
			//если получили FIN закрывыем соединение graceful close
			conn->rcv_next += 1;
			NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
			TCP_ACK, NOT_SAVE_FOR_RETRANSMIT, NULL, 0);

			NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
			TCP_FIN | TCP_ACK, SAVE_FOR_RETRANSMIT, NULL, 0);
			// FIN тоже занимает 1 байт
			conn->tcp_my_seq += 1;
			//ждем финальный ACK

			conn->state = tcp_state_t::TCP_LAST_ACK;

		} else if (client_flags & TCP_ACK) {

			if (client_ack > conn->snd_unack)
				conn->snd_unack = client_ack;

			conn->soc_tx_buff_pos = 0;
			conn->retransmit_timer = 0;
			conn->retransmit_count = 0;
			// считаем data len и отправляем ACK + сохраняем в буффер(потом)
			uint16_t hdr_len = (tcp->offset_reserved >> 4) * 4;
			uint16_t data_len = (uint16_t)(frame->ip_len - frame->ip_hdr_len - hdr_len);

			if (data_len > 0) {

				uint8_t *payload = (uint8_t*) tcp + hdr_len;

				uint16_t free_space = SOCKET_RX_BUFF_LEN
						- conn->soc_rx_buff_pos;

				uint16_t copy_len =
						(data_len <= free_space) ? data_len : free_space;

				memcpy(&conn->socket_rx_buff[conn->soc_rx_buff_pos], payload,
						copy_len);

				conn->soc_rx_buff_pos += copy_len;
				conn->rcv_next += copy_len;
				// Обновляем окно
				conn->window_size = SOCKET_RX_BUFF_LEN - conn->soc_rx_buff_pos;

				NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
				TCP_ACK, NOT_SAVE_FOR_RETRANSMIT, NULL, 0);
			}
		}
	}

	break;

	case tcp_state_t::TCP_FIN_WAIT_1:

		if ((client_flags & TCP_ACK) && (client_ack == conn->tcp_my_seq)) {
			conn->last_activity = get_tick();
			conn->keep_alive_count = 0;
			conn->soc_tx_buff_pos  = 0;
			conn->retransmit_timer = 0;
			conn->retransmit_count = 0;

			if (client_flags & TCP_FIN) {
				conn->rcv_next++;
				NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next,
				TCP_ACK, NOT_SAVE_FOR_RETRANSMIT, NULL, 0);
				NET_TCP_ClientRemove(conn->socket_tag);
				return;
			}
			conn->state = tcp_state_t::TCP_FIN_WAIT_2;
		}

	break;

	case tcp_state_t::TCP_FIN_WAIT_2:

	if ((client_flags & TCP_FIN) && (client_seq == conn->rcv_next)) {
		conn->rcv_next += 1;
		NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK,
		NOT_SAVE_FOR_RETRANSMIT, NULL, 0);
		NET_TCP_ClientRemove(conn->socket_tag);
		return;
	}
	break;

	case tcp_state_t::TCP_LAST_ACK:
	if ((client_flags & TCP_ACK) && (client_ack == conn->tcp_my_seq)
			&& (client_seq == conn->rcv_next)) {
		NET_TCP_ClientRemove(conn->socket_tag);
		return;
	}
	break;

	default:
	break;
}
}

void NET_TCP_Close(tcp_conn_t *conn) {
if (conn->state == tcp_state_t::TCP_ESTABLISHED) {
	NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_FIN,
	SAVE_FOR_RETRANSMIT, NULL, 0);
	conn->state = tcp_state_t::TCP_FIN_WAIT_1;
}
}

int NET_TCP_SendUser(tcp_conn_t *conn, const uint8_t *data, uint16_t len) {

if (conn->state != tcp_state_t::TCP_ESTABLISHED)
	return -1;
// Проверка окна
if (len > conn->window_size)
	return -1;

NET_SendTCP(conn, conn->tcp_my_seq, conn->rcv_next, TCP_ACK | TCP_PSH,
SAVE_FOR_RETRANSMIT, data, len);
conn->tcp_my_seq += len;
return 0;
}

>>>>>>> 7688bfa (update)
