import socket
import struct
import time
import random

# CONFIG
HOST = '127.0.0.1'
PORT = 8080
# to simulate packet loss:
DROP_CHANCE = 0.3

# packet format that has to match C++ struct
# < means little-endian, CPU and C++ struct is like this
HEADER_FORMAT = '<IQB' # I-4, Q-8, B-1
HEADER_SIZE = struct.calcsize(HEADER_FORMAT)

TYPE_DATA = 0x01
TYPE_ACK = 0x02
TYPE_CMD = 0x03

# remember that it's packet_id, timestamp, and type
# convert the number to a readable name
def type_to_string(pkt_type):
    if pkt_type == TYPE_DATA:
        return "DATA"
    elif pkt_type == TYPE_ACK:
        return "ACK"
    elif pkt_type == TYPE_CMD:
        return "CMD"
    else:
        return f"Unknown type: ({pkt_type})"

def main():

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # bind to address, listen on this port
    sock.bind((HOST, PORT))
    print(f"[Gateway] Listening on {HOST}:{PORT}")
    print(f"[Gateway] Header size: {HEADER_SIZE} bytes")
    print()

    while True:
        data, client_addr = sock.recvfrom(1024) # just a small msg for now
        # WHO SENT
        # client_addr says packet came from IP on random port

        # message = data.decode('utf-8')
        # print(f"[RECV] From {client_addr}: {message}")

        print(f"[Received] {len(data)} bytes from {client_addr}")

        # Chaos engine
        if random.random() < DROP_CHANCE:
            print(f"[CHAOS] Dropped packet!")
            print()
            continue

        if len(data) < HEADER_SIZE:
            print(f"Error: Packet too small ({len(data)}) bytes, need {HEADER_SIZE}")
            continue

        # split between the header and payload
        header_bytes = data[:HEADER_SIZE]
        payload_bytes = data[HEADER_SIZE:]

        # bytes to python numbers
        packet_id, timestamp, pkt_type = struct.unpack(HEADER_FORMAT, header_bytes)

        # bytes to text string
        payload = payload_bytes.decode('utf-8')

        print(f"Packet ID: {packet_id}")
        print(f"Timestamp: {timestamp}")
        print(f"Type: {pkt_type}")
        print(f"Payload: {payload}")
        print()

        # === pack back up to send ACK back ===
        ack_header = struct.pack(HEADER_FORMAT, packet_id, timestamp, TYPE_ACK)
        sock.sendto(ack_header, client_addr)
        print(f"[Sent ACK for packet #{packet_id}]") 
        print()

if __name__ == "__main__":
    main()
