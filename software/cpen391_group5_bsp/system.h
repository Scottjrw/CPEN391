/*
 * system.h - SOPC Builder system and BSP software package information
 *
 * Machine generated for CPU 'nios2' in SOPC Builder design 'cpen391_group5_qsys'
 * SOPC Builder design path: ../../cpen391_group5_qsys.sopcinfo
 *
 * Generated: Thu Feb 15 03:09:30 PST 2018
 */

/*
 * DO NOT MODIFY THIS FILE
 *
 * Changing this file will have subtle consequences
 * which will almost certainly lead to a nonfunctioning
 * system. If you do modify this file, be aware that your
 * changes will be overwritten and lost when this file
 * is generated again.
 *
 * DO NOT MODIFY THIS FILE
 */

/*
 * License Agreement
 *
 * Copyright (c) 2008
 * Altera Corporation, San Jose, California, USA.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * This agreement shall be governed in all respects by the laws of the State
 * of California and by the laws of the United States of America.
 */

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

/* Include definitions from linker script generator */
#include "linker.h"


/*
 * CPU configuration
 *
 */

#define ALT_CPU_ARCHITECTURE "altera_nios2_qsys"
#define ALT_CPU_BIG_ENDIAN 0
#define ALT_CPU_BREAK_ADDR 0x00090820
#define ALT_CPU_CPU_FREQ 50000000u
#define ALT_CPU_CPU_ID_SIZE 1
#define ALT_CPU_CPU_ID_VALUE 0x00000000
#define ALT_CPU_CPU_IMPLEMENTATION "fast"
#define ALT_CPU_DATA_ADDR_WIDTH 0x1c
#define ALT_CPU_DCACHE_LINE_SIZE 32
#define ALT_CPU_DCACHE_LINE_SIZE_LOG2 5
#define ALT_CPU_DCACHE_SIZE 1024
#define ALT_CPU_EXCEPTION_ADDR 0x08000020
#define ALT_CPU_FLUSHDA_SUPPORTED
#define ALT_CPU_FREQ 50000000
#define ALT_CPU_HARDWARE_DIVIDE_PRESENT 0
#define ALT_CPU_HARDWARE_MULTIPLY_PRESENT 1
#define ALT_CPU_HARDWARE_MULX_PRESENT 0
#define ALT_CPU_HAS_DEBUG_CORE 1
#define ALT_CPU_HAS_DEBUG_STUB
#define ALT_CPU_HAS_JMPI_INSTRUCTION
#define ALT_CPU_ICACHE_LINE_SIZE 32
#define ALT_CPU_ICACHE_LINE_SIZE_LOG2 5
#define ALT_CPU_ICACHE_SIZE 1024
#define ALT_CPU_INITDA_SUPPORTED
#define ALT_CPU_INST_ADDR_WIDTH 0x1c
#define ALT_CPU_NAME "nios2"
#define ALT_CPU_NUM_OF_SHADOW_REG_SETS 0
#define ALT_CPU_RESET_ADDR 0x08000000


/*
 * CPU configuration (with legacy prefix - don't use these anymore)
 *
 */

#define NIOS2_BIG_ENDIAN 0
#define NIOS2_BREAK_ADDR 0x00090820
#define NIOS2_CPU_FREQ 50000000u
#define NIOS2_CPU_ID_SIZE 1
#define NIOS2_CPU_ID_VALUE 0x00000000
#define NIOS2_CPU_IMPLEMENTATION "fast"
#define NIOS2_DATA_ADDR_WIDTH 0x1c
#define NIOS2_DCACHE_LINE_SIZE 32
#define NIOS2_DCACHE_LINE_SIZE_LOG2 5
#define NIOS2_DCACHE_SIZE 1024
#define NIOS2_EXCEPTION_ADDR 0x08000020
#define NIOS2_FLUSHDA_SUPPORTED
#define NIOS2_HARDWARE_DIVIDE_PRESENT 0
#define NIOS2_HARDWARE_MULTIPLY_PRESENT 1
#define NIOS2_HARDWARE_MULX_PRESENT 0
#define NIOS2_HAS_DEBUG_CORE 1
#define NIOS2_HAS_DEBUG_STUB
#define NIOS2_HAS_JMPI_INSTRUCTION
#define NIOS2_ICACHE_LINE_SIZE 32
#define NIOS2_ICACHE_LINE_SIZE_LOG2 5
#define NIOS2_ICACHE_SIZE 1024
#define NIOS2_INITDA_SUPPORTED
#define NIOS2_INST_ADDR_WIDTH 0x1c
#define NIOS2_NUM_OF_SHADOW_REG_SETS 0
#define NIOS2_RESET_ADDR 0x08000000


