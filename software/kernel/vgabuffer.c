#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/resource.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/of.h>
#include <asm/io.h>
#include <asm/page.h>
#include <linux/mm.h>
#include <linux/log2.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/mutex.h>

#define DRV_NAME "cpen391_vgabuffer"

// Reset register for the FPGA-HPS SDRAM Master
#define FPGA_SDRAM_BRIDGE_REGISTER 0xFFC25080
#define FPGA_SDRAM_BRIDGE_ENABLE 0x3FFF
#define FPGA_SDRAM_BRIDGE_DISABLE 0x0

// Reset Manager for the FPGA
#define RESET_MANAGER_RESET_REGISTER 0xFFD05020
#define RESET_MANAGER_RESET_FPGA_VALUE (BIT(6))

// Registers for the DMA Controller
#define VIDEO_DMA_SPAN 16
#define VIDEO_DMA_BUFFER_OFFSET 0
#define VIDEO_DMA_BACKBUFFER_OFFSET 4
#define VIDEO_DMA_RESOLUTION_OFFSET 8
#define VIDEO_DMA_STATUS_OFFSET 12
#define VIDEO_DMA_CONTROL_OFFSET 12

// -- Register shift macros --
// Width (X) and Height (Y) of screen
#define VIDEO_DMA_RES_X(resolution_reg) (0xFFFF & resolution_reg)
#define VIDEO_DMA_RES_Y(resolution_reg) ((0xFFFF0000 & resolution_reg) >> 16)
// X-Y Addressing Bit Widths
#define VIDEO_DMA_STAT_X_BITS(status_reg) ((0xFF000000 & status_reg) >> 24)
#define VIDEO_DMA_STAT_Y_BITS(status_reg) ((0xFF0000 & status_reg) >> 16)
// Pixel info pixel size in bits = COLOR_BITS * COLOR_PLANES
#define VIDEO_DMA_STAT_COLOR_BITS(status_reg) (1+((0xF00 & status_reg) >> 8))
#define VIDEO_DMA_STAT_COLOR_PLANES(status_reg) (1+((0xC0 & status_reg) >> 6))
// Addressing Mode (X-Y or Consecutive)
#define VIDEO_DMA_STAT_XY_MODE(status_reg) (!(0x2 & status_reg))
#define VIDEO_DMA_STAT_CONSECUTIVE_MODE(status_reg) (0x2 & status_reg)
// Check if module is busy swapping
#define VIDEO_DMA_STAT_SWAPPING(status_reg) (0x1 & status_reg)
// Enable / Disable DMA Controller
#define VIDEO_DMA_CTRL_ENABLE (BIT(2))
#define VIDEO_DMA_CTRL_DISABLE (0x0)

struct cpen391_vgabuffer {
    // Base address of DMA Controller Registers,
    void __iomem *video_dma_base;
    void __iomem *reset_base;
    void __iomem *sdram_bridge_base;
    // Starting address of the framebuffer, allocated as contiguous physical pages
    void __kernel *framebuffer;
    int framebuffer_order;
    unsigned framebuffer_size;
    // Video frame attributes, mainly used to calculate size
    unsigned int width;
    unsigned int height;
    unsigned short bytes_per_pixel;
    bool in_use;
    struct mutex lock;
} vgabuf;

static inline u32 video_dma_read_reg(unsigned offset) {
    return ioread32((u8 *) vgabuf.video_dma_base + offset);
}

static inline void video_dma_write_reg(unsigned offset, u32 val) {
    iowrite32(val, (u8 *) vgabuf.video_dma_base + offset);
}

static unsigned video_dma_calc_bytes_per_pixel(unsigned pix_bits, unsigned pix_planes) {
    unsigned total_bits = pix_bits * pix_planes;
    if (IS_ALIGNED(total_bits, 8) && total_bits > 0) {
        unsigned total_bytes = total_bits / 8;
        if (total_bytes <= 4)
            // 3 byte pixels are always aligned to 4
            return total_bytes == 3 ? 4 : total_bytes;

        printk(KERN_ERR DRV_NAME "Pixels are too large: %d bytes", total_bytes);
        return 0;
    } else {
        printk(KERN_ERR DRV_NAME "Non-byte aligned pixel size: %d bits\n", total_bits);
        return 0;
    }
}

static inline void video_dma_reset(void) {
    iowrite32(0, vgabuf.reset_base);
}

static void cpen391_vgabuffer_vopen(struct vm_area_struct *vma) {
    // Nothing to do here
}

static void cpen391_vgabuffer_vclose(struct vm_area_struct *vma) {
    // Nothing to do here
}

