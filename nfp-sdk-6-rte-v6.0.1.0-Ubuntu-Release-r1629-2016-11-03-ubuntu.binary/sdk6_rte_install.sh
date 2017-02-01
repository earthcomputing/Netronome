#!/bin/bash
# Copyright (C) 2015-2016 Netronome Systems, Inc.  All rights reserved.

# Common shell script to install or uninstall Netronome System SDK6 host side
# application and the required dependencies for Ubuntu.

set_variables() {
  #Later on when installation directories can be specified this variable can be cleared
  SUDO="sudo"

  RTEDIR=`realpath .`

  DEP_DIR="$RTEDIR/dependencies"

  NETRODIR="/opt/netronome"
  SDKDIR="/opt/netronome" 

  RTE_BIN_DIR="$RTEDIR/bin"
  RTE_SCRIPT_DIR="$RTEDIR/scripts"
  RTE_UPSTART_DIR="$RTEDIR/init/upstart"
  RTE_SYSTEMD_DIR="$RTEDIR/init/systemd"
  RTE_LIB_DIR="$RTEDIR/lib"
  RTE_CONFIG_DIR="$RTEDIR/etc/configs"
  RTE_TRBLSHT_DIR="$RTEDIR/tools/troubleshoot"
  RTE_THRIFT_DIR="$RTEDIR/thrift"
  RTE_INSTALL_DIR="/opt/nfp_pif/"  
  
  ERR47_CHECK_SCRIPT="$RTE_SCRIPT_DIR/err47_check.sh"

  #NFP BSP
  BSP_VERSION="2016.11.2.1240-1"
  BSP_BRANCH="nfp-bsp-6000-b0"
  BSP_PACKAGE_NAME_DEB=""$BSP_BRANCH"_"$BSP_VERSION"_amd64.deb"
  BSP_DEV_PACKAGE_NAME_DEB=""$BSP_BRANCH"-dev_"$BSP_VERSION"_amd64.deb"
  BSP_KPACKAGE_NAME_DEB=""$BSP_BRANCH"-dkms_"$BSP_VERSION"_all.deb"
  BSP_DIR="$DEP_DIR/nfp-bsp/dpkg/"
  BSP_INSTALL_DIR="/"

  #NFP SDK 6 Hardware Debug Server
  HW_DBG_SRV_VERSION="6.0.1.0"
  HW_DBG_SRV_PACKAGE_NAME_TGZ="nfp-sdk-hwdbgsrv-"$HW_DBG_SRV_VERSION".tgz"
  HW_DBG_SRV_PACKAGE_NAME_TAR="nfp-sdk-hwdbgsrv-"$HW_DBG_SRV_VERSION".tar"
  HW_DBG_SRV_SUBDIR="nfp-sdk-hwdbgsrv-6/tar/"
  HW_DBG_SRV_RELEASE_LOC="$DEP_DIR/$HW_DBG_SRV_SUBDIR"
  HW_DBG_SRV_INSTALL_DIR="$NETRODIR"

  #Thrift Compiler
  THRIFT_VERSION="0.9.3"

  PACKAGED_BSP_NAME_VER=""$BSP_BRANCH"_""$($SUDO dpkg-deb -f $BSP_DIR/$BSP_PACKAGE_NAME_DEB Version)"
  PACKAGED_BSP_VERSION="$($SUDO dpkg-deb -f $BSP_DIR/$BSP_PACKAGE_NAME_DEB Version)"
  
  NUM_OF_NFP=$(lspci -d 19ee: | wc -l)
}

check_ubuntu_version() {
    if [ -f /etc/lsb-release ]; then
        if grep -q -i "Ubuntu 14.04" /etc/lsb-release; then
            UBUNTU_VERSION="14.04"
        elif grep -q -i "Ubuntu 16.04" /etc/lsb-release; then
            UBUNTU_VERSION="16.04"
        else
            echo -e "Current version of Ubuntu not supported"
            exit 1
        fi
    else
        echo -e "Attempting to run on non-Ubuntu distribution"
        exit 1
    fi
}