/*
 * Define for each module class mastered by the CPU
 *
 */

#define __ALTERA_AVALON_JTAG_UART
#define __ALTERA_AVALON_NEW_SDRAM_CONTROLLER
#define __ALTERA_AVALON_ONCHIP_MEMORY2
#define __ALTERA_AVALON_PIO
#define __ALTERA_AVALON_TIMER
#define __ALTERA_AVALON_UART
#define __ALTERA_NIOS2_QSYS
#define __ALTERA_UP_AVALON_VIDEO_DMA_CONTROLLER
#define __GRAPHICS_CONTROLLER
#define __PIXEL_CLUSTER


/*
 * Draw_Buffer configuration
 *
 */

#define ALT_MODULE_CLASS_Draw_Buffer altera_avalon_onchip_memory2
#define DRAW_BUFFER_ALLOW_IN_SYSTEM_MEMORY_CONTENT_EDITOR 0
#define DRAW_BUFFER_ALLOW_MRAM_SIM_CONTENTS_ONLY_FILE 0
#define DRAW_BUFFER_BASE 0x80000
#define DRAW_BUFFER_CONTENTS_INFO ""
#define DRAW_BUFFER_DUAL_PORT 1
#define DRAW_BUFFER_GUI_RAM_BLOCK_TYPE "AUTO"
#define DRAW_BUFFER_INIT_CONTENTS_FILE "cpen391_group5_qsys_Draw_Buffer"
#define DRAW_BUFFER_INIT_MEM_CONTENT 0
#define DRAW_BUFFER_INSTANCE_ID "NONE"
#define DRAW_BUFFER_IRQ -1
#define DRAW_BUFFER_IRQ_INTERRUPT_CONTROLLER_ID -1
#define DRAW_BUFFER_NAME "/dev/Draw_Buffer"
#define DRAW_BUFFER_NON_DEFAULT_INIT_FILE_ENABLED 0
#define DRAW_BUFFER_RAM_BLOCK_TYPE "AUTO"
#define DRAW_BUFFER_READ_DURING_WRITE_MODE "DONT_CARE"
#define DRAW_BUFFER_SINGLE_CLOCK_OP 1
#define DRAW_BUFFER_SIZE_MULTIPLE 1
#define DRAW_BUFFER_SIZE_VALUE 38400
#define DRAW_BUFFER_SPAN 38400
#define DRAW_BUFFER_TYPE "altera_avalon_onchip_memory2"
#define DRAW_BUFFER_WRITABLE 1


/*
 * Draw_DMA configuration
 *
 */

#define ALT_MODULE_CLASS_Draw_DMA altera_up_avalon_video_dma_controller
#define DRAW_DMA_BASE 0x910c0
#define DRAW_DMA_IRQ -1
#define DRAW_DMA_IRQ_INTERRUPT_CONTROLLER_ID -1
#define DRAW_DMA_NAME "/dev/Draw_DMA"
#define DRAW_DMA_SPAN 16
#define DRAW_DMA_TYPE "altera_up_avalon_video_dma_controller"


/*
 * System configuration
 *
 */

