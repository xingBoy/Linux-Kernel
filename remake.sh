#!/bin/sh

set -x

make uImage
cp arch/arm/boot/uImage /mnt/hgfs/Desktop/CM-SOMB-IMX6_Q4.8.5_TOOL/Profiles/MX6DL\ Linux\ Update/OS\ Firmware/files/

echo "move work over.    :)"

