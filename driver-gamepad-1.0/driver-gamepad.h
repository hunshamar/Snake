#ifndef _DRIVER_GAMEPAD_H_
#define _DRIVER_GAMEPAD_H_

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h> // Necessary functions to optain device numbers
#include <linux/ioport.h> 
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/moduleparam.h>
#include <linux/kdev_t.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include "efm32gg.h"

#define NAME "gamepad"
#define COUNT 1

static int __init gamepad_init(void);
static void __exit gamepad_cleanup(void);
static int gamepad_open(struct inode* inode, struct file* filp);
static int gamepad_release(struct inode* inode, struct file* filp);
static ssize_t gamepad_read(struct file* filp, char* __user buff, size_t count, loff_t* offp);
static ssize_t gamepad_write(struct file* filp, char* __user buff, size_t count, loff_t* offp);

/* Static variables */
static dev_t dev;
struct cdev cdev;
struct class* cl;

#endif