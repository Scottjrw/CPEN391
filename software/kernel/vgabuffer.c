#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

static int vgabuffer_init(void) {
    printk(KERN_ALERT "Hello, world\n");
    return 0;
}

static void vgabuffer_exit(void) {
    printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(vgabuffer_init);
module_exit(vgabuffer_exit);
