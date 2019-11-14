
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/mm.h>
#include <linux/signal.h>

/*
    Offsets from the start of GPIO memory
*/
#define GPIO_EXTIPSELL 0x100
#define GPIO_EXTIPSELH 0x104
#define GPIO_EXTIRISE  0x108
#define GPIO_EXTIFALL  0x10c
#define GPIO_IEN       0x110
#define GPIO_IFC       0x11c
#define GPIO_PC_DIN    (0x1c + 0x48)
#define GPIO_PC_MODEL  (0x04 + 0x48)
#define GPIO_PC_DOUT   (0x0c + 0x48)

/*
    WIll contain the remapped pointers to GPIO ports used in the driver
*/
struct ioports{
    void* EXTIPSELL;
    void* EXTIPSELH;
    void* EXTIRISE;
    void* EXTIFALL;
    void* IEN;
    void* IFC;
    void* PC_DIN;
    void* PC_MODEL;
    void* PC_DOUT;
};

/*
    struct containing information of the driver
    initialized during probe and init functions
*/
struct gamepad{
    int active;
    dev_t devno;
    struct cdev cdriver;
    struct class *my_class;
    struct resource *res;
    struct resource *mem;
    int irq_odd, irq_even;
    struct fasync_struct *async_queue;
    struct ioports my_ioports;
};

static struct gamepad gamepad_driver;

/*
    When the interrupt handler is called, generate a signal (SIGIO).
*/
static irqreturn_t interrupt_handler(int irq, void *dev_id, struct pt_regs *regs){
    iowrite32(0xff, gamepad_driver.my_ioports.IFC);
    kill_fasync(&(gamepad_driver.async_queue), SIGIO, POLL_IN);
    return IRQ_HANDLED;
}

/*
    Cleanup everything we do in my_Open
*/
static int my_release(struct inode *inode, struct file *filp){
    gamepad_driver.active -= 1;
    if (gamepad_driver.active == 0){
        iowrite32( 0 , gamepad_driver.my_ioports.PC_MODEL);
        iowrite32( 0 , gamepad_driver.my_ioports.PC_DOUT);
        //iowrite32( 0 , gamepad_driver.my_ioports.EXTIPSELL);
        iowrite32( 0 , gamepad_driver.my_ioports.EXTIRISE);
        iowrite32( 0 , gamepad_driver.my_ioports.EXTIFALL);
        iowrite32( 0 , gamepad_driver.my_ioports.IEN);
        free_irq(   gamepad_driver.irq_odd , &gamepad_driver.cdriver );
        free_irq(   gamepad_driver.irq_even, &gamepad_driver.cdriver );
        release_mem_region( gamepad_driver.res->start,
                            gamepad_driver.res->end - gamepad_driver.res->start);
    }
    printk(KERN_ERR"Release: active driver users %d\n", gamepad_driver.active);
    return 0;
}

/*
    Initilize and allocate what we need if it is the first 
    time the driver is opened
*/
static int my_open(struct inode *inode, struct file *filp){
    if(gamepad_driver.active == 0){
        //  Get the memory region
        gamepad_driver.mem = request_mem_region(
                            gamepad_driver.res->start,
                            gamepad_driver.res->end - gamepad_driver.res->start,
                            "tdt4258-mem");
    
        if(gamepad_driver.mem == 0){
            printk(KERN_ERR "Memory request failed");
        }
        //  Initialize the GPIO
        iowrite32( 0x33333333 , gamepad_driver.my_ioports.PC_MODEL);
        iowrite32( 0xff       , gamepad_driver.my_ioports.PC_DOUT);
        iowrite32( 0x22222222 , gamepad_driver.my_ioports.EXTIPSELL);
        iowrite32( 0xff       , gamepad_driver.my_ioports.EXTIRISE);
        iowrite32( 0xff       , gamepad_driver.my_ioports.EXTIFALL);
        iowrite32( 0xff       , gamepad_driver.my_ioports.IEN);
        //  Get the IRQ lines
        request_irq(    gamepad_driver.irq_even,
                        (irq_handler_t)interrupt_handler,
                        0,
                        "driver-gamepad",
                        &gamepad_driver.cdriver);
        
        request_irq(    gamepad_driver.irq_odd,
                        (irq_handler_t)interrupt_handler,
                        0,
                        "driver-gamepad",
                        &gamepad_driver.cdriver);
    }
    gamepad_driver.active += 1;
    printk(KERN_ERR"Open: active driver users %d\n", gamepad_driver.active);
    return 0;
}

/*
    Read from GPIO_PC_DIN and copy it into the userspace buffer
*/
static ssize_t my_read(struct file *filp, char __user *buff, size_t count, loff_t *offp){
    int8_t data = ~ioread8(gamepad_driver.my_ioports.PC_DIN);
    copy_to_user(buff, &data, 1);
    return 0;
}

