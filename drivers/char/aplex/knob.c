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
#include <linux/input.h>

#define Control_Module_address				0x44E10000
#define CONFIG_XDMA_EVENT_INTR0_offset		0x9B0	
#define CONFIG_GPMC_A10_offset				0x868	
#define CONFIG_GPMC_A11_offset				0x86C	
#define CONFIG_MACSP0_FSX_offset			0x994	

#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))
#define GET_STRUCT_ADDR (ptr,type,member)\
	 ((unsigned long )ptr - (unsigned long)((type*)0->member)) 
#define KNOD_IRQ				GPIO_TO_PIN(0,19)
#define KNOD_IRQ_BUTTON				GPIO_TO_PIN(3,15)
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

struct input_dev * aplex_knob_input;
int aplex_knob_value;
int irq_num_knob_in;
int irq_num_knob_in_button;
int aplex_knob_value_l;
int aplex_knob_value_r;
int aplex_knob_value_read_count;

static unsigned long aplex_knob_new_jiffies;
static unsigned long aplex_knob_old_jiffies;



static irqreturn_t aplex_knob_botton_irq_handle(int irq ,void *dev_id)
{   	
	int tmp;
	tmp = gpio_get_value(KNOD_IRQ_BUTTON);
	printk("botton = %d\n ",tmp);
	if (tmp)
	{
		input_report_key(aplex_knob_input,KEY_OK,1);
		input_sync(aplex_knob_input);
	}
	else
	{
		input_report_key(aplex_knob_input,KEY_OK,0);
		input_sync(aplex_knob_input);
	}
	return IRQ_NONE;
}

static irqreturn_t aplex_knob_irq_handle(int irq ,void *dev_id)
{
	int newa, newb ,i;
	static int timer_flag = 1,olda = 0, oldb = 0,oolda=0,ooldb=0;
	
	aplex_knob_new_jiffies = jiffies;
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

	}
	
	
	if (aplex_knob_value & APLEX_KNOB_RITH )
	{
		aplex_knob_value_l = 0;
		aplex_knob_value_r = 1;
		/*
		if (aplex_knob_value_read_count)
			complete(&aplex_knob_completion);
		aplex_knob_value_read_count = 0;
		*/
		input_report_key(aplex_knob_input,KEY_RIGHT,1);
		input_sync(aplex_knob_input);
		input_report_key(aplex_knob_input,KEY_RIGHT,0);
		input_sync(aplex_knob_input);
	}
	if (aplex_knob_value & APLEX_KNOB_LEFT )
	{
		aplex_knob_value_l = 1;
		aplex_knob_value_r = 0;
		/*
		if (aplex_knob_value_read_count)
			complete(&aplex_knob_completion);
		aplex_knob_value_read_count = 0;
		*/
		input_report_key(aplex_knob_input,KEY_LEFT,1);
		input_sync(aplex_knob_input);
		input_report_key(aplex_knob_input,KEY_LEFT,0);
		input_sync(aplex_knob_input);
	}
	aplex_knob_old_jiffies = jiffies;

	aplex_knob_value = 0; 
   	return IRQ_NONE;

RE:
	aplex_knob_value = 0; 
   	return IRQ_NONE;
}


static int aplex_knob_open(struct input_dev *dev)
{
	return 0;
}
static int aplex_knob_close(struct input_dev *dev)
{
	return 0;
}
static int aplex_knob_init()
{
	int err;
    /* Allocating GPIOs and setting direction */

	void __iomem * base = ioremap(Control_Module_address , 0x1FFF);
	__raw_writel(PINMUX_KNOD_IRQ     ,(base + CONFIG_XDMA_EVENT_INTR0_offset	 ));
	__raw_writel(PINMUX_KNOD_ROTARYA ,(base + CONFIG_GPMC_A10_offset		 ));
	__raw_writel(PINMUX_KNOD_ROTARYB ,(base + CONFIG_GPMC_A11_offset		 ));
	__raw_writel(0x27,(base + CONFIG_MACSP0_FSX_offset));
    err = gpio_request(KNOD_IRQ_BUTTON,"knob_irq_button");
	if ( err ) {
		pr_err("failed to request gpio knob_irq\n");
	//	return err;
	}
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
	err = gpio_direction_input(KNOD_IRQ_BUTTON);
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



	aplex_knob_input = input_allocate_device();
	if (aplex_knob_input == NULL)
		return -ENOMEM;
	set_bit( EV_KEY, aplex_knob_input->evbit);
	set_bit( KEY_LEFT, aplex_knob_input->keybit);
	set_bit( KEY_RIGHT, aplex_knob_input->keybit);
	set_bit( KEY_OK	, aplex_knob_input->keybit);
	aplex_knob_input->name = "input_knob";
	aplex_knob_input->dev.init_name = "input_knob_t";
	aplex_knob_input->open = aplex_knob_open;
	aplex_knob_input->close = aplex_knob_close;
	
	err = input_register_device(aplex_knob_input);
	if (err)
		return err;

	irq_num_knob_in = gpio_to_irq(KNOD_IRQ);
	irq_num_knob_in_button = gpio_to_irq(KNOD_IRQ_BUTTON);
	//IRQF_TRIGGER_FALLING 
	err = request_irq(irq_num_knob_in ,&aplex_knob_irq_handle ,IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING   ,"IRQ_APLEX_KNOB",&irq_num_knob_in );
	if ( err ) {
		pr_err(" cannot register irq %d .\n",irq_num_knob_in);
		return err;
	}
	err = request_irq(irq_num_knob_in_button,&aplex_knob_botton_irq_handle ,IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING   ,"IRQ_APLEX_KNOB_BOTTON",&irq_num_knob_in_button);
	if ( err ) {
		pr_err(" cannot register irq %d .\n",irq_num_knob_in_button);
		return err;
	}
	aplex_knob_value_read_count = 1;
	printk("knob init success\n");
	return 0;

}

static int __init  aplex_knobm_init()
{
	aplex_knob_init();
	return 0;
}
static void  __exit  aplex_knobm_exit()
{
	
	free_irq(irq_num_knob_in,&irq_num_knob_in);
	input_unregister_device(aplex_knob_input);
}


MODULE_LICENSE("GPL");
module_init(aplex_knobm_init);
module_exit(aplex_knobm_exit);