#define ALT_DEVICE_FAMILY "Cyclone V"
#define ALT_ENHANCED_INTERRUPT_API_PRESENT
#define ALT_IRQ_BASE NULL
#define ALT_LOG_PORT "/dev/null"
#define ALT_LOG_PORT_BASE 0x0
#define ALT_LOG_PORT_DEV null
#define ALT_LOG_PORT_TYPE ""
#define ALT_NUM_EXTERNAL_INTERRUPT_CONTROLLERS 0
#define ALT_NUM_INTERNAL_INTERRUPT_CONTROLLERS 1
#define ALT_NUM_INTERRUPT_CONTROLLERS 1
#define ALT_STDERR "/dev/jtag_uart_0"
#define ALT_STDERR_BASE 0x910f0
#define ALT_STDERR_DEV jtag_uart_0
#define ALT_STDERR_IS_JTAG_UART
#define ALT_STDERR_PRESENT
#define ALT_STDERR_TYPE "altera_avalon_jtag_uart"
#define ALT_STDIN "/dev/jtag_uart_0"
#define ALT_STDIN_BASE 0x910f0
#define ALT_STDIN_DEV jtag_uart_0
#define ALT_STDIN_IS_JTAG_UART
#define ALT_STDIN_PRESENT
#define ALT_STDIN_TYPE "altera_avalon_jtag_uart"
#define ALT_STDOUT "/dev/jtag_uart_0"
#define ALT_STDOUT_BASE 0x910f0
#define ALT_STDOUT_DEV jtag_uart_0
#define ALT_STDOUT_IS_JTAG_UART
#define ALT_STDOUT_PRESENT
#define ALT_STDOUT_TYPE "altera_avalon_jtag_uart"
#define ALT_SYSTEM_NAME "cpen391_group5_qsys"


/*
 * Video_Frame_Buffer configuration
 *
 */

#define ALT_MODULE_CLASS_Video_Frame_Buffer altera_avalon_onchip_memory2
#define VIDEO_FRAME_BUFFER_ALLOW_IN_SYSTEM_MEMORY_CONTENT_EDITOR 0
#define VIDEO_FRAME_BUFFER_ALLOW_MRAM_SIM_CONTENTS_ONLY_FILE 0
#define VIDEO_FRAME_BUFFER_BASE 0x0
#define VIDEO_FRAME_BUFFER_CONTENTS_INFO ""
#define VIDEO_FRAME_BUFFER_DUAL_PORT 1
#define VIDEO_FRAME_BUFFER_GUI_RAM_BLOCK_TYPE "M10K"
#define VIDEO_FRAME_BUFFER_INIT_CONTENTS_FILE "cpen391_group5_qsys_Video_Frame_Buffer"
#define VIDEO_FRAME_BUFFER_INIT_MEM_CONTENT 0
#define VIDEO_FRAME_BUFFER_INSTANCE_ID "NONE"
#define VIDEO_FRAME_BUFFER_IRQ -1
#define VIDEO_FRAME_BUFFER_IRQ_INTERRUPT_CONTROLLER_ID -1
#define VIDEO_FRAME_BUFFER_NAME "/dev/Video_Frame_Buffer"
#define VIDEO_FRAME_BUFFER_NON_DEFAULT_INIT_FILE_ENABLED 0
#define VIDEO_FRAME_BUFFER_RAM_BLOCK_TYPE "M10K"
#define VIDEO_FRAME_BUFFER_READ_DURING_WRITE_MODE "DONT_CARE"
#define VIDEO_FRAME_BUFFER_SINGLE_CLOCK_OP 1
#define VIDEO_FRAME_BUFFER_SIZE_MULTIPLE 1
#define VIDEO_FRAME_BUFFER_SIZE_VALUE 307200
#define VIDEO_FRAME_BUFFER_SPAN 307200
#define VIDEO_FRAME_BUFFER_TYPE "altera_avalon_onchip_memory2"
#define VIDEO_FRAME_BUFFER_WRITABLE 1


/*
 * Video_In_DMA configuration
 *
 */

