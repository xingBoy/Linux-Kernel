
export ARCH=arm
export CROSS_COMPILE=~/myandroid/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-
# Generate ".config" according to default config file under arch/arm/configs.
# To build the kernel image for i.MX 6Dual/Quad, 6QuadPlus, 6DualLite, 6Solo, 6SoloLite,6SoloX, and 7Dual
#gedit ./arch/arm/configs/imx_v7_android_defconfig
#make distclean
make imx_v7_android_defconfig
make -j2 uImage LOADADDR=0x10008000
