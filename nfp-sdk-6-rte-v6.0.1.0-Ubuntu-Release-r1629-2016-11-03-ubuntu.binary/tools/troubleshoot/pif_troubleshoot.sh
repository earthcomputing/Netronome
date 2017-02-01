#!/bin/bash
#
# Copyright (C) 2007-2016 Netronome Systems, Inc.  All rights reserved.
#
# File:       pif_troubleshoot.sh
# Description: Gathers information about the system for troubleshooting
#

# Default values
NETRONOME_DIR=${NETRONOME_DIR:-/opt/netronome}
NETRONOME_BIN=${NETRONOME_BIN:-${NETRONOME_DIR}/bin}
NETRONOME_ETC=${NETRONOME_ETC:-${NETRONOME_DIR}/etc}
NETRONOME_SYSINFO=${NETRONOME_SYSINFO:-${NETRONOME_DIR}/sysinfo}
NETRONOME_NMSB=${NETRONOME_NMSB:-${NETRONOME_DIR}/nmsb}
NETRONOME_NMSB_BIN=${NETRONOME_NMSB_BIN:-${NETRONOME_NMSB}/bin}
NETRONOME_GERCHR=${NETRONOME_GERCHR:-${NETRONOME_DIR}/gerchr}
NETRONOME_GERCHR_BIN=${NETRONOME_GERCHR_BIN:-${NETRONOME_GERCHR}/bin}
NETRONOME_LIBEXEC=${NETRONOME_LIBEXEC:-${NETRONOME_DIR}/libexec}
NETRONOME_SAMPLES=${NETRONOME_SAMPLES:-${NETRONOME_DIR}/samples}
NETRONOME_LOG=${NETRONOME_LOG:-${NETRONOME_DIR}/log}
PIF_LOG=${PIF_LOG:-/dev/stdout}

# Worker states, from me/worker_status.h
WORKER_STATS_XFER_BASE_LW=0
WORKER_STATS_XFER_SIZE_LW=4

if [ $EUID != 0 ]; then
    echo "This script requires root privileges. Please run it as root."
    exit 1
fi

