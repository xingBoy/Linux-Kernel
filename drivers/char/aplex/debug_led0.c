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

#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))
#define GET_STRUCT_ADDR (ptr,type,member)\
	 ((unsigned long )ptr - (unsigned long)((type*)0->member)) 

static ssize_t aplex_debug_led0_write (struct file *file, char __user *buf, size_t size, loff_t * off);

int aplex_debug_led0_init(void) ;
struct cdev * aplex_dled0;
int array[] = {[2] = 9,[4] = 10};
 int aplex_debug_led0_init(void)
{
	int result;
        /* Allocating GPIOs and setting direction */
    //    result = gpio_request(GPIO_TO_PIN(1,20), "buzz");//usr1
        result = gpio_request(GPIO_TO_PIN(1,21), "Leds");//usr1
        if (result != 0)
                printk("gpio_request(1_20) failed!\n");
     //   result = gpio_direction_output(GPIO_TO_PIN(1,20), 1);
        result = gpio_direction_output(GPIO_TO_PIN(1,21), 1);
        if (result != 0)
                printk("gpio_direction(1_20) failed!\n");	
      //  gpio_set_value(GPIO_TO_PIN(1,20), 0);
        gpio_set_value(GPIO_TO_PIN(1,21), 0);
	return result;

}
static int aplex_debug_led0_open(struct inode *inode, struct file *file)
{
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
        	gpio_set_value(GPIO_TO_PIN(1,21), 1);
	}
	else if (buffer == '0') {
        	gpio_set_value(GPIO_TO_PIN(1,21), 0);
	} else {
		printk("kernel debug : write '1' on led0, write '0' off led0\n");
	}

	return 0;
}
static int aplex_debug_led0_release (struct inode *inode, struct file *file)
{
	return 0;
}
struct file_operations aplex_dled0_fops = {
	.owner = THIS_MODULE,
	.open =    aplex_debug_led0_open,
	.read =    aplex_debug_led0_read,
	.write =   aplex_debug_led0_write,
	.release = aplex_debug_led0_release
};

static int __init  aplex_dled0_init()
{
	if(register_chrdev_region(MKDEV(300,0),1,"aple_led0") < 0)
	{
		return 	-ENOMEM	;
	}
	aplex_dled0 = cdev_alloc();
	if(aplex_dled0 == NULL)
	{
		return -ENOMEM;
	}
	cdev_init(aplex_dled0 , &aplex_dled0_fops);
	cdev_add(aplex_dled0 ,MKDEV(300,0),1);// MKDEV(300,0) 300 0 300 1
	aplex_debug_led0_init();
	return 0;
}
static void  __exit  aplex_dled0_exit()
{
	cdev_del(aplex_dled0 );
	unregister_chrdev_region(MKDEV(300,0),1);
}


module_init(aplex_dled0_init);//这个是一条宏
module_exit(aplex_dled0_exit);//
MODULE_AUTHOR("Danny Zhao");
MODULE_LICENSE("GPL");







