#include "udp_tx.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/if.h>

bool sendMulticastMessage(const char* message) {
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return false;
    }


    // Set the interface for the socket to bind to
    if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, "adhoc", strlen("adhoc")) == -1) {
        std::cerr << "Failed to set socket options." << std::endl;
        close(sockfd);
        return 1;
    }

    // Set up server address
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(901); // Set the desired port (900 in this case)
    serverAddr.sin_addr.s_addr = inet_addr("224.0.0.251"); // Set the desired IP address (224.0.0.251 in this case)

    // Prepare the message to be sent
    size_t messageLength = std::strlen(message);

    // Send the message
    ssize_t numBytesSent = sendto(sockfd, message, messageLength, 0, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
    if (numBytesSent == -1) {
        std::cerr << "Failed to send message." << std::endl;
        close(sockfd);
        return false;
    }

    std::cout << "Sent message: " << message << std::endl;

    // Close the socket
    close(sockfd);

    return true;
}