version_compare() {
    if [ "$1" == "$2" ]; then
        echo 0
        return
    fi
    
    local version1
    local version2
    
    IFS=. read -r -a version1 <<< $1
    IFS=. read -r -a version2 <<< $2
    
   len=${#version1[@]}
    if [ ${#version2[@]} > len ]; then
        len=${#version2[@]}
    fi   
    
    for (( idx=0; idx<$len; idx++)); do
        if [ $idx -eq ${#version1[@]} ]; then
            echo 2
            return
        fi
        if [ $idx -eq ${#version2[@]} ]; then
            echo 1
            return
        fi

        if [ ${version1[$idx]} -ne ${version2[$idx]} ]; then            
            if [ ${version1[$idx]} -gt ${version2[$idx]} ]; then
                echo 1
                return
            else
                echo 2
                return
            fi
        fi
    done
    
    echo 0
}

check_nfp_bsp() {
  #Check that the nfp bsp is installed and then version check between installed and packaged
  local REQUIRED_TOOLS=("${BSP_BRANCH}" "${BSP_BRANCH}-dev" "${BSP_BRANCH}-dkms") 
  
  local FAILED=0
  echo -e "\n *** Checking that required NFP BSP tools are installed and accessible"
  for TOOL in "${REQUIRED_TOOLS[@]}"; do
    if ! dpkg --list | grep $TOOL > /dev/null ; then
      echo -e "\n *** $TOOL not installed or not accessible"
      FAILED=1
    fi
  done

  if [ $FAILED -gt 0 ]; then
    while true; do
      read -p "Can version $PACKAGED_BSP_NAME_VER of the NFP Board Support Package (BSP) be installed [Y/n]?" yn
      case $yn in
        [yY]* ) 
            echo "Continue with nfp-bsp installation"
            update_bsp
            return
            ;;
        [nN]* ) 
            echo "NOT INSTALLING NFP BSP. Please correct BSP installation and rerun RTE install script"
            exit 1
            ;;
        * ) 
            echo "Please answer yes or no."
            ;;
      esac
    done
  fi

  local BSP_INSTALLED_VERSION="$($SUDO dpkg-query -W -f '${version}\n' "${BSP_BRANCH}")"
  ver_res=$(version_compare $PACKAGED_BSP_VERSION $BSP_INSTALLED_VERSION)
  if [ $ver_res -eq 1 ]; then 
    while true; do
        read -p "Older version of NFP BSP detected, upgrade NFP BSP to $PACKAGED_BSP_NAME_VER [Y/n]?" yn
        case $yn in
        [yY]* )  
                echo "Updating NFP BSP"  
                update_bsp
                break
                ;;
        [nN]* )  
                echo "NOT UPDATING NFP BSP, continuing RTE installation ... "
                break
                ;;
        * ) 
                echo "Please answer yes or no."
                ;;
        esac
     done
  else
    echo -e "\n Up to date version of NFP BSP ($BSP_INSTALLED_VERSION) detected"
  fi 
}

check_requirements() {
  #Check that all the required system tools are installed and accessible
  REQUIRED_TOOLS=("realpath" "libftdi1" "libjansson4" "build-essential" 
                  "linux-headers-`uname -r`" "dkms")
  
  FAILED=0
  echo -e "\n *** Checking that required tools are installed and accessible"
  for TOOL in "${REQUIRED_TOOLS[@]}"; do
    if ! dpkg --list | grep $TOOL > /dev/null ; then
      echo -e "\n *** Please ensure that $TOOL is installed and accessible via the path."
      FAILED=1
    fi
  done
  if [ $FAILED -gt 0 ]; then
    exit 1
  fi
}

check_nfp_loaded() {
  # check if firmware is loaded (if possible) and
  # prevent installation if it is;
  # this is to avoid potential system crashes
  if command -v nfp-nffw >/dev/null 2>&1; then
    local dev_no=0
    while (( dev_no < NUM_OF_NFP )); do
      echo "Check nfp-nffw status $dev_no of $NUM_OF_NFP"
      
      if nfp-nffw status -n $dev_no | grep -q "Firmware loaded: Yes"; then
        echo -e "\n *** Please ensure that firmware on NFP device $dev_no has been unloaded before proceeding"
        echo -e "\n *** To unload current firmware: nfp-nffw unload -n $dev_no"
        exit 1
      fi
      dev_no=$(( $dev_no + 1 ))
    done
  fi
}

check_err_47() {
    if [ -n "${SYSLOGGER}" ]; then
        ${SYSLOGGER} -t "NFP SDK6 RTE" -p local0.info "Checking for ERR47 workaround. System may reboot if the ERR47 workaround is not installed."; \
    fi
    $ERR47_CHECK_SCRIPT user
}

check_ari_support() {
    # Check for ARI support
    ARI_SUPPORT=$($NETRODIR/bin/nfp-support | grep ARI -m1)
    if [[ "$ARI_SUPPORT" == *"PASS"* ]]; then
      echo "ARI Support detected"
    else
      echo "No ARI Support found. VFs not supported."
    fi
}

usage() {
    cat <<EOH

Usage: $0 <action>
Copyright (C) 2007-2016 Netronome Systems, Inc.

 $0 is the install script for the SDK6 host application on Ubuntu

Where <action> is:
  help or ?          Show this help dialog
  install            Install all dependencies, NFP BSP/Tools update optional (RECOMMENDED)
  install_force_bsp  Install all dependencies forcing NFP BSP/Tools update
  install_rte_only   Install RTE without NFP BSP and Hardware Debug Server
  uninstall          Uninstall all dependencies except NFP BSP/Tools
  uninstall_with_bsp Uninstall all dependances including NFP BSP/Tools
  bsp_version_info   Prints currently installed and packaged NFP BSP/Tools versions

EOH
    exit 1
}

update_bsp() {
  echo -e "\n *** Uninstalling Previous Netronome Systems BSP Component"
  for pkg in `dpkg-query -W -f='${binary:Package}\n' nfp-bsp* 2>/dev/null | grep dev`; do
    $SUDO dpkg --purge $pkg
  done 

  for pkg in `dpkg-query -W -f='${binary:Package}\n' nfp-bsp* 2>/dev/null`; do
    $SUDO dpkg --purge $pkg
  done

  echo -e "\n *** Installing Netronome Systems BSP Component"
  $SUDO dpkg --instdir=$BSP_INSTALL_DIR -i $BSP_DIR/$BSP_PACKAGE_NAME_DEB
  $SUDO dpkg --instdir=$BSP_INSTALL_DIR -i $BSP_DIR/$BSP_DEV_PACKAGE_NAME_DEB
  $SUDO dpkg --instdir=$BSP_INSTALL_DIR -i $BSP_DIR/$BSP_KPACKAGE_NAME_DEB
  $SUDO depmod -a
  $SUDO /sbin/ldconfig  
  
  modprobe nfp  
  local dev_no=0
  while (( dev_no < NUM_OF_NFP )); do
    if $NETRODIR/bin/nfp-hwinfo -n $dev_no 2>&1 | grep 'assembly.model' | sed 's/.*=//g' | grep -E '(carbon|lithium|hydrogen|beryllium|starfighter)' >/dev/null 2>&1 ; then
      local flash_image=$NETRODIR/flash/flash-nic.bin
      # Note, the sed below ifgnores nfp-media changes IE using a breakout cable 
      local nfp_versions=$($NETRODIR/bin/nfp-fis -n $dev_no list -v | sed '/^hwinfo.bin.*$/d')
      
      local flash_versions=$($NETRODIR/bin/nfp-fis --image=$flash_image list -v)     
      
      if [ "$flash_versions" = "$nfp_versions" ]; then
        echo "NFP device $dev_no already has desired flash image"
      else
        echo -e "\n *** Flashing Netronome Systems Devices - Do not interrupt this process!"
        flashed_nfp=true
        $NETRODIR/bin/nfp-arm -n $dev_no --halt && $NETRODIR/bin/nfp-flash -n $dev_no -b 0 -c 0 --i-accept-the-risk-of-overwriting-miniloader -w $flash_image
        if $NETRODIR/bin/nfp-hwinfo -n $dev_no 2>&1 | grep 'assembly.model' | sed 's/.*=//g' | grep -E '(carbon|lithium|hydrogen|beryllium)' >/dev/null 2>&1 ; then
          yes | $NETRODIR/bin/nfp-one -n $dev_no
        fi
        echo -e "Finished flashing Netronome Device number $dev_no."
        RESET_REQUIRED=true
      fi      
    fi  
    dev_no=$(( $dev_no + 1 ))
  done
  
  if (( dev_no == 0 )); then
    while true; do
        read -p "Is this an installation going to be used with NFP SDK simulator [Y/n]?" yn
        case $yn in
            [Yy]* ) echo "SKIPPING FLASHING OPERATION: No supported Netronome hardware detected and SIM mode selected" && break;;
            [Nn]* ) echo "Unable to detect the supported platform. Check output of nfp-hwinfo" && exit 1;;
            * ) echo "Please answer yes or no.";;
        esac        
    done
  fi
}

