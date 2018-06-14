#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/kobject.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");
struct mutex  mutex;
static int sht30_read(struct i2c_client *client,char * result);
struct i2c_client *sht30_client;
struct kobject *sht30_kobj;
struct kobj_type sht30_type;
struct attribute tem_attr = {
    .name = "temperature", //文件
    .mode = 0666,  //指定文件的访问权限
};

struct attribute humi_attr = {
    .name = "humidity", //文件
    .mode = 0666, //指定文件的访问权
};

struct attribute readme_attr = {
    .name = "README", //文件
    .mode = 0666, //指定文件的访问权
};
struct attribute T_h_attr = {  
    .name = "Temperature-humidity", //文件名  
    .mode = 0666, //指定文件的访问权限  
};  
struct attribute *sht30_attrs[] = {   
    &tem_attr,   
    &humi_attr,  
    &T_h_attr,
    &readme_attr,
    NULL,  
};  

ssize_t static sht30_show(struct kobject *kobj, struct attribute *attr, char *buffer)  
{  

    int ret ;
    u8 buf[6] = {0};
    unsigned int TC = 0;
    unsigned int RH = 0;


    if(!sht30_client)
        return -1;
    mutex_lock(&mutex);
    ret = sht30_read(sht30_client,buf);
    mutex_unlock(&mutex);
    if(ret != 6)
    {
        return -1;
    }
    

    if (strcmp(attr->name, "temperature") == 0) {  
       
        TC = (buf[0] << 8) + buf[1];    
        ret = sprintf(buffer, "%d\n", TC);  
    } else if (strcmp(attr->name, "humidity") == 0) {  
        RH = (buf[3]<< 8) + buf[4];
        ret = sprintf(buffer, "%d\n", RH);
    } else if (strcmp(attr->name, "Temperature-humidity") == 0) {  
      
        TC = (buf[0] << 8) + buf[1];    
        RH = (buf[3]<< 8) + buf[4];
        ret = sprintf(buffer, "%d:%d\n",TC,RH );
    }else if(strcmp(attr->name, "README") == 0)
    {
        
        ret = sprintf(buffer, "temperature : A digital representation of temperature\nhumidity:A digital representation of humidity\nTemperature-humidity: The first number is temperature,The second number is humidity\nTemperature formula: temperature = -45 + 175*1.0*TC/65535\nhumidity formula: humidity percentage = 100*1.0*RH/65535\nTC:digital of temperature\nRH:digital of humidity\ntemperature:celsius\n");
    }

    return ret ;  
}  

ssize_t static sht30_store(struct kobject *kobj, struct attribute *attr, const char *buffer, size_t size)  
{  

    return 0;  
}  

struct sysfs_ops sht30_sysfsops = {  
    .show = sht30_show,  
    .store = sht30_store,  
};  

void sht30_obj_release(struct kobject *kobj)  
{  
    if(sht30_kobj)
        kfree(sht30_kobj);  
}  







static int sht30_read(struct i2c_client *client,char * result)
{

	int ret;
    //char addr = 0x44;
    char commnd[2] = {0x2c,0x0D};
   // char softreset[2] = {0x30,0xa2};
    char hardreset[2] = {0x30,0x6d};
   // client->addr = addr;
    
    ret = i2c_master_send(client,commnd , 2);
    if (ret < 0)
        return -1;
    ret = i2c_master_recv(client, result, 6);
    if (ret < 0)
        return -1;

    ret = i2c_master_send(client,hardreset , 2);
    if (ret < 0)
        return -1;
   return 6;
}
static int sht30_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int ret ;
    sht30_client = client;
   // char buf[6] = {0};
	sht30_kobj = kzalloc(sizeof(struct kobject), GFP_KERNEL);
    if(!sht30_kobj)
    {
        return -ENOMEM;
    }
    
    sht30_type.release       = sht30_obj_release;
    sht30_type.default_attrs = sht30_attrs;
    sht30_type.sysfs_ops     = &sht30_sysfsops;
    

   ret =  kobject_init_and_add(sht30_kobj, &sht30_type, NULL, "sht30");
   if (ret < 0)
        kfree(sht30_kobj);

    mutex_init(&mutex);
    return ret;
}

static int sht30_remove(struct i2c_client *client)
{

    kobject_del(sht30_kobj);
    kobject_put(sht30_kobj); 
	return 0;
}

static const struct i2c_device_id sht30_id[] = {
	{ "sht30", 0},
	{}
}; 

static struct of_device_id sht30_dt_match[] = {
	{.compatible = "sht30" },
	{/*northing to be done*/},
};

struct i2c_driver sht30_driver = {
	.driver = {
		.name 			= "sht30",
		.owner 			= THIS_MODULE,
		.of_match_table = of_match_ptr(sht30_dt_match),
	},
	.probe 		= sht30_probe,
	.remove 	= sht30_remove,
	.id_table 	= sht30_id,
};

module_i2c_driver(sht30_driver);

