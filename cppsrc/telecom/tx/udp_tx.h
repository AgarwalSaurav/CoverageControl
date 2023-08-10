#ifndef MULTICASTSENDER_H
#define MULTICASTSENDER_H
#include <memory>
#include <vector>
#include <cstring>
#include <atomic>
#include <thread>

class UDP_TX{
    private:
        int sockfd_;
        // TODO Message Struct
        std::shared_ptr<std::vector<float>> tx_msg_ptr_ = nullptr;
        std::atomic<bool> lock_ {false};
        sockaddr_in serverAddr_{};

    public:
        UDP_TX(std::string ifname = "adhoc");
        ~UDP_TX();
        void Transmit();
        void Trigger(std::shared_ptr<std::vector<float>> message);
        // TODO get position from done()
};
#endif // MULTICASTSENDER_H