#ifndef MULTICASTRECEIVER_H
#define MULTICASTRECEIVER_H
#include <atomic>
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

constexpr int NUM_IPS = 20; // Number of IP addresses
constexpr int BUFFER_SIZE = 1024; // Adjust the buffer size as needed

class UDP_RX{
    private:
        int sockfd_ = -1;
        sockaddr_in serverAddr_{};
        std::atomic<bool> lock_ {false};

        char sharedBuffer[NUM_IPS][BUFFER_SIZE] = {0};
        char cpy_sharedBuffer[NUM_IPS][BUFFER_SIZE] = {0};

    public:
        UDP_RX();
        ~UDP_RX();
        void Receive();
        //std::shared_ptr<char**> Trigger();

};

#endif