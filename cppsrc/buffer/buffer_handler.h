#ifndef COVERAGECONTROL_BUFFER_HANDLER
#define COVERAGECONTROL_BUFFER_HANDLER

#include <memory>
#include <Eigen/Dense>
#include "typedefs.h"

class BufferHandler{

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
        ~BufferHandler();
        void TriggerGNN(); //locks the tx and rx buffers
        void ReleaseTXLock();
        void ReleaseRXLock();
        
        // TODO update return and parameter types etc
        void SetTXBuf(Eigen::MatrixXf buf);
        void SetRXBuf(std::vector<Eigen::MatrixXf> buf);
        Eigen::MatrixXf GetTxBuf();
        std::vector<Eigen::MatrixXf> GetRxBuf();
        
    private:
        bool tx_lock = false;
        bool rx_lock = false;
        Eigen::MatrixXf tx_buf(L_, G_); // output of the gnn
        std::vector<Eigen::MatrixXf> rx_buf;
};

#endif