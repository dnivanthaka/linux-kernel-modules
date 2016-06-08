/*
 * hello-1.c
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void)
{
    printk(KERN_INFO "Hello, World 1\n");
    // A non 0 return means init_module failed. module cannot be loaded.
    return 0;
}

void cleanup_module(void)
{
    printk(KERN_INFO "Goodbye, world 1\n");
}
