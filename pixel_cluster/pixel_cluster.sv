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

/*
A module that takes a Avalon video stream and 
converts it into pixels with x, y coordinates

*/
module pixel_cluster_avalon_st
#(
    parameter N_COLORS = 3,
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
    pixel_y
);

input logic clk;
input logic reset;

// Avalon Streaming Interface
output logic st_ready;
input logic st_startofpacket;
input logic st_endofpacket;
input logic st_valid;
input logic [COLOR_BITS*N_COLORS-1:0] st_data;

// Output stream of pixels with x, y values
output logic [COLOR_BITS*N_COLORS-1:0] pixel_stream;
output logic pixel_valid;
output logic [X_Y_BITS-1:0] pixel_x;
output logic [X_Y_BITS-1:0] pixel_y;

// -- End of IO --

logic done;

always_ff @(posedge clk) begin
    if (reset) begin
        st_ready <= 0;
        pixel_stream <= 0;
        pixel_valid <= 0;
        pixel_x <= 0;
        pixel_y <= 0;
        done <= 0;
    end
    else begin
        st_ready <= 1;

        if (st_valid) begin
            pixel_stream <= st_data;
            // Row major, increment X then Y
            pixel_x <= (pixel_x == X_MAX-1) ? 0 : pixel_x + 1'b1;
            pixel_y <= (pixel_x == X_MAX-1) ? pixel_y + 1'b1 : pixel_y;

            // Start and End Conditions
            if (st_startofpacket) begin
                pixel_x <= 0;
                pixel_y <= 0;
                pixel_valid <= 1;
                done <= 0;
            end
            else if (st_endofpacket) begin
                done <= 1;
            end
        end

        if (done) begin
            pixel_x <= 0;
            pixel_y <= 0;
            pixel_valid <= 0;
            done <= 0;
        end
    end
end
endmodule

/*
 Filters pixels based on their RGB color values
 The pixel_valid_out will be high only when pixel_valid_in is high
 and the pixel matches the compares.
 

 */
module pixel_cluster_color_filter
#(
    parameter N_COLORS = 3,
    parameter COLOR_BITS = 8
)(
    pixel_stream_in,
    pixel_valid_in,
    b_cmp, g_cmp, r_cmp,
    compare,
    less_than,
    pixel_valid_out
);


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
end

endmodule


/*
 Clusters input x,y points into N_CLUSTERS clusters
 Pixels are considered in a cluster if they are within range

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

// diffx = x1 - x2, diffy = y1 - y2
logic signed [X_Y_BITS-1:0] diffX[N_CLUSTERS-1:0];
logic signed [X_Y_BITS-1:0] diffY[N_CLUSTERS-1:0];

// sqrx = diffx*diffx, sqry = diffy*diffy
logic [2*X_Y_BITS-1:0] sqrX[N_CLUSTERS-1:0];
logic [2*X_Y_BITS-1:0] sqrY[N_CLUSTERS-1:0];

// distance = sqrx + sqry
logic [2*X_Y_BITS-1:0] distanceSq[N_CLUSTERS-1:0];

// Calculate the distance purely combinationally
// Cannot pipeline this as the value of distance depends on
// whether or not the slot was updated by previous pixels
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
            if (i + 1 < N_CLUSTERS) begin
                pipeline[i+1] <= pipeline[i];
                pipeline_valid[i+1] <= pipeline_valid[i];
            end

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
Searches through values and finds the maximum

*/
module pixel_cluster_find_max
#(
    parameter VALUE_BITS = 16,
    parameter N_VALUES = 4
) (
    clk,
    reset,
    
    start,
    finish,

    values,
    max_index
);

input logic clk, reset;

input logic start;
output logic finish;

input logic [VALUE_BITS*N_VALUES-1:0] values;
output logic [$clog2(N_VALUES)-1:0] max_index;

// -- End of IO --

typedef enum logic[1:0] {
    idle =      2'b00,
    running =   2'b10,
    done =      2'b01
} state_t;

state_t state;

logic[$clog2(N_VALUES)-1:0] index;

assign finish = state[0];

always_ff @(posedge clk) begin
    if (reset) begin
        running <= 0;
        finish <= 0;
        max_index <= 0;
        index <= 0;
    end
    else begin
        case (state)
            idle:
            if (start)
                state <= running;

            running:
            if (max_index == N_VALUES - 1) begin
                state <= 
            end
            else begin
                if (values[max_index] < values[index]) begin
                    max_index <= index;
                end
                max_index <= max_index + 1'b1;
            end

            done:
            state <= idle;
        endcase
        end
        else if (start) begin
            running <= 1'b1;
            max_index <= 0;
            index <= 0;
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

    // Triggers on read or write for each reg
    read_trigger,
    write_trigger,

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

output logic [N-1:0] read_trigger;
output logic [N-1:0] write_trigger;

always_ff @(posedge clk) begin
    read_trigger <= 0;
    write_trigger <= 0;

    if (reset) begin
        for (int i = 0; i < N; i++)
            write_regs[i] <= 0;
    end
    else if (mm_write) begin
        write_trigger[mm_address] <= 1'b1;

        for (int i = 0; i < REG_BITS/8; i++) begin
            if (mm_byteenable[i]) begin
                write_regs[mm_address*32 + i*8 +: 8] <= mm_writedata[i*8 +: 8];
            end
        end
    end
    else if (mm_read) begin
        read_trigger[mm_address] <= 1'b1;

        for (int i = 0; i < REG_BITS/8; i++) begin
            mm_readdata[i*8 +: 8] <= {8{mm_byteenable[i]}} & read_regs[mm_address*32 + i*8 +: 8];
        end
    end
end

endmodule
