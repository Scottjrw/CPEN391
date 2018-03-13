module cpen391_group5(
    input CLOCK_50, 

    // Buttons, LED
    input [9:0] SW, 
    input [3:0] KEY,
    output [9:0] LEDR,

    // GPIO
    inout [35:0] GPIO_0,
    inout [35:0] GPIO_1,

    // SDRAM
    inout [15:0] DRAM_DQ,
    output [12:0] DRAM_ADDR,
    output [1:0] DRAM_BA,
    output DRAM_CAS_N,
    output DRAM_RAS_N,
    output DRAM_CLK,
    output DRAM_CKE,
    output DRAM_CS_N,
    output DRAM_WE_N,
    output DRAM_UDQM,
    output DRAM_LDQM,


    // VGA
    output VGA_CLK,
    output [7:0] VGA_R,
    output [7:0] VGA_G,
    output [7:0] VGA_B,
    output VGA_BLANK_N,
    output VGA_HS,
    output VGA_VS,
    output VGA_SYNC_N,

    // Video-In
    input TD_CLK27,
    input [7:0] TD_DATA,
    input TD_HS,
    input TD_VS,
    output TD_RESET_N,

    inout HPS_CONV_USB_N,
    output [14:0] HPS_DDR3_ADDR,
    output [2:0] HPS_DDR3_BA,
    output HPS_DDR3_CAS_N,
    output HPS_DDR3_CK_N,
    output HPS_DDR3_CK_P,
    output HPS_DDR3_CKE,
    output HPS_DDR3_CS_N,
    output [3:0] HPS_DDR3_DM,
    inout [31:0] HPS_DDR3_DQ,
    inout [3:0] HPS_DDR3_DQS_N,
    inout [3:0] HPS_DDR3_DQS_P,
    output HPS_DDR3_ODT,
    output HPS_DDR3_RAS_N,
    output HPS_DDR3_RESET_N,
    input HPS_DDR3_RZQ,
    output HPS_DDR3_WE_N,
    output HPS_ENET_GTX_CLK,
    inout HPS_ENET_INT_N,
    output HPS_ENET_MDC,
    inout HPS_ENET_MDIO,
    input HPS_ENET_RX_CLK,
    input [3:0] HPS_ENET_RX_DATA,
    input HPS_ENET_RX_DV,
    output [3:0] HPS_ENET_TX_DATA,
    output HPS_ENET_TX_EN,
    inout [3:0] HPS_FLASH_DATA,
    output HPS_FLASH_DCLK,
    output HPS_FLASH_NCSO,
    inout HPS_GSENSOR_INT,
    inout HPS_I2C_CONTROL,
    inout HPS_I2C1_SCLK,
    inout HPS_I2C1_SDAT,
    inout HPS_I2C2_SCLK,
    inout HPS_I2C2_SDAT,
    inout HPS_KEY_N,
    inout HPS_LED,
    inout HPS_LTC_GPIO,
    output HPS_SD_CLK,
    inout HPS_SD_CMD,
    inout [3:0] HPS_SD_DATA,
    output HPS_SPIM_CLK,
    input HPS_SPIM_MISO,
    output HPS_SPIM_MOSI,
    inout HPS_SPIM_SS,
    input HPS_UART_RX,
    output HPS_UART_TX,
    input HPS_USB_CLKOUT,
    inout [7:0] HPS_USB_DATA,
    input HPS_USB_DIR,
    input HPS_USB_NXT,
    output HPS_USB_STP
);

wire reset = ~KEY[0];

