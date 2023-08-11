#include "buffer_handler.h"
#include "udp_tx.h"
#include "udp_rx.h"
#include <thread>
#include <chrono>
#include <vector> 
#include <random>
#include <iostream>

int main(){

    CoverageControl::BufferHandler buf;
    CoverageControl::UDP_TX udptx;

    buf.Trigger(udptx);
    udptx.Transmit();

    // Sleep for the specified interval
    //std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
}