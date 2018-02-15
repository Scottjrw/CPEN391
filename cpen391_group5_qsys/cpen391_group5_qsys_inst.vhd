	component cpen391_group5_qsys is
		port (
			clk_clk                        : in    std_logic                     := 'X';             -- clk
			led_out_export                 : out   std_logic_vector(7 downto 0);                     -- export
			reset_reset                    : in    std_logic                     := 'X';             -- reset
			sdram_clk_clk                  : out   std_logic;                                        -- clk
			sdram_wire_addr                : out   std_logic_vector(12 downto 0);                    -- addr
			sdram_wire_ba                  : out   std_logic_vector(1 downto 0);                     -- ba
			sdram_wire_cas_n               : out   std_logic;                                        -- cas_n
			sdram_wire_cke                 : out   std_logic;                                        -- cke
			sdram_wire_cs_n                : out   std_logic;                                        -- cs_n
			sdram_wire_dq                  : inout std_logic_vector(15 downto 0) := (others => 'X'); -- dq
			sdram_wire_dqm                 : out   std_logic_vector(1 downto 0);                     -- dqm
			sdram_wire_ras_n               : out   std_logic;                                        -- ras_n
			sdram_wire_we_n                : out   std_logic;                                        -- we_n
			switch_in_export               : in    std_logic_vector(7 downto 0)  := (others => 'X'); -- export
			touchscreen_rxd                : in    std_logic                     := 'X';             -- rxd
			touchscreen_txd                : out   std_logic;                                        -- txd
			vga_controller_CLK             : out   std_logic;                                        -- CLK
			vga_controller_HS              : out   std_logic;                                        -- HS
			vga_controller_VS              : out   std_logic;                                        -- VS
			vga_controller_BLANK           : out   std_logic;                                        -- BLANK
			vga_controller_SYNC            : out   std_logic;                                        -- SYNC
			vga_controller_R               : out   std_logic_vector(7 downto 0);                     -- R
			vga_controller_G               : out   std_logic_vector(7 downto 0);                     -- G
			vga_controller_B               : out   std_logic_vector(7 downto 0);                     -- B
			video_rxd                      : in    std_logic                     := 'X';             -- rxd
			video_txd                      : out   std_logic;                                        -- txd
			video_in_decoder_TD_CLK27      : in    std_logic                     := 'X';             -- TD_CLK27
			video_in_decoder_TD_DATA       : in    std_logic_vector(7 downto 0)  := (others => 'X'); -- TD_DATA
			video_in_decoder_TD_HS         : in    std_logic                     := 'X';             -- TD_HS
			video_in_decoder_TD_VS         : in    std_logic                     := 'X';             -- TD_VS
			video_in_decoder_clk27_reset   : in    std_logic                     := 'X';             -- clk27_reset
			video_in_decoder_TD_RESET      : out   std_logic;                                        -- TD_RESET
			video_in_decoder_overflow_flag : out   std_logic;                                        -- overflow_flag
			wifi_rxd                       : in    std_logic                     := 'X';             -- rxd
			wifi_txd                       : out   std_logic                                         -- txd
		);
	end component cpen391_group5_qsys;

	u0 : component cpen391_group5_qsys
		port map (
			clk_clk                        => CONNECTED_TO_clk_clk,                        --              clk.clk
			led_out_export                 => CONNECTED_TO_led_out_export,                 --          led_out.export
			reset_reset                    => CONNECTED_TO_reset_reset,                    --            reset.reset
			sdram_clk_clk                  => CONNECTED_TO_sdram_clk_clk,                  --        sdram_clk.clk
			sdram_wire_addr                => CONNECTED_TO_sdram_wire_addr,                --       sdram_wire.addr
			sdram_wire_ba                  => CONNECTED_TO_sdram_wire_ba,                  --                 .ba
			sdram_wire_cas_n               => CONNECTED_TO_sdram_wire_cas_n,               --                 .cas_n
			sdram_wire_cke                 => CONNECTED_TO_sdram_wire_cke,                 --                 .cke
			sdram_wire_cs_n                => CONNECTED_TO_sdram_wire_cs_n,                --                 .cs_n
			sdram_wire_dq                  => CONNECTED_TO_sdram_wire_dq,                  --                 .dq
			sdram_wire_dqm                 => CONNECTED_TO_sdram_wire_dqm,                 --                 .dqm
			sdram_wire_ras_n               => CONNECTED_TO_sdram_wire_ras_n,               --                 .ras_n
			sdram_wire_we_n                => CONNECTED_TO_sdram_wire_we_n,                --                 .we_n
			switch_in_export               => CONNECTED_TO_switch_in_export,               --        switch_in.export
			touchscreen_rxd                => CONNECTED_TO_touchscreen_rxd,                --      touchscreen.rxd
			touchscreen_txd                => CONNECTED_TO_touchscreen_txd,                --                 .txd
			vga_controller_CLK             => CONNECTED_TO_vga_controller_CLK,             --   vga_controller.CLK
			vga_controller_HS              => CONNECTED_TO_vga_controller_HS,              --                 .HS
			vga_controller_VS              => CONNECTED_TO_vga_controller_VS,              --                 .VS
			vga_controller_BLANK           => CONNECTED_TO_vga_controller_BLANK,           --                 .BLANK
			vga_controller_SYNC            => CONNECTED_TO_vga_controller_SYNC,            --                 .SYNC
			vga_controller_R               => CONNECTED_TO_vga_controller_R,               --                 .R
			vga_controller_G               => CONNECTED_TO_vga_controller_G,               --                 .G
			vga_controller_B               => CONNECTED_TO_vga_controller_B,               --                 .B
			video_rxd                      => CONNECTED_TO_video_rxd,                      --            video.rxd
			video_txd                      => CONNECTED_TO_video_txd,                      --                 .txd
			video_in_decoder_TD_CLK27      => CONNECTED_TO_video_in_decoder_TD_CLK27,      -- video_in_decoder.TD_CLK27
			video_in_decoder_TD_DATA       => CONNECTED_TO_video_in_decoder_TD_DATA,       --                 .TD_DATA
			video_in_decoder_TD_HS         => CONNECTED_TO_video_in_decoder_TD_HS,         --                 .TD_HS
			video_in_decoder_TD_VS         => CONNECTED_TO_video_in_decoder_TD_VS,         --                 .TD_VS
			video_in_decoder_clk27_reset   => CONNECTED_TO_video_in_decoder_clk27_reset,   --                 .clk27_reset
			video_in_decoder_TD_RESET      => CONNECTED_TO_video_in_decoder_TD_RESET,      --                 .TD_RESET
			video_in_decoder_overflow_flag => CONNECTED_TO_video_in_decoder_overflow_flag, --                 .overflow_flag
			wifi_rxd                       => CONNECTED_TO_wifi_rxd,                       --             wifi.rxd
			wifi_txd                       => CONNECTED_TO_wifi_txd                        --                 .txd
		);

