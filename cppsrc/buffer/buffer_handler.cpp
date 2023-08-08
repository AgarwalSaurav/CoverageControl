#include "buffer_handler.h"

namespace CoverageControl {

    void BufferHandler::Trigger(Eigen::MatrixXf &msg, Point2 v) {
        // TODO some conversion of msg
        Eigen::MatrixXf tx_msg_prime = msg;
        // 0. drone_ctl.set_velocity(v);
        // 1. udp_tx.Trigger(tx_msg_prime);
        // 2. auto[rx_msg, n_pos, pos] udp_rx.Trigger();
        return;
    }

    void BufferHandler::SetTXBuf(const std::shared_ptr<Eigen::MatrixXf> buf_ptr) {
        tx_buf_ = buf_ptr;
    }

    std::shared_ptr<Eigen::MatrixXf> BufferHandler::GetTXBuf(){
        return tx_buf_;
    }

    void BufferHandler::SetRXBuf(const std::shared_ptr<Eigen::MatrixXf> buf_ptr) {
        rx_buf_ = buf_ptr;
    }

    std::shared_ptr<Eigen::MatrixXf> BufferHandler::GetRXBuf(){
        return rx_buf_;
    }
}