#define ALT_MODULE_CLASS_Video_In_DMA altera_up_avalon_video_dma_controller
#define VIDEO_IN_DMA_BASE 0x910e0
#define VIDEO_IN_DMA_IRQ -1
#define VIDEO_IN_DMA_IRQ_INTERRUPT_CONTROLLER_ID -1
#define VIDEO_IN_DMA_NAME "/dev/Video_In_DMA"
#define VIDEO_IN_DMA_SPAN 16
#define VIDEO_IN_DMA_TYPE "altera_up_avalon_video_dma_controller"


/*
 * Video_Out_DMA configuration
 *
 */

#define ALT_MODULE_CLASS_Video_Out_DMA altera_up_avalon_video_dma_controller
#define VIDEO_OUT_DMA_BASE 0x910d0
#define VIDEO_OUT_DMA_IRQ -1
#define VIDEO_OUT_DMA_IRQ_INTERRUPT_CONTROLLER_ID -1
#define VIDEO_OUT_DMA_NAME "/dev/Video_Out_DMA"
#define VIDEO_OUT_DMA_SPAN 16
#define VIDEO_OUT_DMA_TYPE "altera_up_avalon_video_dma_controller"


/*
 * graphics_controller_0 configuration
 *
 */

#define ALT_MODULE_CLASS_graphics_controller_0 graphics_controller
#define GRAPHICS_CONTROLLER_0_BASE 0x4000000
#define GRAPHICS_CONTROLLER_0_IRQ 4
#define GRAPHICS_CONTROLLER_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define GRAPHICS_CONTROLLER_0_NAME "/dev/graphics_controller_0"
#define GRAPHICS_CONTROLLER_0_SPAN 67108864
#define GRAPHICS_CONTROLLER_0_TYPE "graphics_controller"


/*
 * hal configuration
 *
 */

#define ALT_MAX_FD 32
#define ALT_SYS_CLK MAIN_TIMER
#define ALT_TIMESTAMP_CLK MAIN_TIMER


/*
 * jtag_uart_0 configuration
 *
 */

#define ALT_MODULE_CLASS_jtag_uart_0 altera_avalon_jtag_uart
#define JTAG_UART_0_BASE 0x910f0
#define JTAG_UART_0_IRQ 0
#define JTAG_UART_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define JTAG_UART_0_NAME "/dev/jtag_uart_0"
#define JTAG_UART_0_READ_DEPTH 64
#define JTAG_UART_0_READ_THRESHOLD 8
#define JTAG_UART_0_SPAN 8
#define JTAG_UART_0_TYPE "altera_avalon_jtag_uart"
#define JTAG_UART_0_WRITE_DEPTH 64
#define JTAG_UART_0_WRITE_THRESHOLD 8


/*
 * led_out_pio configuration
 *
 */

#define ALT_MODULE_CLASS_led_out_pio altera_avalon_pio
#define LED_OUT_PIO_BASE 0x910b0
#define LED_OUT_PIO_BIT_CLEARING_EDGE_REGISTER 0
#define LED_OUT_PIO_BIT_MODIFYING_OUTPUT_REGISTER 0
#define LED_OUT_PIO_CAPTURE 0
#define LED_OUT_PIO_DATA_WIDTH 8
#define LED_OUT_PIO_DO_TEST_BENCH_WIRING 0
#define LED_OUT_PIO_DRIVEN_SIM_VALUE 0
#define LED_OUT_PIO_EDGE_TYPE "NONE"
#define LED_OUT_PIO_FREQ 50000000
#define LED_OUT_PIO_HAS_IN 0
#define LED_OUT_PIO_HAS_OUT 1
#define LED_OUT_PIO_HAS_TRI 0
#define LED_OUT_PIO_IRQ -1
#define LED_OUT_PIO_IRQ_INTERRUPT_CONTROLLER_ID -1
#define LED_OUT_PIO_IRQ_TYPE "NONE"
#define LED_OUT_PIO_NAME "/dev/led_out_pio"
#define LED_OUT_PIO_RESET_VALUE 0
#define LED_OUT_PIO_SPAN 16
#define LED_OUT_PIO_TYPE "altera_avalon_pio"