static int my_fasync(int fd, struct file *filp, int mode){
    return fasync_helper(fd, filp, mode, &(gamepad_driver.async_queue));
}

static struct file_operations my_fops = {
    .owner      = THIS_MODULE,
    .read       = my_read,
    .open       = my_open,
    .release    = my_release,
    .fasync     = my_fasync,
};

/*
    This function is called by the operating system when it detects a driver
    and device that is compatile (set in the of_device_id struct).
    The argument *dev is used to get information about the device, like
    memory location and interrupt numbers.
    Information is stored in the global gamepad struct, to be used later.
*/
static int my_probe(struct platform_device *dev){
    int err;
    gamepad_driver.active = 0;
    //  Get major and minor numbers.
    err = alloc_chrdev_region(&gamepad_driver.devno, 0, 1, "driver-gamepad");
    if (err < 0){
        printk(KERN_ERR "Allocation failed");
    }
    //  Extract information about the device (TDT4258) from
    //  the argument platform_device *dev.
    gamepad_driver.res = platform_get_resource(dev, IORESOURCE_MEM, 0);
    if (!gamepad_driver.res){
        printk(KERN_ERR "Failed to get resource");
    }
    //  Create class and device.
    gamepad_driver.my_class = class_create(THIS_MODULE, "driver-gamepad");
    device_create(  gamepad_driver.my_class,
                    NULL,
                    gamepad_driver.devno,
                    NULL,
                    "driver-gamepad");
    
    
    //  Remap physical memory adress space to virtual
    //  We dont want it to be cached, but to write to the memory
    //  We use 4 to get 4 bytes mapped, which is the size int.
    gamepad_driver.my_ioports.EXTIPSELL 
        = ioremap_nocache(  gamepad_driver.res->start + GPIO_EXTIPSELL  , 4 );
        
    gamepad_driver.my_ioports.EXTIPSELH 
        = ioremap_nocache(  gamepad_driver.res->start + GPIO_EXTIPSELH  , 4 );
        
    gamepad_driver.my_ioports.EXTIRISE 
        = ioremap_nocache(  gamepad_driver.res->start + GPIO_EXTIRISE   , 4 );
        
    gamepad_driver.my_ioports.EXTIFALL 
        = ioremap_nocache(  gamepad_driver.res->start + GPIO_EXTIFALL   , 4 );
        
    gamepad_driver.my_ioports.IEN 
        = ioremap_nocache(  gamepad_driver.res->start + GPIO_IEN        , 4 );
        
    gamepad_driver.my_ioports.IFC 
        = ioremap_nocache(  gamepad_driver.res->start + GPIO_IFC        , 4 );
                                        
    gamepad_driver.my_ioports.PC_DIN 
        = ioremap_nocache(  gamepad_driver.res->start + GPIO_PC_DIN     , 4 );
        
    gamepad_driver.my_ioports.PC_MODEL 
        = ioremap_nocache(  gamepad_driver.res->start + GPIO_PC_MODEL   , 4 );
        
    gamepad_driver.my_ioports.PC_DOUT 
        = ioremap_nocache(  gamepad_driver.res->start + GPIO_PC_DOUT    , 4 );
    
    //  Get the IRQ numbers
    gamepad_driver.irq_even    = platform_get_irq(dev, 0);
    gamepad_driver.irq_odd     = platform_get_irq(dev, 1);
    //  Initialize the char driver with the my_fops and add it to the system,
    //  making it live immidiately
    cdev_init(&gamepad_driver.cdriver, &my_fops);
    err = cdev_add(&gamepad_driver.cdriver, gamepad_driver.devno, 1);
    if (err < 0) {
        printk(KERN_ERR "Failed to activate char driver.\n");
    }
    return 0;
}

static int my_remove(struct platform_device *dev){
    printk(KERN_ERR "remove");
    return 0;
}

static const struct of_device_id my_of_match[]={
    {   .compatible = "tdt4258", },
    {   },
};

static struct platform_driver my_driver ={
    .probe = my_probe,
    .remove = my_remove,
    .driver = {
        .name   = "my",
        .owner  = THIS_MODULE,
        .of_match_table = my_of_match,
    },
};

static int __init template_init(void){
	platform_driver_register(&my_driver);
	return 0;
}

static void __exit template_cleanup(void){
    //  Cleanup
    cdev_del(&gamepad_driver.cdriver);
    unregister_chrdev_region(gamepad_driver.devno,1);
}

module_init(template_init);
module_exit(template_cleanup);

MODULE_DESCRIPTION("gamepad driver");
MODULE_LICENSE("GPL");


