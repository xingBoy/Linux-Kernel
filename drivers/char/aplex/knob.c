#include <linux/init.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/semaphore.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <mach/gpio.h>
#include <plat/mux.h>
#include <linux/gpio.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <asm/uaccess.h>
#include <linux/completion.h>

#define Control_Module_address				0x44E10000
#define CONFIG_XDMA_EVENT_INTR0_offset		0x9B0	
#define CONFIG_GPMC_A10_offset				0x868	
#define CONFIG_GPMC_A11_offset				0x86C	

#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))
#define GET_STRUCT_ADDR (ptr,type,member)\
	 ((unsigned long )ptr - (unsigned long)((type*)0->member)) 
#define KNOD_IRQ				GPIO_TO_PIN(0,19)
#define PINMUX_KNOD_IRQ			0X37	
#define KNOD_ROTARYA			GPIO_TO_PIN(1,26)
#define PINMUX_KNOD_ROTARYA		0x37	
#define KNOD_ROTARYB			GPIO_TO_PIN(1,27)
#define PINMUX_KNOD_ROTARYB		0x37	

#define APLEX_KNOB_LEFT			0X01
#define APLEX_KNOB_RITH			0X02
#define APLEX_KNOB_UPDATA		0X10
#define APLEX_KNOB_DEBOUNCED_TIMERS		10

DECLARE_COMPLETION(aplex_knob_completion);

struct cdev * aplex_knob_cdev;
int aplex_knob_value;
int irq_num_knob_in;
int aplex_knob_value_l;
int aplex_knob_value_r;
int aplex_knob_value_read_count;

