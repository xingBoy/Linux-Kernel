#!/bin/sh

#export PATH=..uboot-imx.l4.20170807/tools/:$PATH
export ARCH=arm
export CROSS_COMPILE=/home/lee/toolchain/bin/arm-linux-gnueabihf-

#make distclean
#make clean
#vi arch/arm/configs/imx_v7_defconfig
#Add a line “CONFIG_FHANDLE=y” in the file to prevent the error when boot into rootfs.
make imx_v7_defconfig
#make -j2 uImage LOADADDR=0x10008000
#make -j2 imx6dl-sabresd.dtb
#make -j2
make -j2 zImage dtbs

cp arch/arm/boot/zImage /media/sf_share/7112/IMX6_L4.1.15_2.0.0_MFG-TOOL/Profiles/Linux/OS\ Firmware/files/zImage
cp arch/arm/boot/dts/imx6dl-sabresd.dtb /media/sf_share/7112/IMX6_L4.1.15_2.0.0_MFG-TOOL/Profiles/Linux/OS\ Firmware/files/zImage-imx6dl-sabresd.dtb
sync
echo "copy zImage"
echo "copy imx6dl-sabresd.dtb to zImage-imx6dl-sabresd.dtb"
sleep 1
echo "sync(:"
date
