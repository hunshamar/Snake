#include "driver-gamepad.h"



module_init(gamepad_init);
module_exit(gamepad_cleanup);
MODULE_DESCRIPTION("Char driver for gamepad");
MODULE_LICENSE("GPL");

static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = gamepad_open,
    .release = gamepad_release,
    .read = gamepad_read,
    .write = gamepad_write,
};


static int __init gamepad_init(void)
{
    printk("Loading gamepad kernel module into OS\n");

    /* Allocate major and minor numbers */
    int major_number = alloc_chrdev_region(&dev, 0, COUNT, NAME);
    if (major_number < 0) {
        printk(KERN_ALERT "Allocation of device numbers failed\n");
        return -1;
    }

    /* Request access to memory regions of I/O */
    if (request_mem_region(GPIO_PC_MODEL, 1, NAME) == NULL ) 
    {
        printk(KERN_ALERT "Error requesting region: GPIO_PC_MODEL\n");
        return -1;
    }
    if (request_mem_region(GPIO_PC_DOUT, 1, NAME) == NULL ) 
    {
        printk(KERN_ALERT "Error requesting region: GPIO_PC_DOUT\n");
        return -1;
    }
    if (request_mem_region(GPIO_PC_DIN, 1, NAME) == NULL ) 
    {
        printk(KERN_ALERT "Error requesting region: GPIO_PC_DIN\n");
        return -1;
    }

    /* Setup for GPIO */
    iowrite32(0x33333333, GPIO_PC_MODEL); // set buttons as input
    iowrite32(0xFF, GPIO_PC_DOUT); // Enable internal pull-up
    iowrite32(2, GPIO_PA_CTRL); //SEt high drive strength


    /* Allocate and initialize cdev structure */
    cdev_init(&cdev, &my_fops);
    cdev.owner = THIS_MODULE;
    cdev_add(&cdev, dev, COUNT);
      
    
    cl = class_create(THIS_MODULE, NAME);
    device_create(cl, NULL, dev, NULL, NAME);

    return 0;
}

static void __exit gamepad_cleanup(void)
{
    printk("Unloading gamepad kernel module into OS\n");
    
    
    unregister_chrdev_region(dev, COUNT);

    /* Release GPIO memory regions */
    release_mem_region(GPIO_PC_MODEL, 1);
    release_mem_region(GPIO_PC_DIN, 1);
    release_mem_region(GPIO_PC_DOUT, 1);
    


    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&cdev);
}



static int gamepad_open(struct inode* inode, struct file* filp)
{
    printk(KERN_INFO "Driver opened\n");
    return 0;
}


static int gamepad_release(struct inode* inode, struct file* filp)
{
    printk(KERN_INFO "Driver closed\n");
    return 0;
}

static ssize_t gamepad_read(struct file* filp, char* __user buff, size_t count, loff_t* offp)
{
    // Read button status register
    uint32_t button_read = ioread32(GPIO_PC_DIN);
    // Write to buffer
    copy_to_user(buff, &button_read, 1);
    return 1;
}

static ssize_t gamepad_write(struct file* filp, char* __user buff, size_t count, loff_t* offp)
{
    printk(KERN_INFO "This driver is read only, don't do anything \n");
    return 1;
}