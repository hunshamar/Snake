/*
 * This is a demo Linux kernel module.
 */

/*
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h> // Character driver support
*/
#include  <linux/types.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/moduleparam.h>
#include <linux/kdev_t.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/signal.h>
#include <asm/siginfo.h>
#include <linux/interrupt.h>
#include "efm32gg.h"


#include <linux/platform_device.h>
#include <linux/mm.h>



#define COUNT 1 /* The total number of contiguous device numbers you are requesting */
#define NAME "gamepad" /* Name of the device */

static dev_t device_nr;
struct cdev my_cdev;
struct class* my_class;

struct cdev my_cdev;

static ssize_t my_read(struct file* filp, char __user* buff, size_t count, loff_t *f_pos);

static ssize_t my_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp);

static int my_open(struct inode *inode, struct file *filp);

static int my_release(struct inode * inode, struct file *filp);

static struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write,
	.open = my_open,
	.release = my_release
};

static ssize_t my_read(struct file* filp, char __user* buff, size_t count, loff_t *f_pos){
	uint32_t DIN = ioread32(GPIO_PC_DIN);
	copy_to_user(buff, &DIN, 1);
	return 1;
}

static ssize_t my_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp){
	printk("Writing \n");
	return 1;
}

static int my_open(struct inode *inode, struct file *filp){
	printk("Opening \n");
	return 1;
}

static int my_release(struct inode * inode, struct file *filp){
	printk("releasing \n");
	return 1;
}



/*
 * template_init - function to insert this module into kernel space
 *
 * This is the first of two exported functions to handle inserting this
 * code into a running kernel
 *
 * Returns 0 if successfull, otherwise -1
 */

	int err_res;
static int __init template_init(void)
{
	printk("Hello World, here is your module speaking\n");

	/* Register with the kernel and indicate that we are registering a charcter device driver */

	/* Dynamically allocate device number  */
	err_res = alloc_chrdev_region(&device_nr, 0, COUNT, NAME);
	if (err_res < 0){
		printk("Error allocating device number \n");
	}


	cdev_init(&my_cdev, &my_fops);
	my_cdev.owner = THIS_MODULE;
	cdev_add(&my_cdev, device_nr, COUNT);
	my_class = class_create(THIS_MODULE, NAME);
	device_create(my_class, NULL, device_nr, NULL, NAME);

request_region(GPIO_PA_BASE, GPIO_IFC - GPIO_PA_BASE, DRIVER_NAME);

    /* init gpio as in previous exercises */
    *CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO;
    iowrite32(2, GPIO_PA_CTRL);
    iowrite32(0x33333333, GPIO_PC_MODEL);
    iowrite32(0xFF, GPIO_PC_DOUT);
    iowrite32(0x22222222, GPIO_EXTIPSELL);


/*
if (request_mem_region(GPIO_PC_MODEL, 1, NAME) == NULL ) {
        printk(KERN_ALERT "Error requesting GPIO_PC_MODEL memory region, already in use?\n");
        return -1;
    }
    if (request_mem_region(GPIO_PC_DOUT, 1, NAME) == NULL ) {
        printk(KERN_ALERT "Error requesting GPIO_PC_DOUT memory region, already in use?\n");
        return -1;
    }
    if (request_mem_region(GPIO_PC_DIN, 1, NAME) == NULL ) {
        printk(KERN_ALERT "Error requesting GPIO_PC_DIN memory region, already in use?\n");
        return -1;
    }

    iowrite32(0x33333333, GPIO_PC_MODEL);
    iowrite32(0xFF, GPIO_PC_DOUT);
    iowrite32(0x22222222, GPIO_EXTIPSELL);*/

    printk("Loaded \n");
	return 0;
}

/*
 * template_cleanup - function to cleanup this module from kernel space
 *
 * This is the second of two exported functions to handle cleanup this
 * code from a running kernel
 */

static void __exit template_cleanup(void)
{
	 printk("Short life for a small module...\n");
	 /*Free allocated device number*/
	 unregister_chrdev_region(device_nr, COUNT);


    release_mem_region(GPIO_PC_MODEL, 1);
    release_mem_region(GPIO_PC_DIN, 1);
    release_mem_region(GPIO_PC_DOUT, 1);
    

    device_destroy(my_class, device_nr);
    class_destroy(my_class);
    cdev_del(&my_cdev);

}

module_init(template_init);
module_exit(template_cleanup);

MODULE_DESCRIPTION("Small module, demo only, not very useful.");
MODULE_LICENSE("GPL");

