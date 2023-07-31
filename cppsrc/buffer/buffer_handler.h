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

            bool tx_lock = false;
            bool rx_lock = false;
            Eigen::MatrixXf tx_buf; // output of the gnn
            std::vector<Eigen::MatrixXf> rx_buf;

        public:
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
            
            // TODO update return and parameter types etc
            void SetTXBuf(Eigen::MatrixXf buf);
            void SetRXBuf(std::vector<Eigen::MatrixXf> buf);
            //Eigen::MatrixXf GetTxBuf();
            //std::vector<Eigen::MatrixXf> GetRxBuf();

            // TODO Filtering step (send relevant robots)
            // TODO create communication map (positions of robots at a minimum)
            // TODO Threading (tackle first)
    };
}
#endif