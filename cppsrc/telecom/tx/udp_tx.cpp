#include "udp_tx.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/if.h>
#include <vector> 

UDP_TX::UDP_TX(std::string ifname){

    // TODO change string literal
    std::string ifname_ = ifname;

    // Create a socket
    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_ == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return;
    }

    // Set the interface for the socket to bind to
    if (setsockopt(sockfd_, SOL_SOCKET, SO_BINDTODEVICE, "adhoc", strlen("adhoc")) == -1) {
        std::cerr << "Failed to set socket options." << std::endl;
        close(sockfd_);
        return;
    }

    // Set up server address
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(901); // Set the desired port (900 in this case)
    serverAddr.sin_addr.s_addr = inet_addr("224.0.0.251"); // Set the desired IP address (224.0.0.251 in this case)
}

UDP_TX::~UDP_TX(){
    // Close the socket
    close(sockfd_);
}

void UDP_TX::Transmit(const std::vector<float>& message) {

    // Prepare the message to be sent
    size_t messageLength = message.size() * sizeof(float);

    // Send the message
    ssize_t numBytesSent = sendto(sockfd_, reinterpret_cast<const char*>(message.data()), messageLength, 0, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
    if (numBytesSent == -1) {
        std::cerr << "Failed to send message." << std::endl;
        close(sockfd_);
        return;
    }

    std::cout << "Sent message of size: " << messageLength << " bytes." << std::endl;
    // TODO put a sleep variable
    return;
}

void UDP_TX::Trigger(std::shared_ptr<std::vector<float>> message){
    return;
}