static int cpen391_vgabuffer_vfault(struct vm_area_struct *vma, struct vm_fault *vmf) {
    mutex_lock(&vgabuf.lock);
    if (vgabuf.framebuffer) {
        void *fault_kaddr = ((u8 *) vgabuf.framebuffer) + vmf->pgoff * PAGE_SIZE;
        //printk(KERN_ALERT DRV_NAME ": fault at 0x%x, user 0x%x\n", (u32) fault_kaddr, (u32) vmf->virtual_address);

        struct page *page = virt_to_page(fault_kaddr);    
        get_page(page);
        vmf->page = page; 
    }
    mutex_unlock(&vgabuf.lock);
    return 0;
}

struct vm_operations_struct cpen391_vgabuffer_vmops = {
    .open   = cpen391_vgabuffer_vopen,
    .close  = cpen391_vgabuffer_vclose,
    .fault  = cpen391_vgabuffer_vfault,
};

static int video_dma_setup(u32 framebuffer_pa) {
    // Wait until the swap is completed, swaps only occur when the DMA Controller is not fetching a frame
    unsigned long timeout = jiffies + HZ;

    do {
        if (time_after(jiffies, timeout))
            return -EIO;

        video_dma_write_reg(VIDEO_DMA_BACKBUFFER_OFFSET, framebuffer_pa);
        udelay(100);
        video_dma_write_reg(VIDEO_DMA_BUFFER_OFFSET, 0);
        msleep(25);
    } while (video_dma_read_reg(VIDEO_DMA_BUFFER_OFFSET) != framebuffer_pa);

    return 0;
}

static int cpen391_vgabuffer_fopen(struct inode *inode, struct file *filp) {
    int ret;

    mutex_lock(&vgabuf.lock);

    u32 framebuffer_pa = __pa(vgabuf.framebuffer);
    printk(KERN_ALERT DRV_NAME ": allocated buffer at virtual address 0x%x, physical address 0x%x\n",
            (u32) vgabuf.framebuffer, framebuffer_pa);

    // Directly enable the bridge, a todo would be to use the proper driver to enable it
    iowrite32(FPGA_SDRAM_BRIDGE_DISABLE, vgabuf.sdram_bridge_base);
    barrier();
    iowrite32(FPGA_SDRAM_BRIDGE_ENABLE, vgabuf.sdram_bridge_base);
    msleep(25);

    // Put the address in before enabling
    video_dma_write_reg(VIDEO_DMA_BACKBUFFER_OFFSET, framebuffer_pa);
    udelay(100);
    video_dma_write_reg(VIDEO_DMA_BUFFER_OFFSET, 0);
    udelay(100);
    video_dma_write_reg(VIDEO_DMA_CONTROL_OFFSET, VIDEO_DMA_CTRL_ENABLE);
    udelay(100);

    if (video_dma_setup(framebuffer_pa)) {
        // DMA Controller may still be stuck even after enabling the sdram bridge, reset it
        // Note that resetting too many times leaves the device unrecoverable, in that case rebooting is the only option
        video_dma_reset();
        msleep(50);

        video_dma_write_reg(VIDEO_DMA_BACKBUFFER_OFFSET, framebuffer_pa);
        udelay(100);
        video_dma_write_reg(VIDEO_DMA_BUFFER_OFFSET, 0);
        udelay(100);
        video_dma_write_reg(VIDEO_DMA_CONTROL_OFFSET, VIDEO_DMA_CTRL_ENABLE);
        udelay(100);

        if ((ret = video_dma_setup(framebuffer_pa))) {
            printk(KERN_ERR DRV_NAME ": DMA Controller not responding\n");
            goto fail;
        }
    }

    mutex_unlock(&vgabuf.lock);
    return 0;

fail:
    video_dma_reset();
    iowrite32(FPGA_SDRAM_BRIDGE_DISABLE, vgabuf.sdram_bridge_base);
    barrier();
    iowrite32(FPGA_SDRAM_BRIDGE_DISABLE, vgabuf.sdram_bridge_base);
    mutex_unlock(&vgabuf.lock);
    return ret;
}

static int cpen391_vgabuffer_frelease(struct inode *inode, struct file *filp) {
    // Disable the sdram bridge to prevent the DMA controller from dumping random kernel memory
    mutex_lock(&vgabuf.lock);
    video_dma_write_reg(VIDEO_DMA_CONTROL_OFFSET, VIDEO_DMA_CTRL_DISABLE);
    msleep(25);
    iowrite32(FPGA_SDRAM_BRIDGE_DISABLE, vgabuf.sdram_bridge_base);
    barrier();
    iowrite32(FPGA_SDRAM_BRIDGE_DISABLE, vgabuf.sdram_bridge_base);

    mutex_unlock(&vgabuf.lock);
    return 0;
}

