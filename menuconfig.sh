#!/bin/sh

#export PATH=..uboot-imx.l4.20170807/tools/:$PATH
export ARCH=arm
#export CROSS_COMPILE=/home/lee/toolchain/bin/arm-linux-gnueabihf-

#make distclean
#make clean
#vi arch/arm/configs/imx_v7_defconfig
#Add a line “CONFIG_FHANDLE=y” in the file to prevent the error when boot into rootfs.
make menuconfig
sleep 1
echo "sync(:"
date
