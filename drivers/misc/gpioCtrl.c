#include <linux/module.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>

#define SABRESD_VO_VOIP_SPK_PIN		IMX_GPIO_NR(1, 4)
#define SABRESD_AMP_PIN				IMX_GPIO_NR(1, 5)
#define SABRESD_SHUTDOWN_PIN		IMX_GPIO_NR(1, 19)
// MX6DL_PAD_EIM_D20__GPIO_3_20,
#define SABRESD_DETECT_850_PIN		IMX_GPIO_NR(3, 30)
#define SABRESD_CHIP_ENABLE         IMX_GPIO_NR(5, 28)
#define SABRESD_GPS_POWER_ENABLE    IMX_GPIO_NR(2, 2)
#define SABRESD_WL_RESET            IMX_GPIO_NR(2, 5)

#define VO_VOIP_SPK_HEIGHT          66
#define VO_VOIP_SPK_LOW				67
#define AMP_HEIGHT          		68
#define AMP_LOW						69
#define SHUTDOWN_HEIGHT          	70
#define SHUTDOWN_LOW				71
#define DETECT_850                  72

//#define GPIO_CTRL_DEBUG
#ifdef GPIO_CTRL_DEBUG
	#define mDebug(format, ...) printk("File:%s, Function:%s, Line:%d  "format, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#else
	#define mDebug(format, ...)
#endif

static int gpioCtrl_open(struct inode *inode, struct file *file)
{
    mDebug("Dev open.\n");

    return 0;
}

static int gpioCtrl_close(struct inode *inode, struct file *file)
{
    mDebug("Dev close.\n");

    return 0;
}

static ssize_t gpioCtrl_read(struct file *file,
                        char __user *buf,
                        size_t count,
                        loff_t *pos)
{
    mDebug("Read data.\n");

    return 0;
}

static long gpioCtrl_ioctl(struct file * file, unsigned int cmd, unsigned long arg) {

	int ret = 0;

	switch ( cmd ) {
	case VO_VOIP_SPK_HEIGHT :
		gpio_set_value(SABRESD_VO_VOIP_SPK_PIN, 1);
        mDebug("VO_VOIP_SPK_HEIGHT.\n");
		break;
	case VO_VOIP_SPK_LOW :
		gpio_set_value(SABRESD_VO_VOIP_SPK_PIN, 0);
        mDebug("VO_VOIP_SPK_LOW.\n");
		break;
	case AMP_HEIGHT :
		gpio_set_value(SABRESD_AMP_PIN, 1);
        mDebug("AMP_HEIGHT.\n");
		break;
	case AMP_LOW :
		gpio_set_value(SABRESD_AMP_PIN, 0);
        mDebug("AMP_LOW.\n");
		break;
	case SHUTDOWN_HEIGHT :
		gpio_set_value(SABRESD_SHUTDOWN_PIN, 1);
        mDebug("SHUTDOWN_HEIGHT.\n");
		break;
	case SHUTDOWN_LOW :
		gpio_set_value(SABRESD_SHUTDOWN_PIN, 0);
        mDebug("SHUTDOWN_LOW.\n");
		break;
	case DETECT_850 :
        ret = gpio_get_value(SABRESD_DETECT_850_PIN);
        (*(int *)arg) = ret;
        //mDebug("DETECT_850 ret = %d.\n", ret);

        ret = 0;
		break;
	default :
		mDebug("gpioCtrl control error.\n");
		ret =  -1;
		break;
	}
    return ret;
}


struct file_operations fops = {
    .owner      = THIS_MODULE,
    .open       = gpioCtrl_open,
    .release    = gpioCtrl_close,
    .read       = gpioCtrl_read,
	.unlocked_ioctl = gpioCtrl_ioctl,
};

struct miscdevice misc = {
    .minor  = MISC_DYNAMIC_MINOR,
    .name   = "gpioCtrl",
    .fops   = &fops,
};

