. /opt/fsl-imx-x11/4.9.11-1.0.0/environment-setup-cortexa9hf-neon-poky-linux-gnueabi
#!/bin/sh

#echo $PATH
#echo $CROSS_COMPILE
#echo $PATH
#echo $CROSS_COMPILE
#export CROSS_COMPILE=/opt/fsl-imx-fb/4.1.15/environment-setup-cortexa9hf-vfp-neon-pokylinux-gnueabi
#export CROSS_COMPILE=/opt/fsl-imx-fb/4.1.15/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-
#echo $PATH
#echo $CROSS_COMPILE
export ARCH=arm

#make distclean
#make clean
make imx_v7_defconfig
#make -j2 uImage LOADADDR=0x10008000
make -j2 zImage imx6q-sabresd.dtb
#make -j2
#make -j2 zImage dtbs

cp arch/arm/boot/zImage /media/sf_share/7112/IMX6_L4.1.15_2.0.0_MFG-TOOL/Profiles/Linux/OS\ Firmware/files/zImage
cp arch/arm/boot/dts/imx6q-sabresd.dtb /media/sf_share/7112/IMX6_L4.1.15_2.0.0_MFG-TOOL/Profiles/Linux/OS\ Firmware/files/zImage-imx6q-sabresd.dtb
sync
echo "copy zImage"
echo "copy imx6q-sabresd.dtb to zImage-imx6q-sabresd.dtb"
sleep 1
echo "sync(:"
date
