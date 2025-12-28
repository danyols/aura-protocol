#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <chrono>
#include <cstdint>

// configuration
const char* SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 8080

// different packet types
const uint8_t TYPE_DATA = 0x01;
const uint8_t TYPE_ACK = 0x02;
const uint8_t TYPE_CMD = 0x03;

// remove the extra padding
#pragma pack(push, 1)
// start packing with 1 byte-alignment
struct PacketHeader {
    uint32_t packet_id; // 4 bytes, which packet
    uint64_t timestamp; // 8 bytes, when sent
    uint8_t type; // 1 byte, either DATA, ACK OR CMD
    // total of 13 bytes
};
#pragma pack(pop) // telling compiler to go back to normal

uint64_t now_ms() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        system_clock::now().time_since_eopch() // time since 1970
    ).count() // extract number from duration object
}

int main() {
    // UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }
    // std::cout << "Socket created\n";

    struct sockaddr_in server_addr; // address label
    memset(&server_addr, 0, sizeof(server_addr)); // filling every byte of server_addr with 0's
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); // localhost

    const char* msg = "hello from client";   
    sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    // it's IPv4 specific
    std::cout << "Sent: " << msg << "\n";

    close(sock);
    return 0;
}