int __init gpioCtrl_init(void)
{
    int ret;

	ret = gpio_request(SABRESD_VO_VOIP_SPK_PIN, "SABRESD_VO_VOIP_SPK_PIN");
	if ( ret ) {
        mDebug("get SABRESD_VO_VOIP_SPK_PIN gpio FAILED!\n");
		return ret;
	}

	ret = gpio_request(SABRESD_AMP_PIN, "SABRESD_AMP_PIN");
	if ( ret ) {
        mDebug("get SABRESD_AMP_PIN gpio FAILED!\n");
		goto fail1;
	}

	ret = gpio_request(SABRESD_SHUTDOWN_PIN, "SABRESD_SHUTDOWN_PIN");
	if ( ret ) {
        mDebug("get SABRESD_SHUTDOWN_PIN gpio FAILED!\n");
        goto fail2;
	}

	ret = gpio_request(SABRESD_DETECT_850_PIN, "SABRESD_DETECT_850_PIN");
	if ( ret ) {
        mDebug("get SABRESD_DETECT_850 gpio FAILED!\n");
        goto fail3;
	}

	ret = gpio_request(SABRESD_CHIP_ENABLE, "SABRESD_CHIP_ENABLE");
	if ( ret ) {
        mDebug("get SABRESD_CHIP_ENABLE gpio FAILED!\n");
        goto fail4;
	}
	ret = gpio_request(SABRESD_GPS_POWER_ENABLE, "SABRESD_GPS_POWER_ENABLE");
	if ( ret ) {
        mDebug("get SABRESD_GPS_POWER_ENABLE gpio FAILED!\n");
        goto fail5;
	}

	gpio_direction_output(SABRESD_VO_VOIP_SPK_PIN, 0);
	gpio_direction_output(SABRESD_AMP_PIN, 1);
	gpio_direction_output(SABRESD_SHUTDOWN_PIN, 1);
    gpio_direction_input(SABRESD_DETECT_850_PIN);
    gpio_direction_output(SABRESD_CHIP_ENABLE, 1);
    gpio_direction_output(SABRESD_GPS_POWER_ENABLE, 1);
    

    //msleep(10);
    //gpio_set_value(SABRESD_CHIP_ENABLE, 0);
    //msleep(10);
    //gpio_set_value(SABRESD_CHIP_ENABLE, 1);

    // wifi reset
    ret = gpio_request(SABRESD_WL_RESET, "SABRESD_WL_RESET");
    if ( ret ) {
        printk("get SABRESD_WL_RESET gpio FAILED!\n");
        return ret;
    }
    msleep(10);
    gpio_direction_output(SABRESD_WL_RESET, 1);
    msleep(10);
    gpio_set_value(SABRESD_WL_RESET, 0);
    msleep(10);
    gpio_set_value(SABRESD_WL_RESET, 1);
    gpio_free(SABRESD_WL_RESET);


    //msleep(100);
	//gpio_direction_output(SABRESD_SHUTDOWN_PIN, 1);
    //msleep(100);
	//gpio_direction_output(SABRESD_SHUTDOWN_PIN, 0);
    //msleep(100);
	//gpio_direction_output(SABRESD_SHUTDOWN_PIN, 1);

    /*
    if ( gpio_get_value(SABRESD_DETECT_850_PIN) == 1 ) {
		gpio_set_value(SABRESD_SHUTDOWN_PIN, 1);
        msleep(100);
		gpio_set_value(SABRESD_SHUTDOWN_PIN, 0);
        msleep(100);
		gpio_set_value(SABRESD_SHUTDOWN_PIN, 1);
    }
    */

    ret = misc_register(&misc);
    if(ret) {
        mDebug("gpioCtrl_misc_register FAILED!\n");
        goto fail6;
	}

    mDebug("gpioCtrl_misc_register over!\n");
    return ret;

fail6:
    gpio_free(SABRESD_GPS_POWER_ENABLE);

fail5:
    gpio_free(SABRESD_CHIP_ENABLE);
fail4:
    gpio_free(SABRESD_DETECT_850_PIN);
fail3:
    gpio_free(SABRESD_SHUTDOWN_PIN);
fail2:
    gpio_free(SABRESD_AMP_PIN);
fail1:
    gpio_free(SABRESD_VO_VOIP_SPK_PIN);
    
    return ret;
}


void __exit gpioCtrl_exit(void)
{
	gpio_set_value(SABRESD_VO_VOIP_SPK_PIN, 0);
	gpio_set_value(SABRESD_AMP_PIN, 0);
	gpio_set_value(SABRESD_SHUTDOWN_PIN, 0);

	gpio_free(SABRESD_VO_VOIP_SPK_PIN);
	gpio_free(SABRESD_AMP_PIN);
	gpio_free(SABRESD_SHUTDOWN_PIN);
	gpio_free(SABRESD_DETECT_850_PIN);

    misc_deregister(&misc);
}


module_init(gpioCtrl_init);
module_exit(gpioCtrl_exit);

MODULE_LICENSE("GPL");

