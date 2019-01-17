
/*
 * Copyright (C) 2012-2015 Freescale Semiconductor, Inc., All Rights Reserved
 */

#include "compat.h"
#include "intern.h"
#include "sm.h"
#include "key_blob.h"

#define MAX_KEY_LENGTH 32

static struct kobject *kb_kobj;
static struct kobj_attribute *kb_kattr;
static struct attribute_group *kb_attr_group;

static int kb_major;
static struct class *kb_class;

static u8 skeymod[] = {
	0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08,
	0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00
};

kb_addr_t kb_addr;

static ssize_t kb_encap(u32 key_len)
{
	int ret = 0;
	struct device_node *dev_node;
	struct platform_device *pdev;
	struct device *ctrldev, *ksdev;
	struct caam_drv_private *ctrlpriv;
	struct caam_drv_private_sm *kspriv;
	u32 units, unit, keyslot;

	dev_node = of_find_compatible_node(NULL, NULL, "fsl,sec-v4.0");
	if (!dev_node) {
		dev_node = of_find_compatible_node(NULL, NULL, "fsl,sec4.0");
		if (!dev_node)
			return -ENODEV;
	}

	pdev = of_find_device_by_node(dev_node);
	if (!pdev)
		return -ENODEV;

	ctrldev = &pdev->dev;
	ctrlpriv = dev_get_drvdata(ctrldev);
	ksdev = ctrlpriv->smdev;
	kspriv = dev_get_drvdata(ksdev);
	if (kspriv == NULL)
		return -ENODEV;

	units = sm_detect_keystore_units(ksdev);

	if (units < 2)
		return -ENODEV;

	unit = 1;

	sm_establish_keystore(ksdev, unit);
	sm_keystore_slot_alloc(ksdev, unit, key_len, &keyslot);

	sm_keystore_slot_load(ksdev, unit, keyslot, kb_addr.key_addr, key_len);

	sm_keystore_slot_export(ksdev, unit, keyslot, RED_KEY, KEY_COVER_ECB, kb_addr.blob_addr, key_len, skeymod);

	sm_keystore_slot_dealloc(ksdev, unit, keyslot);
	sm_release_keystore(ksdev, unit);

	return ret;
}

static ssize_t kb_decap(u32 blob_len)
{
	int ret = 0;
	struct device_node *dev_node;
	struct platform_device *pdev;
	struct device *ctrldev, *ksdev;
	struct caam_drv_private *ctrlpriv;
	struct caam_drv_private_sm *kspriv;
	u32 units, unit, keyslot;

	dev_node = of_find_compatible_node(NULL, NULL, "fsl,sec-v4.0");
	if (!dev_node) {
		dev_node = of_find_compatible_node(NULL, NULL, "fsl,sec4.0");
		if (!dev_node)
			return -ENODEV;
	}

	pdev = of_find_device_by_node(dev_node);
	if (!pdev)
		return -ENODEV;

	ctrldev = &pdev->dev;
	ctrlpriv = dev_get_drvdata(ctrldev);
	ksdev = ctrlpriv->smdev;
	kspriv = dev_get_drvdata(ksdev);
	if (kspriv == NULL)
		return -ENODEV;

	units = sm_detect_keystore_units(ksdev);

	if (units < 2)
		return -ENODEV;

	unit = 1;

	sm_establish_keystore(ksdev, unit);
	sm_keystore_slot_alloc(ksdev, unit, blob_len - BLOB_OVERHEAD, &keyslot);

	sm_keystore_slot_import(ksdev, unit, keyslot, RED_KEY, KEY_COVER_ECB, kb_addr.blob_addr, blob_len - BLOB_OVERHEAD, skeymod);

	sm_keystore_slot_read(ksdev, unit, keyslot, blob_len - BLOB_OVERHEAD, kb_addr.key_addr);

	sm_keystore_slot_dealloc(ksdev, unit, keyslot);
	sm_release_keystore(ksdev, unit);

	return ret;
}

