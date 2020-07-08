#!/bin/bash 

#usage: ./offload_local_proxy -o <no elements> -i <no elements> -c <no channels> -n <no nodes>
#note: no channels > core number
ulimit -n 1000000


./mod_ins.sh 

# The following conditions must be satisfied.
# total = worker * core 

worker=8
total=`cat /sys/azalea/cpuinfo | head -1 | awk '{print $5}'`
core=$((total/worker)) 

../sideloader/offload_proxy/offload_local_proxy -o 64 -i 64 -c $core -n $worker 
