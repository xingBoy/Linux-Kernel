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

#define IO_VAULE_H          55  
#define IO_VAULE_L          56

#define Control_Module_address				0x44E10000

#define CONFIG_MII1_REFCLK_offset				0x994


#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))
#define GET_STRUCT_ADDR (ptr,type,member)\
	 ((unsigned long )ptr - (unsigned long)((type*)0->member)) 

#define DEBUG_LED_PIN     GPIO_TO_PIN(0,29)


struct cdev * aplex_debug_led;
static int aplex_debug_led_init(void)
{
	int result;

	void __iomem * base = ioremap(Control_Module_address , 0x1FFF);
	__raw_writel(0x37,(base +CONFIG_MII1_REFCLK_offset ));
        /* Allocating GPIOs and setting direction */
        result = gpio_request(DEBUG_LED_PIN, "buzzer");//usr1
        if (result != 0) {
                printk("gpio_request(0_29) failed!\n"); return result; }
        result = gpio_direction_output(DEBUG_LED_PIN, 1);
        if (result != 0) {
                printk("gpio_direction(0_29) failed!\n"); return result; }	
        gpio_set_value(DEBUG_LED_PIN, 0);
	return result;

}
static int aplex_debug_led_open(struct inode *inode, struct file *file)
{
	return 0;
}
static ssize_t aplex_debug_led_read (struct file *file, char __user *buf, size_t size, loff_t * off)
{
	return 0;
}
static int aplex_debug_led_ioctl(struct file *filp,unsigned int cmd,	unsigned long arg)
{ 
	if(cmd == IO_VAULE_H ) {
        	gpio_set_value(DEBUG_LED_PIN,1);
	}
	if(cmd == IO_VAULE_L ) {
        	gpio_set_value(DEBUG_LED_PIN,0);
	}

	return 0;      
}
static ssize_t aplex_debug_led_write (struct file *file, char __user *buf, size_t size, loff_t * off)
{
	return 0;
}
static int aplex_debug_led_release (struct inode *inode, struct file *file)
{
	return 0;
}
struct file_operations debug_ledfops = {
	.owner = THIS_MODULE,
	.open =    aplex_debug_led_open,
	.read =    aplex_debug_led_read,
	.write =   aplex_debug_led_write,
	.release = aplex_debug_led_release,
	.unlocked_ioctl= aplex_debug_led_ioctl

};

static int __init  aplex_debug_ledm_init()
{
	if(register_chrdev_region(MKDEV(300,0),1,"aple_buzz") < 0)
	{
		return 	-ENOMEM	;
	}
	aplex_debug_led = cdev_alloc();
	if(aplex_debug_led == NULL)
	{
		return -ENOMEM;
	}
	cdev_init(aplex_debug_led , &debug_ledfops);
	cdev_add(aplex_debug_led ,MKDEV(300,0),1);
	aplex_debug_led_init();
	return 0;
}
static void  __exit  aplex_debug_ledm_exit()
{
	cdev_del(aplex_debug_led);
	unregister_chrdev_region(MKDEV(300,0),1);
}


module_init(aplex_debug_ledm_init);
module_exit(aplex_debug_ledm_exit);
MODULE_AUTHOR("Danny Zhao");
MODULE_LICENSE("GPL");







