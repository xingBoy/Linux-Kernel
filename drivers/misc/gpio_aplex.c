#include <linux/module.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>

/**
	//GPIO write by zengjf
	MX6DL_PAD_SD2_DAT0__GPIO_1_15,	//GPIO In
	MX6DL_PAD_SD2_DAT1__GPIO_1_14,
	MX6DL_PAD_SD2_DAT2__GPIO_1_13,
	MX6DL_PAD_SD2_DAT3__GPIO_1_12,
	MX6DL_PAD_NANDF_D4__GPIO_2_4,	//GPIO Out
	MX6DL_PAD_NANDF_D5__GPIO_2_5,
	MX6DL_PAD_NANDF_D6__GPIO_2_6,
	MX6DL_PAD_NANDF_D7__GPIO_2_7
 */
#define GPIO_IN0_APLEX				IMX_GPIO_NR(1, 15)
#define GPIO_IN1_APLEX				IMX_GPIO_NR(1, 14)
#define GPIO_IN2_APLEX				IMX_GPIO_NR(1, 13)
#define GPIO_IN3_APLEX				IMX_GPIO_NR(1, 12)
#define GPIO_OUT0_APLEX				IMX_GPIO_NR(2, 4)
#define GPIO_OUT1_APLEX				IMX_GPIO_NR(2, 5)
#define GPIO_OUT2_APLEX				IMX_GPIO_NR(2, 6)
#define GPIO_OUT3_APLEX				IMX_GPIO_NR(2, 7)

#define GPIO_IN0_CMD_APLEX			66
#define GPIO_IN1_CMD_APLEX			67
#define GPIO_IN2_CMD_APLEX			68
#define GPIO_IN3_CMD_APLEX			69
#define GPIO_OUT0_CMD_APLEX			70
#define GPIO_OUT1_CMD_APLEX			71
#define GPIO_OUT2_CMD_APLEX			72
#define GPIO_OUT3_CMD_APLEX			73


static int gpio_aplex_open(struct inode *inode, struct file *file)
{
    //printk("gpio aplex open.\n")
	int ret;
	ret = gpio_request(GPIO_IN0_APLEX, "gpio_in0_aplex");
	if ( ret ) {
        printk("get gpio_in0_aplex gpio FAILED!\n");
		return ret;
	}

	ret = gpio_request(GPIO_IN1_APLEX, "gpio_in1_aplex");
	if ( ret ) {
        printk("get gpio_in1_aplex gpio FAILED!\n");
		return ret;
	}

	ret = gpio_request(GPIO_IN2_APLEX, "gpio_in2_aplex");
	if ( ret ) {
        printk("get gpio_in2_aplex gpio FAILED!\n");
		return ret;
	}

	ret = gpio_request(GPIO_IN3_APLEX, "gpio_in3_aplex");
	if ( ret ) {
        printk("get gpio_in3_aplex gpio FAILED!\n");
		return ret;
	}

	ret = gpio_request(GPIO_OUT0_APLEX, "gpio_out0_aplex");
	if ( ret ) {
        printk("get gpio_out0_aplex gpio FAILED!\n");
		return ret;
	}

	ret = gpio_request(GPIO_OUT1_APLEX, "gpio_out1_aplex");
	if ( ret ) {
        printk("get gpio_out1_aplex gpio FAILED!\n");
		return ret;
	}

	ret = gpio_request(GPIO_OUT2_APLEX, "gpio_out2_aplex");
	if ( ret ) {
        printk("get gpio_out2_aplex gpio FAILED!\n");
		return ret;
	}

	ret = gpio_request(GPIO_OUT3_APLEX, "gpio_out3_aplex");
	if ( ret ) {
        printk("get gpio_out3_aplex gpio FAILED!\n");
		return ret;
	}

	gpio_direction_input(GPIO_IN0_APLEX);
	gpio_direction_input(GPIO_IN1_APLEX);
	gpio_direction_input(GPIO_IN2_APLEX);
	gpio_direction_input(GPIO_IN3_APLEX);
	//set the default value
	gpio_direction_output(GPIO_OUT0_APLEX, 0);
	gpio_direction_output(GPIO_OUT1_APLEX, 0);
	gpio_direction_output(GPIO_OUT2_APLEX, 0);
	gpio_direction_output(GPIO_OUT3_APLEX, 0);

	printk("register gpio input output over.\n");

    return 0;
}

