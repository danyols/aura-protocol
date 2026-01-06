#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <chrono>
#include <cstdint>

// configuration
const char* SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 8080; // SERVER PORT, NOT THE CLIENT
const size_t BUFFER_SIZE = 256; // small, changeable
const int TIMEOUT_MS = 200;
const int MAX_RETRIES = 5;

// different packet types
const uint8_t TYPE_DATA = 0x01;
const uint8_t TYPE_ACK = 0x02;
const uint8_t TYPE_CMD = 0x03;

// remove the extra padding
// start packing with 1 byte-alignment
#pragma pack(push, 1)
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
        system_clock::now().time_since_epoch() // time since 1970
    ).count(); // extract number from duration object
}

const size_t MAX_PAYLOAD_SIZE = BUFFER_SIZE - sizeof(PacketHeader);

int main() {
    
    // UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    // set receive timeout so recvfrom (in the server) doesn't block forever
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = TIMEOUT_MS * 1000; // converting ms to microseconds
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    struct sockaddr_in server_addr; // address label
    memset(&server_addr, 0, sizeof(server_addr)); // filling every byte of server_addr with 0's
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr); // localhost

    // constructing the packet
    PacketHeader header;
    header.packet_id = 1;
    header.timestamp = now_ms();
    header.type = TYPE_DATA;

    // real-life example here
    const char *payload = "{\"temp\": 36.5, \"hr\": 72}"; // going to use json format
    size_t payload_len = strlen(payload);

    // to watch out for buffer overflow
    // using small buffer size for now
    if (payload_len > MAX_PAYLOAD_SIZE) {
        std::cerr << "Error: The payload is too large (it's " << payload_len << " bytes, but max is " << MAX_PAYLOAD_SIZE << ")\n";
        close(sock);
        return 1;
    }

    char buffer[BUFFER_SIZE]; // whatever I sent constant to
    memcpy(buffer, &header, sizeof(header));
    memcpy(buffer + sizeof(header), payload, payload_len);

    size_t total_size = sizeof(header) + payload_len;

    std::cout << "Sending packet #" << header.packet_id << " (" << total_size << " bytes)" << std::endl;

    // ssize_t is for storing as a signed value, in case of error

    // ARQ retry loop
    bool ack_received = false;
    int retries = 0;
    while (!ack_received && retries < MAX_RETRIES) {
        // while NOT received ACK, keep looping
        sendto(sock, buffer, total_size, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

        ssize_t received = recvfrom(sock, buffer, BUFFER_SIZE, 0, nullptr, nullptr);

        if (received > 0) {
            // big enough to be header?
            if (received >= sizeof(PacketHeader)) {

                PacketHeader ack_header;
                memcpy(&ack_header, buffer, sizeof(PacketHeader));

                // match the packets
                if (ack_header.type == TYPE_ACK && ack_header.packet_id == header.packet_id) {
                    ack_received = true;
                    std::cout << "ACK received!\n";
                }
        }
        // if this check fails, ack_received is not set, so loop continues

        } else {
            retries++;
            std::cout << "Timeout, retry " << retries << "/" << MAX_RETRIES << "\n";
        }
    }

    // if (sent < 0) {
    //     std::cerr << "Failed to send packet\n";
    //     close(sock);
    //     return 1;
    // }

    // const char* msg = "hello from client";   
    // sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    // // it's IPv4 specific
    // std::cout << "Sent: " << msg << "\n";

    close(sock);
    std::cout << "Done!\n";
    return 0;
}
