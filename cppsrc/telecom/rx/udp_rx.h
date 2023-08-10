#ifndef MULTICASTRECEIVER_H
#define MULTICASTRECEIVER_H
#include <atomic>

class UDP_RX{
    private:
        int sockfd_;
        sockaddr_in serverAddr_;
        std::atomic<bool> lock_ {false};

        char sharedBuffer[NUM_IPS][BUFFER_SIZE] = {0};
        char cpy_sharedBuffer[NUM_IPS][BUFFER_SIZE] = {0};

    public:
        UDP_RX();
        ~UDP_RX();
        void Receive();
        std::shared_ptr<char**> Trigger();

};

#endif