static int cpen391_vgabuffer_fmmap(struct file *filp, struct vm_area_struct *vma) {
    mutex_lock(&vgabuf.lock);

    unsigned framebuffer_pages = DIV_ROUND_UP(vgabuf.framebuffer_size, PAGE_SIZE);
    unsigned num_pages = (vma->vm_end - vma->vm_start) / PAGE_SIZE;
    unsigned order = get_order(vma->vm_end - vma->vm_start);

    if (vma->vm_pgoff != 0 || order > vgabuf.framebuffer_order || framebuffer_pages > num_pages) {
        mutex_unlock(&vgabuf.lock);
        printk(KERN_ERR DRV_NAME "Invalid mmap for %d pages\n", num_pages);
        return -EINVAL;
    }
    vma->vm_ops = &cpen391_vgabuffer_vmops;
    vma->vm_flags |= VM_DONTDUMP | VM_DONTEXPAND | VM_LOCKED;

    mutex_unlock(&vgabuf.lock);

    cpen391_vgabuffer_vopen(vma);

    return 0;
}

static const struct file_operations cpen391_vgabuffer_fops = {
	.owner		= THIS_MODULE,
    .open       = cpen391_vgabuffer_fopen,
    .release    = cpen391_vgabuffer_frelease,
    .mmap       = cpen391_vgabuffer_fmmap,
};

static struct miscdevice cpen391_vgabuffer_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DRV_NAME,
    .fops = &cpen391_vgabuffer_fops,
};

static int cpen391_vgabuffer_probe(struct platform_device *pdev) {
    int ret;

    mutex_lock(&vgabuf.lock);

    struct resource *video_dma_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!video_dma_res) {
        printk(KERN_ERR DRV_NAME ": no base address for video dma controller\n");
        ret = -EINVAL;
        goto fail;
    }

    resource_size_t video_dma_span = video_dma_res->end - video_dma_res->start;
    WARN(video_dma_span != VIDEO_DMA_SPAN, DRV_NAME ": span of 0x%x is unexpected for DMA controller, changing to 0x%x\n", video_dma_span, VIDEO_DMA_SPAN);

    struct resource *reset_res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
    if (!reset_res) {
        printk(KERN_ERR DRV_NAME ": no base address for reset controller\n");
        ret = -EINVAL;
        goto fail;
    }

    resource_size_t reset_span = reset_res->end - reset_res->start;
    WARN(reset_span != 4, DRV_NAME ": span of 0x%x is unexpected for reset controller, changing to 0x%x\n", reset_span, 4);

    vgabuf.video_dma_base = ioremap(video_dma_res->start, VIDEO_DMA_SPAN);
    if (!vgabuf.video_dma_base) {
        printk(KERN_ERR DRV_NAME ": could not ioremap the dma controller\n");
        ret = -ENOMEM;
        goto fail;
    }
    printk(KERN_ALERT DRV_NAME ": DMA Base Address at 0x%x\n", video_dma_res->start);

    vgabuf.reset_base = ioremap(reset_res->start, 4);
    if (!vgabuf.reset_base) {
        printk(KERN_ERR DRV_NAME ": could not ioremap the reset controller\n");
        ret = -ENOMEM;
        goto fail_iounmap_video;
    }

    vgabuf.sdram_bridge_base = ioremap(FPGA_SDRAM_BRIDGE_REGISTER, 4);
    if (!vgabuf.sdram_bridge_base) {
        printk(KERN_ERR DRV_NAME ": could not ioremap the sdram bridge\n");
        ret = -ENOMEM;
        goto fail_iounmap_reset;
    };

    u32 resolution_reg = video_dma_read_reg(VIDEO_DMA_RESOLUTION_OFFSET);
    vgabuf.width = VIDEO_DMA_RES_X(resolution_reg);
    vgabuf.height = VIDEO_DMA_RES_Y(resolution_reg);
    if (vgabuf.width == 0 || vgabuf.height == 0) {
        printk(KERN_ERR DRV_NAME ": framebuffer has invalid dimensions %d x %d\n", vgabuf.width, vgabuf.height);
        ret = -EINVAL;
        goto fail_iounmap;
    }
    printk(KERN_ALERT DRV_NAME ": DMA Controller has dimensions %d x %d\n", vgabuf.width, vgabuf.height);

    u32 status_reg = video_dma_read_reg(VIDEO_DMA_STATUS_OFFSET);
    unsigned color_bits = VIDEO_DMA_STAT_COLOR_BITS(status_reg);
    unsigned color_planes = VIDEO_DMA_STAT_COLOR_PLANES(status_reg);
    vgabuf.bytes_per_pixel = video_dma_calc_bytes_per_pixel(color_bits, color_planes);
    if (vgabuf.bytes_per_pixel == 0) {
        printk(KERN_ERR DRV_NAME ": Bad color configuration: %d bits x %d planes\n", color_bits, color_planes);
        ret = -EINVAL;
        goto fail_iounmap;
    }
    printk(KERN_ALERT DRV_NAME ": DMA Controller color: %d bits x %d planes, %d bytes per pixel\n",
            color_bits, color_planes, vgabuf.bytes_per_pixel);

    vgabuf.framebuffer_size = (VIDEO_DMA_STAT_CONSECUTIVE_MODE(status_reg) ?
        vgabuf.width * vgabuf.height :
        roundup_pow_of_two(vgabuf.width) * roundup_pow_of_two(vgabuf.height))
        * vgabuf.bytes_per_pixel;
    
    vgabuf.framebuffer_order = get_order(vgabuf.framebuffer_size);
    if (vgabuf.framebuffer_order <= 0 || MAX_ORDER < vgabuf.framebuffer_order) {
        printk(KERN_ERR DRV_NAME ": framebuffer order invalid or too large, order = %d\n", vgabuf.framebuffer_order);
        ret = -EINVAL;
        goto fail_iounmap;
    }

    printk(KERN_ALERT DRV_NAME ": DMA Controller framebuffer of size %d, order %d\n", vgabuf.framebuffer_size, vgabuf.framebuffer_order);

    vgabuf.framebuffer = (void __kernel *) __get_free_pages(__GFP_DIRECT_RECLAIM|__GFP_REPEAT, vgabuf.framebuffer_order);
    if (!vgabuf.framebuffer) {
        printk(KERN_ERR DRV_NAME ": framebuffer not allocated, giving up...\n");
        ret = -ENOMEM;
        goto fail_iounmap;
    }

    ret = misc_register(&cpen391_vgabuffer_device);
    if (ret) {
        printk(KERN_ERR DRV_NAME ": failed to register vgabuffer device\n");
        goto fail_free_framebuffer;
    }

    vgabuf.in_use = true;
    printk(KERN_ALERT DRV_NAME ": Successfully probed vgabuffer\n");
    mutex_unlock(&vgabuf.lock);
    return 0;

