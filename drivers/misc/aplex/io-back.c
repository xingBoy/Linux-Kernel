#include <linux/init.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/semaphore.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <mach/gpio.h>
#include <plat/mux.h>
#include <linux/gpio.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <linux/init.h>
#include <linux/genhd.h>
#include <linux/miscdevice.h>

#define Control_Module_address				0x44E10000

#define CONFIG_GPMC_AD12_offset				0x830	
#define CONFIG_GPMC_AD13_offset				0x834	
#define CONFIG_GPMC_AD14_offset				0x838	
#define CONFIG_GPMC_AD15_offset				0x83c	

#define CONFIG_MCASP0_ACLKX_offset          0x990
#define CONFIG_MCASP0_FSX_offset            0x990
#define CONFIG_MCASP0_AXR0_offset           0x990
#define CONFIG_MCASP0_AHCLKR_offset         0x990



#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))
#define GET_STRUCT_ADDR (ptr,type,member)\
	 ((unsigned long )ptr - (unsigned long)((type*)0->member)) 

#define APLEX_IO_DO1			GPIO_TO_PIN(3,14)	
#define APLEX_IO_DO2			GPIO_TO_PIN(3,15)	
#define APLEX_IO_DO3			GPIO_TO_PIN(3,16)	
#define APLEX_IO_DO4			GPIO_TO_PIN(3,17)	
#define APLEX_IO_DIN1			GPIO_TO_PIN(1,12)	
#define APLEX_IO_DIN2			GPIO_TO_PIN(1,13)	
#define APLEX_IO_DIN3			GPIO_TO_PIN(1,14)	
#define APLEX_IO_DIN4			GPIO_TO_PIN(1,15)	


#define IO_DO1			101
#define IO_DO2			102
#define IO_DO3			103
#define IO_DO4			104

#define IO_DI1			201
#define IO_DI2			202
#define IO_DI3			203
#define IO_DI4			204

#define IO_VAULE_H			55	
#define IO_VAULE_L			56	

