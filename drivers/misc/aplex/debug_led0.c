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
#include <linux/delay.h>
#include <linux/timer.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <linux/init.h>
#include <linux/genhd.h>
#include <linux/miscdevice.h>

#define CONF_RMII1_REF_CLK_offset     0x944
#define Control_Module_address                          0x44E10000

#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))
#define GET_STRUCT_ADDR (ptr,type,member)\
	 ((unsigned long )ptr - (unsigned long)((type*)0->member)) 

#define APLEX_LED_PIN		GPIO_TO_PIN(0,29)
static ssize_t aplex_debug_led0_write (struct file *file, char __user *buf, size_t size, loff_t * off);

int aplex_debug_led0_init(void) ;
struct cdev * aplex_dled0;
int array[] = {[2] = 9,[4] = 10};
 int aplex_debug_led0_init(void)
{
	int result;

	void __iomem * base = ioremap(Control_Module_address , 0x1FFF);
        __raw_writel(0x37 ,(base + CONF_RMII1_REF_CLK_offset));

        result = gpio_request(APLEX_LED_PIN, "Leds");//usr1
        if (result != 0)
                printk("gpio_request(0_29) failed!\n");
        result = gpio_direction_output(APLEX_LED_PIN, 1);
        if (result != 0)
                printk("gpio_direction(0_29) failed!\n");	
        gpio_set_value(APLEX_LED_PIN, 0);
	return result;

}
static int aplex_debug_led0_open(struct inode *inode, struct file *file)
{
	printk("zengjf test for led debug driver. \n");
	return 0;
}
static ssize_t aplex_debug_led0_read (struct file *file, char __user *buf, size_t size, loff_t * off)
{
	printk("my_read \n");
	return 0;
}
static ssize_t aplex_debug_led0_write (struct file *file, char __user *buf, size_t size, loff_t * off)
{
	char buffer = 0;
	buffer = *buf;
	if(buffer == '1') {
        	gpio_set_value(APLEX_LED_PIN, 1);
		printk("kernel debug : led on\n");
	}
	else if (buffer == '0') {
        	gpio_set_value(APLEX_LED_PIN, 0);
		printk("kernel debug : led off\n");
	} else {
		printk("kernel debug : write '1' on led0, write '0' off led0\n");
	}

	return 0;
}
static int aplex_debug_led0_release (struct inode *inode, struct file *file)
{
	return 0;
}
static long  aplex_debug_led0_ioctl(struct file *filp,unsigned int cmd,	unsigned long arg)
{ 
     int ret ;
	 ret = 0;
	if(cmd == 1) {
		if (arg == '1') {
			gpio_set_value(APLEX_LED_PIN, 1);
		}
		if (arg == '0') {
			gpio_set_value(APLEX_LED_PIN, 0);
		}
	}
	return 0;
}
struct file_operations aplex_dled0_fops = {
	.owner = THIS_MODULE,
	.open =    aplex_debug_led0_open,
	.read =    aplex_debug_led0_read,
	.write =   aplex_debug_led0_write,
	.release = aplex_debug_led0_release,
	.unlocked_ioctl = aplex_debug_led0_ioctl,
};
static struct miscdevice aplex_led_miscdev =
{
         .minor = MISC_DYNAMIC_MINOR,
         .name  = "led_dev",
         .fops  = &aplex_dled0_fops,
};


static int __init  aplex_dled0_init()
{
	aplex_debug_led0_init();
        misc_register(&aplex_led_miscdev);
        return 0;
}
static void  __exit  aplex_dled0_exit()
{
         misc_deregister(&aplex_led_miscdev);
}


module_init(aplex_dled0_init);
module_exit(aplex_dled0_exit);
MODULE_AUTHOR("Danny Zhao");
MODULE_LICENSE("GPL");