fail_free_framebuffer:
    __free_pages(vgabuf.framebuffer, vgabuf.framebuffer_order);
    vgabuf.framebuffer = NULL;
fail_iounmap:
    iounmap(vgabuf.sdram_bridge_base);
    vgabuf.sdram_bridge_base = NULL;
fail_iounmap_reset:
    iounmap(vgabuf.reset_base);
    vgabuf.reset_base = NULL;
fail_iounmap_video:
    iounmap(vgabuf.video_dma_base);
    vgabuf.video_dma_base = NULL;
fail:
    mutex_unlock(&vgabuf.lock);
    return ret;
}

static int cpen391_vgabuffer_remove(struct platform_device *pdev) {
    mutex_lock(&vgabuf.lock);
    vgabuf.in_use = false;

    misc_deregister(&cpen391_vgabuffer_device);
    __free_pages(vgabuf.framebuffer, vgabuf.framebuffer_order);
    vgabuf.framebuffer = NULL;
    iounmap(vgabuf.sdram_bridge_base);
    vgabuf.sdram_bridge_base = NULL;
    iounmap(vgabuf.reset_base);
    vgabuf.reset_base = NULL;
    iounmap(vgabuf.video_dma_base);
    vgabuf.video_dma_base = NULL;

    mutex_unlock(&vgabuf.lock);
    return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id cpen391_vgabuffer_match[] = {
	{ .compatible = "cpen391_group5,vgabuffer", },
	{},
};
MODULE_DEVICE_TABLE(of, cpen391_vgabuffer_match);

#else
#error cpen391_vgabuffer requires CONFIG_OF
#endif /* CONFIG_OF */

static struct platform_driver cpen391_vgabuffer_platform_device = {
    .probe = cpen391_vgabuffer_probe,
    .remove = cpen391_vgabuffer_remove,
    .driver = {
        .name = DRV_NAME,
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(cpen391_vgabuffer_match),
    },
};

static int __init vgabuffer_init(void) {
    int ret;

    vgabuf.video_dma_base = NULL;
    vgabuf.reset_base = NULL;
    vgabuf.sdram_bridge_base = NULL;
    vgabuf.framebuffer = NULL;
    vgabuf.framebuffer_order = 0;
    vgabuf.framebuffer_size = 0;
    vgabuf.width = 0;
    vgabuf.height = 0;
    vgabuf.bytes_per_pixel = 0;
    vgabuf.in_use = false;
    mutex_init(&vgabuf.lock);

    ret = platform_driver_register(&cpen391_vgabuffer_platform_device);
    if (ret) return ret;

    return ret;

}

static void __exit vgabuffer_exit(void) {
    platform_driver_unregister(&cpen391_vgabuffer_platform_device);
}

MODULE_DESCRIPTION("Altera UP Video DMA Controller RAM Frame Buffer");
MODULE_AUTHOR("Jack Guo, CPEN391 Group 5");
MODULE_LICENSE("GPL");
module_init(vgabuffer_init);
module_exit(vgabuffer_exit);
