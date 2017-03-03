#include <linux/init.h>
#include <linux/module.h>
#include <linux/leds.h>
#include <linux/io.h>
#include <linux/semaphore.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <mach/gpio.h>
#include <plat/mux.h>
#include <linux/gpio.h>

#define Control_Module_address				0x44E10000

#define CONFIG_MCASP0_AXR1					0x9A8


#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))
#define GET_STRUCT_ADDR (ptr,type,member)\
	 ((unsigned long )ptr - (unsigned long)((type*)0->member)) 


struct cdev * aplex_serail_ctl;
static int aplex_serail_ctl_init(void)
{
	int result;

	void __iomem * base = ioremap(Control_Module_address , 0x1FFF);
	__raw_writel(0x37,(base + CONFIG_MCASP0_AXR1));
        /* Allocating GPIOs and setting direction */
        result = gpio_request(GPIO_TO_PIN(3,20), "serail_ctl");//usr1
        if (result != 0)
                printk("gpio_request(3_20) failed!\n");
        result = gpio_direction_output(GPIO_TO_PIN(3,20), 1);
        if (result != 0)
                printk("gpio_direction(3_20) failed!\n");	
        gpio_set_value(GPIO_TO_PIN(3,20), 0);
	return result;

}
static int aplex_serail_ctl_open(struct inode *inode, struct file *file)
{
	return 0;
}
static ssize_t aplex_serail_ctl_read (struct file *file, char __user *buf, size_t size, loff_t * off)
{
	return 0;
}
static int aplex_serail_ctl_ioctl(struct file *filp,unsigned int cmd,	unsigned long arg)
{ 
	if(cmd == '1') {
        	gpio_set_value(GPIO_TO_PIN(3,20), 1);
	}
	else if (cmd == '0') {
		gpio_set_value(GPIO_TO_PIN(3,20), 0);
	} else {
		printk("kernel debug info : cmd  = '1' on serail_ctl, cmd =  '0' off serail_ctl\n");
	}

	return 0;      
}
static ssize_t aplex_serail_ctl_write (struct file *file, char __user *buf, size_t size, loff_t * off)
{
	char buffer = 0;
	buffer = *buf;
	if(buffer == '1') {
        	gpio_set_value(GPIO_TO_PIN(3,20), 1);
	}
	else if (buffer == '0') {
        	gpio_set_value(GPIO_TO_PIN(3,20), 0);
	} else {
		printk("kernel debug info : write '1' on serail_ctl, write  '0' off serail_ctl\n");
	}

	return 0;
}
static int aplex_serail_ctl_release (struct inode *inode, struct file *file)
{
	return 0;
}
struct file_operations serail_ctl_fops = {
	.owner = THIS_MODULE,
	.open =    aplex_serail_ctl_open,
	.read =    aplex_serail_ctl_read,
	.write =   aplex_serail_ctl_write,
	.release = aplex_serail_ctl_release,
	.unlocked_ioctl= aplex_serail_ctl_ioctl

};

static int __init  aplex_serail_ctlm_init()
{
	if(register_chrdev_region(MKDEV(310,0),1,"aple_serail_ctl") < 0)
	{
		return 	-ENOMEM	;
	}
	
	aplex_serail_ctl = cdev_alloc();
	if(aplex_serail_ctl == NULL)
	{
		return -ENOMEM;
	}
	cdev_init(aplex_serail_ctl , &serail_ctl_fops);
	cdev_add(aplex_serail_ctl ,MKDEV(310,0),1);
	aplex_serail_ctl_init();
	return 0;
}
static void  __exit  aplex_serail_ctlm_exit()
{
	cdev_del(aplex_serail_ctl);
	unregister_chrdev_region(MKDEV(310,0),1);
}


module_init(aplex_serail_ctlm_init);
module_exit(aplex_serail_ctlm_exit);
MODULE_AUTHOR("Danny Zhao");
MODULE_LICENSE("GPL");







