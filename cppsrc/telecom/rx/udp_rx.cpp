#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <net/if.h>
#include <vector>
#include <arpa/inet.h>
#include <memory>
#include "SharedBuffer.h"
#include "udp_rx.h"

UDP_RX::UDP_RX(){
    // Create a socket
    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_ == -1) {
        std::cerr << "Failed to create socket." << std::endl;
    }

    // Set the desired interface name
    const char* interfaceName = "adhoc"; // Replace with your desired interface name

    // Set the interface for the socket to bind to
    if (setsockopt(sockfd_, SOL_SOCKET, SO_BINDTODEVICE, interfaceName, strlen(interfaceName)) == -1) {
        std::cerr << "Failed to set socket options." << std::endl;
        close(sockfd_);
    }

    // Set up server address
    sockaddr_in serverAddr_{};
    serverAddr_.sin_family = AF_INET;
    serverAddr_.sin_port = htons(901); // Set the desired port (900 in this case)
    serverAddr_.sin_addr.s_addr = inet_addr("224.0.0.251"); // Set the desired IP address (224.0.0.251 in this case)

    // Bind the socket to the server address
    if (bind(sockfd_, reinterpret_cast<sockaddr*>(&serverAddr_), sizeof(serverAddr_)) == -1) {
        std::cerr << "Failed to bind socket." << std::endl;
        close(sockfd_);
    }
    std::cout << "Server is listening on IP " << inet_ntoa(serverAddr_.sin_addr) << ", port " << ntohs(serverAddr_.sin_port) << std::endl;
}

UDP_RX::~UDP_RX(){
    // Close the socket
    close(sockfd_);
}


void UDP_RX::Receive() {

    char sharedBuffer[NUM_IPS][BUFFER_SIZE] = {0};

    // Receive and handle incoming messages
    std::vector<float> buffer(BUFFER_SIZE / sizeof(float));
    sockaddr_in clientAddr{};
    socklen_t clientAddrLen = sizeof(clientAddr);
    ssize_t numBytesReceived;

    while (true) {
        // Receive message from a client
        numBytesReceived = recvfrom(sockfd_, buffer.data(), BUFFER_SIZE, 0, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
        if (numBytesReceived == -1) {
            std::cerr << "Failed to receive message." << std::endl;
            close(sockfd_);
        }

        // Convert the received data to a vector of floats
        size_t numFloatsReceived = numBytesReceived / sizeof(float);
        std::vector<float> receivedData(buffer.begin(), buffer.begin() + numFloatsReceived);

        // Get the IP address as a string
        std::string ipAddress = inet_ntoa(clientAddr.sin_addr);

        // Find the index of the IP address in the range 10.42.0.1 to 10.42.0.20
        int index = std::stoi(ipAddress.substr(ipAddress.find_last_of('.') + 1)) - 1;

        if (!lock_){
            // Store the received data in the corresponding row of the shared buffer
            if (receivedData.size() <= BUFFER_SIZE / sizeof(float)) {
                for (size_t i = 0; i < receivedData.size(); ++i) {
                    sharedBuffer[index][i] = receivedData[i];
                }
            }
        }

        // Handle the received message (e.g., process data, etc.)
        std::cout << "Received message from " << ipAddress << ": ";
        for (const auto& value : receivedData) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
        }

    return;
}

// TODO change return
std::shared_ptr<char**> UDP_RX::Trigger(){
    lock_ = true;
    // copy of buffer
    for (int i = 0; i < NUM_IPS; i++) {
        for (int j = 0; j < BUFFER_SIZE; j++) {
            cpy_sharedBuffer[i][j] = sharedBuffer[i][j];
        }
    }

    // clear original shared buffer
    memset(sharedBuffer, 0, sizeof(sharedBuffer));

    std::shared_ptr<char**> buf_ptr = std::make_shared<char**>(cpy_sharedBuffer);

    // set lock to false
    lock_ = false;
    // TODO positions of nbors
    // TODO position of this robot

    // shared pointer to shared buffer
    return buf_ptr;
}
