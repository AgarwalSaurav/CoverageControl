#include "buffer_handler.h"
#include "udp_tx.h"
#include "udp_rx.h"
#include <thread>
#include <chrono>
#include <vector> 
#include <random>
#include <iostream>
#include <boost/program_options.hpp>

namespace CC = CoverageControl;
namespace po = boost::program_options;

int main(int argc, char* argv[]){
    //std::cout << "Hello World" << std::endl;

    bool isTX;
    bool isRX;

    // argument handling
    try{
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "show help message")
            ("tx", "set TX flag")
            ("rx", "set RX flag");
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }
        
        if(vm.count("tx")){
            std::cout << "TX flag set" << std::endl;
            isTX = true;
        }
        if(vm.count("rx")){
            std::cout << "RX flag set" << std::endl;
            isRX = true;
        }
    } catch(const po::error &e){
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    if(isTX && isRX){
        std::cerr << "Error: cannot set as both TX and RX" << std::endl;
        return -1;
    }

    if(isTX){
        CC::BufferHandler buf;
        CC::UDP_TX udptx;

        buf.Trigger(udptx);
        udptx.Transmit();
    }
    else if(isRX){
        std::cout << "RX not implemented yet" << std::endl;
    }

    // Sleep for the specified interval
    //std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
    return 0;
}