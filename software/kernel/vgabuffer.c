#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/page.h>
#include <linux/mm.h>

// Reset register for the FPGA-HPS SDRAM Master
#define FPGA_SDRAM_MASTER_BASE 0xFFC25080
#define FPGA_SDRAM_ENABLE_READ 0x3FFF
#define FPGA_SDRAM_DISABLE_READ 0x0

// Light Weight Bridge
#define LW_FPGA_BRIDGE_BASE 0xFF200000

// Video DMA Controller Attached to LW Bridge
#define VIDEO_DMA_BASE (LW_FPGA_BRIDGE_BASE+0x0)
#define VIDEO_DMA_SPAN 16
#define VIDEO_DMA_BUFFER_OFFSET 0
#define VIDEO_DMA_BACKBUFFER_OFFSET 4
#define VIDEO_DMA_RESOLUTION_OFFSET 8
#define VIDEO_DMA_STATUS_OFFSET 12
#define VIDEO_DMA_REG(offset) (video_dma_base_addr+offset)

static unsigned vga_width = 640;
static unsigned vga_height = 480;
static unsigned vga_bytes_per_pixel = 4;

static u8 *video_dma_base_addr = NULL;
static volatile void *vgabuffer = NULL;
static unsigned vgabuffer_order = 0;

static void set_pixel(unsigned x, unsigned y, u32 pixel) {
    volatile u32 *pix_addr = ((volatile u32 *) vgabuffer) + y * vga_width + x;

    *pix_addr = pixel;
}

static int set_fpga_sdram_reg(u32 value) {
    void *reg = ioremap_nocache(FPGA_SDRAM_MASTER_BASE, 4);
    if (reg == NULL)
        return -1;
    
    iowrite32(0, reg);
    barrier();
    iowrite32(value, reg);

    iounmap(reg);

    return 0;
}

static void cleanup(void);

static int vgabuffer_init(void) {
    printk(KERN_ALERT "VGA Buffer is beginning initialization\n");

    video_dma_base_addr = ioremap_nocache(VIDEO_DMA_BASE, VIDEO_DMA_SPAN);
    if (video_dma_base_addr == NULL) {
        printk(KERN_ERR "Failed to ioremap device regs\n");
        return -1;
    }

    unsigned vga_buf_size = vga_width * vga_height * vga_bytes_per_pixel;
    int vga_buf_order = get_order(vga_buf_size);

    if (vga_buf_order < 0) {
        printk(KERN_ERR "get_order bad return %d\n", vga_buf_order);
        cleanup();
        return -1;
    }

    printk(KERN_ALERT "VGA Buffer to allocate %u kbytes, order %d\n", vga_buf_size / 1024, vga_buf_order);

    unsigned long pages = __get_free_pages(GFP_KERNEL | __GFP_REPEAT, vga_buf_order);

    if (pages == 0) {
        printk(KERN_ERR "Failed to allocate pages for vgabuffer...\n");
        cleanup();
        return -1;
    }

    vgabuffer = (void *) pages;
    vgabuffer_order = vga_buf_order;

    unsigned long vgabuffer_phys = __pa(vgabuffer);

    printk(KERN_ALERT "Successfully allocated vgabuffer at va 0x%lx, pa 0x%lx\n", pages, vgabuffer_phys);

    if (set_fpga_sdram_reg(FPGA_SDRAM_ENABLE_READ)) {
        printk(KERN_ERR "Failed to write to the fpga sdram reg\n");
        cleanup();
        return -1;
    }

    while (ioread32(VIDEO_DMA_REG(VIDEO_DMA_BUFFER_OFFSET)) != vgabuffer_phys) {
        iowrite32(vgabuffer_phys, VIDEO_DMA_REG(VIDEO_DMA_BACKBUFFER_OFFSET));
        barrier();
        iowrite32(0, VIDEO_DMA_REG(VIDEO_DMA_BUFFER_OFFSET));
    }

    printk(KERN_ALERT "Buffer Register Val: 0x%x", ioread32(VIDEO_DMA_REG(VIDEO_DMA_BUFFER_OFFSET)));

    for (int x = 0; x < vga_width; x++)
        for (int y = 0; y < vga_height; y++)
            set_pixel(x, y, 0x0);

    for (int i = 0; i < 250; i++)
        set_pixel(i, i, 0xFF << 16);

    for (int i = 0; i < 250; i++)
        set_pixel(639 - i, i, 0xFF << 16);

    return 0;

}

static void cleanup(void) {
    if (vgabuffer != NULL)
        free_pages((unsigned long) vgabuffer, vgabuffer_order);

    if (video_dma_base_addr != NULL)
        iounmap(video_dma_base_addr);
}


static void vgabuffer_exit(void) {
    printk(KERN_ALERT "VGA Buffer is exiting\n");

    cleanup();
}

MODULE_LICENSE("Dual BSD/GPL");
module_init(vgabuffer_init);
module_exit(vgabuffer_exit);
module_param(vga_width, uint, S_IRUGO);
MODULE_PARM_DESC(vga_width, "VGA Screen width in pixels");
module_param(vga_height, uint, S_IRUGO);
MODULE_PARM_DESC(vga_height, "VGA Screen height in pixels");
module_param(vga_bytes_per_pixel, uint, S_IRUGO);
MODULE_PARM_DESC(vga_height, "How many bytes a pixel takes up");
