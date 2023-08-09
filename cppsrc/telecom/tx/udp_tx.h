#ifndef MULTICASTSENDER_H
#define MULTICASTSENDER_H
#include <memory>
#include <vector>
#include <cstring>

class UDP_TX{
    private:
        int sockfd_;
        std::unique_ptr<std::vector<float>> tx_msg_ptr_;

    public:
        UDP_TX(std::string ifname = "adhoc");
        ~UDP_TX();
        void Transmit(const std::vector<float>& message);
        void Trigger(std::shared_ptr<std::vector<float>> message);
        // TODO get position from done()
}
#endif // MULTICASTSENDER_H