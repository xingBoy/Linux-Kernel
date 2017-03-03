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

#define Control_Module_address				0x44E10000
#define CONFIG_GPMC_A0_offset				0x840	
#define CONFIG_GPMC_A1_offset				0x844	
#define CONFIG_GPMC_A2_offset				0x848	
#define CONFIG_GPMC_A3_offset				0x84c	

#define CONFIG_UART0_TXD_offset				0x974	
#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))
#define GET_STRUCT_ADDR (ptr,type,member)\
	 ((unsigned long )ptr - (unsigned long)((type*)0->member)) 

#define APLEX_IO_DO1			GPIO_TO_PIN(1,16)	
#define APLEX_IO_DO2			GPIO_TO_PIN(1,17)	
//#define APLEX_IO_DIN1			GPIO_TO_PIN(1,18)	
#define APLEX_IO_DIN1			GPIO_TO_PIN(1,11)	
#define APLEX_IO_DIN2			GPIO_TO_PIN(1,19)	


#define IO_DO1			101
#define IO_DO2			102

#define IO_DI1			103
#define IO_DI2			104

#define IO_DI_H			200	
#define IO_DI_L			100	

struct cdev * aplex_io;
static int aplex_io_set_init(void)
{
	int err;

	void __iomem * base = ioremap(Control_Module_address , 0x1FFF);
	__raw_writel(0x37 ,(base + CONFIG_GPMC_A0_offset		 ));
	__raw_writel(0x37 ,(base + CONFIG_GPMC_A1_offset		 ));
	__raw_writel(0x37 ,(base + CONFIG_GPMC_A2_offset		 ));
	__raw_writel(0x37 ,(base + CONFIG_GPMC_A3_offset		 ));
//	__raw_writel(0x37 ,(base +CONFIG_UART0_TXD_offset ));
        /* Allocating GPIOs and setting direction */
        err = gpio_request(APLEX_IO_DO1,"aplex_io_do1");//usr1
        if (err != 0)
                printk("gpio_request(1_16) failed!\n");
        err = gpio_request(APLEX_IO_DO2,"aplex_io_do2");//usr1
        if (err != 0)
                printk("gpio_request(1_17) failed!\n");
        err = gpio_request(APLEX_IO_DIN1,"aplex_io_din1");//usr1
        if (err != 0)
                printk("gpio_request(1_18) failed!\n");
        err = gpio_request(APLEX_IO_DIN2,"aplex_io_din2");//usr1
        if ( err != 0)
                printk("gpio_request(1_19) failed!\n");

        err = gpio_direction_output(APLEX_IO_DO1, 0);
        if (err != 0)
                printk("gpio_direction(1_16) failed!\n");	
        err = gpio_direction_output(APLEX_IO_DO2, 0);
        if (err != 0)
                printk("gpio_direction(1_17) failed!\n");	
		err = gpio_direction_input(APLEX_IO_DIN1);
        if (err != 0)
                printk("gpio_direction(1_18) failed!\n");	
		err = gpio_direction_input(APLEX_IO_DIN2);
        if (err != 0)
                printk("gpio_direction(1_19) failed!\n");	

        gpio_set_value(APLEX_IO_DO1, 0);
        gpio_set_value(APLEX_IO_DO2, 0);
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
	 switch (cmd) {
		 case IO_DO1:
			 if (arg == IO_DI_H)
				gpio_set_value(APLEX_IO_DO1, 1);
			 if (arg == IO_DI_L)
				gpio_set_value(APLEX_IO_DO1, 0);
			 ret = 2;
			 break;
		 case IO_DO2:
			 if (arg == IO_DI_H)
				gpio_set_value(APLEX_IO_DO2, 1);
			 if (arg == IO_DI_L)
				gpio_set_value(APLEX_IO_DO2, 0);
			 ret = 2;
			 break;
		 case IO_DI1:
			 ret = gpio_get_value(APLEX_IO_DIN1);
		  	if (ret)
				ret = IO_DI_H;			
		 	else
				ret = IO_DI_L;			
			 break;
		 case IO_DI2:
			 ret = gpio_get_value(APLEX_IO_DIN2);
		  	if (ret)
				ret = IO_DI_H;			
		 	else
				ret = IO_DI_L;			
			 break;

	 }
	return ret;
	return ret;
}
static ssize_t aplex_io_write (struct file *file, char __user *buf, size_t size, loff_t * off)
{
     int ret ;
	 int cmd ,arg;
	 
	 ret = 0;
	 if(size < 2)
		 return 0;
	 cmd = *buf;
	 arg = *(buf+1);
	
	 switch (cmd) {
		 case IO_DO1:
			 if (arg == IO_DI_H)
				gpio_set_value(APLEX_IO_DO1, 1);
			 if (arg == IO_DI_L)
				gpio_set_value(APLEX_IO_DO1, 0);
			 ret = 2;
			 break;
		 case IO_DO2:
			 if (arg == IO_DI_H)
				gpio_set_value(APLEX_IO_DO2, 1);
			 if (arg == IO_DI_L)
				gpio_set_value(APLEX_IO_DO2, 0);
			 ret = 2;
			 break;
		 case IO_DI1:
			 ret = gpio_get_value(APLEX_IO_DIN1);
		  	if (ret)
				ret = IO_DI_H;			
		 	else
				ret = IO_DI_L;			
			 break;
		 case IO_DI2:
			 ret = gpio_get_value(APLEX_IO_DIN2);
		  	if (ret)
				ret = IO_DI_H;			
		 	else
				ret = IO_DI_L;			
			 break;

	 }
	return ret;
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






