#include "buffer_handler.h"

namespace CoverageControl {

    void BufferHandler::TriggerGNN() {
        rx_lock = true;
        tx_lock = true;

        // TODO call appropriate functions, maybe some other stuff... 
        return;
    }

    void BufferHandler::ReleaseRXLock() {
        rx_lock = false;
    }

    void BufferHandler::ReleaseTXLock() {
        tx_lock = false;
    }

    bool BufferHandler::GetTXLock() {
        return rx_lock;
    }

    bool BufferHandler::GetRXLock() {
        return tx_lock;
    }

    // TODO sort out type and if and where to use pointer for buf
    void BufferHandler::SetTXBuf(const std::shared_ptr<Eigen::MatrixXf> buf_ptr) {
        tx_buf_ = buf_ptr;
    }

    std::shared_ptr<Eigen::MatrixXf> BufferHandler::GetTXBuf(){
        return tx_buf_;
    }

    void BufferHandler::SetRXBuf(std::vector<Eigen::MatrixXf> buf){}

/*    Eigen::MatrixXf BufferHandler::GetTXBuf() {
        return tx_buf;
    }
    std::vector<Eigen::MatrixXf> BufferHandler::GetRXBuf(){
        return rx_buf;
    }
    */
}