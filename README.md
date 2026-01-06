# AURA (Agentic UDP Reliability & Adaptation)

A custom reliable UDP protocol implementation demonstrating ARQ (Automatic Repeat Request) for IoT device communication over lossy networks.

## Overview

This project simulates a wireless IoT device (C++ client) sending telemetry data to a cloud gateway (Python server) over UDP, with custom reliability mechanisms to handle packet loss.

**Why not just use TCP?**

TCP is reliable but introduces these problems for real-time IoT:
- **Head-of-line blocking**: One lost packet freezes everything behind it
- **Heavy overhead**: Connection state, flow control, congestion control
- **No control**: You can't decide which packets are worth retrying

With UDP + custom ARQ, I control exactly what gets retransmitted. Old sensor readings can be dropped, but critical commands must be delivered.

## Architecture (for now)

```
┌─────────────────┐         UDP + ARQ         ┌─────────────────┐
│   C++ Client    │ ◄──────────────────────► │  Python Gateway │
│  (IoT Device)   │    Custom Binary Protocol │ (Cloud Server)  │
└─────────────────┘                           └─────────────────┘
```

## Features

- **Custom Binary Protocol**: 13-byte packed header (packet_id, timestamp, type)
- **Stop-and-Wait ARQ**: Send packet → wait for ACK → retry on timeout
- **Configurable Retries**: 200ms timeout, max 5 retries
- **Chaos Engine**: Simulates packet loss (configurable drop rate) to test reliability

## Protocol Format

```
┌──────────────┬──────────────┬──────────┬─────────────────┐
│  packet_id   │  timestamp   │   type   │     payload     │
│   4 bytes    │   8 bytes    │  1 byte  │   variable      │
└──────────────┴──────────────┴──────────┴─────────────────┘
               Header (13 bytes)           JSON data
```

Packet types:
- `0x01` DATA - Sensor telemetry
- `0x02` ACK - Acknowledgment (server to client)
- `0x03` CMD - Control command (planned)

## How to Run

### Prereqs
- linux or wsl
- g++ (C++ compiler)
- some version of python

Install build tools if needed!

### 1. Start the Gateway Server
```bash
cd gateway
python3 server.py
```

### 2. Run the Client (in another terminal)
```bash
cd client
make
./build/aura_client
```

### Testing Packet Loss
Adjust `DROP_CHANCE` in `gateway/server.py` (default 0.3 = 30% loss):
```python
DROP_CHANCE = 0.5  # 50% packet loss
```

The client retries until it gets an ACK or hits max retries.

## Technical Decisions

### Why `#pragma pack`?
Without packing, the compiler adds padding bytes for CPU alignment. A 13-byte struct becomes 16 bytes. Since we're sending raw bytes over the network and parsing them in Python with `struct.unpack`, both sides must agree on exact byte positions. `#pragma pack(push, 1)` ensures no padding.

### Why Stop-and-Wait instead of Sliding Window?
Stop-and-Wait (send 1 packet, wait for ACK) is simpler to implement and sufficient for low-throughput IoT sensors. Sliding Window would allow sending multiple packets before waiting, increasing throughput but adding complexity with out-of-order handling. For this project, simplicity is better.

### Why I chose to use manual `memcpy` instead of serialization libraries?
Demonstrates embedded-style memory management. On a real microcontroller, you wouldn't have Protobuf or JSON libraries. Manual buffer construction shows understanding of memory layout and is more representative of firmware development (I believe).

## What I Learned

- **Socket programming**: `socket()`, `sendto()`, `recvfrom()`, `setsockopt()` for timeouts
- **Binary protocols**: Packing structs, byte order (`htons`), cross-language serialization
- **ARQ fundamentals**: Timeouts, retransmission, acknowledgments (ACKs)
- **Network simulation**: Chaos engineering to test reliability under failure conditions

## Future Work

- [ ] AI agent that monitors packet loss rate and tells client to adapt (reduce data rate, switch modes)
- [ ] Multi-threaded client with separate listener for incoming commands
- [ ] Sliding window for higher throughput
- [ ] DTLS or HMAC for security

## Project Structure (for now, might change later on)

```
aura-protocol/
├── client/
│   ├── src/main.cpp      # IoT device simulator
│   ├── Makefile
│   └── build/
├── gateway/
│   └── server.py         # Cloud gateway with chaos engine
├── docs/
│   └── DESIGN.md         # Detailed protocol design
└── README.md
```
