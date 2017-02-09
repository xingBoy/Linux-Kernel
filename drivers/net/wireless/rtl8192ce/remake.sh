#!/bin/bash
KERNEL= /home/myzr/parker_kernel
ROOT_PATH=./install
if [ $# -eq 0 ]
then
    make -s -C ${KERNEL} M=$PWD modules
elif [ $# -eq 1 -a "$1" = "clean" ]
then
    make -s -C ${KERNEL} M=$PWD modules clean
elif [ $# -eq 1 -a "$1" = "install" ]
then
    make -s -C ${KERNEL} M=$PWD INSTALL_MOD_PATH=${ROOT_PATH} modules_install
else
    echo "usage:"
    echo "      mm"
    echo "      mm clean"
    echo "      mm install"
fi
