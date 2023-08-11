# ******
# RX
# ******
add_library(udp_rx udp_rx.cpp)
target_include_directories(udp_rx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})