install_dependencies() {
  echo -e "\n *** Installing Netronome Systems Hardware Debug Server"
  $SUDO mkdir -p "$HW_DBG_SRV_INSTALL_DIR"
  # Do extraction in 2 steps due to tar padding causing warning during unzip
  $SUDO gunzip -k $HW_DBG_SRV_RELEASE_LOC/$HW_DBG_SRV_PACKAGE_NAME_TGZ || true
  $SUDO tar -xf $HW_DBG_SRV_RELEASE_LOC/$HW_DBG_SRV_PACKAGE_NAME_TAR -C "$HW_DBG_SRV_INSTALL_DIR"
  rm $HW_DBG_SRV_RELEASE_LOC/$HW_DBG_SRV_PACKAGE_NAME_TAR

  cd "$HW_DBG_SRV_INSTALL_DIR/nfp-sdk-hwdbgsrv"
  BSPROOT=$NETRODIR
  $SUDO make server-all

  cd $RTEDIR
}

install_rte() {
  echo -e "\n *** Installing Netronome Systems SDK6 Run Time Environment"
  $SUDO mkdir -p "$RTE_INSTALL_DIR/bin/"
  $SUDO mkdir -p "$RTE_INSTALL_DIR/scripts"
  $SUDO mkdir -p "$RTE_INSTALL_DIR/lib"
  $SUDO mkdir -p "$RTE_INSTALL_DIR/log"
  $SUDO mkdir -p "$RTE_INSTALL_DIR/etc/configs"
  $SUDO mkdir -p "$RTE_INSTALL_DIR/tools/troubleshoot"
  $SUDO mkdir -p "$RTE_INSTALL_DIR/thrift"
  $SUDO cp -a "$RTE_BIN_DIR/." "$RTE_INSTALL_DIR/bin/"
  $SUDO cp -a "$RTE_LIB_DIR/." "$RTE_INSTALL_DIR/lib/"
  $SUDO cp -a "$RTE_SCRIPT_DIR/." "$RTE_INSTALL_DIR/scripts"
  $SUDO cp -a "$RTE_CONFIG_DIR/." "$RTE_INSTALL_DIR/etc/configs"
  $SUDO cp -a "$RTE_TRBLSHT_DIR/." "$RTE_INSTALL_DIR/tools/troubleshoot"
  $SUDO cp -a "$RTE_THRIFT_DIR/." "$RTE_INSTALL_DIR/thrift"
  if [ "$UBUNTU_VERSION" == "14.04" ]; then
    $SUDO cp "$RTE_UPSTART_DIR/nfp-sdk6-rte"* "/etc/init/"    
    $SUDO cp "$RTE_UPSTART_DIR/nfp-hwdbg-srv"* "/etc/init/"  
  else
    $SUDO cp "$RTE_SYSTEMD_DIR/nfp-sdk6-rte"* "/lib/systemd/system/"        
    $SUDO cp "$RTE_SYSTEMD_DIR/nfp-hwdbg-srv"* "/lib/systemd/system/"
    $SUDO systemctl daemon-reload
  fi

  $SUDO cp -a "$RTE_CONFIG_DIR/." "$RTE_INSTALL_DIR/etc/configs"
  
  $SUDO cp -a "$RTEDIR/NFP_SDK_EULA.txt" "$RTE_INSTALL_DIR/"
}

