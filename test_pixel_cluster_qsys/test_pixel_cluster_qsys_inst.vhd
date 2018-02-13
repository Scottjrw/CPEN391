	component test_pixel_cluster_qsys is
		port (
			clk_clk            : in  std_logic                    := 'X'; -- clk
			reset_reset_n      : in  std_logic                    := 'X'; -- reset_n
			pixel_cluster_leds : out std_logic_vector(9 downto 0)         -- leds
		);
	end component test_pixel_cluster_qsys;

	u0 : component test_pixel_cluster_qsys
		port map (
			clk_clk            => CONNECTED_TO_clk_clk,            --           clk.clk
			reset_reset_n      => CONNECTED_TO_reset_reset_n,      --         reset.reset_n
			pixel_cluster_leds => CONNECTED_TO_pixel_cluster_leds  -- pixel_cluster.leds
		);

