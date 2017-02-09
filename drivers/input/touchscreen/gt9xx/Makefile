ifneq ($(KERNELRELEASE),)
	obj-m := gt9xxm.o
	gt9xxm-objs := gt9xx.o gt9xx_update.o goodix_tool.o
else 
KDIR := /home/myzr/tetra_kernel
all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers *.order
endif