uninstall_dependencies() {
  echo -e "\n *** Uninstalling Netronome Systems Hardware Debug Server"
  $SUDO rm -rf "$HW_DBG_SRV_INSTALL_DIR/nfp-sdk-hwdbgsrv"

  $SUDO /sbin/ldconfig
}

uninstall_rte() {
  echo -e "\n *** Uninstalling Netronome Systems SDK6 Run Time Environment"
  $SUDO rm -rf "$RTE_INSTALL_DIR"

  if [ "$UBUNTU_VERSION" == "14.04" ]; then
    $SUDO rm -rf "/etc/init/nfp-sdk6-rte"*    
    $SUDO rm -rf "/etc/init/nfp-hwdbg-srv"*
  else
    $SUDO rm -rf "/usr/lib/systemd/system/nfp-sdk6-rte"*    
    $SUDO rm -rf "/usr/lib/systemd/system/nfp-hwdbg-srv"*
    $SUDO systemctl daemon-reload
  fi
}

uninstall_bsp() {
  echo -e "\n *** Uninstalling Previous Netronome Systems NFP BSP/Tools Components"
  for pkg in `dpkg-query -W -f='${binary:Package}\n' nfp-bsp* 2>/dev/null | grep dev`; do
    $SUDO dpkg --purge $pkg
  done

  for pkg in `dpkg-query -W -f='${binary:Package}\n' nfp-bsp* 2>/dev/null`; do
    $SUDO dpkg --purge $pkg
  done

  $SUDO /sbin/ldconfig
}

