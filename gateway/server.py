import socket
import struct

HOST = '127.0.0.1'
PORT = 8080

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