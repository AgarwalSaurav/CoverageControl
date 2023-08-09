#!/bin/bash

### ad-hoc network configuration

#name of interface may vary between different wifi adapters.
sudo nmcli device set wlx9cefd5fae3d8 managed no
sudo ip link set wlx9cefd5fae3d8 down
sudo iwconfig wlx9cefd5fae3d8 mode ad-hoc
sudo iwconfig wlx9cefd5fae3d8 channel 3
sudo iwconfig wlx9cefd5fae3d8 essid 'drones'
sudo iwconfig wlx9cefd5fae3d8 key 6f6c6f6c6f
sudo iwconfig wlx9cefd5fae3d8 ap 66:5C:05:92:27:06
sudo ip link set wlx9cefd5fae3d8 up
sudo ip addr add 10.42.0.11/24 dev wlx9cefd5fae3d8 # ip address must be different for each node

### request to send/clear to send interaction off
sudo iwconfig wlx9cefd5fae3d8 rts off

### transmission power configuration.
# 20 is the maximum value for most wifi adapters.
sudo iwconfig wlx9cefd5fae3d8 txpower 20
