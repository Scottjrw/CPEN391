module pixel_cluster
#(
    parameter ADDR_BITS = 2,
    parameter BYTE_WIDTH = 4
)(
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

input logic [ADDR_BITS-1:0] mm_address;
input logic [BYTE_WIDTH-1:0] mm_byteenable;
input logic mm_read, mm_write;
input logic [BYTE_WIDTH*8-1:0] mm_writedata;
output logic [BYTE_WIDTH*8-1:0] mm_readdata;

output logic [9:0] leds;

logic [BYTE_WIDTH*8-1:0] buffer[ADDR_BITS**2-1:0];

assign leds = {buffer[0][1:0], buffer[1][1:0], buffer[2][1:0], buffer[3][1:0]};

always_ff @(posedge clk) begin
    if (mm_write) begin
        buffer[mm_address] <= mm_writedata;
    end
    else if (mm_read) begin
        mm_readdata <= buffer[mm_address];
    end
end

endmodule
