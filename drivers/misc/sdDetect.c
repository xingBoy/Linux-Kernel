/*sdDetect.c
   ok335x's sdDetect driver. 

   Copyright (c) 2013 forlinx co.ltd
   By DUYAHUI  <duyahui@forlinx.com> 
   omap_hsmmc_cd_handler
*/

#include <linux/init.h>
#include <linux/export.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/types.h>
#include <linux/io.h>
#include <linux/pwm/pwm.h>
#include <linux/fs.h>  
#include <plat/mmc.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>


#define DEBUG 
#if defined(DEBUG)            
#define DPRINTK(fmt,arg...) printk(fmt,##arg); 
#else
#define DPRINTK(fmt,arg...)
#endif

#define SDDETECT        56 

#define DEV_NAME    "sdDetect"

extern struct omap_hsmmc_host *sdDetect;
extern irqreturn_t omap_hsmmc_cd_handler(int irq, void *dev_id);


static int sdDetect_open(struct inode *inode, struct file *filp)
{

    //printk("sdDetect open\n");
    return 0;
}

static int sdDetect_release(struct inode *inode, struct file *filp)
{
    //printk("sdDetect release\n");

    return 0;
}


static long sdDetect_ioctl(struct file *filp,
                        unsigned int cmd, unsigned long arg)
{
    //printk("sdDetect ioctl with cmd:%d,arg:%d\n",cmd,arg);

    switch (cmd) {
    case SDDETECT:
        omap_hsmmc_cd_handler(160, sdDetect);
        break;
    default:
        break;
    }

    return 0;
}

static struct file_operations sdDetect_fops = {
    .owner   = THIS_MODULE,
    .unlocked_ioctl     = sdDetect_ioctl,
    .open    = sdDetect_open,
    .release = sdDetect_release,
};

static struct miscdevice sdDetect_miscdev =
{
     .minor    = MISC_DYNAMIC_MINOR,
     .name    = DEV_NAME,
     .fops    = &sdDetect_fops,
};


static int __init sdDetect_init(void)
{
    //printk(" zengjf sdDetect init.\n");
    misc_register(&sdDetect_miscdev);
    return 0;
}

static void __exit sdDetect_exit(void)
{
   misc_deregister(&sdDetect_miscdev);
}

module_init(sdDetect_init);
module_exit(sdDetect_exit);

MODULE_AUTHOR("duyahui <duyahui@aplex.com>");
MODULE_DESCRIPTION("sdDetect driver");
MODULE_LICENSE("GPL");

