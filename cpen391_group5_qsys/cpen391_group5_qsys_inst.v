	cpen391_group5_qsys u0 (
		.clk_clk                        (<connected-to-clk_clk>),                        //              clk.clk
		.led_out_export                 (<connected-to-led_out_export>),                 //          led_out.export
		.reset_reset                    (<connected-to-reset_reset>),                    //            reset.reset
		.sdram_clk_clk                  (<connected-to-sdram_clk_clk>),                  //        sdram_clk.clk
		.sdram_wire_addr                (<connected-to-sdram_wire_addr>),                //       sdram_wire.addr
		.sdram_wire_ba                  (<connected-to-sdram_wire_ba>),                  //                 .ba
		.sdram_wire_cas_n               (<connected-to-sdram_wire_cas_n>),               //                 .cas_n
		.sdram_wire_cke                 (<connected-to-sdram_wire_cke>),                 //                 .cke
		.sdram_wire_cs_n                (<connected-to-sdram_wire_cs_n>),                //                 .cs_n
		.sdram_wire_dq                  (<connected-to-sdram_wire_dq>),                  //                 .dq
		.sdram_wire_dqm                 (<connected-to-sdram_wire_dqm>),                 //                 .dqm
		.sdram_wire_ras_n               (<connected-to-sdram_wire_ras_n>),               //                 .ras_n
		.sdram_wire_we_n                (<connected-to-sdram_wire_we_n>),                //                 .we_n
		.switch_in_export               (<connected-to-switch_in_export>),               //        switch_in.export
		.touchscreen_rxd                (<connected-to-touchscreen_rxd>),                //      touchscreen.rxd
		.touchscreen_txd                (<connected-to-touchscreen_txd>),                //                 .txd
		.vga_controller_CLK             (<connected-to-vga_controller_CLK>),             //   vga_controller.CLK
		.vga_controller_HS              (<connected-to-vga_controller_HS>),              //                 .HS
		.vga_controller_VS              (<connected-to-vga_controller_VS>),              //                 .VS
		.vga_controller_BLANK           (<connected-to-vga_controller_BLANK>),           //                 .BLANK
		.vga_controller_SYNC            (<connected-to-vga_controller_SYNC>),            //                 .SYNC
		.vga_controller_R               (<connected-to-vga_controller_R>),               //                 .R
		.vga_controller_G               (<connected-to-vga_controller_G>),               //                 .G
		.vga_controller_B               (<connected-to-vga_controller_B>),               //                 .B
		.video_in_decoder_TD_CLK27      (<connected-to-video_in_decoder_TD_CLK27>),      // video_in_decoder.TD_CLK27
		.video_in_decoder_TD_DATA       (<connected-to-video_in_decoder_TD_DATA>),       //                 .TD_DATA
		.video_in_decoder_TD_HS         (<connected-to-video_in_decoder_TD_HS>),         //                 .TD_HS
		.video_in_decoder_TD_VS         (<connected-to-video_in_decoder_TD_VS>),         //                 .TD_VS
		.video_in_decoder_clk27_reset   (<connected-to-video_in_decoder_clk27_reset>),   //                 .clk27_reset
		.video_in_decoder_TD_RESET      (<connected-to-video_in_decoder_TD_RESET>),      //                 .TD_RESET
		.video_in_decoder_overflow_flag (<connected-to-video_in_decoder_overflow_flag>), //                 .overflow_flag
		.video_rxd                      (<connected-to-video_rxd>),                      //            video.rxd
		.video_txd                      (<connected-to-video_txd>)                       //                 .txd
	);

