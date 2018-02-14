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
    .write_regs(wr_regs_out),
    .trigger()
);

endmodule


module pixel_cluster_avalon_st
#(
    parameter COLOR_BITS = 8,
    parameter X_MAX = 320,
    parameter Y_MAX = 240,
    parameter X_Y_BITS = 16
)(
    clk,
    reset,

    st_ready,
    st_startofpacket,
    st_endofpacket,
    st_valid,
    st_data,

    pixel_stream,
    pixel_valid,
    pixel_x,
    pixel_y,
    cluster_reset
);

parameter N_COLORS = 3;

input logic clk;
input logic reset;

output logic st_ready;
input logic st_startofpacket;
input logic st_endofpacket;
input logic st_valid;
input logic [COLOR_BITS*N_COLORS-1:0] st_data;

output logic [COLOR_BITS*N_COLORS-1:0] pixel_stream;
output logic pixel_valid;
output logic pixel_x;
output logic pixel_y;
output logic cluster_reset;


endmodule

/*
 Filters pixels based on their RGB color values
 The pixel_valid_out will be high only when pixel_valid_in is high
 and the pixel matches the compares.
 

 */
module pixel_cluster_color_filter
#(
    parameter COLOR_BITS = 8
)(
    pixel_stream_in,
    pixel_valid_in,
    b_cmp, g_cmp, r_cmp,
    compare,
    less_than,
    pixel_valid_out
);

parameter N_COLORS = 3;

// Stream of ST pixels
input logic [N_COLORS*COLOR_BITS-1:0] pixel_stream_in;

// Avalon ST valid signal
input logic pixel_valid_in;

// Compare values for RGB values
input logic [COLOR_BITS-1:0] b_cmp, g_cmp, r_cmp;

// Whether or not to compare this color
input logic [N_COLORS-1:0] compare;

// 0 is stream >= cmp, 1 is stream < cmp, "stream value is less than compare value"
input logic [N_COLORS-1:0] less_than;

output logic pixel_valid_out;

// -- End of IO --

logic [N_COLORS-1:0] compare_ok;

always_comb begin
    compare_ok[0] = (pixel_stream_in[0*COLOR_BITS +: COLOR_BITS] < b_cmp) ? less_than[0] : ~less_than[0];
    compare_ok[1] = (pixel_stream_in[1*COLOR_BITS +: COLOR_BITS] < g_cmp) ? less_than[1] : ~less_than[1];
    compare_ok[2] = (pixel_stream_in[2*COLOR_BITS +: COLOR_BITS] < r_cmp) ? less_than[2] : ~less_than[2];

    pixel_valid_out = (&(compare_ok | ~compare)) & pixel_valid_in;
    pixel_stream_out = pixel_stream_in;
end

endmodule


/*
 Clusters input x,y points into N_CLUSTERS clusters
 Pixels are considered in a cluster if they are within range

 This module needs some serious pipelining to achieve the 1 pixel/clk rate
 at which the video IP cores send (without buffering).

 */
module pixel_cluster_x_y_clusterer
#(
    parameter N_CLUSTERS = 4,
    parameter X_Y_BITS = 16,
    parameter COUNTER_BITS = 16
)(
    clk,
    reset,
    X, Y,
    valid,
    range,
    X_clusters,
    Y_clusters,
    cluster_counters
);

input logic clk, reset;

input logic [X_Y_BITS-1:0] X, Y;

// X, Y is valid
input logic valid;

input logic [X_Y_BITS-1:0] range;

// All slots concatenated
output logic [X_Y_BITS*N_CLUSTERS-1:0] X_clusters, Y_clusters;
output logic [COUNTER_BITS*N_CLUSTERS-1:0] cluster_counters;

// -- End of IO --


typedef struct {
    reg [X_Y_BITS-1:0] X;
    reg [X_Y_BITS-1:0] Y;
} Point;

Point slots[N_CLUSTERS-1:0];
logic [COUNTER_BITS-1:0] count[N_CLUSTERS-1:0];

logic [X_Y_BITS*2-1:0] rangeSq;

always_comb begin
    for (int i = 0; i < N_CLUSTERS; i++) begin
        X_clusters[i*X_Y_BITS +: X_Y_BITS] = slots[i].X;
        Y_clusters[i*X_Y_BITS +: X_Y_BITS] = slots[i].Y;
        cluster_counters[i*COUNTER_BITS +: COUNTER_BITS] = count[i];
    end
end

Point pipeline[N_CLUSTERS-1:0];
logic [N_CLUSTERS-1:0] pipeline_valid;

// Stage 1, diffx = x1 - x2, diffy = y1 - y2
logic signed [X_Y_BITS-1:0] diffX[N_CLUSTERS-1:0];
logic signed [X_Y_BITS-1:0] diffY[N_CLUSTERS-1:0];

// Stage 2, sqrx = diffx*diffx, sqry = diffy*diffy
logic [2*X_Y_BITS-1:0] sqrX[N_CLUSTERS-1:0];
logic [2*X_Y_BITS-1:0] sqrY[N_CLUSTERS-1:0];

// Stage 3, distance = sqrx + sqry
logic [2*X_Y_BITS-1:0] distanceSq[N_CLUSTERS-1:0];

always_comb begin
    rangeSq = range * range;
    for (int i = 0; i < N_CLUSTERS; i++) begin
        diffX[i] = slots[i].X - pipeline[i].X;
        diffY[i] = slots[i].Y - pipeline[i].Y;
        sqrX[i] = diffX[i] * diffX[i];
        sqrY[i] = diffY[i] * diffY[i];
        distanceSq[i] = sqrX[i] + sqrY[i];
    end
end

always_ff @(posedge clk) begin
    if (reset) begin
        pipeline_valid <= 0;
        for (int i = 0; i < N_CLUSTERS; i++) begin
            slots[i].X <= 0;
            slots[i].Y <= 0;
            count[i] <= 0;
        end
    end
    else begin
        pipeline[0].X <= X;
        pipeline[0].Y <= Y;
        pipeline_valid[0] <= valid;

        for (int i = 0; i < N_CLUSTERS; i++) begin
            pipeline[i+1] <= pipeline[i];
            pipeline_valid[i+1] <= pipeline_valid[i];

            if (pipeline_valid[i] && (count[i] == 0 || distanceSq[i] <= rangeSq)) begin
                if (count[i] == 0) begin
                    slots[i].X <= pipeline[i].X;
                    slots[i].Y <= pipeline[i].Y;
                end
                else begin
                    slots[i].X <= (pipeline[i].X + slots[i].X) / 2;
                    slots[i].Y <= (pipeline[i].Y + slots[i].Y) / 2;
                end
                count[i] <= count[i] + 1'b1;
                // The value is already in a slot, prevent it from going down further
                if (i + 1 < N_CLUSTERS)
                    pipeline_valid[i+1] <= 1'b0;
            end
        end
    end
end

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
    parameter REG_BITS = 32
)(
    clk,
    reset,

    // Registers
    read_regs,
    write_regs,

    trigger,

    // Avalon MM Signals
    mm_address,
    mm_byteenable,
    mm_read,
    mm_write,
    mm_readdata,
    mm_writedata

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

output logic [N-1:0] trigger;

always_ff @(posedge clk) begin
    trigger <= 0;

    if (reset) begin
        for (int i = 0; i < N; i++)
            write_regs[i] <= 0;
    end
    else if (mm_write) begin
        trigger[mm_address] <= 1'b1;

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
