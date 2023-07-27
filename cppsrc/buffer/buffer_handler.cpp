#include "buffer_handler.h"

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

// TODO sort out type and if and where to use pointer for buf
void BufferHandler::SetTXBuf(Eigen::MatrixXf buf){}
void BufferHandler::SetRXBuf(std::vector<Eigen::MatrixXf> buf){}

Eigen::MatrixXf BufferHandler::GetTXBuf() {
    return tx_buf;
}
std::vector<Eigen::MatrixXf> BufferHandler::GetRXBuf(){
    return rx_buf;
}