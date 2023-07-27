
sudo ip link set wlx9cefd5f8f545 down
sudo ip link set wlx9cefd5f8f545 name adhoc

sudo iw dev adhoc set type ibss
sudo iw dev adhoc set mcast_rate 54
sudo ip link set adhoc up

sudo iw dev adhoc ibss join drones 2412


sudo ip link set adhoc multicast on
sudo ip addr add 10.42.0.1/24 dev adhoc
