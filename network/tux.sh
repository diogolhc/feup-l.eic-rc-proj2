#!/bin/bash
table_num=$1
tuxy_num=$2
echo "Table: $table_num"
echo "Computer: $tuxy_num"
ifconfig eth0 down
ifconfig eth1 down
systemctl restart networking
ifconfig eth0 down
ifconfig eth1 down
if [ $tuxy_num -eq 2 ]
then
    ifconfig eth0 up
    ifconfig eth0 172.16."$table_num"1.1/24
    route add default gw 172.16."$table_num"1.1 eth0
    ip route add 172.16."$table_num"0.0/24 via 172.16."$table_num"1.253 dev eth0
    route add default gw 172.16."$table_num"1.254
elif [ $tuxy_num -eq 3 ]
then
    ifconfig eth0 up
    ifconfig eth0 172.16."$table_num"0.1/24
    route add default gw 172.16."$table_num"0.1 eth0
    ip route add 172.16."$table_num"1.0/24 via 172.16."$table_num"0.254 dev eth0
    route add default gw 172.16."$table_num"0.254
elif [ $tuxy_num -eq 4 ]
then
    ifconfig eth0 up
    ifconfig eth0 172.16."$table_num"0.254/24
    route add default gw 172.16."$table_num"0.254 eth0
    ifconfig eth1 up
    ifconfig eth1 172.16."$table_num"1.253/24
    route add default gw 172.16."$table_num"1.253 eth1
    echo 1 > /proc/sys/net/ipv4/ip_forward
    echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
    route add default gw 172.16."$table_num"1.254
else
    echo "Bad input"
fi
