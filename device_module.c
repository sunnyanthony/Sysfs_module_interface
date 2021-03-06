#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/timer.h>
#include <linux/sysfs.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define device_max_num (1024)
#define BUF_MAX PAGE_SIZE
struct class    *device_class;
struct device   *device_interface_device;
unsigned char *temp;
dev_t devt; //MKDEV(major,minor)
unsigned char * temp_buf[BUF_MAX];

/*
struct sysfs_for_operation{
}
*/

// ************************************************* //
static ssize_t device_name_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        return snprintf(buf, PAGE_SIZE,"%s : %s\n",dev->kobj.name,(unsigned char *)dev->driver_data);
}
static DEVICE_ATTR(name, S_IRUSR | S_IRGRP,device_name_show,NULL);

static ssize_t device_stream_store(struct device *dev, struct device_attribute *attr, char *buf,ssize_t size)
{
	/* do any you what to call */
        return snprintf(temp_buf, PAGE_SIZE,buf);
}
static DEVICE_ATTW(stream, S_IRUSR | S_IRGRP, NULL, device_stream_store);
//DEVICE_ATTR_RW(_name) _RO _WO... _name##_show

static struct attribute device_sysfs_attrs[] = {
        &dev_attr_name.attr,
        &dev_attr_stream.attr,
        NULL,
};

ATTRIBUTE_GROUPS(device_sysfs);

static int sysfs_suspend(struct device *dev)
{
        return 0;
}
static int sysfs_resume(struct device *dev)
{
        return 0;
}
//register_syscore_ops
//unregister_syscore_ops

static int __init interface_init(void)
{
	int ret;
	devt = 0;
	ret = alloc_chrdev_region(&devt,0,device_max_num,KBUILD_MODNAME);
	if(ret<0)
		goto init_failed;
        device_class = class_create(THIS_MODULE, "device_interface");
        if (IS_ERR(device_class)) {
                printk(KERN_ERR "%s: couldn't create class\n", __FILE__);
                return PTR_ERR(device_class);
        }
        device_class->suspend = sysfs_suspend;
        device_class->resume = sysfs_resume;
        device_class->dev_groups = device_sysfs_groups;
	temp = kmalloc(32,GFP_KERNEL);
	ret = snprintf(temp,32,"device interface");
        device_interface_device = device_create(device_class, NULL, devt, temp, "device_0");
	if(!device_interface_device)
		goto register_failed;

init_failed:
	return ret;

register_failed:
	class_destroy(device_class);
        return 0;
}
static void __exit interface_exit(void)
{
        device_destroy(device_class, devt);
        class_destroy(device_class);
	kfree(temp);
}
module_init(interface_init);
module_exit(interface_exit);

MODULE_DESCRIPTION("Module sysfs");
MODULE_LICENSE("GPL v2");