/*
cpen391_group5_qsys sys_inst(
    .clk_clk(CLOCK_50),
    .reset_reset(reset),
    .led_out_export(), // EMPTY
    .switch_in_export(SW[7:0]),

    // SDRAM
    .sdram_clk_clk(DRAM_CLK),
    .sdram_wire_addr(DRAM_ADDR),
    .sdram_wire_ba(DRAM_BA),
    .sdram_wire_cas_n(DRAM_CAS_N),
    .sdram_wire_cke(DRAM_CKE),
    .sdram_wire_cs_n(DRAM_CS_N),
    .sdram_wire_dq(DRAM_DQ),
    .sdram_wire_dqm({DRAM_UDQM, DRAM_LDQM}),
    .sdram_wire_ras_n(DRAM_RAS_N),
    .sdram_wire_we_n(DRAM_WE_N),

    // Touchscreen UART
    .touchscreen_rxd(GPIO_1[33]),
    .touchscreen_txd(GPIO_1[35]),
	 
    .video_rxd(GPIO_1[32]),
    .video_txd(GPIO_1[34]),

    .wifi_rxd(GPIO_1[29]),
    .wifi_txd(GPIO_1[31]),

    // VGA
    .vga_controller_CLK(VGA_CLK),
    .vga_controller_HS(VGA_HS),
    .vga_controller_VS(VGA_VS),
    .vga_controller_BLANK(VGA_BLANK_N),
    .vga_controller_SYNC(VGA_SYNC_N),
    .vga_controller_R(VGA_R),
    .vga_controller_G(VGA_G),
    .vga_controller_B(VGA_B),

    // Video-In
    .video_in_decoder_TD_CLK27(TD_CLK27),
    .video_in_decoder_TD_DATA(TD_DATA),
    .video_in_decoder_TD_HS(TD_HS),
    .video_in_decoder_TD_VS(TD_VS),
    .video_in_decoder_clk27_reset(reset),
    .video_in_decoder_TD_RESET(TD_RESET_N),
    .video_in_decoder_overflow_flag()

);
*/

