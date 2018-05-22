#!/bin/bash
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-

make sbc7109_defconfig

make uImage
#make modules


cp arch/arm/boot/uImage $APLEX_PATH  -rf

echo "make uImage over :)"
