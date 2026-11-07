"""Minimal SNTPv4 server for MCORE tests (RFC 4330, unicast mode).

Answers every client request with the host clock as a stratum-2 server.
Usage: python sntp_server.py [bind_address] [port]   (default 0.0.0.0 123)
"""
import socket
import struct
import sys
import time

NTP_UNIX_OFFSET = 2208988800


def ntp_now():
    t = time.time() + NTP_UNIX_OFFSET
    sec = int(t)
    frac = int((t - sec) * (1 << 32)) & 0xFFFFFFFF
    return sec & 0xFFFFFFFF, frac


def reply(request):
    if len(request) < 48 or (request[0] & 0x07) != 3:
        return None
    version = (request[0] >> 3) & 0x07
    recv_s, recv_f = ntp_now()
    originate = request[40:48]          # client's transmit timestamp
    li_vn_mode = (0 << 6) | (version << 3) | 4
    tx_s, tx_f = ntp_now()
    return struct.pack(
        "!BBbb11I",
        li_vn_mode, 2, 6, -20,          # stratum 2, poll 2^6, precision ~1 us
        0, 0,                           # root delay, root dispersion
        int.from_bytes(b"LOCL", "big"), # reference id
        recv_s, recv_f,                 # reference timestamp
        *struct.unpack("!II", originate),
        recv_s, recv_f,                 # receive timestamp
        tx_s, tx_f,                     # transmit timestamp
    )


def main():
    addr = sys.argv[1] if len(sys.argv) > 1 else "0.0.0.0"
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 123
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((addr, port))
    print(f"SNTP server on {addr}:{port}", flush=True)
    while True:
        data, peer = sock.recvfrom(512)
        answer = reply(data)
        if answer is not None:
            sock.sendto(answer, peer)
            print(f"answered {peer[0]}:{peer[1]}", flush=True)


if __name__ == "__main__":
    main()
