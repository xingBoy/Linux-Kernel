#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>


#define MX6CHECK_CMD_APLEX			73


static int mx6check_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int mx6check_close(struct inode *inode, struct file *file)
{
    return 0;
}

long mx6check_ioctl(struct file * file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	
	switch (cmd) {
		case MX6CHECK_CMD_APLEX : 
			ret = 7112;
			//printk("test mx6check ret = %d\n", ret);
			break;
		default :
			printk("mx6check_ioctl cmd failed.\n");
			return -1;
	}

	return ret;
}

struct file_operations mx6check_fops = {
    .owner      = THIS_MODULE,
    .open       = mx6check_open,
    .release    = mx6check_close,
    //.read       = mx6check_read,
	.unlocked_ioctl = mx6check_ioctl,
};

struct miscdevice mx6check_misc = {
    .minor  = MISC_DYNAMIC_MINOR,
    .name   = "mx6check",
    .fops   = &mx6check_fops,
};

int __init mx6check_init(void)
{
    int ret;

    ret = misc_register(&mx6check_misc);
    if(ret)
        printk("mx6check register FAILED!\n");
	else 
		printk("mx6check has register\n");

    return ret;
}

void __exit mx6check_exit(void)
{
    misc_deregister(&mx6check_misc);
}

module_init(mx6check_init);
module_exit(mx6check_exit);

MODULE_LICENSE("GPL");

