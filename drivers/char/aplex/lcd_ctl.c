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
#include <linux/time.h>
#include <linux/delay.h>


#define Control_Module_address				0x44E10000
#define CONFIG_GPMC_A5_offset				0x854	
#define CONFIG_GPMC_A7_offset				0x85c	
#define CONFIG_GPMC_A8_offset				0x860	
#define CONFIG_GPMC_A9_offset				0x864	

#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))
#define GET_STRUCT_ADDR (ptr,type,member)\
	 ((unsigned long )ptr - (unsigned long)((type*)0->member)) 


#define CLD_CTL_CS						21
#define CLD_CTL_CLK					    24
#define CLD_CTL_MOSI					25
struct cdev * my_cdev;
int array[] = {[2] = 9,[4] = 10};
int my_lcd_ctl_open(struct inode *inode, struct file *file);
int my_lcd_ctl_init()
{
	int result;

	void __iomem * base = ioremap(Control_Module_address , 0x1FFF);
	__raw_writel(0x37 ,(base + CONFIG_GPMC_A5_offset  ));
	__raw_writel(0x37 ,(base + CONFIG_GPMC_A8_offset  ));
	__raw_writel(0x37 ,(base + CONFIG_GPMC_A9_offset  ));
        /* Allocating GPIOs and setting direction */
        result = gpio_request(GPIO_TO_PIN(1,CLD_CTL_CS	), "lcd_cs");//usr1
        result = gpio_request(GPIO_TO_PIN(1,CLD_CTL_CLK	), "lcd_clk");//usr1
        result = gpio_request(GPIO_TO_PIN(1,CLD_CTL_MOSI), "lcd_mosi");//usr1
        if (result != 0)
                printk("gpio_request(1_20) failed!\n");
        result = gpio_direction_output(GPIO_TO_PIN(1,CLD_CTL_CS	 ), 1);
        result = gpio_direction_output(GPIO_TO_PIN(1,CLD_CTL_CLK ), 1);
        result = gpio_direction_output(GPIO_TO_PIN(1,CLD_CTL_MOSI), 1);
        if (result != 0)
                printk("gpio_direction(1_20) failed!\n");	
        gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CS	 ), 1);
        gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK ), 1);
        gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 1);
		my_lcd_ctl_open(0,NULL);
	return 0;

}
void lcd_ctl_send_byte(char cmd,unsigned char data) {

    gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK), 0);
	mdelay(1);
	if ( cmd ) 
		gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 1);
	else
		gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 0);
	gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK), 1);
	mdelay(1);

	gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK), 0);
	mdelay(1);
	if ( data & 0x80 ) 
		gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 1);
	else
		gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 0);
	gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK), 1);
	mdelay(1);
	
	gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK), 0);
	mdelay(1);
	if ( data & 0x40 ) 
		gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 1);
	else
		gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 0);
	gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK), 1);
	mdelay(1);

	gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK), 0);
	mdelay(1);
	if ( data & 0x20 ) 
		gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 1);
	else
		gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 0);
	gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK), 1);
	mdelay(1);

	gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK), 0);
	mdelay(1);
	if ( data & 0x10 ) 
		gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 1);
	else
		gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 0);
	gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK), 1);
	mdelay(1);

	gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK), 0);
	mdelay(1);
	if ( data & 0x08 ) 
		gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 1);
	else
		gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 0);
	gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK), 1);
	mdelay(1);

	gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK), 0);
	mdelay(1);
	if ( data & 0x04 ) 
		gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 1);
	else
		gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 0);
	gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK), 1);
	mdelay(1);

	gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK), 0);
	mdelay(1);
	if ( data & 0x02 ) 
		gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 1);
	else
		gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 0);
	gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK), 1);
	mdelay(1);

	gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK), 0);
	mdelay(1);
	if ( data & 0x01 ) 
		gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 1);
	else
		gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_MOSI), 0);
	gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CLK), 1);
}
int my_lcd_ctl_open(struct inode *inode, struct file *file)
{
    gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CS	 ), 0);
	mdelay(1);
	lcd_ctl_send_byte(0,0x29);
	lcd_ctl_send_byte(0,0x11);
	lcd_ctl_send_byte(0,0x36);
	lcd_ctl_send_byte(1,0x00);

	lcd_ctl_send_byte(0,0x3a);
	lcd_ctl_send_byte(1,0x70);

	lcd_ctl_send_byte(0,0xb0);
	lcd_ctl_send_byte(1,0x00);

	lcd_ctl_send_byte(0,0xb8);
	lcd_ctl_send_byte(1,0x01);

	lcd_ctl_send_byte(0,0xb9);
	lcd_ctl_send_byte(1,0x00);
	lcd_ctl_send_byte(1,0xff);

	lcd_ctl_send_byte(0,0xb0);
	lcd_ctl_send_byte(1,0x03);
	
    gpio_set_value(GPIO_TO_PIN(1,CLD_CTL_CS	 ), 1);
	printk ("my_char open\n");
	return 0;
}
ssize_t my_lcd_ctl_read (struct file *file, char __user *buf, size_t size, loff_t * off)//off 我们不用操作 内核维护
{
	printk("my_read \n");
	return 0;
}
ssize_t my_lcd_ctl_write (struct file *file, char __user *buf, size_t size, loff_t * off)
{
	char buffer = 0;
	buffer = *buf;
	printk("my_write \n");

	return 0;
}
int my_lcd_ctl_release (struct inode *inode, struct file *file)
{
	printk("my_release \n");
	return 0;
}
struct file_operations fops = {
	.owner = THIS_MODULE,
	.open =     my_lcd_ctl_open,
	.read =     my_lcd_ctl_read,
	.write =    my_lcd_ctl_write,
	.release =  my_lcd_ctl_release
};

static int __init  hello_lcd_ctl_init()
{
	if(register_chrdev_region(MKDEV(309,0),1,"lcd_ctl") < 0)
	{
		return 	-ENOMEM	;
	}
	my_cdev = cdev_alloc();
	if(my_cdev == NULL)
	{
		printk("cdev_alloc error.\n");
		return -ENOMEM;
	}
	cdev_init(my_cdev, &fops);
	cdev_add(my_cdev,MKDEV(309,0),1);// MKDEV(300,0) 300 0 300 1
	my_lcd_ctl_init();
	return 0;
}
static void  __exit  hello_lcd_ctl_exit()
{
	cdev_del(my_cdev);
	unregister_chrdev_region(MKDEV(309,0),1);
}


MODULE_LICENSE("GPL");
module_init(hello_lcd_ctl_init);//这个是一条宏
module_exit(hello_lcd_ctl_exit);//








