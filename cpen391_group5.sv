module cpen391_group5(
    input CLOCK_50, 
    input [9:0] SW, 
    input [3:0] KEY,
    output [9:0] LEDR,
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
    output DRAM_LDQM
);

cpen391_group5_qsys sys_inst(
    .clk_clk(CLOCK_50),
    .reset_reset(~KEY[0]),
    .led_out_export(LEDR[7:0]),
    .switch_in_export(SW[7:0]),
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
);

endmodule
