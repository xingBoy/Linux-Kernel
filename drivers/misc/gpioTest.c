#include <linux/module.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>

#define GPIO_TEST				IMX_GPIO_NR(1, 19)

#define GPIO_TEST_CMD			71


static int gpio_test_open(struct inode *inode, struct file *file)
{
    printk("gpio aplex open.\n");
	int ret = 0;
	ret = gpio_request(GPIO_TEST, "sabresd_aplex_test");
   	if ( ret ) {
   	    printk("get sabresd_aplex_test gpio FAILED! in %s\n", __func__);
   	    return ret;
   	}
   	gpio_direction_output(GPIO_TEST, 0);
   	msleep(100);
   	gpio_set_value(GPIO_TEST, 1);
   	gpio_free(GPIO_TEST);

	return ret;
    return 0;
}

static int gpio_test_close(struct inode *inode, struct file *file)
{
    return 0;
}

long gpio_test_ioctl(struct file * file, unsigned int cmd, unsigned long arg)
{
}

struct file_operations gpio_test_fops = {
    .owner      = THIS_MODULE,
    .open       = gpio_test_open,
    .release    = gpio_test_close,
    //.read       = gpio_aplex_read,
	.unlocked_ioctl = gpio_test_ioctl,
};

struct miscdevice gpio_test_misc = {
    .minor  = MISC_DYNAMIC_MINOR,
    .name   = "gpio_test",
    .fops   = &gpio_test_fops,
};

int __init gpio_test_init(void)
{
    int ret;

    ret = misc_register(&gpio_test_misc);
    if(ret)
        printk("gpio_misc_register FAILED!\n");
	else 
		printk("gpio aplex has register\n");

    return ret;
}

void __exit gpio_test_exit(void)
{
    misc_deregister(&gpio_test_misc);
}

module_init(gpio_test_init);
module_exit(gpio_test_exit);

MODULE_LICENSE("GPL");

