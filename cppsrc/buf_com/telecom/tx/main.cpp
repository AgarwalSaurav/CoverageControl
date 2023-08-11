#include "udp_tx.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector> 
#include <random>

int main() {
    int intervalSeconds = 2;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    // Access the shared buffer
    //for (int i = 0; i < NUM_IPS; ++i) {
    //    std::cout << "Data from IP " << i + 1 << ": " << sharedBuffer[i] << std::endl;
    //}

    // Call the sendMulticastMessage function with the given message
    std::vector<float> message;
    for (int i = 0; i < 1024; ++i) {
        message.push_back(dist(gen));
    }

    std::shared_ptr<std::vector<float>> msg_ptr = std::make_shared<std::vector<float>>(message);

    UDP_TX udptx;
    udptx.Trigger(msg_ptr);
    udptx.Transmit();

    // Sleep for the specified interval
    std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));

    return 0;
}