struct cdev * aplex_io;
static int aplex_io_set_init(void)
{
	int err;

	void __iomem * base = ioremap(Control_Module_address , 0x1FFF);
	__raw_writel(0x37 ,(base + CONFIG_GPMC_AD12_offset		 ));
	__raw_writel(0x37 ,(base + CONFIG_GPMC_AD13_offset		 ));
	__raw_writel(0x37 ,(base + CONFIG_GPMC_AD14_offset		 ));
	__raw_writel(0x37 ,(base + CONFIG_GPMC_AD15_offset		 ));
	
	__raw_writel(0x37 ,(base + CONFIG_MCASP0_ACLKX_offset 		 ));
	__raw_writel(0x37 ,(base + CONFIG_MCASP0_FSX_offset   		 ));
	__raw_writel(0x37 ,(base + CONFIG_MCASP0_AXR0_offset  		 ));
	__raw_writel(0x37 ,(base + CONFIG_MCASP0_AHCLKR_offset		 ));
        /* Allocating GPIOs and setting direction */
        err = gpio_request(APLEX_IO_DO1,"aplex_io_do1");//usr1
        if (err != 0) {
                printk("gpio_request(3_14) failed!\n"); return err; }
        err = gpio_request(APLEX_IO_DO2,"aplex_io_do2");//usr1
        if (err != 0) {
                printk("gpio_request(3_15) failed!\n"); return err; }
        err = gpio_request(APLEX_IO_DO3,"aplex_io_do3");//usr1
        if (err != 0) {
                printk("gpio_request(3_16) failed!\n"); return err; }
        err = gpio_request(APLEX_IO_DO4,"aplex_io_do4");//usr1
        if (err != 0) {
                printk("gpio_request(3_17) failed!\n"); return err; }
        err = gpio_request(APLEX_IO_DIN1,"aplex_io_din1");//usr1
        if (err != 0) {
                printk("gpio_request(1_12) failed!\n"); return err; }
        err = gpio_request(APLEX_IO_DIN2,"aplex_io_din2");//usr1
        if ( err != 0) {
                printk("gpio_request(1_13) failed!\n"); return err; }
        err = gpio_request(APLEX_IO_DIN3,"aplex_io_din3");//usr1
        if ( err != 0) {
                printk("gpio_request(1_14) failed!\n"); return err; }
        err = gpio_request(APLEX_IO_DIN4,"aplex_io_din4");//usr1
        if ( err != 0) {
                printk("gpio_request(1_15) failed!\n"); return err; }

        err = gpio_direction_output(APLEX_IO_DO1, 0);
        if (err != 0) {
                printk("gpio_direction(3_14) failed!\n"); return err; }
        err = gpio_direction_output(APLEX_IO_DO2, 0);
        if (err != 0) {
                printk("gpio_direction(3_15) failed!\n"); return err; }
        err = gpio_direction_output(APLEX_IO_DO3, 0);
        if (err != 0) {
                printk("gpio_direction(3_16) failed!\n"); return err; }	
        err = gpio_direction_output(APLEX_IO_DO4, 0);
        if (err != 0) {
                printk("gpio_direction(3_17) failed!\n"); return err; }	

		err = gpio_direction_input(APLEX_IO_DIN1);
        if (err != 0) {
                printk("gpio_direction(1_12) failed!\n"); return err; }	
		err = gpio_direction_input(APLEX_IO_DIN2);
        if (err != 0) {
                printk("gpio_direction(1_12) failed!\n"); return err; }	
		err = gpio_direction_input(APLEX_IO_DIN3);
        if (err != 0) {
                printk("gpio_direction(1_12) failed!\n"); return err; }	
		err = gpio_direction_input(APLEX_IO_DIN4);
        if (err != 0) {
                printk("gpio_direction(1_12) failed!\n"); return err; }	

        gpio_set_value(APLEX_IO_DO1, 0);
        gpio_set_value(APLEX_IO_DO2, 0);
        gpio_set_value(APLEX_IO_DO3, 0);
        gpio_set_value(APLEX_IO_DO4, 0);
return err;

}
static int aplex_io_open(struct inode *inode, struct file *file)
{
	return 0;
}
static ssize_t aplex_io_read (struct file *file, char __user *buf, size_t size, loff_t * off)
{
	return 0;
}
static long  aplex_io_ioctl(struct file *filp,unsigned int cmd,	unsigned long arg)
{ 
     int ret ;
	 ret = 0;
	if(cmd ==  IO_DO1) {
			if (arg == IO_VAULE_H) {
				gpio_set_value(APLEX_IO_DO1, 1);
			}
			if (arg == IO_VAULE_L) {
				gpio_set_value(APLEX_IO_DO1, 0);
			}
	}
	if(cmd == IO_DO2) {
			if (arg == IO_VAULE_H) {
				gpio_set_value(APLEX_IO_DO2, 1);
			}
			if (arg == IO_VAULE_L) {
				gpio_set_value(APLEX_IO_DO2, 0);
			}
	}
	if(cmd == IO_DO2) {
			if (arg == IO_VAULE_H) {
				gpio_set_value(APLEX_IO_DO3, 1);
			}
			if (arg == IO_VAULE_L) {
				gpio_set_value(APLEX_IO_DO3, 0);
			}
	}
	if(cmd == IO_DO2) {
			if (arg == IO_VAULE_H) {
				gpio_set_value(APLEX_IO_DO4, 1);
			}
			if (arg == IO_VAULE_L) {
				gpio_set_value(APLEX_IO_DO4, 0);
			}
	}
	if (cmd == IO_DI1) {
		ret = gpio_get_value(APLEX_IO_DIN1);
		if (ret)
			ret = IO_VAULE_H;			
		else
			ret = IO_VAULE_L;			
	}
	if (cmd == IO_DI2) {
		ret = gpio_get_value(APLEX_IO_DIN2);
		if (ret)
			ret = IO_VAULE_H;			
		else
			ret = IO_VAULE_L;			
	}
	if (cmd == IO_DI3) {
		ret = gpio_get_value(APLEX_IO_DIN3);
		if (ret)
			ret = IO_VAULE_H;			
		else
			ret = IO_VAULE_L;			
	}
	if (cmd == IO_DI4) {
		ret = gpio_get_value(APLEX_IO_DIN4);
		if (ret)
			ret = IO_VAULE_H;			
		else
			ret = IO_VAULE_L;			
	}
	/*
	else {
			printk("kernel debug info: the parameter cmd = 1 , 2 , 3 , 4\n");
	}
	*/
	return ret;
}
static ssize_t aplex_io_write (struct file *file, char __user *buf, size_t size, loff_t * off)
{
	return 0;
}
int aplex_io_release (struct inode *inode, struct file *file)
{
	return 0;
}
struct file_operations io_fops = {
	.owner = THIS_MODULE,
	.open =    aplex_io_open,
	.read =    aplex_io_read,
	.write =   aplex_io_write,
	.release = aplex_io_release,
	.unlocked_ioctl= aplex_io_ioctl
};

static int __init  aplex_io_init()
{
	if(register_chrdev_region(MKDEV(304,0),1,"aplex_io") < 0)
	{
		return 	-ENOMEM	;
	}
	aplex_io = cdev_alloc();
	if(aplex_io == NULL)
	{
		return -ENOMEM;
	}
	cdev_init(aplex_io , &io_fops);
	cdev_add(aplex_io ,MKDEV(304,0),1);
	aplex_io_set_init();
	return 0;
}
static void  __exit  aplex_io_exit()
{
	cdev_del(aplex_io);
	unregister_chrdev_region(MKDEV(304,0),1);
}


module_init(aplex_io_init);
module_exit(aplex_io_exit);
MODULE_AUTHOR("Danny Zhao");
MODULE_LICENSE("GPL");






