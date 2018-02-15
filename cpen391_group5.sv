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
    output TD_RESET_N
);

wire reset = ~KEY[0];

cpen391_group5_qsys sys_inst(
    .clk_clk(CLOCK_50),
    .reset_reset(reset),
    .led_out_export(), // EMPTY
    .switch_in_export(SW[7:0]),

    .pixel_cluster_leds(LEDR),

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

/*
test_pixel_cluster_qsys sys2_inst(
    .clk_clk(CLOCK_50),
    .reset_reset_n(~reset),
    .pixel_cluster_leds(LEDR)
);
*/

endmodule
