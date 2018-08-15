#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/of.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <linux/signal.h>
#include <linux/pinctrl/consumer.h>
MODULE_LICENSE("GPL");


#define APLEX_DEF_STATE    "default"
#define APLEX_GPIO_STATE   "gpio"

struct pinctrl       *aplex_pinctrl;
struct pinctrl_state *aplex_default;
struct pinctrl_state *aplex_gpio;
bool aplex_default_flags = false;


static int aplex_pinctrl_init(struct platform_device *pdata)
{
    aplex_pinctrl = devm_pinctrl_get(&pdata->dev);
    if (IS_ERR_OR_NULL(aplex_pinctrl)) {
        printk("Failed to get pin ctrl\n");
        return PTR_ERR(aplex_pinctrl);
    }
    aplex_default = pinctrl_lookup_state(aplex_pinctrl,
                                         APLEX_DEF_STATE);
    if (IS_ERR_OR_NULL(aplex_default)) {
        printk("Failed to lookup pinctrl default state\n");
        return PTR_ERR(aplex_default);
    }   

    aplex_gpio = pinctrl_lookup_state(aplex_pinctrl,
                                      APLEX_GPIO_STATE );
    if (IS_ERR_OR_NULL(aplex_gpio)) {
        printk("Failed to lookup pinctrl gpio state\n");
        //return PTR_ERR(aplex_gpio);
    }   

    //printk("get pin state  %s,---ok\n", __func__);
    return 0;

}   


static ssize_t aplex_pin_show(struct device* cd,struct device_attribute *attr, char* buf) 
{   
    ssize_t ret = 0;
    if(!aplex_default_flags)
    {
        sprintf(buf, "current pin state:'gpio'\nplease input  'default' to change the configuration of the current pin\n");
    }else
    {
        sprintf(buf, "current pin state:'default'\nplease input  'gpio' to change the configuration of the current pin\n");
    }   
    ret = strlen(buf) + 1;
    return ret;
}       


static ssize_t aplex_pin_store(struct device* cd, struct device_attribute *attr, const char* buf, size_t len) 
{ 
    int result = 0;
    if(strncmp("default",buf,7) == 0)
    {
        if(!aplex_default_flags){
            if (IS_ERR_OR_NULL(aplex_default)) {
                printk("Failed to lookup pinctrl default state\n");
                return PTR_ERR(aplex_default);
            } 
            result = pinctrl_select_state(aplex_pinctrl, aplex_default);
            if (result) 
            {
                printk("%s: Can not set %s pins\n",
                       __func__, "default");
            }else
            {
                aplex_default_flags = true;
            }

        }
    }else if(strncmp("gpio",buf,4) == 0)
    {
        if(aplex_default_flags)
        {
            if (IS_ERR_OR_NULL(aplex_gpio)) {
                printk("Failed to lookup pinctrl default state\n");
                return PTR_ERR(aplex_gpio);
            } 
            result = pinctrl_select_state(aplex_pinctrl, aplex_gpio);
            if(result)
            {
                printk("%s: Can not set %s pins\n",
                       __func__, "gpio");
            }else
            {
                aplex_default_flags = false;
            }
        }
    }else
    {
        return -EINVAL;
    }

    return len; 
}

static DEVICE_ATTR(aplex_pin,S_IRUGO | S_IWUSR, aplex_pin_show, aplex_pin_store);
int am335x_pin_config_probe(struct platform_device * pdevice)
{   
    int ret = 0;
    ret = aplex_pinctrl_init(pdevice);
    if(ret)
    {
        return -EPERM;
    }
    ret = pinctrl_select_state(aplex_pinctrl, aplex_default);
    if (ret) {
        printk("%s: Can not set %s pins\n",__func__, "default");
        return ret;
    }

    aplex_default_flags = true;
    if((ret = device_create_file(&pdevice->dev, &dev_attr_aplex_pin))!=0) 
    {
        printk("Unable to createsysfs entry: '%s'\n",dev_attr_aplex_pin.attr.name);
        return ret;

    }

    return ret;

}

int am335x_pin_config_remove(struct platform_device * pdevice)
{
    device_remove_file(&pdevice->dev, &dev_attr_aplex_pin);
    printk("am335x-pin-comfig%s,%d\n",__func__, __LINE__);
    return 0;
}

struct of_device_id  idts[] = {
    {.compatible = "aplex_pin_config"},
    {/*Nothing to done.*/},
};

struct platform_driver  aplex_pdriver = {
    .probe = am335x_pin_config_probe,
    .remove = am335x_pin_config_remove,
    .driver = {
        .name = "aplex_pin_config",
        .of_match_table = idts,
    },
};

module_platform_driver(aplex_pdriver);
