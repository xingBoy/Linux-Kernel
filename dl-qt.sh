#. /opt/fsl-imx-x11/4.9.88-2.0.0/environment-setup-cortexa9hf-neon-poky-linux-gnueabi
#!/bin/sh

export ARCH=arm
export PATH=~/gcc-linaro/bin/:$PATH
export CROSS_COMPILE=arm-linux-gnueabihf-

#make distclean
#make clean
make imx_v7_defconfig
#make -j2 uImage LOADADDR=0x10008000
#make -j2 imx6dl-sabresd.dtb
make -j4 zImage imx6dl-sabresd.dtb
#make -j2 dtbs

cp arch/arm/boot/zImage /mnt/hgfs/dshare/7112/IMX6_L4.1.15_2.0.0_MFG-TOOL/Profiles/Linux/OS\ Firmware/files/zImage
cp arch/arm/boot/dts/imx6dl-sabresd.dtb /mnt/hgfs/dshare/7112/IMX6_L4.1.15_2.0.0_MFG-TOOL/Profiles/Linux/OS\ Firmware/files/zImage-imx6dl-sabresd.dtb
sync
sleep 1
echo "copy zImage"
echo "copy imx6dl-sabresd.dtb to zImage-imx6dl-sabresd.dtb"
echo "sync(:"
date
