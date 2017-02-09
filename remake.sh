#!/bin/sh

# set various
# Ubuntu挂载的SHARE_PATHdows下的共享目录路径
SHARE_PATH="/mnt/hgfs/Desktop"
# boot.img合成的工具的文件夹的路径
BOOTIMG_TOOLS_PATH="/home/myzr/myandroid/out/target/product/sabresd_6dq/bootimgTools"
#BOOTIMG_TOOLS_PATH="/home/myzr/bootimgTools"
# Android编译之后输出的OUT_PATH目录路径
OUT_PATH_PATH="/home/myzr/myandroid/OUT_PATH/target/product/sabresd_6dq"
# SHARE_PATHdows下的MFG烧写软件的android目录在共享目录下的路径
SHARE_PATH_PATH="SBC7112_A4.2_TOOL/Profiles/MX6DL Linux Update/OS Firmware/files/android"

echo "\nWill cp boot.img to path: ${SHARE_PATH}/${SHARE_PATH_PATH}/boot.img\n"

make uImage
if [ $? -ne 0 ] ; then      #如果编译内核错误，后面就没必要运行了
	echo "error: make uImage failed."
	exit -1;
fi

if [ ! -d ${BOOTIMG_TOOLS_PATH}/boot ]; then
	echo "error: ${BOOTIMG_TOOLS_PATH}/boot/boot.img-kernel not exist."
	echo "       please split a boot.img at first."
	exit -1;
fi

if [ ! -f ${BOOTIMG_TOOLS_PATH}/boot/boot.img-kernel ]; then
	echo "error: ${BOOTIMG_TOOLS_PATH}/boot/boot.img-kernel not exist."
	echo "       please split a boot.img at first."
	exit -1;
fi

rm "${BOOTIMG_TOOLS_PATH}/boot/boot.img-kernel" -rf
echo "cp arch/arm/boot/zImage ${BOOTIMG_TOOLS_PATH}/boot/boot.img-kernel"
cp arch/arm/boot/zImage ${BOOTIMG_TOOLS_PATH}/boot/boot.img-kernel

#repack boot.img file
echo "cd ${BOOTIMG_TOOLS_PATH}"
cd ${BOOTIMG_TOOLS_PATH}
./repack
echo "boot.img repacked over."


cp newboot.img "${SHARE_PATH}/${SHARE_PATH_PATH}/boot.img"
echo "move work over.    :)"

