#!/bin/bash
# This script creates virtual CAN port using slcan
# You can use it in 2 ways:
# ./scripts/create_slcan_from_serial.sh - use automatic device path search
# ./scripts/create_slcan_from_serial.sh /dev/ttyACMx - use user device path

# 1. Set tty settings
if [ $# == 1 ]; then
    DEV_PATH=$1
else
    source $(dirname "$0")/get_sniffer_symlink.sh
    DEV_PATH=$DEV_PATH_SYMLINK
fi
if [ -z $DEV_PATH ]; then
    echo "Can't find expected tty device."
    exit 1
fi

is_root="$(whoami)"
if [[ $is_root =~ "root" ]]; then
    SUDO=""
else
    SUDO="sudo"
fi

# 2. Run daemon slcand from can-utils - link serial interface with a virtual CAN device
# It will get name slcan name base
#   -o              option means open command
#   -s8             option means 1000 Kbit/s CAN bitrate
#   -t hw           option means UART flow control
#   -S $BAUD_RATE   option means uart baud rate
#   $DEV_PATH       position argument means port name
# slcand -o -s8 -t hw -S $BAUD_RATE $DEV_PATH
BAUD_RATE=1000000
$SUDO slcand -o -c -f -s8 -t hw -S $BAUD_RATE $DEV_PATH


$SUDO ip link set up slcan0
$SUDO slcan_attach $DEV_PATH

# Setup SocketCAN queue discipline type
# By default it uses pfifo_fast with queue size 10.
# This queue blocks an application when queue is full.
# So, we use pfifo_head_drop. This queueing discipline drops the earliest enqueued
# packet in the case of queue overflow. 
# More about queueing disciplines:
# https://rtime.felk.cvut.cz/can/socketcan-qdisc-final.pdf
$SUDO tc qdisc add dev slcan0 root handle 1: pfifo_head_drop limit 1000