testhps sys_inst(
    .clk_clk(CLOCK_50),
    .reset_reset_n(~reset),

    // HPS IO
    .hps_io_hps_io_emac1_inst_TX_CLK(HPS_ENET_GTX_CLK),
    .hps_io_hps_io_emac1_inst_TX_CTL(HPS_ENET_TX_EN),
    .hps_io_hps_io_emac1_inst_TXD0(HPS_ENET_TX_DATA[0]),
    .hps_io_hps_io_emac1_inst_TXD1(HPS_ENET_TX_DATA[1]),
    .hps_io_hps_io_emac1_inst_TXD2(HPS_ENET_TX_DATA[2]),
    .hps_io_hps_io_emac1_inst_TXD3(HPS_ENET_TX_DATA[3]),
    .hps_io_hps_io_emac1_inst_RX_CLK(HPS_ENET_RX_CLK),
    .hps_io_hps_io_emac1_inst_RX_CTL(HPS_ENET_RX_DV),
    .hps_io_hps_io_emac1_inst_RXD0(HPS_ENET_RX_DATA[0]),
    .hps_io_hps_io_emac1_inst_RXD1(HPS_ENET_RX_DATA[1]),
    .hps_io_hps_io_emac1_inst_RXD2(HPS_ENET_RX_DATA[2]),
    .hps_io_hps_io_emac1_inst_RXD3(HPS_ENET_RX_DATA[3]),
    .hps_io_hps_io_emac1_inst_MDIO(HPS_ENET_MDIO),
    .hps_io_hps_io_emac1_inst_MDC(HPS_ENET_MDC),
    .hps_io_hps_io_qspi_inst_CLK(HPS_FLASH_DCLK),
    .hps_io_hps_io_qspi_inst_SS0(HPS_FLASH_NCSO),
    .hps_io_hps_io_qspi_inst_IO0(HPS_FLASH_DATA[0]),
    .hps_io_hps_io_qspi_inst_IO1(HPS_FLASH_DATA[1]),
    .hps_io_hps_io_qspi_inst_IO2(HPS_FLASH_DATA[2]),
    .hps_io_hps_io_qspi_inst_IO3(HPS_FLASH_DATA[3]),
    .hps_io_hps_io_sdio_inst_CLK(HPS_SD_CLK),
    .hps_io_hps_io_sdio_inst_CMD(HPS_SD_CMD),
    .hps_io_hps_io_sdio_inst_D0(HPS_SD_DATA[0]),
    .hps_io_hps_io_sdio_inst_D1(HPS_SD_DATA[1]),
    .hps_io_hps_io_sdio_inst_D2(HPS_SD_DATA[2]),
    .hps_io_hps_io_sdio_inst_D3(HPS_SD_DATA[3]),
    .hps_io_hps_io_usb1_inst_CLK(HPS_USB_CLKOUT),
    .hps_io_hps_io_usb1_inst_STP(HPS_USB_STP),
    .hps_io_hps_io_usb1_inst_DIR(HPS_USB_DIR),
    .hps_io_hps_io_usb1_inst_NXT(HPS_USB_NXT),
    .hps_io_hps_io_usb1_inst_D0(HPS_USB_DATA[0]),
    .hps_io_hps_io_usb1_inst_D1(HPS_USB_DATA[1]),
    .hps_io_hps_io_usb1_inst_D2(HPS_USB_DATA[2]),
    .hps_io_hps_io_usb1_inst_D3(HPS_USB_DATA[3]),
    .hps_io_hps_io_usb1_inst_D4(HPS_USB_DATA[4]),
    .hps_io_hps_io_usb1_inst_D5(HPS_USB_DATA[5]),
    .hps_io_hps_io_usb1_inst_D6(HPS_USB_DATA[6]),
    .hps_io_hps_io_usb1_inst_D7(HPS_USB_DATA[7]),
    //hps_io_hps_io_spim1_inst_CLK(HPS_SPIM_CLK),
    //hps_io_hps_io_spim1_inst_MOSI(HPS_SPIM_MOSI),
    //hps_io_hps_io_spim1_inst_MISO(HPS_SPIM_MISO),
    //hps_io_hps_io_spim1_inst_SS0(HPS_SPIM_SS),
    .hps_io_hps_io_uart0_inst_RX(HPS_UART_RX),
    .hps_io_hps_io_uart0_inst_TX(HPS_UART_TX),
    .hps_io_hps_io_i2c0_inst_SDA(HPS_I2C1_SDAT),
    .hps_io_hps_io_i2c0_inst_SCL(HPS_I2C1_SCLK),
    //hps_io_hps_io_i2c1_inst_SDA(HPS_I2C2_SDAT),
    //hps_io_hps_io_i2c1_inst_SCL(HPS_I2C2_SCLK),
    //.hps_io_hps_io_gpio_inst_GPIO09(HPS_CONV_USB_N),
    //.hps_io_hps_io_gpio_inst_GPIO35(HPS_ENET_INT_N),
    //.hps_io_hps_io_gpio_inst_GPIO40(HPS_LTC_GPIO),
    //.hps_io_hps_io_gpio_inst_GPIO48(HPS_I2C_CONTROL),
    //.hps_io_hps_io_gpio_inst_GPIO53(HPS_LED),
    //.hps_io_hps_io_gpio_inst_GPIO54(HPS_KEY_N),
    //.hps_io_hps_io_gpio_inst_GPIO61(HPS_GSENSOR_INT),

    // HPS Memory
    .hps_mem_mem_a(HPS_DDR3_ADDR),
    .hps_mem_mem_ba(HPS_DDR3_BA),
    .hps_mem_mem_ck(HPS_DDR3_CK_P),
    .hps_mem_mem_ck_n(HPS_DDR3_CK_N),
    .hps_mem_mem_cke(HPS_DDR3_CKE),
    .hps_mem_mem_cs_n(HPS_DDR3_CS_N),
    .hps_mem_mem_ras_n(HPS_DDR3_RAS_N),
    .hps_mem_mem_cas_n(HPS_DDR3_CAS_N),
    .hps_mem_mem_we_n(HPS_DDR3_WE_N),
    .hps_mem_mem_reset_n(HPS_DDR3_RESET_N),
    .hps_mem_mem_dq(HPS_DDR3_DQ),
    .hps_mem_mem_dqs(HPS_DDR3_DQS_P),
    .hps_mem_mem_dqs_n(HPS_DDR3_DQS_N),
    .hps_mem_mem_odt(HPS_DDR3_ODT),
    .hps_mem_mem_dm(HPS_DDR3_DM),
    .hps_mem_oct_rzqin(HPS_DDR3_RZQ),

    // VGA
    .vga_CLK(VGA_CLK),
    .vga_HS(VGA_HS),
    .vga_VS(VGA_VS),
    .vga_BLANK(VGA_BLANK_N),
    .vga_SYNC(VGA_SYNC_N),
    .vga_R(VGA_R),
    .vga_G(VGA_G),
    .vga_B(VGA_B)
);

/*
test_pixel_cluster_qsys sys_inst(
    .clk_clk(CLOCK_50),
    .reset_reset_n(~reset),
    .pixel_cluster_leds(LEDR)
);
*/

endmodule
