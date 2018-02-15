
module cpen391_group5_qsys (
	clk_clk,
	led_out_export,
	reset_reset,
	sdram_clk_clk,
	sdram_wire_addr,
	sdram_wire_ba,
	sdram_wire_cas_n,
	sdram_wire_cke,
	sdram_wire_cs_n,
	sdram_wire_dq,
	sdram_wire_dqm,
	sdram_wire_ras_n,
	sdram_wire_we_n,
	switch_in_export,
	touchscreen_rxd,
	touchscreen_txd,
	vga_controller_CLK,
	vga_controller_HS,
	vga_controller_VS,
	vga_controller_BLANK,
	vga_controller_SYNC,
	vga_controller_R,
	vga_controller_G,
	vga_controller_B,
	video_rxd,
	video_txd,
	video_in_decoder_TD_CLK27,
	video_in_decoder_TD_DATA,
	video_in_decoder_TD_HS,
	video_in_decoder_TD_VS,
	video_in_decoder_clk27_reset,
	video_in_decoder_TD_RESET,
	video_in_decoder_overflow_flag,
	wifi_rxd,
	wifi_txd);	

	input		clk_clk;
	output	[7:0]	led_out_export;
	input		reset_reset;
	output		sdram_clk_clk;
	output	[12:0]	sdram_wire_addr;
	output	[1:0]	sdram_wire_ba;
	output		sdram_wire_cas_n;
	output		sdram_wire_cke;
	output		sdram_wire_cs_n;
	inout	[15:0]	sdram_wire_dq;
	output	[1:0]	sdram_wire_dqm;
	output		sdram_wire_ras_n;
	output		sdram_wire_we_n;
	input	[7:0]	switch_in_export;
	input		touchscreen_rxd;
	output		touchscreen_txd;
	output		vga_controller_CLK;
	output		vga_controller_HS;
	output		vga_controller_VS;
	output		vga_controller_BLANK;
	output		vga_controller_SYNC;
	output	[7:0]	vga_controller_R;
	output	[7:0]	vga_controller_G;
	output	[7:0]	vga_controller_B;
	input		video_rxd;
	output		video_txd;
	input		video_in_decoder_TD_CLK27;
	input	[7:0]	video_in_decoder_TD_DATA;
	input		video_in_decoder_TD_HS;
	input		video_in_decoder_TD_VS;
	input		video_in_decoder_clk27_reset;
	output		video_in_decoder_TD_RESET;
	output		video_in_decoder_overflow_flag;
	input		wifi_rxd;
	output		wifi_txd;
endmodule
