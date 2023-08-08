#ifndef COVERAGECONTROL_BUFFER_HANDLER
#define COVERAGECONTROL_BUFFER_HANDLER

#include <memory>
#include <Eigen/Dense>
#include "typedefs.h"

namespace CoverageControl {
    class BufferHandler{
        private:
            int L_;
            int K_;
            int G_;
            int X_0_;
            Point2 pt_;

            std::shared_ptr<Eigen::MatrixXf> tx_buf_ = nullptr;
            std::shared_ptr<Eigen::MatrixXf> rx_buf_ = nullptr;

        public:
            bool tx_lock = false;
            bool rx_lock = false;
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
            void TriggerGNN(); //locks the tx and rx buffers
            void ReleaseTXLock();
            void ReleaseRXLock();

            bool GetTXLock();
            bool GetRXLock();
            
            void SetTXBuf(const std::shared_ptr<Eigen::MatrixXf> buf_ptr);
            std::shared_ptr<Eigen::MatrixXf> GetTXBuf();
            void SetRXBuf(const std::shared_ptr<Eigen::MatrixXf> buf_ptr);
            std::shared_ptr<Eigen::MatrixXf> GetRXBuf();

            // TODO Filtering step (send relevant robots)
            // TODO create communication map (positions of robots at a minimum)
            // TODO Threading (tackle first)
    };
}
#endif