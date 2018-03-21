module mm_reset
(
    clk,
    reset,

    mm_address,
    mm_byteenable,
    mm_read,
    mm_write,
    mm_readdata,
    mm_writedata,

    reset0,
    reset1,
    reset2,
    reset3
);
input clk, reset;

// Pixel Cluster Regs
parameter N_REGS = 4;
parameter CYCLES = 100;
parameter ADDR_BITS = 2;
parameter REG_BITS = 32;

// Avalon MM Slave
input logic [ADDR_BITS-1:0] mm_address;
input logic [REG_BITS/8-1:0] mm_byteenable;
input logic mm_read;
input logic mm_write;
output logic [REG_BITS-1:0] mm_readdata;
input logic [REG_BITS-1:0] mm_writedata;

output logic reset0;
output logic reset1;
output logic reset2;
output logic reset3;

// -- End of IO --

//typedef logic [REG_BITS-1:0] MMRegister;

logic [(REG_BITS*N_REGS)-1:0] regs_read_regs;
logic [(REG_BITS*N_REGS)-1:0] regs_write_regs;

logic [N_REGS-1:0] regs_read_trigger;
logic [N_REGS-1:0] regs_write_trigger;

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

    .read_regs(regs_read_regs),
    .write_regs(regs_write_regs),

    .read_trigger(regs_read_trigger),
    .write_trigger(regs_write_trigger)
);

assign regs_read_regs[REG_BITS*0] = reset0;
mm_reset_hold #(.CYCLES(CYCLES)) inst0(
    .clk(clk),
    .reset(reset),
    .in(regs_write_trigger[0]),
    .out(reset0)
);

assign regs_read_regs[REG_BITS*1] = reset1;
mm_reset_hold #(.CYCLES(CYCLES)) inst1(
    .clk(clk),
    .reset(reset),
    .in(regs_write_trigger[1]),
    .out(reset1)
);

assign regs_read_regs[REG_BITS*2] = reset2;
mm_reset_hold #(.CYCLES(CYCLES)) inst2(
    .clk(clk),
    .reset(reset),
    .in(regs_write_trigger[2]),
    .out(reset2)
);

assign regs_read_regs[REG_BITS*3] = reset3;
mm_reset_hold #(.CYCLES(CYCLES)) inst3(
    .clk(clk),
    .reset(reset),
    .in(regs_write_trigger[3]),
    .out(reset3)
);

endmodule

module mm_reset_hold
#(
    parameter CYCLES=2
)(
    clk,
    reset,
    in,
    out
);

input logic clk, reset;
input logic in;
output logic out;

logic [$clog2(CYCLES)-1:0] count;

always_ff @(posedge clk) begin
    if (in) begin
        count <= 1;
        out <= 1;
    end
    else if (count == 0) begin
        out <= 0;
    end else begin
        count <= count + 1;
    end
end

endmodule