static ssize_t kb_encap_mfg(struct kobject *kobj, struct kobj_attribute *attr,
			     const char *buf, size_t count)
{
	struct file *filp;
	int ret = 0;
	u32 key_len;
	unsigned long value;

	ret = kstrtoul(buf, 16, &value);
	if (ret < 0)
		return -EINVAL;

	if (value == 1)
		filp = filp_open("/device/key.bin", O_RDONLY, 0);
	else
		return -EINVAL;

	if (IS_ERR(filp)) {
		printk("key file open failed\n");
		return -ENOENT;
	}

	memset(kb_addr.key_addr, 0, MAX_KEY_LENGTH);
	memset(kb_addr.blob_addr, 0, MAX_KEY_LENGTH + BLOB_OVERHEAD);

	key_len = kernel_read(filp, 0, kb_addr.key_addr, MAX_KEY_LENGTH);

	filp_close(filp, NULL);

	if (key_len < 0) {
		printk("key file read failed\n");
		return -EIO;
	}

	kb_encap(key_len);

	if (value == 1)
		filp = filp_open("/device/key_blob", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		return -EINVAL;

	if (IS_ERR(filp)) {
		printk("blob file open failed\n");
		return -ENOENT;
	}

	if (kernel_write(filp, kb_addr.blob_addr, key_len + BLOB_OVERHEAD, 0) < 0) {
		printk("blob file write failed\n");
		ret = -EIO;
	}

	filp_close(filp, NULL);

	return ret ? 0 : count;
}

static ssize_t kb_decap_mfg(struct kobject *kobj, struct kobj_attribute *attr,
			     const char *buf, size_t count)
{
	struct file *filp;
	int ret = 0;
	u32 blob_len;
	unsigned long value;

	ret = kstrtoul(buf, 16, &value);
	if (ret < 0)
		return -EINVAL;

	if (value == 1)
		filp = filp_open("/device/key_blob", O_RDONLY, 0);
	else
		return -EINVAL;

	if (IS_ERR(filp)) {
		printk("blob file open failed\n");
		return -ENOENT;
	}

	memset(kb_addr.key_addr, 0, MAX_KEY_LENGTH);
	memset(kb_addr.blob_addr, 0, MAX_KEY_LENGTH + BLOB_OVERHEAD);

	blob_len = kernel_read(filp, 0, kb_addr.blob_addr, MAX_KEY_LENGTH + BLOB_OVERHEAD);

	filp_close(filp, NULL);

	if (blob_len < 0) {
		printk("blob file read failed\n");
		return -EIO;
	}

	kb_decap(blob_len);

	if (value == 1)
		filp = filp_open("/device/key.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		return -EINVAL;

	if (IS_ERR(filp)) {
		printk("key file open failed\n");
		return -ENOENT;
	}

	if (kernel_write(filp, kb_addr.key_addr, blob_len - BLOB_OVERHEAD, 0) < 0) {
		printk("key file write failed\n");
		ret = -EIO;
	}

	filp_close(filp, NULL);

	return ret ? 0 : count;
}

static long kb_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int errval = 0;

	switch (cmd) {
		case KB_IOCTL_ENCAP:
		{
			kb_parameter_t kb;
			
			if (copy_from_user(&kb, (kb_parameter_t *)arg, sizeof(kb_parameter_t)))
				return -EFAULT;
			kb_encap(kb.key_len);
			break;
		}
		case KB_IOCTL_DECAP:
	    {
			kb_parameter_t kb;
			
			if (copy_from_user(&kb, (kb_parameter_t *)arg, sizeof(kb_parameter_t)))
				return -EFAULT;
			kb_decap(kb.blob_len);
			break;
	    }
		case KB_IOCTL_GET_PHY_ADDR:
		{
			if (copy_to_user((kb_addr_t *)arg, &kb_addr, sizeof(kb_addr_t)))
				return -EFAULT;
			break;
		}
		default:
		    break;
	}

	return errval;
}

static int kb_mmap(struct file *file, struct vm_area_struct *vma)
{
	vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);

	if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
				vma->vm_end - vma->vm_start,
				vma->vm_page_prot)) {
		printk(KERN_ERR
				"mmap failed!\n");
		return -ENOBUFS;
	}
	return 0;
}

static int kb_open(struct inode *inode, struct file *file)
{
	int errval = 0;

	return errval;
}

static int kb_release(struct inode *inode, struct file *file)
{
	int errval = 0;

	return errval;
}

static const struct file_operations kb_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = kb_ioctl,
	.mmap = kb_mmap,
	.open = kb_open,
	.release = kb_release,
};

