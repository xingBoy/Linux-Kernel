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

#define Control_Module_address                0x44E10000

//#define CONFIG_GPMC_CLK_offset              0x88c
#define CONFIG_GPMC_CLK_offset                0x960


#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))
#define GET_STRUCT_ADDR (ptr,type,member)\
     ((unsigned long )ptr - (unsigned long)((type*)0->member)) 

//#define BUZZER_PIN     GPIO_TO_PIN(2,1)
#define BUZZER_PIN     GPIO_TO_PIN(0,6)


struct cdev * aplex_buzz;
static int aplex_buzz_init(void)
{
    int result;

    void __iomem * base = ioremap(Control_Module_address , 0x1FFF);
    __raw_writel(0x37,(base +CONFIG_GPMC_CLK_offset ));
        /* Allocating GPIOs and setting direction */
        result = gpio_request(BUZZER_PIN, "buzzer");//usr1
        if (result != 0) {
                printk("gpio_request(2_1) failed!\n"); return result; }
        result = gpio_direction_output(BUZZER_PIN, 1);
        if (result != 0) {
                printk("gpio_direction(2_1) failed!\n"); return result; }    
        gpio_set_value(BUZZER_PIN, 0);
    return result;

}
static int aplex_buzz_open(struct inode *inode, struct file *file)
{
    return 0;
}
static ssize_t aplex_buzz_read (struct file *file, char __user *buf, size_t size, loff_t * off)
{
    return 0;
}
static int aplex_buzz_ioctl(struct file *filp,unsigned int cmd,    unsigned long arg)
{ 
    if(cmd == IO_VAULE_H ) {
        printk(" buzz iotcl IO_VALUE_H.\n");
            gpio_set_value(BUZZER_PIN,1);
    }
    if(cmd == IO_VAULE_L ) {
        printk(" buzz iotcl IO_VALUE_L.\n");
            gpio_set_value(BUZZER_PIN,0);
    }

    return 0;      
}
static ssize_t aplex_buzz_write (struct file *file, char __user *buf, size_t size, loff_t * off)
{
    return 0;
}
static int aplex_buzz_release (struct inode *inode, struct file *file)
{
    return 0;
}
struct file_operations buzz_fops = {
    .owner = THIS_MODULE,
    .open =    aplex_buzz_open,
    .read =    aplex_buzz_read,
    .write =   aplex_buzz_write,
    .release = aplex_buzz_release,
    .unlocked_ioctl= aplex_buzz_ioctl

};

static int __init  aplex_buzzm_init()
{
    if(register_chrdev_region(MKDEV(301,0),1,"aple_buzz") < 0)
    {
        return     -ENOMEM    ;
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
static void  __exit  aplex_buzzm_exit()
{
    cdev_del(aplex_buzz);
    unregister_chrdev_region(MKDEV(301,0),1);
}


module_init(aplex_buzzm_init);
module_exit(aplex_buzzm_exit);
MODULE_AUTHOR("Danny Zhao");
MODULE_LICENSE("GPL");
