#include <linux/module.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>

#define 	RELAY_R		IMX_GPIO_NR(1, 15)		//GPIO_0
#define 	RELAY_Y		IMX_GPIO_NR(1, 14)		//GPIO_1
#define 	RELAY_G		IMX_GPIO_NR(1, 13)		//GPIO_2
#define 	RELAY_STOP	IMX_GPIO_NR(1, 12)		//GPIO_3

#define 	SENSOROUT1	IMX_GPIO_NR(2, 0)		//SEN_A1+,SEN_A1-	//GPIO_4
#define 	SENSOROUT2	IMX_GPIO_NR(2, 1)		//SEN_A2+,SEN_A2-	//GPIO_5
#define 	SENSOROUT3	IMX_GPIO_NR(2, 2)		//SEN_B1+,SEN_B1-	//GPIO_6
#define 	LAMP1		IMX_GPIO_NR(2, 3)		//GPIO_7
#define 	LAMP2		IMX_GPIO_NR(2, 4)		//GPIO_8
#define 	LAMP3		IMX_GPIO_NR(2, 5)		//GPIO_9
#define 	LAMP4		IMX_GPIO_NR(2, 6)		//GPIO_10
#define 	LAMP5		IMX_GPIO_NR(2, 7)		//GPIO_11

#define 	TP2			IMX_GPIO_NR(3, 16)		//GPIO_12
#define 	EXOUT		IMX_GPIO_NR(3, 17)		//继电器	//GPIO_13
#define 	TP5			IMX_GPIO_NR(3, 18)		//GPIO_14
#define 	TP7			IMX_GPIO_NR(3, 19)		//GPIO_15

#define 	DIS_IN1		IMX_GPIO_NR(3, 23)		//SEN_A+,SEN_A-	//GPIO_16
#define 	DIS_IN2		IMX_GPIO_NR(3, 28)		//TP8,TP9			//GPIO_17
        	
#define 	DIS_IN3		IMX_GPIO_NR(3, 30)		//SEN_B+,SEN_B-	//GPIO_18
#define 	DIS_IN4		IMX_GPIO_NR(3, 31)		//TP10,TP11		//GPIO_19
        
#define 	TP1			IMX_GPIO_NR(1, 4)		//GPIO_20
#define 	TP3			IMX_GPIO_NR(1, 5)		//GPIO_21
        
#define 	TP4			IMX_GPIO_NR(7, 11)		//GPIO_22
#define 	TP6			IMX_GPIO_NR(7, 12)		//GPIO_23

#define 	GPIO_24		IMX_GPIO_NR(5, 30)		//GPIO_24
#define 	GPIO_25		IMX_GPIO_NR(5, 31)		//GPIO_25
#define 	GPIO_26		IMX_GPIO_NR(6, 0)		//GPIO_26
#define 	GPIO_27		IMX_GPIO_NR(6, 1)		//GPIO_27
#define 	GPIO_28		IMX_GPIO_NR(6, 2)		//GPIO_28
#define 	GPIO_29		IMX_GPIO_NR(6, 3)		//GPIO_29
#define 	GPIO_30		IMX_GPIO_NR(6, 4)		//GPIO_30
#define 	GPIO_31		IMX_GPIO_NR(6, 5)		//GPIO_31

#define 	GPIO_32    	IMX_GPIO_NR(4, 8) 
#define 	GPIO_33     IMX_GPIO_NR(4, 9) 

#define 	GPIO_34     IMX_GPIO_NR(4, 14)
#define 	GPIO_35    	IMX_GPIO_NR(4, 15)

#define 	ARRAYSIZE(a)		(sizeof(a)/sizeof(a[0]))

#define		TYPE			'A'
#define		GPIO_INPUT		_IOWR(TYPE, 0, int)
#define		GPIO_OUTPUT		_IOWR(TYPE, 1, int)
#define		GPIO_SET		_IOWR(TYPE, 2, int)
#define		GPIO_GET		_IOWR(TYPE, 3, int)
	