static int __init key_blob_init(void)
{
	struct device_node *dev_node;
	struct platform_device *pdev;

	struct attribute **attrs;
	int ret;

	dev_node = of_find_compatible_node(NULL, NULL, "fsl,sec-v4.0");
	if (!dev_node) {
		dev_node = of_find_compatible_node(NULL, NULL, "fsl,sec4.0");
		if (!dev_node)
			return -ENODEV;
	}

	pdev = of_find_device_by_node(dev_node);
	if (!pdev)
		return -ENODEV;

	of_node_get(dev_node);

	kb_addr.key_addr = dma_alloc_coherent(&pdev->dev, MAX_KEY_LENGTH, &kb_addr.key_phy_addr, GFP_KERNEL | GFP_DMA);
	if (!kb_addr.key_addr)
		return -ENOMEM;
	kb_addr.blob_addr = dma_alloc_coherent(&pdev->dev, MAX_KEY_LENGTH + BLOB_OVERHEAD, &kb_addr.blob_phy_addr, GFP_KERNEL | GFP_DMA);
	if (!kb_addr.blob_addr)
		return -ENOMEM;

	/* The last one is NULL, which is used to detect the end */
	attrs = devm_kzalloc(&pdev->dev, 3 * sizeof(*attrs),
			     GFP_KERNEL);
	kb_kattr = devm_kzalloc(&pdev->dev, 2 * sizeof(*kb_kattr),
				 GFP_KERNEL);
	kb_attr_group = devm_kzalloc(&pdev->dev, sizeof(*kb_attr_group),
				      GFP_KERNEL);
	if (!attrs || !kb_kattr || !kb_attr_group)
		return -ENOMEM;

	sysfs_attr_init(kb_kattr[0].attr);
	kb_kattr[0].attr.name = "encap";
	kb_kattr[0].attr.mode = 0600;
	kb_kattr[0].show = NULL;
	kb_kattr[0].store = kb_encap_mfg;
	attrs[0] = &kb_kattr[0].attr;

	sysfs_attr_init(kb_kattr[1].attr);
	kb_kattr[1].attr.name = "decap";
	kb_kattr[1].attr.mode = 0600;
	kb_kattr[1].show = NULL;
	kb_kattr[1].store = kb_decap_mfg;
	attrs[1] = &kb_kattr[1].attr;

	kb_attr_group->attrs = attrs;

	kb_kobj = kobject_create_and_add("kb", NULL);
	if (!kb_kobj) {
		dev_err(&pdev->dev, "failed to add kobject\n");
		return -ENOMEM;
	}

	ret = sysfs_create_group(kb_kobj, kb_attr_group);
	if (ret) {
		dev_err(&pdev->dev, "failed to create sysfs group: %d\n", ret);
		kobject_put(kb_kobj);
		return ret;
	}

	kb_major = register_chrdev(0, "kb", &kb_fops);
	if (kb_major < 0) {
		printk("KB: Unable to register driver\n");
		return -ENODEV;
	}
	kb_class = class_create(THIS_MODULE, "kb");
	if (IS_ERR(kb_class)) {
		printk("KB: Unable to create class\n");
		unregister_chrdev(kb_major, "kb");
		return PTR_ERR(kb_class);
	}
	device_create(kb_class, NULL, MKDEV(kb_major, 0), NULL, "kb");

	return 0;
}

static void __exit key_blob_exit(void)
{
	struct device_node *dev_node;
	struct platform_device *pdev;

	dev_node = of_find_compatible_node(NULL, NULL, "fsl,sec-v4.0");
	if (!dev_node) {
		dev_node = of_find_compatible_node(NULL, NULL, "fsl,sec4.0");
		if (!dev_node)
			return;
	}

	pdev = of_find_device_by_node(dev_node);
	if (!pdev)
		return;

	of_node_put(dev_node);

	dma_free_coherent(&pdev->dev, MAX_KEY_LENGTH, (void *)kb_addr.key_addr, kb_addr.key_phy_addr);
	dma_free_coherent(&pdev->dev, MAX_KEY_LENGTH + BLOB_OVERHEAD, (void *)kb_addr.blob_addr, kb_addr.blob_phy_addr);

	sysfs_remove_group(kb_kobj, kb_attr_group);
	kobject_put(kb_kobj);

	device_destroy(kb_class, MKDEV(kb_major, 0));
	class_destroy(kb_class);
	unregister_chrdev(kb_major, "kb");

	return;
}

module_init(key_blob_init);
module_exit(key_blob_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("FSL CAAM KEY BLOB");
MODULE_AUTHOR("Freescale Semiconductor - MCU");

