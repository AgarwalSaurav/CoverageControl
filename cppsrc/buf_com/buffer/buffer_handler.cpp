#include "buffer_handler.h"

namespace CoverageControl {

    BufferHandler::BufferHandler(){
        // Generate a random message for testing purposes
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        // Call the sendMulticastmsg_ function with the given msg_
        for (int i = 0; i < 1024; ++i) {
            msg_.push_back(dist(gen));
        }
    }

    void BufferHandler::Trigger(UDP_TX &udp_tx) {
        // TODO add arguments
        
        // Buffer owns the memory unless it deletes it before telecom is done
        tx_msg_prime_ = std::make_shared<std::vector<float>>(msg_);

        // 0. drone_ctl.set_velocity(v);
        // TODO

        // 1. TX Trigger to load new message into transmiter
        udp_tx.Trigger(tx_msg_prime_);

        // 2. auto[rx_msg_, n_pos, pos] udp_rx.Trigger();
        // TODO

        return;
    }
}