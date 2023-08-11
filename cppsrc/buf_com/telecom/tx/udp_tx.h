#ifndef MULTICASTSENDER_H
#define MULTICASTSENDER_H
#include <memory>
#include <atomic>
#include <thread>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/if.h>
#include <vector> 

namespace CoverageControl{
    class UDP_TX{
        private:
            int sockfd_ = -1;
            // TODO Message Struct
            std::shared_ptr<std::vector<float>> tx_msg_ptr_ = nullptr;
            std::atomic<bool> lock_ {false};
            sockaddr_in serverAddr_{};

        public:
            UDP_TX();
            ~UDP_TX();
            void Transmit();
            void Trigger(std::shared_ptr<std::vector<float>> message);
            // TODO get position from done()
    };
}
#endif // MULTICASTSENDER_H