#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

int main() {
    // UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }
    std::cout << "Socket created\n";

    struct sockaddr_in server_addr; // address label
    memset(&server_addr, 0, sizeof(server_addr)); // filling every byte of server_addr with 0's
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", ) // localhost

    const char* msg = "hello from client";   
    sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    // IPv4 specific
    std::cout << "Sent: " << msg << "\n";

    close(sock);
    return 0;
}
