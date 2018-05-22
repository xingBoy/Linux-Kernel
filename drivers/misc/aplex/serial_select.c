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

#define SERIAL_LOOP		55 
#define SERIAL_485		56
#define SERIAL_232		57	
#define SERIAL_422		58

#define Control_Module_address				0x44E10000

#define CONFIG_GPMC_AD8_offset				0x820
#define CONFIG_GPMC_AD9_offset				0x824
#define CONFIG_GPMC_XDMA_EVENT_INT0_offset				0x9B0


#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))
#define GET_STRUCT_ADDR (ptr,type,member)\
	 ((unsigned long )ptr - (unsigned long)((type*)0->member)) 


static int aplex_serial_select_init(void)
{
	int result;
	void __iomem * base = ioremap(Control_Module_address , 0x1FFF);
	__raw_writel(0x37,(base +CONFIG_GPMC_AD8_offset));
	__raw_writel(0x37,(base +CONFIG_GPMC_AD9_offset));
	__raw_writel(0x37,(base +CONFIG_GPMC_XDMA_EVENT_INT0_offset));
        /* Allocating GPIOs and setting direction */
        result = gpio_request(GPIO_TO_PIN(0,22), "serial_01");//usr1
        if (result != 0) {
                printk("gpio_request(0_22) failed!\n"); return result; }
        result = gpio_request(GPIO_TO_PIN(0,23), "serial_02");//usr1
        if (result != 0) {
                printk("gpio_request(1_23) failed!\n"); return result; }
        result = gpio_request(GPIO_TO_PIN(0,19), "serial_02");//usr1
        if (result != 0) {
                printk("gpio_request(0_19) failed!\n"); return result; }

        result = gpio_direction_output(GPIO_TO_PIN(0,22), 1);
        if (result != 0) {
                printk("gpio_direction(0_22) failed!\n"); return result; }	
        result = gpio_direction_output(GPIO_TO_PIN(0,23), 1);
        if (result != 0) {
                printk("gpio_direction(0_23) failed!\n"); return result; }	
        result = gpio_direction_output(GPIO_TO_PIN(0,19), 1);
        if (result != 0) {
                printk("gpio_direction(0_19) failed!\n"); return result; }	
        gpio_set_value(GPIO_TO_PIN(0,19), 0);
        gpio_set_value(GPIO_TO_PIN(0,22), 1);
        gpio_set_value(GPIO_TO_PIN(0,23), 0);
	return result;

}
static int aplex_serial_select_open(struct inode *inode, struct file *file)
{
	return 0;
}
static ssize_t aplex_serial_select_read (struct file *file, char __user *buf, size_t size, loff_t * off)
{
	return 0;
}
static int aplex_serial_select_ioctl(struct file *filp,unsigned int cmd,	unsigned long arg)
{ 
	if(cmd == SERIAL_LOOP) {
        	gpio_set_value(GPIO_TO_PIN(0,22), 0);
        	gpio_set_value(GPIO_TO_PIN(0,23), 0);
        	gpio_set_value(GPIO_TO_PIN(0,19), 0);
	}
	if(cmd == SERIAL_232) {
        	gpio_set_value(GPIO_TO_PIN(0,22), 1);
        	gpio_set_value(GPIO_TO_PIN(0,23), 0);
        	gpio_set_value(GPIO_TO_PIN(0,19), 0);
	}
	if(cmd == SERIAL_485) {
        	gpio_set_value(GPIO_TO_PIN(0,22), 1);
        	gpio_set_value(GPIO_TO_PIN(0,23), 1);
        	gpio_set_value(GPIO_TO_PIN(0,19), 1);
	}
	if(cmd == SERIAL_422) {
        	gpio_set_value(GPIO_TO_PIN(0,22), 0);
        	gpio_set_value(GPIO_TO_PIN(0,23), 1);
        	gpio_set_value(GPIO_TO_PIN(0,19), 0);
	}

	return 0;      
}
static ssize_t aplex_serial_select_write (struct file *file, char __user *buf, size_t size, loff_t * off)
{
	return 0;
}
static int aplex_serial_select_release (struct inode *inode, struct file *file)
{
	return 0;
}
struct file_operations serial_select_fops = {
	.owner = THIS_MODULE,
	.open =    aplex_serial_select_open,
	.read =    aplex_serial_select_read,
	.write =   aplex_serial_select_write,
	.release = aplex_serial_select_release,
	.unlocked_ioctl= aplex_serial_select_ioctl

};

static int __init  aplex_serial_s_init()
{
	if(register_chrdev_region(MKDEV(301,0),1,"aple_buzz") < 0)
	{
		return 	-ENOMEM	;
	}
	aplex_buzz = cdev_alloc();
	if(aplex_buzz == NULL)
	{
		return -ENOMEM;
	}
	cdev_init(aplex_buzz , &buzz_fops);
	cdev_add(aplex_buzz ,MKDEV(301,0),1);
	aplex_buzz_init();
	return 0;
}
static void  __exit  aplex_serial_s_exit()
{
	cdev_del(aplex_buzz);
	unregister_chrdev_region(MKDEV(301,0),1);
}


module_init(aplex_buzzm_init);
module_exit(aplex_buzzm_exit);
MODULE_AUTHOR("Danny Zhao");
MODULE_LICENSE("GPL");