static unsigned long aplex_knob_new_jiffies;
static unsigned long aplex_knob_old_jiffies;
static irqreturn_t aplex_knob_irq_handle(int irq ,void *dev_id)
{
	int newa, newb ,i;
	static int timer_flag = 1,olda = 0, oldb = 0,oolda=0,ooldb=0;
	
	aplex_knob_new_jiffies = jiffies;
//	printk("timer = %lu ...\n", aplex_knob_new_jiffies);
	timer_flag = 0;
	if (timer_flag) { 
		if (aplex_knob_new_jiffies - aplex_knob_old_jiffies < 3) {
			goto RE;	
		} else {
			timer_flag = 0;
		}
	}

	newa = gpio_get_value(KNOD_ROTARYA);
	newb = gpio_get_value(KNOD_ROTARYB);

//	printk("newa = %d , newb = %d\n",newa,newb);
	if ((newa == olda) && (newb == oldb))
	{
		goto RE;
	}
	else
	{
		if ((newa == newb) && (newa == 0) && (oolda == ooldb) && (oolda == 1))
		{
			if ((olda == 1) && (oldb == 0))
				aplex_knob_value = APLEX_KNOB_LEFT | APLEX_KNOB_UPDATA	;
			if ((olda == 0) && (oldb == 1))
				aplex_knob_value = APLEX_KNOB_RITH | APLEX_KNOB_UPDATA	;

		}


		oolda = olda;
		ooldb = oldb;
	
		olda = newa;
		oldb = newb;

	//printk("ooa = %d  oob = %d, oa = %d  ob = %d a = %d b = %d\n",oolda,ooldb,olda,oldb,newa, newb);
	}
	
	
//	printk("ooa = %d  oob = %d, oa = %d  ob = %d a = %d b = %d\n",oolda,ooldb,olda,oldb,newa, newb);
#if 0
	if (olda != oldb) {
		
		if ((newa == 0) &&  (newb == 0) ) {
			if (olda == 1) {
				aplex_knob_value = APLEX_KNOB_LEFT | APLEX_KNOB_UPDATA	;
				//printk("===================left===\n");
			}
			else {
				aplex_knob_value = APLEX_KNOB_RITH | APLEX_KNOB_UPDATA	;
				//printk("===================right===\n");
			}
			olda = newa;
			oldb = newb;
			timer_flag = 1;
		}
	}
	else {
		if (newa == newb) {
			olda = newa;
			oldb = newb;
		}
		else {
		   if( (newa != newb) && (olda == 1)) {
				olda = newa;
				oldb = newb;
			}
		}
	}
#endif
	if (aplex_knob_value & APLEX_KNOB_RITH )
	{
		aplex_knob_value_l = 0;
		aplex_knob_value_r = 1;
//		printk("knob_rith -------------------- rith .\n");
		if (aplex_knob_value_read_count)
			complete(&aplex_knob_completion);
		aplex_knob_value_read_count = 0;
	}
	if (aplex_knob_value & APLEX_KNOB_LEFT )
	{
		aplex_knob_value_l = 1;
		aplex_knob_value_r = 0;
//		printk("knob_left -------------------- left .\n");
		if (aplex_knob_value_read_count)
			complete(&aplex_knob_completion);
		aplex_knob_value_read_count = 0;
	}
	aplex_knob_old_jiffies = jiffies;

	aplex_knob_value = 0; 
   	return IRQ_NONE;

RE:
	aplex_knob_value = 0; 
   	return IRQ_NONE;
}
static int aplex_knob_init()
{
	int err;
    /* Allocating GPIOs and setting direction */

	void __iomem * base = ioremap(Control_Module_address , 0x1FFF);
	__raw_writel(PINMUX_KNOD_IRQ     ,(base + CONFIG_XDMA_EVENT_INTR0_offset	 ));
	__raw_writel(PINMUX_KNOD_ROTARYA ,(base + CONFIG_GPMC_A10_offset		 ));
	__raw_writel(PINMUX_KNOD_ROTARYB ,(base + CONFIG_GPMC_A11_offset		 ));
    err = gpio_request(KNOD_IRQ,"knob_irq");
	if ( err ) {
		pr_err("failed to request gpio knob_irq\n");
	//	return err;
	}
    err = gpio_request(KNOD_ROTARYA,"knob_rotarya");
	if ( err ) {
		pr_err("failed to request gpio knob_rotarya\n");
	//	return err;
	}
    err = gpio_request(KNOD_ROTARYB,"knob_rotarab");
	if ( err ) {
		pr_err("failed to request gpio knob_rotarab\n");
	//	return err;
	}

	err = gpio_direction_input(KNOD_IRQ);
	if ( err ) {
		pr_err("failed to set knob_irq ro input module.\n");
		return err;
	}
	err = gpio_direction_input(KNOD_ROTARYA);
	if ( err ) {
		pr_err("failed to set knob_rotarya ro input module.\n");
		return err;
	}
	err = gpio_direction_input(KNOD_ROTARYB);
	if ( err ) {
		pr_err("failed to set knob_rotaryb ro input module.\n");
		return err;
	}

	irq_num_knob_in = gpio_to_irq(KNOD_IRQ);
	//IRQF_TRIGGER_FALLING 
	err = request_irq(irq_num_knob_in ,&aplex_knob_irq_handle ,IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING   ,"IRQ_APLEX_KNOB",&irq_num_knob_in );
	if ( err ) {
		pr_err(" cannot register irq %d .\n",irq_num_knob_in);
		return err;
	}
	aplex_knob_value_read_count = 1;
	printk("knob init success\n");
	return 0;

}
static int aplex_knob_open(struct inode *inode, struct file *file)
{
	//printk("knob_open hand \n");
	aplex_knob_value_read_count = 1;
	return 0;
}
static ssize_t aplex_knob_read (struct file *file, char __user *buf, size_t size, loff_t * off)
{
	char tmp;
	ssize_t re = 0;
	wait_for_completion(&aplex_knob_completion);
	aplex_knob_value_read_count = 1;
	if (aplex_knob_value_r ) {
		tmp = APLEX_KNOB_RITH;
		re = copy_to_user(buf,&tmp,1);
	}
	if (aplex_knob_value_l ) {
		tmp = APLEX_KNOB_LEFT;
		re = copy_to_user(buf,&tmp,1);
	}
	aplex_knob_value_l = 0;
	aplex_knob_value_r = 0;

	aplex_knob_value = 0;

	//printk("knob_read hand \n");
	return re;
}
static ssize_t aplex_knob_write (struct file *file, char __user *buf, size_t size, loff_t * off)
{

	return 0;
}
static int aplex_knob_release (struct inode *inode, struct file *file)
{
	return 0;
}
struct file_operations aplex_knob_fops = {
	.owner = THIS_MODULE,
	.open =    aplex_knob_open,
	.read =    aplex_knob_read,
	.write =   aplex_knob_write,
	.release = aplex_knob_release
};

static int __init  aplex_knobm_init()
{
	if(register_chrdev_region(MKDEV(302,0),1,"aplex_knob") < 0)
	{
		printk("knob register_chardev error.\n");
		return 	-ENOMEM	;
	}
	aplex_knob_cdev = cdev_alloc();
	if(aplex_knob_cdev == NULL)
	{
		printk("cdev_alloc error.\n");
		return -ENOMEM;
	}
	cdev_init(aplex_knob_cdev, &aplex_knob_fops);
	cdev_add(aplex_knob_cdev,MKDEV(302,0),1);
	aplex_knob_init();
	return 0;
}
static void  __exit  aplex_knobm_exit()
{
	
	free_irq(irq_num_knob_in,&irq_num_knob_in);
	cdev_del(aplex_knob_cdev);
	unregister_chrdev_region(MKDEV(302,0),1);
}


MODULE_LICENSE("GPL");
module_init(aplex_knobm_init);
module_exit(aplex_knobm_exit);








