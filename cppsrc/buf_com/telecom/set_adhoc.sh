sudo ip link set wlx9cefd5f8f545 down
sudo ip link set wlx9cefd5f8f545 name adhoc

sudo iw dev adhoc set type ibss
sudo iw dev adhoc set mcast_rate 54
sudo ip link set adhoc up

sudo iw dev adhoc ibss join drones 2412


sudo ip link set adhoc multicast on
sudo ip addr add 10.42.0.1/24 dev adhoc

## Original Interface name may not be "wlx9cefd5f8f545". 
## If this is the case, use the command "iwconfig" to know the interface name and replace it in lines 1 and 2