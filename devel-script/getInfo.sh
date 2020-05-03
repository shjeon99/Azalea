#/bin/bash

# Install acpica-tools and run this script with privilege.

FILE=./apic.dat

mem_start=`cat /sys/firmware/memmap/\`ls -1 /sys/firmware/memmap | sort  -g | tail -n 1\`/start`
mem_end=`cat /sys/firmware/memmap/\`ls -1 /sys/firmware/memmap | sort  -g | tail -n 1\`/end`
#echo $mem_start $mem_end
cat /sys/firmware/acpi/tables/APIC > apic.dat
iasl apic.dat > /dev/null 2>&1
cpulist=`cat ./apic.dsl | grep -w "Local Apic ID \| Processor x2Apic ID" | awk '{printf "%d,",strtonum("0x" $8) }'`
#echo $cpulist
insmod ../sideloader/lk/lk.ko meminfo="$mem_start,$mem_end" cpuinfo="$cpulist"
rm apic.dat apic.dsl
 

