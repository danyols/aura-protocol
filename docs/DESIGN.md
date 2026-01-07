# AURA Protocol Design Document

Technical details of the protocol design, security considerations, and architectural decisions.

## Protocol Specification

### Header Format (13 bytes, little-endian)

| Offset | Field | Type | Description |
|--------|-------|------|-------------|
| 0-3 | packet_id | uint32 | packet identifier |
| 4-11 | timestamp | uint64 | Milliseconds since Unix epoch |
| 12 | type | uint8 | Packet type flag |

### Packet Types

| Value | Name | Direction | Purpose |
|-------|------|-----------|---------|
| 0x01 | DATA | Client → Server | Sensor telemetry payload |
| 0x02 | ACK | Server → Client | Acknowledgment of received packet |
| 0x03 | CMD | Server → Client | Control command (mode change, etc.) |

### Wire Format Example

DATA packet with JSON payload:

Bytes 0-3:   01 00 00 00        (packet_id = 1, little-endian)
Bytes 4-11:  A0 1B 2C 8D 01 00 00 00  (timestamp)
Byte 12:     01                 (type = DATA)
Bytes 13+:   {"temp": 36.5, "hr": 72}  (UTF-8 payload)

## Security Considerations

### 1. UDP Spoofing

**Risk:** UDP is connectionless. Any actor can send packets claiming to be the client.

### 2. Replay Attacks

**Risk:** Attacker captures a valid packet and replays it later.

**Mitigation:** Use an increasing packet_id. Server rejects any packet_id ≤ last seen ID. Timestamps can also enforce these freshness windows.

### 3. Buffer Overflows

**Risk:** Malformed packets with unexpected sizes could overflow buffers.

**Mitigation:**
- Always pass exact buffer size to `recvfrom()`
- Validate `received >= sizeof(PacketHeader)` before parsing
- Never treat raw network data as null-terminated strings without explicit checks

## Architectural Tradeoffs

### Stop-and-Wait vs Sliding Window

Considering approaches, throughput, how complex, and what I went with.

| Stop-and-Wait | Lower (idle while waiting) | Simple | Yes |
| Sliding Window | Higher (pipeline packets) | Complex (reordering, buffers) | No |

**Rationale:** For low-frequency IoT telemetry (1-10 packets/sec), Stop-and-Wait is sufficient. The simplicity reduces bugs and is appropriate.

### JSON vs Binary Payloads

| Format | Size | Parse Speed | Debuggability |
|--------|------|-------------|---------------|
| JSON | Larger | Slower | Easy to read |
| Binary (Protobuf, etc.) | Smaller | Faster | Requires tooling |

**Current choice:** JSON for debugging simplicity.  
**Future optimization:** Switch to packed binary structs for 10x smaller payloads.

### C++ Manual Memory vs Smart Pointers

**Choice:** Manual `memcpy` and stack-allocated buffers.

**Rationale:** Demonstrates embedded-style memory management. Real microcontrollers often lack dynamic allocation. Shows understanding of memory layout without the abstraction layers.