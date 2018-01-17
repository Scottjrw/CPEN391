	component cpen391_group5_qsys is
		port (
			led_out_export   : out   std_logic_vector(7 downto 0);                     -- export
			sdram_wire_addr  : out   std_logic_vector(12 downto 0);                    -- addr
			sdram_wire_ba    : out   std_logic_vector(1 downto 0);                     -- ba
			sdram_wire_cas_n : out   std_logic;                                        -- cas_n
			sdram_wire_cke   : out   std_logic;                                        -- cke
			sdram_wire_cs_n  : out   std_logic;                                        -- cs_n
			sdram_wire_dq    : inout std_logic_vector(15 downto 0) := (others => 'X'); -- dq
			sdram_wire_dqm   : out   std_logic_vector(1 downto 0);                     -- dqm
			sdram_wire_ras_n : out   std_logic;                                        -- ras_n
			sdram_wire_we_n  : out   std_logic;                                        -- we_n
			switch_in_export : in    std_logic_vector(7 downto 0)  := (others => 'X'); -- export
			sdram_clk_clk    : out   std_logic;                                        -- clk
			reset_reset      : in    std_logic                     := 'X';             -- reset
			clk_clk          : in    std_logic                     := 'X'              -- clk
		);
	end component cpen391_group5_qsys;

	u0 : component cpen391_group5_qsys
		port map (
			led_out_export   => CONNECTED_TO_led_out_export,   --    led_out.export
			sdram_wire_addr  => CONNECTED_TO_sdram_wire_addr,  -- sdram_wire.addr
			sdram_wire_ba    => CONNECTED_TO_sdram_wire_ba,    --           .ba
			sdram_wire_cas_n => CONNECTED_TO_sdram_wire_cas_n, --           .cas_n
			sdram_wire_cke   => CONNECTED_TO_sdram_wire_cke,   --           .cke
			sdram_wire_cs_n  => CONNECTED_TO_sdram_wire_cs_n,  --           .cs_n
			sdram_wire_dq    => CONNECTED_TO_sdram_wire_dq,    --           .dq
			sdram_wire_dqm   => CONNECTED_TO_sdram_wire_dqm,   --           .dqm
			sdram_wire_ras_n => CONNECTED_TO_sdram_wire_ras_n, --           .ras_n
			sdram_wire_we_n  => CONNECTED_TO_sdram_wire_we_n,  --           .we_n
			switch_in_export => CONNECTED_TO_switch_in_export, --  switch_in.export
			sdram_clk_clk    => CONNECTED_TO_sdram_clk_clk,    --  sdram_clk.clk
			reset_reset      => CONNECTED_TO_reset_reset,      --      reset.reset
			clk_clk          => CONNECTED_TO_clk_clk           --        clk.clk
		);

