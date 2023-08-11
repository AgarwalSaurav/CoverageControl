#ifndef COVERAGECONTROL_BUFFER_HANDLER
#define COVERAGECONTROL_BUFFER_HANDLER

#include <memory>
#include <vector>
#include <chrono>
#include <random>
#include <Eigen/Dense>
#include "typedefs.h"
#include "udp_tx.h"

namespace CoverageControl {
    class BufferHandler{
        private:
            int L_;
            int K_;
            int G_;
            int X_0_;
            Point2 pt_;

            std::vector<float> msg_;
            std::shared_ptr<Eigen::MatrixXf> tx_msg_ = nullptr;
            std::shared_ptr<std::vector<float>> tx_msg_prime_ = nullptr;
            //std::shared_ptr<Eigen::MatrixXf> ConvertTXBuf_(Eigen::MatrixXf &msg);

        public:
            BufferHandler();
            BufferHandler(const int L, 
                        const int K,
                        const int G,
                        const int X_0,
                        const Point2 pt) :
                        L_{L},
                        K_{K},
                        G_{G},
                        X_0_{X_0},
                        pt_{pt}
                        {}

            void Trigger(UDP_TX &udp_tx);

            // TODO Filtering step (send relevant robots)
            // TODO create communication map (positions of robots at a minimum)
            // TODO Threading (tackle first)
    };
}
#endif