/*
 * main_timer configuration
 *
 */

#define ALT_MODULE_CLASS_main_timer altera_avalon_timer
#define MAIN_TIMER_ALWAYS_RUN 0
#define MAIN_TIMER_BASE 0x91080
#define MAIN_TIMER_COUNTER_SIZE 32
#define MAIN_TIMER_FIXED_PERIOD 0
#define MAIN_TIMER_FREQ 50000000
#define MAIN_TIMER_IRQ 3
#define MAIN_TIMER_IRQ_INTERRUPT_CONTROLLER_ID 0
#define MAIN_TIMER_LOAD_VALUE 49999
#define MAIN_TIMER_MULT 0.001
#define MAIN_TIMER_NAME "/dev/main_timer"
#define MAIN_TIMER_PERIOD 1
#define MAIN_TIMER_PERIOD_UNITS "ms"
#define MAIN_TIMER_RESET_OUTPUT 0
#define MAIN_TIMER_SNAPSHOT 1
#define MAIN_TIMER_SPAN 32
#define MAIN_TIMER_TICKS_PER_SEC 1000
#define MAIN_TIMER_TIMEOUT_PULSE_OUTPUT 0
#define MAIN_TIMER_TYPE "altera_avalon_timer"


/*
 * pixel_cluster_0 configuration
 *
 */

#define ALT_MODULE_CLASS_pixel_cluster_0 pixel_cluster
#define PIXEL_CLUSTER_0_BASE 0x91000
#define PIXEL_CLUSTER_0_IRQ 5
#define PIXEL_CLUSTER_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define PIXEL_CLUSTER_0_NAME "/dev/pixel_cluster_0"
#define PIXEL_CLUSTER_0_SPAN 64
#define PIXEL_CLUSTER_0_TYPE "pixel_cluster"


/*
 * sdram configuration
 *
 */

#define ALT_MODULE_CLASS_sdram altera_avalon_new_sdram_controller
#define SDRAM_BASE 0x8000000
#define SDRAM_CAS_LATENCY 3
#define SDRAM_CONTENTS_INFO
#define SDRAM_INIT_NOP_DELAY 0.0
#define SDRAM_INIT_REFRESH_COMMANDS 2
#define SDRAM_IRQ -1
#define SDRAM_IRQ_INTERRUPT_CONTROLLER_ID -1
#define SDRAM_IS_INITIALIZED 1
#define SDRAM_NAME "/dev/sdram"
#define SDRAM_POWERUP_DELAY 100.0
#define SDRAM_REFRESH_PERIOD 7.8125
#define SDRAM_REGISTER_DATA_IN 1
#define SDRAM_SDRAM_ADDR_WIDTH 0x19
#define SDRAM_SDRAM_BANK_WIDTH 2
#define SDRAM_SDRAM_COL_WIDTH 10
#define SDRAM_SDRAM_DATA_WIDTH 16
#define SDRAM_SDRAM_NUM_BANKS 4
#define SDRAM_SDRAM_NUM_CHIPSELECTS 1
#define SDRAM_SDRAM_ROW_WIDTH 13
#define SDRAM_SHARED_DATA 0
#define SDRAM_SIM_MODEL_BASE 0
#define SDRAM_SPAN 67108864
#define SDRAM_STARVATION_INDICATOR 0
#define SDRAM_TRISTATE_BRIDGE_SLAVE ""
#define SDRAM_TYPE "altera_avalon_new_sdram_controller"
#define SDRAM_T_AC 5.4
#define SDRAM_T_MRD 3
#define SDRAM_T_RCD 15.0
#define SDRAM_T_RFC 70.0
#define SDRAM_T_RP 15.0
#define SDRAM_T_WR 14.0


/*
 * switch_in_pio configuration
 *
 */

