#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <net/if.h>

#include <arpa/inet.h>
#include "SharedBuffer.h"


int main() {
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // Set the desired interface name
    const char* interfaceName = "adhoc"; // Replace with your desired interface name

    // Set the interface for the socket to bind to
    if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, interfaceName, strlen(interfaceName)) == -1) {
        std::cerr << "Failed to set socket options." << std::endl;
        close(sockfd);
        return 1;
    }


    // Set up server address
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(901); // Set the desired port (900 in this case)
    serverAddr.sin_addr.s_addr = inet_addr("224.0.0.251"); // Set the desired IP address (224.0.0.251 in this case)

    // Bind the socket to the server address
    if (bind(sockfd, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to bind socket." << std::endl;
        close(sockfd);
        return 1;
    }

    std::cout << "Server is listening on IP " << inet_ntoa(serverAddr.sin_addr) << ", port " << ntohs(serverAddr.sin_port) << std::endl;


    char sharedBuffer[NUM_IPS][BUFFER_SIZE] = {0};

    // Receive and handle incoming messages
    char buffer[BUFFER_SIZE];
    sockaddr_in clientAddr{};
    socklen_t clientAddrLen = sizeof(clientAddr);
    ssize_t numBytesReceived;

    while (true) {
        // Receive message from a client
        numBytesReceived = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
        if (numBytesReceived == -1) {
            std::cerr << "Failed to receive message." << std::endl;
            close(sockfd);
            return 1;
        }

        buffer[numBytesReceived] = '\0';

        // Get the IP address as a string
        std::string ipAddress = inet_ntoa(clientAddr.sin_addr);

        // Find the index of the IP address in the range 10.42.0.1 to 10.42.0.20
        int index = std::stoi(ipAddress.substr(ipAddress.find_last_of('.') + 1)) - 1;

        // Store received data in the corresponding row of the shared buffer
        std::strcpy(sharedBuffer[index], buffer);

        // Handle the received message (e.g., process data, etc.)
        std::cout << "Received message from " << ipAddress << ": " << sharedBuffer[index] << std::endl;

        }

    // Close the socket
    close(sockfd);

    return 0;
}
