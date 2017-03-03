#!/bin/bash
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- uImage

echo "make uImage over :)"
