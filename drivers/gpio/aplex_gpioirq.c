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

MODULE_LICENSE("GPL");

#define DEVFSILE_NANE  "witch"
dev_t devno;
int major = 0;
int minor = 0;
int count = 1;
u32 gpio_nu ;
u32 irq_flags ;
static int flags = 0;
struct cdev    *pdev;
struct device  *pdevice;
struct class   *pclass;

struct device_node  *np;
struct property * proper;
struct fasync_struct *fasync_queue;
struct work_struct  workqueue;
int demo_open(struct inode *inode, struct file *filp)
{
    //printk("%s,%d\n",__func__, __LINE__);

    if(!flags)
    {
        flags = 1;
    }else
    {
        return -EBUSY;
    }
    return 0;
}

int demo_release(struct inode *inode, struct file *filp)
{

    flags = 0;
    return 0;
}
int demo_fasync(int fd, struct file * filp, int mode)
{
    printk("%s,%d\n",__func__, __LINE__);
    return fasync_helper(fd, filp, mode, &fasync_queue);
}

void workqueue_func(struct work_struct *work)
{
    //msleep(10000);
    if(fasync_queue)
    {
        kill_fasync(&fasync_queue, SIGIO, POLL_IN);
    }

}
struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = demo_open,
    .release = demo_release,
    .fasync = demo_fasync,
};



int dev_file_create(void)
{
    int ret =  0;
    ret = alloc_chrdev_region(&devno,minor, count, DEVFSILE_NANE);//创建设备号并且注册设备号
    if(ret)
    {
        printk("Failed to alloc_chrdev_region.\n");
        return ret;
    }

    printk("devno:%d major:%d  minor:%d \n", devno,MAJOR(devno), MINOR(devno));

    pdev = cdev_alloc();//创建设备描述结构体（创建一个设备类型）
    if(pdev==NULL)
    {
        printk("Failed to cdev_alloc.\n");
        goto err1;
    }

    cdev_init(pdev, &fops);//绑定设备操作函数集

    ret = cdev_add(pdev, devno, count);//把设备加入到内核设备队列
    if(ret)
    {
        printk("Failed to cdev_add.\n");
        goto err2;
    }
    pclass = class_create(THIS_MODULE, "gpio_to_irq");//正确返回类结构体地址，错误返回一个数（不是地址）
    if(IS_ERR(pclass))
    {
        ret = PTR_ERR(pclass);
        goto err3;
    }

    pdevice = device_create(pclass , NULL, devno, NULL, "switch");//创建设备节点并绑定设备
    if(pdevice == NULL)
    {
        printk("Failed to device_create.\n");
        goto err4;
    }

    return 0;
err4:
    class_destroy(pclass);//销毁类结构体
err3:
    cdev_del(pdev);//从内核循环队列中删除设备结构体
err2:
    kfree(pdev);//销毁设备结构体
err1:
    unregister_chrdev_region(devno, count);//删除设备号
    return ret;
}


static irqreturn_t reset_ip_handler(int irqno, void * dev_id)
{
    //printk("reset_ip ok!\n");

    schedule_work(&workqueue);

    return IRQ_HANDLED;
}

int demo_probe(struct platform_device * pdevice)
{
    int ret = 0;
    u32  out_value[2] = {100};
    np = pdevice->dev.of_node;

    of_property_read_u32_array(np, "gpio_nu", out_value, 2);
    gpio_nu = out_value[0];
    irq_flags = out_value[1];
    gpio_request(gpio_nu,"gpio_irq");
    gpio_direction_input(gpio_nu);
    // gpio_direction_output(gpio_nu,1);
    // gpio_export(gpio_nu, 1);

    INIT_WORK(&workqueue, workqueue_func);

    ret = request_irq(gpio_to_irq(gpio_nu), reset_ip_handler,out_value[1],"reset_ip", NULL);
    if(ret)
    {
        printk("Failed to request_irq.\n");
        return ret;
    }
    printk("demo_probe ok!\n");
    ret = dev_file_create();

    return ret;

}


int demo_remove(struct platform_device * pdevice)
{

    printk("%s,%d\n",__func__, __LINE__);
    device_destroy(pclass, devno);//销毁设备节点结构体
    class_destroy(pclass);
    cdev_del(pdev);
    kfree(pdev);
    unregister_chrdev_region(devno, count);
    //gpio_unexport(gpio_nu);
    free_irq(gpio_to_irq(gpio_nu), NULL);
    gpio_free(gpio_nu);
    return 0;
}

struct of_device_id  idts[] = {
    {.compatible = "gpio_to_irq"},
    {/*Nothing to done.*/},
};

struct platform_driver  pdriver = {
    .probe = demo_probe,
    .remove = demo_remove,
    .driver = {
        .name = "demo",
        .of_match_table = idts,
    },
};

module_platform_driver(pdriver);
