module pixel_cluster
//#(
//)
(
    clk,
    reset,
    mm_address,
    mm_byteenable,
    mm_read,
    mm_write,
    mm_readdata,
    mm_writedata,

    leds
);
input clk, reset;


// Pixel Cluster Regs
parameter N_REGS = 2;
parameter ADDR_BITS = 4;
parameter REG_BITS = 32;

input logic [ADDR_BITS-1:0] mm_address;
input logic [REG_BITS/8-1:0] mm_byteenable;
input logic mm_read;
input logic mm_write;
output logic [REG_BITS-1:0] mm_readdata;
input logic [REG_BITS-1:0] mm_writedata;

output logic [9:0] leds;
wire [REG_BITS*N_REGS-1:0] wr_regs_out;

always_comb begin
    for (int i = 0; i < 8; i++) begin
        leds[i] = wr_regs_out[i*8];
    end
end

pixel_cluster_regs
#(
    .N(N_REGS),
    .ADDR_BITS(ADDR_BITS),
    .REG_BITS(REG_BITS)
) regs_inst (
    .clk(clk),
    .reset(reset),
    .mm_address(mm_address),
    .mm_byteenable(mm_byteenable),
    .mm_read(mm_read),
    .mm_write(mm_write),
    .mm_readdata(mm_readdata),
    .mm_writedata(mm_writedata),

    .read_regs({32'hAABBCCDD, 32'h11223344}),
    .write_regs(wr_regs_out)
);

endmodule


/*
Module to manage the Avalon MM interface
Has N 32-bit registers for read and write

*/
module pixel_cluster_regs
#(
    // Number of Read and Write Registers
    parameter N = 16,
    parameter ADDR_BITS = 4,
    parameter REG_BITS = 32,
    parameter TRIGGER_REG = 0
)(
    clk,
    reset,

    // Registers
    read_regs,
    write_regs,

    // Avalon MM Signals
    mm_address,
    mm_byteenable,
    mm_read,
    mm_write,
    mm_readdata,
    mm_writedata,

    trigger

);

input logic clk, reset;

// Avalon MM
input logic [ADDR_BITS-1:0] mm_address;
input logic [REG_BITS/8-1:0] mm_byteenable;
input logic mm_read;
input logic mm_write;
output logic [REG_BITS-1:0] mm_readdata;
input logic [REG_BITS-1:0] mm_writedata;

input logic [(REG_BITS*N)-1:0] read_regs;
output logic [(REG_BITS*N)-1:0] write_regs;

output logic trigger;

always_ff @(posedge clk) begin
    trigger = 1'b0;

    if (reset) begin
        for (int i = 0; i < N; i++)
            write_regs[i] <= 0;
    end
    else if (mm_write) begin
        if (mm_address == TRIGGER_REG)
            trigger = 1'b1;

        for (int i = 0; i < REG_BITS/8; i++) begin
            if (mm_byteenable[i]) begin
                write_regs[mm_address*32 + i*8 +: 8] <= mm_writedata[i*8 +: 8];
            end
        end
    end
    else if (mm_read) begin
        for (int i = 0; i < REG_BITS/8; i++) begin
            mm_readdata[i*8 +: 8] <= {8{mm_byteenable[i]}} & read_regs[mm_address*32 + i*8 +: 8];
        end
    end
end

endmodule
