#!/usr/bin/env python3
"""
ntp_server.py — minimal SNTPv4 server for the bench: answers with this PC's
clock (UTC). The board's network has no Internet, and on Windows port 123
belongs to the time service (W32Time), so it listens on 12300 by default.

    python bench/ntp_server.py [--port 12300]

Every request is logged with the client's address; Ctrl+C to stop.
"""
from __future__ import annotations

import argparse
import socket
import struct
import time

NTP_UNIX_OFFSET = 2208988800


def ntp_now() -> tuple[int, int]:
    t = time.time() + NTP_UNIX_OFFSET
    secs = int(t)
    return secs & 0xFFFFFFFF, int((t - secs) * 2**32) & 0xFFFFFFFF


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--port", type=int, default=12300)
    a = ap.parse_args()
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("0.0.0.0", a.port))
    print(f"SNTP server on UDP {a.port}", flush=True)
    while True:
        data, peer = sock.recvfrom(512)
        rx_s, rx_f = ntp_now()
        if len(data) < 48 or (data[0] & 0x7) != 3:   # client mode only
            continue
        origin = data[40:48]                         # the client's transmit timestamp
        tx_s, tx_f = ntp_now()
        reply = struct.pack(
            "!BBbbII4s8s8sIIII",
            (0 << 6) | (4 << 3) | 4,                 # LI 0, v4, server
            3,                                       # stratum: synced from upstream
            data[2], -20,                            # poll (echoed), precision ~1 us
            0, 0, b"LOCL",                           # root delay/dispersion, reference id
            struct.pack("!II", rx_s, rx_f),          # reference time
            origin,
            rx_s, rx_f, tx_s, tx_f)
        sock.sendto(reply, peer)
        print(f"{time.strftime('%H:%M:%S')} {peer[0]}:{peer[1]} <- {tx_s - NTP_UNIX_OFFSET}", flush=True)


if __name__ == "__main__":
    main()
