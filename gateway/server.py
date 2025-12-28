import socket
import struct

# CONFIG
HOST = '127.0.0.1'
PORT = 8080

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

    while True:
        data, client_addr = sock.recvfrom(1024) # just a small msg for now

        message = data.decode('utf-8')
        print(f"[RECV] From {client_addr}: {message}")

if __name__ == "__main__":
    main()