static int gpio_aplex_close(struct inode *inode, struct file *file)
{
    //printk("Dev close.\n");
	//set back for default value
	gpio_direction_output(GPIO_IN0_APLEX, 0);
	gpio_direction_output(GPIO_IN1_APLEX, 0);
	gpio_direction_output(GPIO_IN2_APLEX, 0);
	gpio_direction_output(GPIO_IN3_APLEX, 0);
	gpio_direction_output(GPIO_OUT0_APLEX, 0);
	gpio_direction_output(GPIO_OUT1_APLEX, 0);
	gpio_direction_output(GPIO_OUT2_APLEX, 0);
	gpio_direction_output(GPIO_OUT3_APLEX, 0);
	gpio_free(GPIO_IN0_APLEX);
	gpio_free(GPIO_IN1_APLEX);
	gpio_free(GPIO_IN2_APLEX);
	gpio_free(GPIO_IN3_APLEX);
	gpio_free(GPIO_OUT0_APLEX);
	gpio_free(GPIO_OUT1_APLEX);
	gpio_free(GPIO_OUT2_APLEX);
	gpio_free(GPIO_OUT3_APLEX);

    return 0;
}

long gpio_ioctl(struct file * file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	
	switch (cmd) {
		case GPIO_IN0_CMD_APLEX  : 
			ret = gpio_get_value(GPIO_IN0_APLEX);
			//printk("test gpio in0 ret %d\n", ret);
			break;
		case GPIO_IN1_CMD_APLEX  : 
			ret = gpio_get_value(GPIO_IN1_APLEX);
			//printk("test gpio in1 ret %d\n", ret);
			break;
		case GPIO_IN2_CMD_APLEX  : 
			ret = gpio_get_value(GPIO_IN2_APLEX);
			//printk("test gpio in2 ret %d\n", ret);
			break;
		case GPIO_IN3_CMD_APLEX  : 
			ret = gpio_get_value(GPIO_IN3_APLEX);
			//printk("test gpio in3 ret %d\n", ret);
			break;
		case GPIO_OUT0_CMD_APLEX :
			gpio_set_value(GPIO_OUT0_APLEX, arg != 0 ? 1 : 0);
			//printk("test gpio out0 arg %d\n", (int)arg);
			break;
		case GPIO_OUT1_CMD_APLEX :
			gpio_set_value(GPIO_OUT1_APLEX, arg != 0 ? 1 : 0);
			//printk("test gpio out1 arg %d\n", (int)arg);
			break;
		case GPIO_OUT2_CMD_APLEX :
			gpio_set_value(GPIO_OUT2_APLEX, arg != 0 ? 1 : 0);
			//printk("test gpio out2 arg %d\n", (int)arg);
			break;
		case GPIO_OUT3_CMD_APLEX :
			gpio_set_value(GPIO_OUT3_APLEX, arg != 0 ? 1 : 0);
			//printk("test gpio out3 arg %d\n", (int)arg);
			break;
		default :
			printk("gpio_ioctl cmd failed.\n");

			return -1;
	}

	return ret;
}

struct file_operations gpio_fops = {
    .owner      = THIS_MODULE,
    .open       = gpio_aplex_open,
    .release    = gpio_aplex_close,
    //.read       = gpio_aplex_read,
	.unlocked_ioctl = gpio_ioctl,
};

struct miscdevice gpio_misc = {
    .minor  = MISC_DYNAMIC_MINOR,
    .name   = "gpio_aplex",
    .fops   = &gpio_fops,
};

int __init gpio_aplex_init(void)
{
    int ret;

    ret = misc_register(&gpio_misc);
    if(ret)
        printk("gpio_misc_register FAILED!\n");
	else 
		printk("gpio aplex has register\n");

    return ret;
}

void __exit gpio_aplex_exit(void)
{
    misc_deregister(&gpio_misc);

}

module_init(gpio_aplex_init);
module_exit(gpio_aplex_exit);

MODULE_LICENSE("GPL");

