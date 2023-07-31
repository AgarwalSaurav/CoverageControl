#include "udp_tx.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    const char* message = "Hello, multicast!";
    int intervalSeconds = 2;

    while (true) {
        // Access the shared buffer
        //for (int i = 0; i < NUM_IPS; ++i) {
        //    std::cout << "Data from IP " << i + 1 << ": " << sharedBuffer[i] << std::endl;
        //}

        // Call the sendMulticastMessage function with the given message
        sendMulticastMessage(message);

        // Sleep for the specified interval
        std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
    }

    return 0;
}