typedef struct gpio_state {
	int gpio_index;
	int value;
}GPIO_T;

//static unsigned int io_table[] = {
//		RELAY_R, RELAY_Y, RELAY_G, RELAY_STOP, SENSOROUT1, SENSOROUT2, SENSOROUT3, 
//		LAMP1, LAMP2, LAMP3, LAMP4, LAMP5, TP2, EXOUT, TP5, TP7,
//		DIS_IN1, DIS_IN2, DIS_IN3, DIS_IN4, TP1, TP3, TP4, TP6, 
//		GPIO_24, GPIO_25, GPIO_26, GPIO_27, GPIO_28, GPIO_29, GPIO_30, GPIO_31, 
//		GPIO_32, GPIO_33, GPIO_34, GPIO_35, 
//};

static int gpio_aplex_open(struct inode *inode, struct file *file)
{
	int i;
	int ret = 0;
//    printk("gpio aplex open.\n");
//	for (i = 0; i < ARRAYSIZE(io_table); i++) {
//		ret = gpio_request(io_table[i], "");
//		if (ret != 0) {
//			printk("gpio request error, get gpio%d fail\n", i);
//			return ret;
//		}
//	}
    return 0;
}

static int gpio_aplex_close(struct inode *inode, struct file *file)
{
	int i;
//    printk("gpio aplex close.\n");
//	for (i = 0; i < ARRAYSIZE(io_table); i++) {
//		gpio_free(io_table[i]);
//	}
    return 0;
}

long gpio_ioctl(struct file * file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	int gpio_index;
	GPIO_T gpio_arg;
	int value;
	
	if (copy_from_user(&gpio_arg, (GPIO_T *)arg, sizeof(GPIO_T)) != 0)
		return -EFAULT;
	
//	gpio_index = io_table[gpio_arg.gpio_index];
	gpio_index = gpio_arg.gpio_index;
	value = gpio_arg.value;

	switch (cmd) {
		case GPIO_INPUT: 
			ret = gpio_request(gpio_index, "");
			if (ret != 0) {
				printk("gpio request gpio%d fail\n", gpio_arg.gpio_index);
				return ret;
			}
			ret = gpio_direction_input(gpio_index);
			gpio_free(gpio_index);
			break;
		case GPIO_OUTPUT: 
			ret = gpio_request(gpio_index, "");
			if (ret != 0) {
				printk("gpio request gpio%d fail\n", gpio_arg.gpio_index);
				return ret;
			}
			ret = gpio_direction_output(gpio_index, value);
			gpio_free(gpio_index);
			break;
		case GPIO_SET: 
			ret = gpio_request(gpio_index, "");
			if (ret != 0) {
				printk("gpio request gpio%d fail\n", gpio_arg.gpio_index);
				return ret;
			}
			gpio_set_value(gpio_index, value != 0 ? 1: 0);
			gpio_free(gpio_index);
			break;
		case GPIO_GET:
			ret = gpio_request(gpio_index, "");
			if (ret != 0) {
				printk("gpio request gpio%d fail\n", gpio_arg.gpio_index);
				return ret;
			}
			ret = gpio_get_value(gpio_index);
			gpio_arg.value = ret;
			gpio_free(gpio_index);
			break;
		default :
			printk("gpio_ioctl cmd failed.\n");
			return -1;
	}

	if (copy_to_user((GPIO_T *)arg, &gpio_arg, sizeof(GPIO_T)) != 0)
		return -EFAULT;

	return 0;
}

struct file_operations gpio_fops = {
    .owner      = THIS_MODULE,
    .open       = gpio_aplex_open,
    .release    = gpio_aplex_close,
	.unlocked_ioctl = gpio_ioctl,
};

struct miscdevice gpio_misc = {
    .minor  = MISC_DYNAMIC_MINOR,
    .name   = "gpio",
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
