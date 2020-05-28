#/bin/bash

# Install acpica-tools and run this script with privilege.



rmmod lk.ko

FILE=./apic.dat

#mem_start=`cat /sys/firmware/memmap/\`ls -1 /sys/firmware/memmap | sort  -g | tail -n 1\`/start`
mem_start=`head -n 1 /proc/meminfo | awk '{print $2}' | awk '{num=strtonum($1); num=rshift(num,20)+2; num=lshift(num,30); printf "0x%x",num}'`
mem_end=`cat /sys/firmware/memmap/\`ls -1 /sys/firmware/memmap | sort  -g | tail -n 1\`/end`
#echo $mem_start $mem_end
cat /sys/firmware/acpi/tables/APIC > apic.dat
iasl apic.dat > /dev/null 2>&1
cat ./apic.dsl | grep -w "Local Apic ID \| Processor x2Apic ID" | awk '{printf "%d\n",strtonum("0x" $8) }' > apic.list 
cat /proc/cpuinfo | grep 'initial apicid' | awk '{print $4}' > cpu.list 
cpulist=`comm -3 apic.list cpu.list 2>/dev/null | awk '{printf "%d,", $1}'`
#echo $cpulist
#echo "insmod ../sideloader/lk/lk.ko meminfo=\"$mem_start,$mem_end\" cpuinfo=\"$cpulist\""
insmod ../sideloader/lk/lk.ko meminfo="$mem_start,$mem_end" cpuinfo="$cpulist"
rm apic.dat apic.dsl apic.list cpu.list 
 

