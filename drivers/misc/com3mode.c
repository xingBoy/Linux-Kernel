#include <linux/module.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>

#define SABRESD_COM3_MODE_0	   IMX_GPIO_NR(1, 4)
#define SABRESD_COM3_MODE_1	   IMX_GPIO_NR(1, 5)
#define SABRESD_COM3_TERM	   IMX_GPIO_NR(7, 11)

#define TEST_RS232_CMD_APLEX          66
#define TEST_RS485_CMD_APLEX          67
#define TEST_RS422_CMD_APLEX          68
#define TEST_LOOPBACK_CMD_APLEX       69


static int test_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int test_close(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t test_read(struct file *file,
                        char __user *buf,
                        size_t count,
                        loff_t *pos)
{
    return 0;
}

static long test_ioctl(struct file * file, unsigned int cmd, unsigned long arg) {
/*
	#define TEST_RS232_CMD_APLEX          66
	#define TEST_RS485_CMD_APLEX          67
	#define TEST_RS422_CMD_APLEX          68
	#define TEST_LOOPBACK_CMD_APLEX       69
*/
	switch ( cmd ) {
		case TEST_RS232_CMD_APLEX :
			gpio_direction_output(SABRESD_COM3_MODE_0, 1);
			gpio_direction_output(SABRESD_COM3_MODE_1, 0);
			gpio_direction_output(SABRESD_COM3_TERM, 0);
			//printk("TEST_RS232_CMD_APLEX");
			break;
		case TEST_RS485_CMD_APLEX :
			gpio_direction_output(SABRESD_COM3_MODE_0, 0);
			gpio_direction_output(SABRESD_COM3_MODE_1, 1);
			gpio_direction_output(SABRESD_COM3_TERM, 0);
			//printk("TEST_RS485_CMD_APLEX");
			break;
		case TEST_RS422_CMD_APLEX :
			gpio_direction_output(SABRESD_COM3_MODE_0, 1);
			gpio_direction_output(SABRESD_COM3_MODE_1, 1);
			gpio_direction_output(SABRESD_COM3_TERM, 1);
			//printk("TEST_RS422_CMD_APLEX");
			break;
		case TEST_LOOPBACK_CMD_APLEX :
			gpio_direction_output(SABRESD_COM3_MODE_0, 0);
			gpio_direction_output(SABRESD_COM3_MODE_1, 0);
			gpio_direction_output(SABRESD_COM3_TERM, 0);
			//printk("TEST_LOOPBACK_CMD_APLEX");
			break;
		default:
			printk("RS232/RS485/RS422 control error.\n");
			return -1;
			break;
	}
	return 0;
}

struct file_operations fops = {
    .owner      = THIS_MODULE,
    .open       = test_open,
    .release    = test_close,
    .read       = test_read,
	.unlocked_ioctl = test_ioctl,
};

struct miscdevice misc = {
    .minor  = MISC_DYNAMIC_MINOR,
    .name   = "com3mode",
    .fops   = &fops,
};

int __init test_init(void)
{
    int ret;

	ret = gpio_request(SABRESD_COM3_MODE_0, "com3_mode_0");
	if ( ret ) {
        printk("get com3_mode_0 gpio FAILED!\n");
		return ret;
	}

	ret = gpio_request(SABRESD_COM3_MODE_1, "com3_mode_1");
	if ( ret ) {
		gpio_free(SABRESD_COM3_MODE_0);
        printk("get com3_mode_1 gpio FAILED!\n");
		return ret;
	}

	ret = gpio_request(SABRESD_COM3_TERM, "term");
	if ( ret ) {
		gpio_free(SABRESD_COM3_MODE_0);
		gpio_free(SABRESD_COM3_MODE_1);
        printk("get com3_term gpio FAILED!\n");
		return ret;
	}

	gpio_direction_output(SABRESD_COM3_MODE_0, 1);
	gpio_direction_output(SABRESD_COM3_MODE_1, 0);
	gpio_direction_output(SABRESD_COM3_TERM, 0);

    ret = misc_register(&misc);
    if(ret)
        printk("RS232_misc_register FAILED!\n");

    return ret;
}


void __exit test_exit(void)
{
	gpio_direction_output(SABRESD_COM3_MODE_0, 0);
	gpio_direction_output(SABRESD_COM3_MODE_1, 0);
	gpio_direction_output(SABRESD_COM3_TERM, 0);
	gpio_free(SABRESD_COM3_MODE_0);
	gpio_free(SABRESD_COM3_MODE_1);
	gpio_free(SABRESD_COM3_TERM);

    misc_deregister(&misc);
}


module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");