set -e

SYSLOGGER=$(which logger 2>/dev/null)
RESET_REQUIRED=false

check_ubuntu_version

check_requirements

if [ "$1" == 'install' ]; then    
  set_variables
  
  check_nfp_loaded
  
  check_nfp_bsp   
  
  check_err_47

  install_dependencies

  install_rte

  check_ari_support

  echo -e "\n *** Netronome Systems SDK6 RTE Installation Complete!"
  if [ "$RESET_REQUIRED" = true ]; then
    echo -e "\n *** Netronome Systems Devices upgraded - PLEASE REBOOT SYSTEM"
  fi
elif [ "$1" == 'install_force_bsp' ]; then    
  set_variables
  
  check_err_47
  
  check_nfp_loaded

  update_bsp

  install_dependencies

  install_rte

  check_ari_support

  echo -e "\n *** Netronome Systems SDK6 RTE and NFP BSP/Tools Installation Complete!"
  if [ "$RESET_REQUIRED" = true ]; then
    echo -e "\n *** Netronome Systems Devices upgraded - PLEASE REBOOT SYSTEM"
  fi
elif [ "$1" == "install_rte_only" ]; then
  set_variables
  
  install_rte

  echo -e "\n *** Netronome Systems SDK6 RTE Installation Complete!"
elif [ "$1" == 'uninstall' ]; then
  set_variables

  uninstall_dependencies

  uninstall_rte

  echo -e "\n *** Finished Uninstalling Netronome Systems SDK6 RTE!"
  echo -e "\n Some files might be left in /opt/netronome that might need to be removed manually"
elif [ "$1" == 'uninstall_with_bsp' ]; then
  set_variables

  uninstall_dependencies

  uninstall_rte
  uninstall_bsp

  echo -e "\n *** Finished Uninstalling Netronome Systems SDK6 RTE and NFP BSP/Tools"
  echo -e "\n Some files might be left in /opt/netronome that might need to be removed manually"
elif [ "$1" == 'bsp_version_info' ]; then
  set_variables
  echo -e "\n NFP-BSP release packaged with this installation: $PACKAGED_BSP_NAME_VER"
  echo -e "\n Currently installed NFP-BSP packages:"
  for pkg in `dpkg-query -W -f='${binary:Package}\n' nfp-bsp-release* 2>/dev/null`; do    
    BSP_INSTALLED_PACKAGE="$($SUDO dpkg -s $pkg | grep Package)"
    BSP_INSTALLED_VERSION="$($SUDO dpkg -s $pkg | grep Version)"
    echo -e "\n   $BSP_INSTALLED_PACKAGE $BSP_INSTALLED_VERSION"
  done
  if [ "$pkg" == '' ]; then
    echo -e "\n No NFP-BSP currently installed"
  fi 
elif [ "$1" == 'help' ] || [ "$1" == '?' ]; then
  usage
else
  echo -e "\n ERROR: UNKNOWN SCRIPT ACTION"
  usage
fi
