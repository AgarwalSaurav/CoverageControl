#include "udp_tx.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/if.h>
#include <vector> 

namespace CoverageControl{
    UDP_TX::UDP_TX(){

        // TODO change string literal
        std::string ifname_ = "adhoc";

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
        serverAddr_.sin_family = AF_INET;
        serverAddr_.sin_port = htons(901); // Set the desired port (900 in this case)
        serverAddr_.sin_addr.s_addr = inet_addr("224.0.0.251"); // Set the desired IP address (224.0.0.251 in this case)
        
    }

    UDP_TX::~UDP_TX(){
        // Close the socket
        close(sockfd_);
    }

    void UDP_TX::Transmit() {
        // hang until we get the first message
        while(tx_msg_ptr_ == nullptr){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::cout << "Waiting for message..." << std::endl;
        }

        std::vector<float> message;

        while(true){
            if(!lock_){
                if (tx_msg_ptr_ != nullptr){
                    message = *tx_msg_ptr_;
                }
            }
            // Prepare the message to be sent
            size_t messageLength = message.size() * sizeof(float);
            

            //std::string new_message = "helloworld";
            //size_t messageLength = new_message.size() * sizeof(char);

            /*for(char val : new_message){
                std::cout << val << std::endl;
            }*/
            //std::cout << sockfd_ << std::endl;

            // Send the message
            ssize_t numBytesSent = sendto(sockfd_, reinterpret_cast<const char*>(message.data()), messageLength, 0, reinterpret_cast<sockaddr*>(&serverAddr_), sizeof(serverAddr_));
            if (numBytesSent == -1) {
                perror("sendto error");
                std::cerr << "Failed to send message." << std::endl;
                close(sockfd_);
                return;
            }

            std::cout << "Sent message of size: " << messageLength << " bytes." << std::endl;
            
            // TODO sleep adjustment
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        return;
    }

    void UDP_TX::Trigger(std::shared_ptr<std::vector<float>> message){
        lock_ = true;
        // TODO make lock_guard / mutex
        tx_msg_ptr_ = message;
        lock_ = false;
        return;
    }
}