#define ALT_MODULE_CLASS_switch_in_pio altera_avalon_pio
#define SWITCH_IN_PIO_BASE 0x910a0
#define SWITCH_IN_PIO_BIT_CLEARING_EDGE_REGISTER 0
#define SWITCH_IN_PIO_BIT_MODIFYING_OUTPUT_REGISTER 0
#define SWITCH_IN_PIO_CAPTURE 0
#define SWITCH_IN_PIO_DATA_WIDTH 8
#define SWITCH_IN_PIO_DO_TEST_BENCH_WIRING 0
#define SWITCH_IN_PIO_DRIVEN_SIM_VALUE 0
#define SWITCH_IN_PIO_EDGE_TYPE "NONE"
#define SWITCH_IN_PIO_FREQ 50000000
#define SWITCH_IN_PIO_HAS_IN 1
#define SWITCH_IN_PIO_HAS_OUT 0
#define SWITCH_IN_PIO_HAS_TRI 0
#define SWITCH_IN_PIO_IRQ -1
#define SWITCH_IN_PIO_IRQ_INTERRUPT_CONTROLLER_ID -1
#define SWITCH_IN_PIO_IRQ_TYPE "NONE"
#define SWITCH_IN_PIO_NAME "/dev/switch_in_pio"
#define SWITCH_IN_PIO_RESET_VALUE 0
#define SWITCH_IN_PIO_SPAN 16
#define SWITCH_IN_PIO_TYPE "altera_avalon_pio"


/*
 * touchscreen_uart configuration
 *
 */

#define ALT_MODULE_CLASS_touchscreen_uart altera_avalon_uart
#define TOUCHSCREEN_UART_BASE 0x91060
#define TOUCHSCREEN_UART_BAUD 9600
#define TOUCHSCREEN_UART_DATA_BITS 8
#define TOUCHSCREEN_UART_FIXED_BAUD 1
#define TOUCHSCREEN_UART_FREQ 50000000
#define TOUCHSCREEN_UART_IRQ 1
#define TOUCHSCREEN_UART_IRQ_INTERRUPT_CONTROLLER_ID 0
#define TOUCHSCREEN_UART_NAME "/dev/touchscreen_uart"
#define TOUCHSCREEN_UART_PARITY 'N'
#define TOUCHSCREEN_UART_SIM_CHAR_STREAM ""
#define TOUCHSCREEN_UART_SIM_TRUE_BAUD 0
#define TOUCHSCREEN_UART_SPAN 32
#define TOUCHSCREEN_UART_STOP_BITS 1
#define TOUCHSCREEN_UART_SYNC_REG_DEPTH 2
#define TOUCHSCREEN_UART_TYPE "altera_avalon_uart"
#define TOUCHSCREEN_UART_USE_CTS_RTS 0
#define TOUCHSCREEN_UART_USE_EOP_REGISTER 0


/*
 * video_uart configuration
 *
 */

#define ALT_MODULE_CLASS_video_uart altera_avalon_uart
#define VIDEO_UART_BASE 0x91040
#define VIDEO_UART_BAUD 38400
#define VIDEO_UART_DATA_BITS 8
#define VIDEO_UART_FIXED_BAUD 1
#define VIDEO_UART_FREQ 50000000
#define VIDEO_UART_IRQ 2
#define VIDEO_UART_IRQ_INTERRUPT_CONTROLLER_ID 0
#define VIDEO_UART_NAME "/dev/video_uart"
#define VIDEO_UART_PARITY 'N'
#define VIDEO_UART_SIM_CHAR_STREAM ""
#define VIDEO_UART_SIM_TRUE_BAUD 0
#define VIDEO_UART_SPAN 32
#define VIDEO_UART_STOP_BITS 1
#define VIDEO_UART_SYNC_REG_DEPTH 2
#define VIDEO_UART_TYPE "altera_avalon_uart"
#define VIDEO_UART_USE_CTS_RTS 0
#define VIDEO_UART_USE_EOP_REGISTER 0

#endif /* __SYSTEM_H_ */