function usage() {
cat <<EOH
Usage: $0 [ OPTIONS ]
Copyright (C) 2007-2015 Netronome Systems, Inc.

PIF troubleshooting utility gathers information about the system which may aid
in troubleshooting.

Without any options the script will save information about the following items
to files in /opt/netronome/sysinfo: host processor, memory, operating system,
compiler, kernel log, PIF logs, PIF debug counters, ARM ramdisk and kernel
versions and the status of PIF components. An archive of the files containing
this information is created in /opt/netronome/sysinfo/archives, and should be
included in bug reports sent to Netronome Systems technical support.

Instead of saving all of the abovementioned information to disk, it can also be
displayed individually on standard output by using the options below:

 -X       Run ALL the optional display items that follow and print to stdout
         (if in doubt then use this option)
 -d       Display the kernel ring buffer (dmesg)
 -l       Display device information (lspci)
 -H       Display device HWINFO as well as PIF configuration files
 -b       Display the last boot's kernel log, syslog, OVS logs and all
          Netronome logs in /opt/netronome/log
 -v       Display the software version information
 -t       Display the status of the system (ovs-ctl status)
 -s       Display information about host system
 -T       Display host \`top' util
 -R       Display current process information
 -O       Display OVS database and configuration
 -S       Display NFP symbols
 -M       Display ME status information
 -N       Display NFP status information
 -a       Display ARP/Routing tables of the system
 -n       Display port/netdev information
 -f       Display flow entries
 -F       Display flow cache lock counts
 -i       PIF statistics
 -D       Display NFD info
 -c       Display control channel VF info
 -g       Display GRO dump
 -C       Display NFP counters
 -L       Display symbol names and addresses
 -w       Display worker states
 -e       Display ECC info
 -G       Dump GPRs and Local Memory (stops system)

Other options:
 -h       This help message

Parsing options
 -P      Pass the name of a concatenated dump file containing any of the above
         troubleshoot items and split it out into seperate files, as would be
         created if one used the archive mechansim (ns_pif_troubleshoot.sh with
         no arguments). This can be used to parse a dump file sent from the
         customer if they fail to generate a .tgz file using the archive
         mechanism. We use the troubleshoot section headers as markers in order
         to split out the seperate sections.
 -p      The target output directory for the files generated using -P

Example 1:
    ./pif_troubleshoot -sv

    Displays information about the host system and the various versions.

Example 2:
    ./pif_troubleshoot

    Runs all the troubleshoot items and sends the output into seperate section
    files which are then tarred and zipped into an archive whose default location
    is /opt/netronome/sysinfo/

Example 3:
    ./pif_troubleshoot > /tmp/dump.txt

    Runs all the troubleshoot items and writes the contents in one large
    concatenated text file.

Example 4:
    ./pif_troubleshoot -P /tmp/dump.txt -p /tmp/items/

    Reads in the dump.txt file, finds all the troubleshoot items it contains
    and writes them each to seperate files named after their sections in the
    folder /tmp/items/
EOH
exit 1
}

function get_last_me_island_id() {
    hwinfo=$(findcmd nfp-hwinfo $NETRONOME_BIN)
    if [ -z "$hwinfo" ]; then
       [ -n "$print_step" ] && echo failed
       echo "Error: nfp-hwinfo not found - is BSP package installed?." >> $output
       return 1
    fi

    if [ -z $($hwinfo 2>/dev/null | grep chip.model=NFP624) ]; then
        # Full power
        echo 38
    else
        # Half power
        echo 37
    fi
    return 0
}

function get_last_gro_island_id() {
    hwinfo=$(findcmd nfp-hwinfo $NETRONOME_BIN)
    if [ -z "$hwinfo" ]; then
       [ -n "$print_step" ] && echo failed
       echo "Error: nfp-hwinfo not found - is BSP package installed?." >> $output
       return 1
    fi

    if [ -z $($hwinfo 2>/dev/null | grep chip.model=NFP624) ]; then
        # Full power
        echo 49
    else
        # Half power
        echo 48
    fi
    return 0
}

function troubleshoot_heading() {
    local item_name=$1
    local description=$2
    echo -e "===[ TROUBLESHOOT ItemName: \t$item_name ]===" >> $output
    echo -e "===[ TROUBLESHOOT Description: \t$description ]===" >> $output
    echo "" >> $output
}

function heading() {
    local head=$1
    local len=${#head}
    local hlen=$(((72 - len) / 2))
    local bar=
    for i in `seq 1 $hlen`; do
       bar="$bar="
    done
    echo -n $bar >> $output
    [ $((len % 2)) -ne 0 ] && echo -n '=' >> $output
    echo -n "[ $head ]" >> $output
    echo $bar >> $output
}

function align() {
    local spc=$(($2 - ${#1}))
    for i in `seq 1 $spc`; do
        echo -n ' '
    done
    echo -n $1
}

function kernel() {
    local bits smp preempt
    local version=$(uname -r)
    local arch=$(uname -m)
    local extra=$(uname -v)
    local compiler=$(sed -n 's/Linux version [^(]* ([^(]*) (\(.*\)) #.*/\1/p' /proc/version)
    kerncompiler=$(echo $compiler | sed -n 's/.* version \([0-9.]\+\) .*/\1/p')
    kerncompiler=${kerncompiler%.[0-9]} # drop patch version
    [ "$arch" == x86_64 ] && bits=64 || bits=32
    [ "${extra/SMP}" != "$extra" ] && smp=yes || smp=no
    [ "${extra/PREEMPT}" != "$extra" ] && preempt=yes || preempt=no

    heading "Kernel"
    echo "Version    : $version"
    echo "Architecture: $arch ($bits-bit)"
    echo "Compiler    : $compiler"
    echo "Commandline : "$(cat /proc/cmdline)
    echo "SMP        : $smp"
    echo "Preempt    : $preempt"
    echo
}

function kernel_limits() {
    heading "Kernel User Hard Limits for user"$(whoami)
    echo "Hard limits:"
    ulimit -Ha
    echo "Soft limits:"
    ulimit -Sa
}

function processor() {
    local ht
    local processor=$(uname -p)
    if [ "$processor" == unknown -o "$processor" == i686 -o "$processor" == x86_64 ]; then
        processor=$(grep '^model name' /proc/cpuinfo | head -n 1 | sed -n 's/.*: \(.*\)/\1/p')
    fi
    local cpus=$(grep ^processor /proc/cpuinfo | wc -l)
    grep ^flags /proc/cpuinfo | grep -wq ht && ht=yes || ht=no

    heading "Processor"
    echo "Processor    : $processor"
    echo "Number of CPUs: $cpus"
    echo "Hyperthreading: $ht"
    echo
}

function memory() {
    local mem_total=$(free | grep '^Mem:' | awk '{ print $2 }')
    local mem_free=$(free | grep '^-/+' | awk '{ print $4 }')
    local swap=$(free | grep '^Swap:')
    local swap_total=$(echo "$swap" | awk '{ print $2 }')
    local swap_free=$(echo "$swap" | awk '{ print $4 }')

    heading "Memory"
    echo -n "Total memory: "; align $mem_total  9; echo -n " kB ("; align $((mem_total  / 1024)) 5; echo " MB)"
    echo -n "Free memory : "; align $mem_free   9; echo -n " kB ("; align $((mem_free   / 1024)) 5; echo " MB)"
    echo -n "Total swap  : "; align $swap_total 9; echo -n " kB ("; align $((swap_total / 1024)) 5; echo " MB)"
    echo -n "Free swap   : "; align $swap_free  9; echo -n " kB ("; align $((swap_free  / 1024)) 5; echo " MB)"
    echo
}

function operating_system() {
    if [ -e /etc/issue ]; then
        local distro=$(head -n 1 /etc/issue)
    else
        local distro=""
    fi
    if [ -z "$distro" -a -e /etc/gentoo-release ]; then
        distro=$(cat /etc/gentoo-release)
    else
        # Remove \x escape sequences
        distro=${distro%% \\*}
    fi

    heading "Operating System"
    echo "Distribution: $distro"
    echo
}

function compiler() {
    heading "Compiler"
    if [ -z "$(which gcc 2>/dev/null)" ]; then
        echo "Not installed"
        return
    fi

    local version=$(gcc -v 2>&1 | grep '^gcc version')
    syscompiler=$(echo $version | sed -n 's/.* version \([0-9.]\+\) .*/\1/p')
    syscompiler=${syscompiler%.[0-9]} # drop patch version
    local arch=$(gcc -v 2>&1 | grep ^Target | sed -n 's/Target: \([^-]*\).*/\1/p')
    [ -z "$arch" ] && arch=unknown
    local bits; [ "$arch" == x86_64 ] && bits=64 || bits=32

    echo "Version    : $version"
    echo "Architecture: $arch ($bits-bit)"

    if [ "$kerncompiler" != "$syscompiler" ]; then
        echo
        echo "Warning: Kernel compiler ($kerncompiler) does not match system compiler ($syscompiler)"
    fi
    echo
}

function findcmd() {
    local cmd=$1
    local default_location=$2
    local path
    if [ -x $default_location/$cmd ]; then
        path=$default_location/$cmd
    else
        path=$(which $cmd 2>/dev/null)
    fi
    echo $path
}

function ns_common() {
    local display=$1
    local save_msg=$2
    local display_msg=$3
    local item_name=$4
    local file_name="$NETRONOME_SYSINFO/$item_name.txt"
    if [ -n "$display" -o -n "$run_all" ]; then
       output=/dev/stdout
       troubleshoot_heading "$item_name" "$display_msg"
    else
       echo -n "$save_msg"
       output=$file_name
       echo -n "" > $output
       troubleshoot_heading "$item_name" "$display_msg"
    fi
}

#---------------------------------------------------------------------
# Dump kernel ring buffer
#---------------------------------------------------------------------
function ns_dmesg() {
    ns_common "$show_dmesg" "Saving kernel ring buffer (dmesg)... " "Kernel ring buffer (dmesg)" "dmesg"
    dmesg >> $output
    local ret=$?
    [ -n "$print_step" ] && echo done
    return $ret
}

#---------------------------------------------------------------------
# Devices info
#---------------------------------------------------------------------
function ns_lspci() {
    local ret=0
    ns_common "$show_lspci" "Saving device info (lspci)... " "Device info (lspci)" "lspci-nfe"
    lspci=$(findcmd lspci /usr/sbin)
    if [ -n "$lspci" ]; then
        heading "Entire lspci output"
        $lspci -v >> $output
        heading "Netronome Specific lspci output"
        $lspci -vvvv -d 19ee: >> $output
        ret=$?
        [ -n "$print_step" ] && echo done
    else
        [ -n "$print_step" ] && echo failed
        echo "Error: lspci not found - please install the pciutils package for your system." >> $output
        ret=1
    fi
    return $ret
}

#---------------------------------------------------------------------
# Detailed Device info
#---------------------------------------------------------------------
function ns_hwinfo() {
    local ret=0
    ns_common "$show_hwinfo" "Saving NFE HWINFO and PIF config files..." "PIF config files and HWINFO" "hwinfo"

    heading "/etc/netronome.conf contents"
    if [ -f /etc/netronome.conf ]; then
        cat /etc/netronome.conf >> $output
        echo "" >> $output
    else
        echo "File not found" >> $output
    fi
    echo >> $output

    hwinfo=$(findcmd nfp-hwinfo $NETRONOME_BIN)
    if [ -z "$hwinfo" ]; then
       [ -n "$print_step" ] && echo failed
       echo "Error: nfp-hwinfo not found - is BSP package installed?." >> $output
       ret=1
    else
        #TODO Multi-card support still required
        heading "Hardware Info"
        $hwinfo >> $output
        ret=$((ret + $?))
    fi

    echo "" >> $output
    heading "NFP Programmables"
    ${NETRONOME_BIN}/nfp-programmables >> $output 2>&1
    ret=$((ret + $?))

    echo "" >> $output
    heading "NFP BSP Troubleshoot"
    ${NETRONOME_BIN}/nfp-troubleshoot >> $output 2>&1
    ret=$((ret + $?))

#     python=$(findcmd python)
#     if [ ! -z $python ]; then
#         heading "DPDK NIC bind status"
#         $python dpdk_nic_bind.py --status >> $output 2>&1
#     fi

    if [ $ret -eq 0 ]; then
        [ -n "$print_step" ] && echo done
    else
        [ -n "$print_step" ] && echo failed
    fi
    return $ret
}

#-----------------------------------------------------------------------
# Kernel bootup log (specifically for e820 memory map), PIF log / syslog
#-----------------------------------------------------------------------
function ns_bootlog() {
    local ret=0
    local filename=""
    local single_file=0
    ns_common "$show_bootlog" "Saving last logs... " "Boot log / syslog / OVS logs / Netronome logs" "logs"

    heading "Last 100,000 lines of syslog"
    tmpfilename="/tmp/pif_troubleshoot.$$"
    for partfilename in `ls -1tr /var/log/messages*gz /var/log/syslog*gz 2>/dev/null | tail -2`; do
        gzip -d -c < $partfilename >> $tmpfilename 2>/dev/null || cat $partfilename >> $tmpfilename 2>/dev/null
    done
    for partfilename in `ls -1tr /var/log/messages* /var/log/syslog* 2>/dev/null | grep -v gz | tail -2`; do
        cat $partfilename >> $tmpfilename 2>/dev/null
    done
    tail -100000 $tmpfilename >> $output
    rm -f $tmpfilename

    if [ -e /var/log/messages ]; then
        filename="/var/log/messages"
    elif [ -e /var/log/syslog ]; then
        filename="/var/log/syslog"
    else
        echo "Could not find system log file" >> $output
        ret=1
    fi

    heading "Last 100,000 lines of ovs-vswitchd log"
    tail -100000 /usr/local/var/log/openvswitch/ovs-vswitchd.log >> $output

    heading "Last 100,000 lines of ovsdb-server log"
    tail -100000 /usr/local/var/log/openvswitch/ovsdb-server.log >> $output

    heading "Interesting parts of kernel log"
    # Determine whether the lines we are interested in can be found in the default log file
    if grep -q ' Freeing unused kernel memory' "$filename"; then
       if grep -q ' Linux version ' "$filename"; then
          single_file=1
       fi
    fi

    # Prepare file containing combination of rotated parts if needed
    if [ $single_file -eq 0 ]; then
       filename="/tmp/ns_pif_troubleshoot.$$"
       echo "" > $filename
       for partfilename in `ls -1tr /var/log/messages*gz /var/log/syslog*gz 2>/dev/null | tail -10`; do
          gzip -d -c < $partfilename >> $filename 2>/dev/null || cat $partfilename >> $filename 2>/dev/null
       done
       for partfilename in `ls -1tr /var/log/messages* /var/log/syslog* 2>/dev/null | grep -v gz | tail -10`; do
          cat $partfilename >> $filename 2>/dev/null
       done
    fi

    # Find the line (from the end of the log) where the boot messages we are interested in ends
    endline=$(tac $filename 2>/dev/null | grep -m 1 -hn ' Freeing unused kernel memory' | cut -f 1 -d':')
    [ -z "$endline" ] && endline=0

    # Find the line (from the end of the log) where the last boot messages started
    startline=$(tac $filename 2>/dev/null | grep -m 1 -hn ' Linux version ' | cut -f 1 -d':')
    if [ -n "$startline" ]; then
       tail -n $startline $filename 2>/dev/null | head -n $((startline-endline+1)) >> $output
       ret=$?
    else
       echo "Could not find start of boot messages (log may have been rotated)" >> $output
    fi

    # Delete temp file if needed
    if [ $single_file -eq 0 ]; then
       rm -f $filename
    fi

    for fn in /opt/netronome/log/*.txt; do
       if [ -f $fn ]; then
          heading "$fn contents"
          cat $fn >> $output
          echo "" >> $output
       fi
    done

    if [ $ret -eq 0 ]; then
        [ -n "$print_step" ] && echo done
    else
        [ -n "$print_step" ] && echo failed
    fi
    return $ret
}

#---------------------------------------------------------------------
# Versions
#---------------------------------------------------------------------
function ns_versions() {
    local ret=0
    ns_common "$show_versions" "Saving versions... " "Versions ($(date))" "version"

    ${NETRONOME_BIN}/ovs-ctl version >> $output
    if [ $ret -eq 0 ]; then
        [ -n "$print_step" ] && echo done
    else
        [ -n "$print_step" ] && echo failed
    fi
    return $ret
}

#---------------------------------------------------------------------
# PIF components
#---------------------------------------------------------------------
function ns_piftest() {
    local ret=0
    ns_common "$show_piftest" "Saving status of PIF components... " "Status of PIF components" "health"
    if [ -x $NETRONOME_BIN/ovs-ctl ]; then
        $NETRONOME_BIN/ovs-ctl status >> $output 2>&1
        ret=$?
        [ -n "$print_step" ] && echo done
    else
        [ -n "$print_step" ] && echo failed
        echo "Error: Could not find ovs-ctl."
        ret=1
    fi
    return $ret
}

#---------------------------------------------------------------------
# Host system information (processor, memory, kernel, etc.)
#---------------------------------------------------------------------
function ns_sysinfo() {
    ns_common "$show_sysinfo" "Saving information about host system... " "Host system information" "sysinfo"
    ( echo "Netronome Systems - System Troubleshoot Information"
    echo
    date
    echo
    kernel
    kernel_limits
    operating_system
    processor
    memory
    compiler
    ) >> $output
    heading "/proc/meminfo output"
    cat /proc/meminfo >> $output

    lstopo=$(findcmd lstopo-no-graphics)
    if [ -n "$lstopo" ]; then
        echo >> $output
        heading "Processor Topology"
        $lstopo --no-io --of txt >> $output 2>&1
    fi

    if [ -n "$print_step" ]; then
       echo done
    fi

    return 0
}

#---------------------------------------------------------------------
# Host Top Output
#---------------------------------------------------------------------

function ns_host_top() {
    local ret=0
    local iterations=4
    ns_common "$show_host_top" "Saving \`top' util output... " "\`top' util output - $iterations iterations" "top"

    top=$(findcmd top)
    if [ -z "$top" ]; then
       [ -n "$print_step" ] && echo failed
       echo "Error: top command not found."
       return 1
    fi

    $top -H -b -n$iterations >> $output
    ret=$?

    if [ $ret -eq 0 ]; then
        [ -n "$print_step" ] && echo done
    else
        [ -n "$print_step" ] && echo failed
    fi
    return $ret
}

#---------------------------------------------------------------------
# Flow info
#---------------------------------------------------------------------

function ns_flow_info() {
    local ret=0
    ns_common "$show_flow_entries" "Saving flow entries... " "Flow entries" "flow_entries"

    ovs_vsctl=$(findcmd ovs-vsctl)
    ovs_ofctl=$(findcmd ovs-ofctl)
    if [ -z $ovs_vsctl -o -z $ovs_ofctl ]; then
        [ -n "$print_step" ] && echo failed
        return 1
    fi

    for bridge in $($ovs_vsctl show 2>/dev/null | grep Bridge | sed 's/.*"\(.*\)"/\1/g' 2>/dev/null); do
        heading "Flow entries for Bridge $bridge"
        $ovs_ofctl -OOpenFlow13 dump-flows $bridge >> $output 2>&1
        echo >> $output
        ret=$((ret + $?))
    done

    if [ $ret -eq 0 ]; then
        [ -n "$print_step" ] && echo done
    else
        [ -n "$print_step" ] && echo failed
    fi
    return $ret
}

#---------------------------------------------------------------------
# Flow cache locks info
#---------------------------------------------------------------------

function ns_fc_locks() {
    local ret=0
    ns_common "$show_fc_locks" "Saving flow cache diagnostics (this may take up to 10 minutes)... " "Flow cache diagnostics" "fc_diagnostics"

    if [ ! -x ${NETRONOME_BIN}/nfp-rtsym ]; then
        [ -n "$print_step"] && echo failed
        return 1
    fi

    python=$(findcmd python)
    if [ -z $python ]; then
        [ -n "$print_step"] && echo failed
        return 1
    fi

    for bucket in $(${NETRONOME_BIN}/nfp-rtsym -L 2>&1 | grep _FC_.*_BUCKETS_BASE | awk '{print $1}' 2>/dev/null) ; do
        heading "Flow cache locks for bucket $bucket"
        ${NETRONOME_BIN}/nfp-rtsym -v $bucket > /tmp/fcdiag
        $python fc_diag.py -s /tmp/fcdiag >> $output 2>&1
        echo >> $output
        ret=$((ret + $?))
    done

    heading "Flow cache timeout error counters"
    for isl in $WORKER_ME_LIST; do
        ${NETRONOME_BIN}/nfp-rtsym i$isl._fc_timeout_err_shared_lock_fail >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            heading "i$isl.Warning: failed to get shared lock"
            for ctx in $(seq 0 7); do
                printf "ctx%i: " $ctx >> $output
                addr=$((ctx * 4))
                ${NETRONOME_BIN}/nfp-rtsym -v -l 4 i$isl._fc_timeout_err_shared_lock_fail:$addr 2>/dev/null | sed 's/^.*: //g' | tr '\n' ' ' >> $output
                echo >> $output
            done
        fi

        ${NETRONOME_BIN}/nfp-rtsym i$isl._fc_timeout_err_exclusive_lock_fail >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            heading "i$isl.Warning: failed to get exclusive lock"
            for ctx in $(seq 0 7); do
                printf "ctx%i: " $ctx >> $output
                addr=$((ctx * 4))
                ${NETRONOME_BIN}/nfp-rtsym -v -l 4 i$isl._fc_timeout_err_exclusive_lock_fail:$addr 2>/dev/null | sed 's/^.*: //g' | tr '\n' ' ' >> $output
                echo >> $output
            done
        fi

        ${NETRONOME_BIN}/nfp-rtsym i$isl._fc_timeout_err_head_ptr_zero >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            heading "i$isl.Error: Head pointer is zero in CAM entry"
            for ctx in $(seq 0 7); do
                printf "ctx%i: " $ctx >> $output
                addr=$((ctx * 4))
                ${NETRONOME_BIN}/nfp-rtsym -v -l 4 i$isl._fc_timeout_err_head_ptr_zero:$addr 2>/dev/null | sed 's/^.*: //g' | tr '\n' ' ' >> $output
                echo >> $output
            done
        fi

        ${NETRONOME_BIN}/nfp-rtsym i$isl._fc_timeout_err_next_ptr_zero >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            heading "i$isl.Error: Next pointer is zero in entry"
            for ctx in $(seq 0 7); do
                printf "ctx%i: " $ctx >> $output
                addr=$((ctx * 4))
                ${NETRONOME_BIN}/nfp-rtsym -v -l 4 i$isl._fc_timeout_err_next_ptr_zero:$addr 2>/dev/null | sed 's/^.*: //g' | tr '\n' ' ' >> $output
                echo >> $output
            done
        fi

        ${NETRONOME_BIN}/nfp-rtsym i$isl._fc_timeout_err_free_entry_zero >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            heading "i$isl.Error: Free entry to put on the ring is zero"
            for ctx in $(seq 0 7); do
                printf "ctx%i: " $ctx >> $output
                addr=$((ctx * 4))
                ${NETRONOME_BIN}/nfp-rtsym -v -l 4 i$isl._fc_timeout_err_free_entry_zero:$addr 2>/dev/null | sed 's/^.*: //g' | tr '\n' ' ' >> $output
                echo >> $output
            done
        fi
    done

    if [ $ret -eq 0 ]; then
        [ -n "$print_step" ] && echo done
    else
        [ -n "$print_step" ] && echo failed
    fi
    return $ret
}




#---------------------------------------------------------------------
# PIF statistics
#---------------------------------------------------------------------

function ns_pif_stats() {
    local ret=0
    ns_common "$show_pif_stats" "Saving pif and flow cache statistics " "Statistics" "pif_stats"

    if [ ! -x ${NETRONOME_BIN}/nfp-rtsym ]; then
        [ -n "$print_step"] && echo failed
        return 1
    fi

    heading "PIF statistics"
        $NETRONOME_BIN/nfp-rtsym -v _pif_count* >> $output 2>&1
            echo >> $output
            echo >> $output

    heading "Flow cache statistics"
        $NETRONOME_BIN/nfp-rtsym -v _fc* >> $output 2>&1
            echo >> $output
            echo >> $output

}


#---------------------------------------------------------------------
# NFD info
#---------------------------------------------------------------------

function ns_nfd_info() {
    local ret=0
    ns_common "$show_nfd_info" "Saving NFD info..." "NFD Info" "nfd_info"

    if [ ! -x ${NETRONOME_BIN}/nfp-rtsym ]; then
        [ -n "$print_step"] && echo failed
        return 1
    fi

    if [ ! -x ${NETRONOME_BIN}/nfp-reg ]; then
        [ -n "$print_step"] && echo failed
        return 1
    fi

    for pcie in `seq 0 3`; do
        init_state=$(${NETRONOME_BIN}/nfp-reg xpbm:Pcie${pcie}IsldXpbmMap.Overlay.IslandControl.ClockResetControl.RemoveReset 2>&1 | grep -oE '0x[0-9a-zA-Z]+')
        # Effectively mask the first 4 bits and make sure they are all high
        if `echo "$init_state" | grep -E "0x[0-9a-zA-Z]{0,1}[fF]$" >/dev/null`; then
            island=$(($pcie+4))
            heading "NFD on PCIe ${pcie}"

            heading "Configuration"
            ${NETRONOME_BIN}/nfp-rtsym nfd_cfg_base${pcie} >> $output 2>&1
            ret=$((ret + $?))
            echo >> $output
            sleep 1s
            heading "Configuration after 1 second"
            ${NETRONOME_BIN}/nfp-rtsym nfd_cfg_base${pcie} >> $output 2>&1
            ret=$((ret + $?))
            echo >> $output
            echo >> $output

            heading "PCIe Internal Target Queue"
            ${NETRONOME_BIN}/nfp-reg pcie:i${island}.PcieInternalTargets.Queue{0..255} >> $output 2>&1
            ret=$((ret + $?))
            echo >> $output
            sleep 1s
            heading "PCIe Internal Target Queue after 1 second"
            ${NETRONOME_BIN}/nfp-reg pcie:i${island}.PcieInternalTargets.Queue{0..255} >> $output 2>&1
            ret=$((ret + $?))
            echo >> $output
            echo >> $output


                # not supported by pif
#             heading "NFD_OUT Atomics"
#             ${NETRONOME_BIN}/nfp-rtsym i${island}.nfd_out_atomics >> $output 2>&1
#             ret=$((ret + $?))
#             echo >> $output
#             sleep 1s
#             heading "NFD_OUT Atomics after 1 second"
#             ${NETRONOME_BIN}/nfp-rtsym i${island}.nfd_out_atomics >> $output 2>&1
#             ret=$((ret + $?))
#             echo >> $output
#             echo >> $output

            heading "NFD FLR Registers"
            ${NETRONOME_BIN}/nfp-reg xpbm:Pcie${pcie}IsldXpbmMap.Island.PcieXpb.PcieComponentCfgXpb.PCIeVfFlrInProg0.PCIeVfStat1 >> $output 2>&1
            ${NETRONOME_BIN}/nfp-reg xpbm:Pcie${pcie}IsldXpbmMap.Island.PcieXpb.PcieComponentCfgXpb.PCIeVfFlrInProg1.PCIeVfStat1 >> $output 2>&1
            ${NETRONOME_BIN}/nfp-reg xpbm:Pcie${pcie}IsldXpbmMap.Island.PcieXpb.PcieIntMgr.Status >> $output 2>&1
        fi
    done

    heading "Message Control Channel Netdev info"
    nfd_iface=`cat /sys/module/nfp_fallback/control/msg_netdev 2>/dev/null`

    ethtool=$(findcmd ethtool)
    if [ -n "$ethtool" -a -n "$nfd_iface" ]; then
        cmd="$ethtool -W $nfd_iface 2000"
        echo "Output of command [$cmd]:" >> $output
        $cmd >> $output 2>&1
        ret=$((ret + $?))
        echo >> $output

        cmd="$ethtool -w $nfd_iface data"
        echo "Output of command [$cmd]:" >> $output
        $cmd /tmp/$nfd_iface 2>&1
        cat /tmp/$nfd_iface >> $output
        ret=$((ret + $?))
        echo >> $output
    else
        echo "No netdev info, possibly due to control message channel being unresponsive" >> $output
        echo >> $output
    fi

    heading "Packet Control Channel Netdev info"
    nfd_iface=`cat /sys/module/nfp_fallback/control/fpkt_netdev 2>/dev/null`

    ethtool=$(findcmd ethtool)
    if [ -n "$ethtool" -a -n "$nfd_iface" ]; then
        cmd="$ethtool -W $nfd_iface 2000"
        echo "Output of command [$cmd]:" >> $output
        $cmd >> $output 2>&1
        ret=$((ret + $?))
        echo >> $output

        cmd="$ethtool -w $nfd_iface data"
        echo "Output of command [$cmd]:" >> $output
        $cmd /tmp/$nfd_iface 2>&1
        cat /tmp/$nfd_iface >> $output
        ret=$((ret + $?))
        echo >> $output
    else
        echo "No netdev info, possibly due to control message channel being unresponsive" >> $output
        echo >> $output
    fi

    if [ $ret -eq 0 ]; then
        [ -n "$print_step" ] && echo done
    else
        [ -n "$print_step" ] && echo failed
    fi

    return $ret
}

#---------------------------------------------------------------------
# Control channel VF info
#---------------------------------------------------------------------

function ns_ctrlchan_info() {
    local ret=0
    ns_common "$show_ctrlchan_info" "Saving control channel info..." "Control channel info" "ctrl_chan_info"

    if [ ! -x ./ctrl_chan_vf.sh ]; then
        [ -n "$print_step" ] && echo failed
        return 1
    fi

    heading "Control channel VF info"
    ./ctrl_chan_vf.sh >> $output 2>&1
    ret=$((ret + $?))
    echo >> $output

    if [ $ret -eq 0 ]; then
        [ -n "$print_step" ] && echo done
    else
        [ -n "$print_step" ] && echo failed
    fi
    return $ret
}

#---------------------------------------------------------------------
# GRO dump
#---------------------------------------------------------------------

function ns_grodump() {
    local ret=0
    ns_common "$show_gro_dump" "Saving GRO dump..." "GRO dump" "gro_dump"

    if [ ! -x ./grodump.sh ]; then
        [ -n "$print_step" ] && echo failed
        return 1
    fi

    for i in {0..16}; do
        ./grodump.sh $i >> $output 2>&1
        ret=$((ret + $?))
        echo >> $output
    done

    for i in 20 24 28; do
        ./grodump.sh $i >> $output 2>&1
        ret=$((ret + $?))
        echo >> $output
    done

    if [ $ret -eq 0 ]; then
        [ -n "$print_step" ] && echo done
    else
        [ -n "$print_step" ] && echo failed
    fi
    return $ret
}

#---------------------------------------------------------------------
# Symbol name and address dump
#---------------------------------------------------------------------

function ns_rtsym_dump() {
    local ret=0
    ns_common "$show_rtsym" "Saving symbol list dump..." "Symbol list dump" "rtsym_list"

    if [ ! -x ${NETRONOME_BIN}/nfp-rtsym ]; then
        [ -n "$print_step"] && echo failed
        return 1
    fi

    ${NETRONOME_BIN}/nfp-rtsym -L >> $output 2>&1
    echo >> $output

    if [ $ret -eq 0 ]; then
        [ -n "$print_step" ] && echo done
    else
        [ -n "$print_step" ] && echo failed
    fi
    return $ret
}

#---------------------------------------------------------------------
# Worker state dump from xfer registers (at least r2239)
#---------------------------------------------------------------------

function ns_worker_state() {
    local ret=0
    ns_common "$show_worker_state" "Saving worker states..." "Worker state dump" "worker-state"

    if [ ! -x ${NETRONOME_BIN}/nfp-mereg ]; then
        [ -n "$print_step"] && echo failed
        return 1
    fi

    firstreg=$WORKER_STATS_XFER_BASE_LW
    lastreg=$((firstreg + $WORKER_STATS_XFER_SIZE_LW - 1))
    for isl in $WORKER_ME_LIST; do
        for me in $(seq 0 9); do
            heading "i$isl.me$me Worker Stats"
            for ctx in $(seq 0 7); do
                for reg in $(seq $firstreg $lastreg); do
                    if [ $reg -lt 256 ]; then
                        val=$(${NETRONOME_BIN}/nfp-mereg -q i$isl.me$me.ctx$ctx.oxfer_$reg)
                        printf "i%i.me%i.ctx%i.oxfer_%i=0x%08x\n" $isl $me $ctx $reg $val >> $output 2>&1
                    fi
                done
                echo >> $output
            done
        done
    done

    if [ $ret -eq 0 ]; then
        [ -n "$print_step" ] && echo done
    else
        [ -n "$print_step" ] && echo failed
    fi
    return $ret
}

#---------------------------------------------------------------------
# Process info
#---------------------------------------------------------------------

function ns_ps_info() {
    ns_common "$show_ps_info" "Saving current process information... " "Process information" "processes"

    ps=$(findcmd ps)
    if [ -z "$ps" ]; then
       [ -n "$print_step" ] && echo failed
       echo "Error: ps command not found." >> $output
       return 1
    fi

    heading "Current Processes"
    $ps -eLA -o comm,psr,pid,tid,pcpu,cputime >> $output

    [ -n "$print_step" ]  && echo done
    return 0
}

#---------------------------------------------------------------------
# Dump OVS database and config
#---------------------------------------------------------------------

# function ns_ovs_info() {
#
#     local ret=0
#     ns_common "$show_ovs" "Saving current OVS configuration information... " "OVS configuration information" "openvswitch"
#
#     heading "OVSDB - Overview"
#     vsctl_cmd=$(findcmd ovs-vsctl)
#     if [ -n "$vsctl_cmd" ]; then
#         echo "$vsctl_cmd show" >> $output
#         $vsctl_cmd show >> $output 2>&1
#         echo >> $output
#         ret=$((ret + $?))
#     else
#         ret=1
#     fi
#
#     heading "OVSDB - All Tables"
#     cmd=$(findcmd ovsdb-client)
#     if [ -n "$cmd" ]; then
#         echo "$cmd dump 'Open_vSwitch' --format=list" >> $output
#         $cmd dump 'Open_vSwitch' --format=list >> $output 2>&1
#         echo >> $output
#         ret=$((ret + $?))
#     else
#         ret=1
#     fi
#
#     ofctl_cmd=$(findcmd ovs-ofctl)
#     if [ -n "$ofctl_cmd" -a -n "$vsctl_cmd" ]; then
#         for bridge in $($vsctl_cmd list-br  2>/dev/null); do
#             heading "OVS OpenFlow Overview for Bridge $bridge"
#             echo "$ofctl_cmd -O OpenFlow10 -O OpenFlow11 -O OpenFlow12 -O OpenFlow13 show $bridge" >> $output
#             $ofctl_cmd -O OpenFlow10 -O OpenFlow11 -O OpenFlow12 -O OpenFlow13 show $bridge >> $output 2>&1
#
#             heading "OVS OpenFlow Rules for Bridge $bridge"
#             echo "$ofctl_cmd -O OpenFlow10 -O OpenFlow11 -O OpenFlow12 -O OpenFlow13 dump-flows $bridge" >> $output
#             $ofctl_cmd -O OpenFlow10 -O OpenFlow11 -O OpenFlow12 -O OpenFlow13 dump-flows $bridge >> $output 2>&1
#             echo >> $output
#             ret=$((ret + $?))
#         done
#     else
#         ret=1
#     fi
#
#     cmd=$(findcmd ovs-dpctl)
#     if [ -n "$cmd" ]; then
#         heading "OVS Datapath Info"
#         echo "$cmd show" >> $output
#         $cmd show >> $output 2>&1
#         echo >> $output
#         ret=$((ret + $?))
#
#         heading "OVS Datapath Rules"
#         echo "$cmd dump-flows" >> $output
#         $cmd dump-flows >> $output 2>&1
#         echo >> $output
#         ret=$((ret + $?))
#     else
#         ret=1
#     fi
#
#     heading "RH Masks"
#     cat /sys/module/nfp_offloads/control/rh_masks >> $output 2>&1
#     ret=$((ret + $?))
#     echo >> $output
#
#     heading "RH Entries"
#     cat /sys/module/nfp_offloads/control/rh_entries >> $output 2>&1
#     ret=$((ret + $?))
#     echo >> $output
#
#     if [ $ret -eq 0 ]; then
#         [ -n "$print_step" ] && echo done
#     else
#         [ -n "$print_step" ] && echo failed
#     fi
#     return $ret
# }

#---------------------------------------------------------------------
# Dump ARP/Routing Table entries
#---------------------------------------------------------------------

function ns_arp_route() {

    local ret=0
    ns_common "$show_arp_routing" "Saving ARP/Routing tables... " "ARP/Routing tables" "arp_route_tables"

    heading "Host ARP Table"
    arp=$(findcmd arp)
    if [ -n "$arp" ]; then
        arp -nv >> $output 2>&1
        echo >> $output
        ret=$((ret + $?))
    fi

    heading "Host Routing Table"
    route=$(findcmd route)
    if [ -n "$route" ]; then
        $route -nFCeev >> $output 2>&1
        echo >> $output
        ret=$((ret + $?))
    fi

    if [ $ret -eq 0 ]; then
        [ -n "$print_step" ] && echo done
    else
        [ -n "$print_step" ] && echo failed
    fi
    return $ret
}

#---------------------------------------------------------------------
# Dump NFP Symbols
#---------------------------------------------------------------------

function ns_nfp_symbols() {

    local ret=0
    local rc=0
    local retry_count=0
    ns_common "$show_nfp_symbols" "Saving NFP symbols (this may take a while)... " "NFP Symbols" "rtsym"

    if [ ! -x ${NETRONOME_BIN}/nfp-rtsym ]; then
        [ -n "$print_step" ] && echo failed
        return 1
    fi

    for symbol in $(${NETRONOME_BIN}/nfp-rtsym --list 2>/dev/null | grep -Pv "Name[ ]*Resource[ ]*Address[ ]*Size" | cut -d ' ' -f 1); do

        # We skip the payloads symbols for the Flow Cache which are very large
        # and only needed if the utility is invoked to print data to stdout
        if [ ! "$output" = "/dev/stdout" ]; then
            if `echo "$symbol" | grep -E '_EMU_[0-9]_PAYLOADS_BASE' 1>/dev/null 2>&1`; then
                continue;
            fi
        fi

        heading "Content of NFP symbol $symbol"

        rc=-1
        retry_count=0
        while [ $retry_count -lt 3 ] && [ ! $rc -eq 0 ]; do
            ${NETRONOME_BIN}/nfp-rtsym $symbol >> $output 2>&1
            rc=$?
            retry_count=$(($retry_count+1))

            echo >> $output
        done
    done

    # We dont count failures here, since some symbols cannot be dumped i.e. LMEM symbols.
    # We do retry dumping symbols in case there is an intermittent issue
    [ -n "$print_step" ] && echo done
    return $ret
}

function ns_nfp_info() {

    local ret=0
    ns_common "$show_nfp_info" "Saving NFP Debug Information (this may take up to 5 minutes)... " "NFP Debug Information" "nfp"

    nfpnffw=$(findcmd nfp-nffw)
    if [ ! -z "$nfpnffw" ]; then
        heading "Firmware status"
        $nfpnffw status >> $output 2>&1
    fi

    # First make sure PIF is running
    lsmod=$(findcmd lsmod)
    if [ -z "$lsmod" ]; then
        [ -n "$print_step" ] && echo failed
        return 1
    fi
    $lsmod | grep -q nfp_offloads
    ret=$?
    if [ $ret -ne 0 ]; then
        echo "PIF does not appear to be running, skipping NFP Debug Information." >> $output
        [ -n "$print_step" ] && echo failed
        return $ret
    fi

    if [ ! -x ${NETRONOME_BIN}/nfp ]; then
        [ -n "$print_step" ] && echo failed
        return 1
    fi

    heading "Pico code version"
    echo "nfp-cpp --len 16 8@nbi:0x300FF0" >> $output 2>&1
    ${NETRONOME_BIN}/nfp-cpp --len 16 8@nbi:0x300FF0 >> $output 2>&1
    ret=$((ret + $?))

    heading "NBI Preclassifier at t=0s"
    ${NETRONOME_BIN}/nfp -m pc -e show config 0 >> $output 2>&1
    ret=$((ret + $?))

    heading "NBI Preclassifier sequence numbers at t=0s"
    echo "nfp -m pc -e show seq 0 0-7" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m pc -e show seq 0 0-7 >> $output 2>&1
    ret=$((ret + $?))

    heading "NBI DMA BPE's at t=0s"
    echo "nfp -m dma -e show bp 0 0-7" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m dma -e show bp 0 0-7 >> $output 2>&1
    ret=$((ret + $?))

    heading "NBI DMA BLQ's at t=0s"
    echo "nfp -m dma -e show blq 0 0-3" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m dma -e show blq 0 0-3 >> $output 2>&1
    ret=$((ret + $?))

    heading "NBI DMA show int at t=0s"
    echo "nfp -m dma -e show int 0" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m dma -e show int 0 >> $output 2>&1
    ret=$((ret + $?))

    heading "NBI DMA STATS at t=0s"
    echo "nfp -m dma -e show stats 0" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m dma -e show stats 0 >> $output 2>&1
    ret=$((ret + $?))

    heading "NFP BLM INFO at t=0s"
    echo "nfp -m blm -e show info 0" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m blm -e show info 0 >> $output 2>&1
    ret=$((ret + $?))

    heading "NFP BLM STATS at t=0s"
    echo "nfp -m blm -e show stats 0 0-3" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m blm -e show stats 0 0-3 >> $output 2>&1
    ret=$((ret + $?))

    heading "NFP BLM CACHE at t=0s" >> $output 2>&1
    echo "nfp -m blm -e show cache 0 0-3" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m blm -e show cache 0 0-3 >> $output 2>&1
    ret=$((ret + $?))

    heading "NFP CTM STATS at t=0s" >> $output 2>&1
    echo "nfp -m ctm -e show stats $FIRST_WORKER_ME-$LAST_WORKER_ME" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m ctm -e show stats $FIRST_WORKER_ME-$LAST_WORKER_ME >> $output 2>&1
    ret=$((ret + $?))

    heading "NFP TM minipkt at t=0s" >> $output 2>&1
    echo "nfp -m tm -e show minipktfc 0 0-127" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m tm -e show minipktfc 0 0-127 >> $output 2>&1
    ret=$((ret + $?))

    heading "NFP TM Queue status at t=0s" >> $output 2>&1
    echo "nfp -m tm -e show qstatus 0 0-864" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m tm -e show qstatus 0 0-864 >> $output 2>&1
    ret=$((ret + $?))

    sleep 1s

    heading "NBI Preclassifier at t=1s"
    ${NETRONOME_BIN}/nfp -m pc -e show config 0 >> $output 2>&1
    ret=$((ret + $?))

    heading "NBI Preclassifier sequence numbers at t=1s"
    echo "nfp -m pc -e show seq 0 0-7" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m pc -e show seq 0 0-7 >> $output 2>&1
    ret=$((ret + $?))

    heading "NBI DMA BPE's at t=1s"
    echo "nfp -m dma -e show bp 0 0-7" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m dma -e show bp 0 0-7 >> $output 2>&1
    ret=$((ret + $?))

    heading "NBI DMA BLQ's at t=1s"
    echo "nfp -m dma -e show blq 0 0-3" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m dma -e show blq 0 0-3 >> $output 2>&1
    ret=$((ret + $?))

    heading "NBI DMA show int at t=1s"
    echo "nfp -m dma -e show int 0" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m dma -e show int 0 >> $output 2>&1
    ret=$((ret + $?))

    heading "NBI DMA STATS at t=1s"
    echo "nfp -m dma -e show stats 0" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m dma -e show stats 0 >> $output 2>&1
    ret=$((ret + $?))

    heading "NFP BLM INFO at t=1s"
    echo "nfp -m blm -e show info 0" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m blm -e show info 0 >> $output 2>&1
    ret=$((ret + $?))

    heading "NFP BLM STATS at t=1s"
    echo "nfp -m blm -e show stats 0 0-3" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m blm -e show stats 0 0-3 >> $output 2>&1
    ret=$((ret + $?))

    heading "NFP BLM CACHE at t=1s" >> $output 2>&1
    echo "nfp -m blm -e show cache 0 0-3" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m blm -e show cache 0 0-3 >> $output 2>&1
    ret=$((ret + $?))

    heading "NFP CTM STATS at t=1s" >> $output 2>&1
    echo "nfp -m ctm -e show stats $FIRST_WORKER_ME-$LAST_WORKER_ME" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m ctm -e show stats $FIRST_WORKER_ME-$LAST_WORKER_ME >> $output 2>&1
    ret=$((ret + $?))

    heading "NFP TM minipkt at t=1s" >> $output 2>&1
    echo "nfp -m tm -e show minipktfc 0 0-127" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m tm -e show minipktfc 0 0-127 >> $output 2>&1
    ret=$((ret + $?))

    heading "NFP TM Queue status at t=1s" >> $output 2>&1
    echo "nfp -m tm -e show qstatus 0 0-864" >> $output 2>&1
    ${NETRONOME_BIN}/nfp -m tm -e show qstatus 0 0-864 >> $output 2>&1
    ret=$((ret + $?))

    # Ignore return values for now, as queue errors need to be ignored
    heading "BLM queue descriptors at t=0s"
    for q in {0..3}; do
        ${NETRONOME_BIN}/nfp -m dma -e show qdump $q >> $output 2>&1
    done

    sleep 1s
    heading "BLM queue descriptors at t=1s"
    for q in {0..3}; do
        ${NETRONOME_BIN}/nfp -m dma -e show qdump $q >> $output 2>&1
    done

    for pcie in `seq 0 3`; do
        init_state=$(${NETRONOME_BIN}/nfp-reg xpbm:Pcie${pcie}IsldXpbmMap.Overlay.IslandControl.ClockResetControl.RemoveReset 2>&1 | grep -oE '0x[0-9a-zA-Z]+')
        # Effectively mask the first 4 bits and make sure they are all high
        if `echo "$init_state" | grep -E "0x[0-9a-zA-Z]{0,1}[fF]$" >/dev/null`; then
            island=$(($pcie+4))
            heading "Island ${island} PCIE DMA controller status"
            ${NETRONOME_BIN}/nfp-reg pcie:i${island}.PcieInternalTargets.DMAController.DMAQStatFrmPCI{0..1} >> $output 2>&1
            ${NETRONOME_BIN}/nfp-reg pcie:i${island}.PcieInternalTargets.DMAController.DMAQStatToPCI{0..1} >> $output 2>&1
        fi
    done

    heading "ME packet and buffer credits"
    for i in $WORKER_ME_LIST; do
        echo "nfp-rtsym i$i.me_credit_buckets" >> $output 2>&1
        ${NETRONOME_BIN}/nfp-rtsym i$i.me_credit_buckets >> $output 2>&1
    done

    if ${NETRONOME_BIN}/nfp-hwinfo 2>&1 | grep 'assembly.model' | sed 's/.*=//g' | grep 'FWA-6522C' >/dev/null 2>&1; then
        heading "FWA CPLD Status"
        echo "ipmitool i2c bus=6 0x82 6 0x80" >> $output
        ipmitool i2c bus=6 0x82 6 0x80 >> $output
        echo "ipmitool i2c bus=6 0x82 6 0xa0" >> $output
        ipmitool i2c bus=6 0x82 6 0xa0 >> $output
        echo "ipmitool i2c bus=6 0x82 6 0xc0" >> $output
        ipmitool i2c bus=6 0x82 6 0xc0 >> $output
        echo "ipmitool i2c bus=6 0x82 6 0xe0" >> $output
        ipmitool i2c bus=6 0x82 6 0xe0 >> $output

        lmpclient=$(findcmd lmp_client)
        heading "FWA lmp client"
        echo "lmp_client -i version -r" >> $output
        $lmpclient -i version -r >> $output
    fi

    if [ $ret -eq 0 ]; then
        [ -n "$print_step" ] && echo done
    else
        [ -n "$print_step" ] && echo failed
    fi
    return $ret
}

function ns_ecc() {

    local ret=0
    ns_common "$show_ecc" "Saving Error Correct Code info... " "ECC Information" "ecc"

    if [ ! -x ${NETRONOME_BIN}/nfp-ecc ]; then
        [ -n "$print_step" ] && echo failed
        return 1
    fi

    ${NETRONOME_BIN}/nfp-ecc -a >> $output 2>&1

    [ -n "$print_step" ] && echo done
    return $ret
}

function nfpreg() {
    ${NETRONOME_BIN}/nfp-reg "$1" 2>&1 > /dev/null
    ret=$?
    ${NETRONOME_BIN}/nfp-reg "$1" 2>&1 | sed -e 's/[^=]*=//'
    return $ret
}

function dump_me() {
    isl="$1"
    me="$2"
    if [ -z "$me" ] || [ -z "$isl" ]; then
        return 1
    fi

    heading "i$isl.me$me"
    sts=`${NETRONOME_BIN}/nfp-reg -B mecsr:i$isl.me$me.ActCtxStatus 2>/dev/null | sed -e 's/[^=]*=//'`
    echo -n "mecsr:i$isl.me$me.ActCtxStatus=$sts, " >> $output
    # If sts >> 31, AB0=1
    if (((sts >> 31) == 1)); then
        pc=$(((sts & 0x1FFFF00) >> 8))
        ctx=$((sts & 7))
        echo "PC = $pc, active context = $ctx" >> $output
    else
        echo "No active context" >> $output
    fi
    echo >> $output

    echo "====[ SIG_EVENTS WAKE_EVENT      PC READY ]====" >> $output
    for i in 0 1 2 3 4 5 6 7 ; do
        echo -n "CTX${i}: " >> $output
        val=`nfpreg mecsr:i${isl}.me${me}.ctx${i}.IndCtxSglEvt.Signals`
        if [ $? -ne 0 ]; then
            echo "mecsr:i${isl}.me${me}.ctx${i} Failed, skipping" >> $output
        else
            printf "0x%08x " $(( ($val << 1) | 1 )) >> $output 2>&1
            val=`nfpreg mecsr:i${isl}.me${me}.ctx${i}.IndCtxWkpEvt.WakeupEvents`
            val2=`nfpreg mecsr:i${isl}.me${me}.ctx${i}.IndCtxWkpEvt.Voluntary`
            printf "0x%08x " $(( ($val << 1) | $val2 )) >> $output 2>&1
            ${NETRONOME_BIN}/nfp-reg mecsr:i${isl}.me${me}.ctx${i}.IndCtxStatus 2>/dev/null > /tmp/dumpme
            val=`cat /tmp/dumpme | grep ContextPC | sed -e 's/[^=]*=//'`
            printf "%7d " $val >> $output 2>&1
            val=`cat /tmp/dumpme | grep ReadyToRun | sed -e 's/[^=]*=//'`
            printf "%5d\n" $val >> $output 2>&1
        fi
    done
    echo >> $output

    return 0
}

function format_debug_counters() {
    CNTRS="$(${NETRONOME_BIN}/nfp-rtsym -L 2>&1 |grep __cntr__)"

    # Read each line of output from CNTRS command
    for cntr in $CNTRS; do
        # Bash separates on whitespaces, so check if the value is the value name
        if [[ "${cntr}" == *__cntr__ ]]; then
            vals="$(${NETRONOME_BIN}/nfp-rtsym $cntr 2>&1)"
            out="$(echo $cntr | cut -c -$((${#cntr}-8)))"
            val1="$(echo $vals | cut -f2 -d\ | cut -c 3-)"
            val2="$(echo $vals | cut -f3 -d\ | cut -c 3-)"
            hex_val="$(echo $val2$val1 | sed 's/^0*//')"
            printf "%-30s: %s\n" $out $((16#$hex_val)) >> $output 2>&1
        fi
    done
}

function ns_dump_mes() {

    local ret=0
    ns_common "$show_me_info" "Saving ME Information (this may take a while)... " "ME Information" "me_dump"

    if [ ! -x ${NETRONOME_BIN}/nfp-reg -o ! -x ${NETRONOME_BIN}/nfp-rtsym ]; then
        [ -n "$print_step" ] && echo failed
        return 1
    fi

    heading "ME active packets at t=0s"
    ${NETRONOME_BIN}/nfp -m ctm -e show stats $FIRST_WORKER_ME-$LAST_WORKER_ME >> $output 2>&1
    ret=$((ret + $?))

    sleep 1s
    heading "ME active packets at t=1s"
    ${NETRONOME_BIN}/nfp -m ctm -e show stats $FIRST_WORKER_ME-$LAST_WORKER_ME >> $output 2>&1
    ret=$((ret + $?))

    heading "ME signalled events, wakeup it events and Context current program counter at t=0s"
    for mei in $WORKER_ME_LIST; do
        for me in {0..11}; do
            dump_me $mei $me
            ret=$((ret + $?))
        done
    done

    sleep 1s
    heading "ME signalled events, wakeup it events and Context current program counter at t=1s"
    for mei in $WORKER_ME_LIST; do
        for me in {0..11}; do
            dump_me $mei $me
            ret=$((ret + $?))
        done
    done

    heading "ME Mailboxes at t=0s"
    ${NETRONOME_BIN}/nfp-reg -B mecsr:i{$FIRST_WORKER_ME..$LAST_WORKER_ME}.me{0..11}.Mailbox{0..3} >> $output 2>&1
    ret=$((ret + $?))

    sleep 1s
    heading "ME Mailboxes at t=1s"
    ${NETRONOME_BIN}/nfp-reg -B mecsr:i{$FIRST_WORKER_ME..$LAST_WORKER_ME}.me{0..11}.Mailbox{0..3} >> $output 2>&1
    ret=$((ret + $?))

    heading "Worker counters at t=0s"
    format_debug_counters

    sleep 1s
    heading "Worker counters at t=1s"
    format_debug_counters

#     for isl in $PCIE_LIST; do
#         heading "NFD credits i${isl}"
#         ${NETRONOME_BIN}/nfp-rtsym -l512 i${isl}.nfd_out_atomics >> $output 2>&1
#         ret=$((ret + $?))
#     done

    if [ $ret -eq 0 ]; then
        [ -n "$print_step" ] && echo done
    else
        [ -n "$print_step" ] && echo failed
    fi
    return $ret
}

function ns_debug_counters() {
    ns_common "$show_nfp_counters" "Saving NFP counters..." "NFP Counters" "nfp_counters"

    heading "Worker counters"
    format_debug_counters

    return $?
}

function port_info() {

    local ret=0
    ns_common "$show_port_info" "Saving Port Information... " "Port Information" "ports"

    netstat=$(findcmd netstat)
    ethtool=$(findcmd ethtool)
    ifconfig=$(findcmd ifconfig)
    ip=$(findcmd ip)

    # At least need netstat to get the list of interfaces
    if [ ! -x $netstat ]; then
        [ -n "$print_step" ] && echo failed
        return 1
    fi

    # Ignore ethtool return values as they are too strict
    for netdev in $(netstat -i 2>&1 | grep -vP "(Kernel Interface table|Iface)" | cut -d ' ' -f 1); do

        heading "Netdev $netdev info"
        if [ -n "$ethtool" ]; then
            echo " * ethtool $netdev" >> $output
            $ethtool $netdev >> $output 2>&1
            echo "" >> $output
            echo " * ethtool -i $netdev" >> $output
            $ethtool -i $netdev >> $output 2>&1
            echo "" >> $output
            echo " * ethtool -S $netdev" >> $output
            $ethtool -S $netdev >> $output 2>&1
            echo "" >> $output
            echo " * ethtool -k $netdev" >> $output
            $ethtool -k $netdev >> $output 2>&1
            echo "" >> $output
            echo " * ethtool --show-priv-flags $netdev" >> $output
            $ethtool --show-priv-flags $netdev >> $output 2>&1
            echo "" >> $output
        else
            echo " <ethtool not available>" >> $output
            echo "" >> $output
        fi

        if [ -n "$ifconfig" ]; then
            echo " * ifconfig $netdev" >> $output
            $ifconfig $netdev >> $output 2>&1
            ret=$((ret + $?))
            echo "" >> $output
        else
            echo " <ifconfig not available>" >> $output
            echo "" >> $output
        fi

        if [ -n "$ip" ]; then
            echo " * ip link show $netdev" >> $output
            $ip link show $netdev >> $output 2>&1
            ret=$((ret + $?))
            echo "" >> $output
        else
            echo " <ip not available>" >> $output
            echo "" >> $output
        fi

    done

    heading "Netstat output"
    netstat -i >> $output 2>&1
    ret=$((ret + $?))

    heading "nfp-phymod output"
    if [ -x ${NETRONOME_BIN}/nfp-phymod ]; then
        echo " * nfp-phymod -E" >> $output
        ${NETRONOME_BIN}/nfp-phymod -E >> $output 2>&1
        echo "" >> $output

        echo " * nfp-phymod -P" >> $output
        ${NETRONOME_BIN}/nfp-phymod -P >> $output 2>&1
    else
        echo " <nfp-phymod not available>" >> $output
    fi
    echo "" >> $output

    heading "nfp_nbi_phymod output"
    if [ -e /sys/module/nfp_fallback/control/nfp_nbi_phymod ]; then
        cat /sys/module/nfp_fallback/control/nfp_nbi_phymod >> $output 2>&1
    else
        echo " <nfp_nbi_phymod not available>" >> $output
    fi

    if [ $ret -eq 0 ]; then
        [ -n "$print_step" ] && echo done
    else
        [ -n "$print_step" ] && echo failed
    fi
    return $ret

}

function disable_me() {
    ${NETRONOME_BIN}/nfp-reg mecsr:i$1.me$2.CtxEnables.CtxEnables=0 1>/dev/null 2>&1
}

function dump_gpr_loop() {
    if [ -z "$1" -o -z "$2" -o -z "$3" -o -z "$4" ]; then
        return 1
    fi

    for isl in $(seq $1 $2); do
        for me in $(seq $3 $4); do
            me_list=$(grep ":i$isl.me$me. " $firmware_list_name | awk '{print $2}' | awk -F ":" '{print $1}' )
            if [ -n $(echo "$me_list" | grep \/) ]; then me_list=$(echo $me_list | awk -F "/" '{print $NF}'); fi
            if [ -n "$me_list" -a -z "$(echo $me_list | grep 'nfd')" ]; then
                disable_me $isl $me
                ret=$((ret + $?))
                heading "GPRs on i$isl.me$me: $me_list"
                ${NETRONOME_BIN}/nfp-mereg i$isl.me$me >> $output 2>&1
                ret=$((ret + $?))
                heading "LM on i$isl.me$me: $me_list"
                ${NETRONOME_BIN}/nfp-mem -l 4096 i$isl.me$me.lmem >> $output 2>&1
                ret=$((ret + $?))
                echo >> $output
            fi
        done
    done
}

function ns_dump_gprs() {
    local ret=0
    ns_common "$dump_gprs" "Dumping GPRs and Local Memory. MEs are stopped for this!... " "GPR Dump" "gprs"

    nfpnffw="${NETRONOME_BIN}/nfp-nffw"
    if [ ! -x "$nfpnffw" ]; then
        [ -n "$print_step" ] && echo failed
        return 1
    fi

    firmwarename=$($nfpnffw status | grep "Firmware name" | awk '{ print $3 }' )
    if [ -z "$firmwarename" ]; then
        [ -n "$print_step" ] && echo failed
        echo "Couldn't determine firmware name" >> $output
        return 1
    fi

    firmware_list_name="$NETRONOME_ETC/firmware_meta_$firmwarename"
    if [ ! -e "$firmware_list_name" ]; then
        [ -n "$print_step" ] && echo failed
        echo "Couldn't read firmware list meta file: $firmware_list_name" >> $output
        return 1
    fi

    MAX_GRO_ISLAND_ID=$(get_last_gro_island_id)
    if [ -z "$MAX_GRO_ISLAND_ID" ]; then
        [ -n "$print_step" ] && echo failed
        echo "Couldn't determine maximum GRO island ID" >> $output
        return 1
    fi

    dump_gpr_loop $FIRST_WORKER_ME $LAST_WORKER_ME 0 11
    dump_gpr_loop 48 $MAX_GRO_ISLAND_ID 0 3

    [ -n "$print_step" ] && echo done
    return $ret
}

##############################################################################

run_timeout() {
    SEC=$1
    shift
    ($*) &
    PID1=$!
    (sleep $SEC ; echo TIMED OUT AFTER $SEC SECONDS;
    kill $PID1 ; wait $PID1) 2>/dev/null &
    PID2=$!
    wait $PID1 2>/dev/null
    kill $PID2 2>/dev/null
    wait $PID2 2>/dev/null
    PID2=$((PID2 + 1))
    if [ $(ps $PID | grep sleep | wc -l) -eq 1 ]; then
        kill $PID2 2>/dev/null
        wait $PID2 2>/dev/null
    fi
}

##############################################################################

# Parse and split a concatenated troubleshoot dump
# For example: If the user did: ns_pif_troubleshoot.sh -X > dump.txt
#            We could then do: ns_pif_troubleshoot.sh -P dump.txt -p <target_dir>
#            And the result would be multiple files in <target_dir> named
#            according to the original troubleshoot sections.
parse_troubleshoot_concat_dump() {
    local dump_file="$1"
    local target_dir="$2"
    local ret=0;
    local original_dir=`pwd`
    if [ -e "$dump_file" -a -f "$dump_file" -a -s "$dump_file" ]; then
       if [ -d $target_dir -a -w $target_dir ]; then
          echo "Processing '$dump_file', output will go to $target_dir"
          cd $target_dir
       else
          echo "Bad output directory target (check write permissions etc)"
          return 1
       fi
    else
       echo "Bad concatenated dumpfile specified - file does not exist or is invalid"
       return 1
    fi

    csplit "$dump_file" /ItemName/ {*} 1>/dev/null
    #We now have a list of xx<N> named files, each one a separate troubleshoot sections

    #remove the initial line chunk before the first section
    rm xx00

    #Rename all the xx<N> files to the original troubleshoot section name
    for file in xx*
    do
       new_name=`grep ItemName $file | cut -d ' ' -f 4`
       mv $file $new_name.txt
       echo "created $new_name.txt"
    done

    cd $original_dir
}
##############################################################################

do_chdir=""
while getopts "XdlHbvtsTROSMNanfFiDcghCQLweGP:p:" opt; do
    case $opt in
          "?") usage; exit 1;;
          h) usage;;
          X) run_all=1;;
          d) show_dmesg=1;;
          l) show_lspci=1;;
          H) show_hwinfo=1;;
          b) show_bootlog=1;;
          v) show_versions=1;;
          t) show_piftest=1;;
          s) show_sysinfo=1;;
          O) show_ovs=1;;
          T) show_host_top=1;;
          R) show_ps_info=1;;
          S) show_nfp_symbols=1;;
          M) show_me_info=1;;
          N) show_nfp_info=1;;
          n) show_port_info=1;;
          a) show_arp_routing=1;;
          f) show_flow_entries=1;;
          F) show_fc_locks=1;;
          i) show_pif_stats=1;;
          D) show_nfd_info=1;;
          c) show_ctrlchan_info=1;;
          g) show_gro_dump=1;;
          C) show_nfp_counters=1;;
          L) show_rtsym=1;;
          w) show_worker_state=1;;
          e) show_ecc=1;;
          G) dump_gprs=1;;
          P) parse_concat_dump_file=$OPTARG;;
          p) parse_concat_dump_target_dir=$OPTARG;;
          o) case $OPTARG in
             /*) NETRONOME_SYSINFO=$OPTARG ;;
             *)  NETRONOME_SYSINFO=`pwd`"/$OPTARG" ;;
            esac
    esac
done
shift $(($OPTIND - 1))

no_save="$show_dmesg$show_lspci$show_hwinfo$show_bootlog$show_versions$show_piftest\
$show_sysinfo$show_ovs$show_host_top$show_ps_info$show_port_info\
$show_nfp_symbols$show_me_info$show_nfp_info$show_arp_routing$show_flow_entries\
$show_fc_locks$show_pif_stats$show_nfd_info$show_ctrlchan_info$show_gro_dump$show_nfp_counters$show_rtsym\
$show_worker_state$show_ecc$dump_gprs"

cd $(dirname $0)

if [ -n "$parse_concat_dump_file" ]; then
    if [ -z "$parse_concat_dump_target_dir" ]; then
       echo "You need to specify a target output directory as well with the -p flag"
       exit 1
    fi
    parse_troubleshoot_concat_dump "$parse_concat_dump_file" "$parse_concat_dump_target_dir"
    exit $?
fi

if [ -z "$run_all" -a -z "$no_save" ]; then
    # Remove old info
    rm -f $NETRONOME_SYSINFO/*.txt
    mkdir -p $NETRONOME_SYSINFO || ( echo "Couldn't create $NETRONOME_SYSINFO" && exit 1 )
fi

if [ -z "$run_all" -a -z "$no_save" ]; then
    print_step=1
    echo "Netronome Systems - System Troubleshoot Information"
    echo
fi

if [ -z "${NETRONOME_BIN}/nfp-hwinfo" ]; then
    echo "Error: nfp-hwinfo not found - is the Netronome BSP package installed?." >> $output
    return 1
fi

MODEL=$(${NETRONOME_BIN}/nfp-hwinfo assembly.model 2>&1 | cut -d\= -f2)
case "$MODEL" in
    starfighter1)
        PLATFORM_CONFIG="Starfighter"
        NBI_LIST="0"
        PCIE_LIST="4 5 6 7"
        WORKER_ME_LIST="32 33 34 35 36 37 38"
        ;;
    starfighter2)
        PLATFORM_CONFIG="Starfighter2"
        NBI_LIST="0"
        PCIE_LIST="4"
        WORKER_ME_LIST="32 33 34 35 36 37"
        ;;
    hydrogen)
        PLATFORM_CONFIG="Hydrogen"
        NBI_LIST="0"
        PCIE_LIST="4"
        WORKER_ME_LIST="32 33 34 35 36"
        ;;
    kirtland)
        PLATFORM_CONFIG="OSVP"
        NBI_LIST="0"
        PCIE_LIST="4 5"
        WORKER_ME_LIST="32 33 34 35 36 37"
        ;;
    bataan)
        PLATFORM_CONFIG="CDP"
        NBI_LIST="0 1"
        PCIE_LIST="4 5 6 7"
        WORKER_ME_LIST="32 33 34 35 36 37 38"
        ;;
    FWA-6522C)
        PLATFORM_CONFIG="CDP"
        NBI_LIST="0 1"
        MAC_CORE_LIST="0 1"
        PCIE_LIST="4 5 6 7"
        WORKER_ME_LIST="32 33 34 35 36 37 38"
        ;;
    *)
        echo "Unknown assembly model (${MODEL}). Assuming conservative defaults"
        PLATFORM_CONFIG="Unknown"
        NBI_LIST="0"
        MAC_CORE_LIST="0"
        PCIE_LIST="4"
        WORKER_ME_LIST="32 33 34 35 36 37"
        ;;
esac

FIRST_WORKER_ME=$(echo "$WORKER_ME_LIST" | grep -Eo '^[0-9]+')
LAST_WORKER_ME=$(echo "$WORKER_ME_LIST" | grep -Eo '[0-9]+$')

ret=0
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_dmesg"         ]; then run_timeout 120 ns_dmesg;        let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_lspci"         ]; then run_timeout 120 ns_lspci;        let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_hwinfo"        ]; then run_timeout 120 ns_hwinfo;       let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_bootlog"       ]; then run_timeout 120 ns_bootlog;      let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_versions"      ]; then run_timeout 120 ns_versions;     let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_piftest"       ]; then run_timeout 120 ns_piftest;      let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_sysinfo"       ]; then run_timeout 120 ns_sysinfo;      let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_host_top"      ]; then run_timeout 120 ns_host_top;     let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_flow_entries"  ]; then run_timeout 120 ns_flow_info;    let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_fc_locks"      ]; then run_timeout 3600 ns_fc_locks;    let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_pif_stats"     ]; then run_timeout 3600 ns_pif_stats;  let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_nfd_info"      ]; then run_timeout 120 ns_nfd_info;     let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_ctrlchan_info" ]; then run_timeout 120 ns_ctrlchan_info;let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_gro_dump"      ]; then run_timeout 120 ns_grodump;      let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_ps_info"       ]; then run_timeout 120 ns_ps_info;      let ret+=$?; fi
# if [ -n "$run_all" -o -z "$no_save" -o -n "$show_ovs"           ]; then run_timeout 120 ns_ovs_info;     let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_arp_routing"   ]; then run_timeout 120 ns_arp_route;    let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_port_info"     ]; then run_timeout 120 port_info;       let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_nfp_symbols"   ]; then run_timeout 3600 ns_nfp_symbols; let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_me_info"       ]; then run_timeout 3600 ns_dump_mes;    let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_nfp_info"      ]; then run_timeout 300 ns_nfp_info;     let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_rtsym"         ]; then run_timeout 900 ns_rtsym_dump;   let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_worker_state"  ]; then run_timeout 120 ns_worker_state; let ret+=$?; fi
if [ -n "$run_all" -o -z "$no_save" -o -n "$show_ecc"           ]; then run_timeout 120 ns_ecc;          let ret+=$?; fi

# XXX: Add ns_dump_gprs at the end of the default mode, which will stop all MEs!
# XXX: Has to be at the end of the no_save script!
# And PIF will need a restart after running this script with no flags!
if [ -n "$run_all" -o -z "$no_save" -o -n "$dump_gprs"          ]; then run_timeout 30 ns_dump_gprs; let ret+=$?; fi

if [ -n "$show_nfp_counters" ]; then run_timeout 30 ns_debug_counters; let ret+=$?; fi

rm -f /tmp/ns_pif_troubleshoot.*

#---------------------------------------------------------------------
# Create archive
#---------------------------------------------------------------------
if [ -z "$no_save" -a -z "$run_all" ]; then
    ARCHIVE_NAME=sysinfo-`date +%F.%H%M`
    echo
    echo -n "Archiving system information... "
    cd $NETRONOME_SYSINFO
    mkdir -p archives
    arch=archives/$ARCHIVE_NAME.tgz
    ln -s . $ARCHIVE_NAME
    tar czf $arch $ARCHIVE_NAME/*.txt
    rm -f $ARCHIVE_NAME
    echo "done"
    echo
    echo "Please review the contents of $NETRONOME_SYSINFO and e-mail"
    echo "$NETRONOME_SYSINFO/$arch"
    echo "to support@netronome.com along with a description of the problem you have"
    echo "encountered. If possible, also provide steps to reproduce the problem."
fi

exit $ret

