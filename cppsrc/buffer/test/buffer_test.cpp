#define BOOST_TEST_MODULE MyTestModule
#include <boost/test/included/unit_test.hpp>
#include "../buffer_handler.h"

namespace CoverageControl{
    
    /**
     * Test: Trigger GNN
    */
    BOOST_AUTO_TEST_CASE(BufferTriggerTest) {

        bool rx_expected = true;
        bool tx_expected = true;

        Point2 coord(1.0,1.0);
        BufferHandler buf(5, 2, 256, 0, coord);
        buf.TriggerGNN();
        bool rx_actual = buf.GetRXLock();
        bool tx_actual = buf.GetTXLock();

        BOOST_TEST(rx_expected == rx_actual);
        BOOST_TEST(tx_expected == tx_actual);
    }

    BOOST_AUTO_TEST_CASE(BufferSetTXTest) {
        std::shared_ptr<Eigen::MatrixXf> test_ptr = std::make_shared<Eigen::MatrixXf>(5,2);
        Point2 coord(1.0,1.0);
        BufferHandler buf(5, 2, 256, 0, coord);
        buf.SetTXBuf(test_ptr);
        std::shared_ptr<Eigen::MatrixXf> ptr = buf.GetTXBuf();
        BOOST_TEST(ptr == test_ptr);
    }

    BOOST_AUTO_TEST_CASE(BufferSetRXTest) {
        std::shared_ptr<Eigen::MatrixXf> test_ptr = std::make_shared<Eigen::MatrixXf>(5,2);
        Point2 coord(1.0,1.0);
        BufferHandler buf(5, 2, 256, 0, coord);
        buf.SetRXBuf(test_ptr);
        std::shared_ptr<Eigen::MatrixXf> ptr = buf.GetRXBuf();
        BOOST_TEST(ptr == test_ptr);
    }

    
}
