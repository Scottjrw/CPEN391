module pixel_cluster
(
    clk,
    reset,

    mm_address,
    mm_byteenable,
    mm_read,
    mm_write,
    mm_readdata,
    mm_writedata,

    st_ready,
    st_startofpacket,
    st_endofpacket,
    st_valid,
    st_data,

    irq_send_irq

);
input clk, reset;

parameter N_CLUSTERS = 4;

// Pixel Cluster Regs
localparam REGS_PER_CLUSTER = 4;
localparam CONTROL_REGS = 4;
localparam N_REGS = CONTROL_REGS + N_CLUSTERS*REGS_PER_CLUSTER;
parameter REG_BITS = 32;
parameter ADDR_BITS = 5;

// 24 bit color
parameter N_COLORS = 3;
parameter COLOR_BITS = 8;

parameter X_MAX = 320;
parameter Y_MAX = 240;

parameter X_Y_BITS = 16;
parameter COUNTER_BITS = 16;

// Avalon MM Slave
input logic [ADDR_BITS-1:0] mm_address;
input logic [REG_BITS/8-1:0] mm_byteenable;
input logic mm_read;
input logic mm_write;
output logic [REG_BITS-1:0] mm_readdata;
input logic [REG_BITS-1:0] mm_writedata;

// Avalon ST Sink
output logic st_ready;
input logic st_startofpacket;
input logic st_endofpacket;
input logic st_valid;
input logic [COLOR_BITS*N_COLORS-1:0] st_data;

// Avalon Interrupt Sender
output logic irq_send_irq;

// -- End of IO --

typedef logic [REG_BITS-1:0] MMRegister;

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

logic [COLOR_BITS*N_COLORS-1:0] avalon_st_pixel_stream;
logic avalon_st_pixel_valid;
logic [X_Y_BITS-1:0] avalon_st_pixel_x;
logic [X_Y_BITS-1:0] avalon_st_pixel_y;
logic avalon_st_pixel_end;

pixel_cluster_avalon_st
#(
    .N_COLORS(N_COLORS),
    .COLOR_BITS(COLOR_BITS),
    .X_MAX(X_MAX),
    .Y_MAX(Y_MAX),
    .X_Y_BITS(X_Y_BITS)
) avalon_st_inst (
    .clk(clk),
    .reset(reset),

    .st_ready(st_ready),
    .st_startofpacket(st_startofpacket),
    .st_endofpacket(st_endofpacket),
    .st_valid(st_valid),
    .st_data(st_data),

    .pixel_stream(avalon_st_pixel_stream),
    .pixel_valid(avalon_st_pixel_valid),
    .pixel_x(avalon_st_pixel_x),
    .pixel_y(avalon_st_pixel_y),
    .pixel_end(avalon_st_pixel_end)
);

logic [COLOR_BITS-1:0] color_filter_b_cmp[1:0];
logic [COLOR_BITS-1:0] color_filter_g_cmp[1:0];
logic [COLOR_BITS-1:0] color_filter_r_cmp[1:0];
logic [N_COLORS-1:0] color_filter_compare[1:0];
logic [N_COLORS-1:0] color_filter_less_than[1:0];
logic [1:0] color_filter_pixel_valid_out;

pixel_cluster_color_filter
#(
    .N_COLORS(N_COLORS),
    .COLOR_BITS(COLOR_BITS)
) color_filter_inst (
    .pixel_stream_in(avalon_st_pixel_stream),
    .pixel_valid_in(avalon_st_pixel_valid),
    .b_cmp(color_filter_b_cmp[0]), 
    .g_cmp(color_filter_g_cmp[0]), 
    .r_cmp(color_filter_r_cmp[0]),
    .compare(color_filter_compare[0]),
    .less_than(color_filter_less_than[0]),
    .pixel_valid_out(color_filter_pixel_valid_out[0])
);

pixel_cluster_color_filter
#(
    .N_COLORS(N_COLORS),
    .COLOR_BITS(COLOR_BITS)
) color_filter_inst_1 (
    .pixel_stream_in(avalon_st_pixel_stream),
    .pixel_valid_in(avalon_st_pixel_valid),
    .b_cmp(color_filter_b_cmp[1]), 
    .g_cmp(color_filter_g_cmp[1]), 
    .r_cmp(color_filter_r_cmp[1]),
    .compare(color_filter_compare[1]),
    .less_than(color_filter_less_than[1]),
    .pixel_valid_out(color_filter_pixel_valid_out[1])
);

logic clusterer_valid_in;
assign clusterer_valid_in = &color_filter_pixel_valid_out;

logic clusterer_reset;
logic [X_Y_BITS-1:0] clusterer_range;
logic [X_Y_BITS*N_CLUSTERS-1:0] clusterer_X_clusters,
                                clusterer_Y_clusters;
logic [COUNTER_BITS*N_CLUSTERS-1:0] clusterer_cluster_counters;
logic [X_Y_BITS*N_CLUSTERS-1:0] clusterer_cluster_min_X, 
                                clusterer_cluster_min_Y,
                                clusterer_cluster_max_X,
                                clusterer_cluster_max_Y;

typedef struct packed {
    reg [REG_BITS-1:0] count;
    reg [X_Y_BITS-1:0] max_Y;
    reg [X_Y_BITS-1:0] max_X;
    reg [X_Y_BITS-1:0] min_Y;
    reg [X_Y_BITS-1:0] min_X;
    reg [X_Y_BITS-1:0] Y;
    reg [X_Y_BITS-1:0] X;
} Pixel_Cluster;

Pixel_Cluster [N_CLUSTERS-1:0] clusterer_clusters;

always_comb begin
    for(int i = 0; i < N_CLUSTERS; i++) begin
        clusterer_clusters[i].count = { {REG_BITS-COUNTER_BITS{1'b0}},
            clusterer_cluster_counters[i*COUNTER_BITS +: COUNTER_BITS] };
        clusterer_clusters[i].max_Y = clusterer_cluster_max_Y[i*X_Y_BITS +: X_Y_BITS];
        clusterer_clusters[i].max_X = clusterer_cluster_max_X[i*X_Y_BITS +: X_Y_BITS];
        clusterer_clusters[i].min_Y = clusterer_cluster_min_Y[i*X_Y_BITS +: X_Y_BITS];
        clusterer_clusters[i].min_X = clusterer_cluster_min_X[i*X_Y_BITS +: X_Y_BITS];
        clusterer_clusters[i].Y = clusterer_Y_clusters[i*X_Y_BITS +: X_Y_BITS];
        clusterer_clusters[i].X = clusterer_X_clusters[i*X_Y_BITS +: X_Y_BITS];
    end
end

pixel_cluster_x_y_clusterer
#(
    .N_CLUSTERS(N_CLUSTERS),
    .X_Y_BITS(X_Y_BITS),
    .COUNTER_BITS(COUNTER_BITS),
    .MAX_X_Y(X_MAX > Y_MAX ? X_MAX : Y_MAX)
) clusterer_inst (
    .clk(clk),
    .reset(clusterer_reset),
    .X(avalon_st_pixel_x),
    .Y(avalon_st_pixel_y),
    .valid(clusterer_valid_in),
    .range(clusterer_range),
    .X_clusters(clusterer_X_clusters),
    .Y_clusters(clusterer_Y_clusters),
    .cluster_counters(clusterer_cluster_counters),
    .cluster_min_X(clusterer_cluster_min_X), 
    .cluster_min_Y(clusterer_cluster_min_Y),
    .cluster_max_X(clusterer_cluster_max_X),
    .cluster_max_Y(clusterer_cluster_max_Y)
);

logic end_delayed;

pixel_cluster_delay
#(
    .DELAY(N_CLUSTERS + 4)
) delay_start_find (
    .clk(clk),
    .reset(reset),

    .in(avalon_st_pixel_end),
    .out(end_delayed)
);

logic end_delayed_capture;

logic irq_reset;

pixel_cluster_capture
cap_finish (
    .clk(clk),
    .reset(irq_reset),

    .in(end_delayed),
    .out(end_delayed_capture)
);

// IRQ Send after max is found
assign irq_send_irq = end_delayed_capture;

// -- Avalon MM Register Map --

MMRegister register_control;
MMRegister register_compare_0;
MMRegister register_compare_1;
MMRegister register_range;

localparam Control_Register_Num = 0;
localparam Compare_0_Register_Num = 1;
localparam Compare_1_Register_Num = 2;
localparam Range_Register_Num = 3;

assign {
    register_range,
    register_compare_1,
    register_compare_0,
    register_control
} = regs_write_regs;

assign regs_read_regs = {
    clusterer_clusters,
    register_range,
    register_compare_1,
    register_compare_0,
    {REG_BITS{1'b0}}
};

// Control Register
// Reset clusterer and irq when written to
assign clusterer_reset = regs_write_trigger[Control_Register_Num];
assign irq_reset = regs_write_trigger[Control_Register_Num];

// Color Compare 0 Register
assign {
    color_filter_less_than[0], 
    color_filter_compare[0],
    color_filter_r_cmp[0],
    color_filter_g_cmp[0],
    color_filter_b_cmp[0]
} = register_compare_0;

// Color Compare 1 Register
assign {
    color_filter_less_than[1], 
    color_filter_compare[1],
    color_filter_r_cmp[1],
    color_filter_g_cmp[1],
    color_filter_b_cmp[1]
} = register_compare_1;

// Range
assign {
    clusterer_range
} = register_range;


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
    pixel_y,
    pixel_end
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
output logic pixel_end;

// -- End of IO --

logic done;

always_ff @(posedge clk) begin
    if (reset) begin
        st_ready <= 0;
        pixel_stream <= 0;
        pixel_valid <= 0;
        pixel_x <= 0;
        pixel_y <= 0;
        pixel_end <= 0;
        done <= 0;
    end
    else begin
        st_ready <= 1;
        pixel_end <= 0;

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
            pixel_end <= 1;
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
    parameter COUNTER_BITS = 16,
    parameter MAX_X_Y = 320
)(
    clk,
    reset,
    X, Y,
    valid,
    range,
    X_clusters,
    Y_clusters,
    cluster_counters,
    cluster_min_X,
    cluster_min_Y,
    cluster_max_X,
    cluster_max_Y
);

input logic clk, reset;

input logic [X_Y_BITS-1:0] X, Y;

// X, Y is valid
input logic valid;

input logic [X_Y_BITS-1:0] range;

// All slots concatenated
output logic [X_Y_BITS*N_CLUSTERS-1:0] X_clusters, Y_clusters;
output logic [COUNTER_BITS*N_CLUSTERS-1:0] cluster_counters;
output logic [X_Y_BITS*N_CLUSTERS-1:0] cluster_min_X, cluster_min_Y,
                                       cluster_max_X, cluster_max_Y;

// -- End of IO --

localparam CALC_X_Y_BITS = $clog2(MAX_X_Y);
localparam ZERO_X_Y_BITS = X_Y_BITS - CALC_X_Y_BITS;

typedef struct {
    reg [CALC_X_Y_BITS-1:0] X;
    reg [CALC_X_Y_BITS-1:0] Y;
} Point;

Point slots[N_CLUSTERS-1:0];
logic [COUNTER_BITS-1:0] count[N_CLUSTERS-1:0];
Point slot_min[N_CLUSTERS-1:0];
Point slot_max[N_CLUSTERS-1:0];

always_comb begin
    for (int i = 0; i < N_CLUSTERS; i++) begin
        X_clusters[i*X_Y_BITS +: X_Y_BITS] = {{ZERO_X_Y_BITS{1'b0}}, slots[i].X};
        Y_clusters[i*X_Y_BITS +: X_Y_BITS] = {{ZERO_X_Y_BITS{1'b0}}, slots[i].Y};
        cluster_counters[i*COUNTER_BITS +: COUNTER_BITS] = count[i];
        cluster_min_X[i*X_Y_BITS +: X_Y_BITS] = {{ZERO_X_Y_BITS{1'b0}}, slot_min[i].X};
        cluster_min_Y[i*X_Y_BITS +: X_Y_BITS] = {{ZERO_X_Y_BITS{1'b0}}, slot_min[i].Y};
        cluster_max_X[i*X_Y_BITS +: X_Y_BITS] = {{ZERO_X_Y_BITS{1'b0}}, slot_max[i].X};
        cluster_max_Y[i*X_Y_BITS +: X_Y_BITS] = {{ZERO_X_Y_BITS{1'b0}}, slot_max[i].Y};
    end
end

Point pipeline[N_CLUSTERS-1:0];
logic [N_CLUSTERS-1:0] pipeline_valid;

// diffx = x1 - x2, diffy = y1 - y2
logic signed [CALC_X_Y_BITS-1:0] diffX[N_CLUSTERS-1:0];
logic signed [CALC_X_Y_BITS-1:0] diffY[N_CLUSTERS-1:0];

// sqrx = diffx*diffx, sqry = diffy*diffy
logic [2*CALC_X_Y_BITS-1:0] sqrX[N_CLUSTERS-1:0];
logic [2*CALC_X_Y_BITS-1:0] sqrY[N_CLUSTERS-1:0];

// distance = sqrx + sqry
logic [2*CALC_X_Y_BITS:0] distanceSq[N_CLUSTERS-1:0];

// range to compare distance with
logic [2*CALC_X_Y_BITS:0] rangeSq;

// Calculate the distance purely combinationally
// Cannot pipeline this as the value of distance depends on
// whether or not the slot was updated by previous pixels
always_comb begin
    for (int i = 0; i < N_CLUSTERS; i++) begin
        diffX[i] = slots[i].X - pipeline[i].X;
        diffY[i] = slots[i].Y - pipeline[i].Y;
        sqrX[i] = diffX[i] * diffX[i];
        sqrY[i] = diffY[i] * diffY[i];
        distanceSq[i] = sqrX[i] + sqrY[i];
    end
end

always_ff @(posedge clk)
    rangeSq = range * range;

always_ff @(posedge clk) begin
    if (reset) begin
        pipeline_valid <= 0;
        for (int i = 0; i < N_CLUSTERS; i++) begin
            slots[i].X <= 0;
            slots[i].Y <= 0;
            count[i] <= 0;
            slot_min[i].X <= 0;
            slot_min[i].Y <= 0;
            slot_max[i].X <= 0;
            slot_max[i].Y <= 0;
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
                    slot_min[i].X <= pipeline[i].X;
                    slot_min[i].Y <= pipeline[i].Y;
                    slot_max[i].X <= pipeline[i].X;
                    slot_max[i].Y <= pipeline[i].Y;
                end
                else begin
                    slots[i].X <= (pipeline[i].X + slots[i].X) / 2;
                    slots[i].Y <= (pipeline[i].Y + slots[i].Y) / 2;
                    slot_min[i].X <= (pipeline[i].X < slot_min[i].X) ? pipeline[i].X : slot_min[i].X;
                    slot_min[i].Y <= (pipeline[i].Y < slot_min[i].Y) ? pipeline[i].Y : slot_min[i].Y;
                    slot_max[i].X <= (pipeline[i].X > slot_max[i].X) ? pipeline[i].X : slot_max[i].X;
                    slot_max[i].Y <= (pipeline[i].Y > slot_max[i].Y) ? pipeline[i].Y : slot_max[i].Y;
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

if multiple max values exist, the first one is returned

** Not used anymore, we use a different algorithm now **

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
assign finish = state[0];

logic [$clog2(N_VALUES)-1:0] index;

logic [VALUE_BITS-1:0] cur_value, max_value;
assign cur_value = values[VALUE_BITS*index +: VALUE_BITS];
assign max_value = values[VALUE_BITS*max_index +: VALUE_BITS];

always_ff @(posedge clk) begin
    if (reset) begin
        state <= idle;
        max_index <= 0;
        index <= 0;
    end
    else begin
        case (state)
            idle:
            if (start) begin
                state <= running;
                max_index <= 0;
                index <= 0;
            end

            running: begin
                index <= index + 1'b1;

                if (index == N_VALUES - 1) begin
                    state <= done;
                end

                if (max_value < cur_value) begin
                    max_index <= index;
                end
            end

            done:
            state <= idle;
        endcase
    end
end


endmodule

/*
 Delays a pulse by DELAY clock cycles
 DELAY must be > 0

*/
module pixel_cluster_delay
#(
    parameter DELAY = 1
)(
    clk,
    reset,

    in,
    out
);

input logic clk, reset;

input logic in;
output logic out;

// -- End of IO --

logic [DELAY-1:0] delay_pipe;

assign delay_pipe[0] = in;

generate
if (DELAY == 1) begin
    always_ff @(posedge clk) begin
        if (reset)
            out <= 0;
        else
            out <= delay_pipe;
    end
end
else begin
    always_ff @(posedge clk) begin
        if (reset)
            {out, delay_pipe[DELAY-1:1]} = 0;
        else
            {out, delay_pipe[DELAY-1:1]} <= delay_pipe;
    end
end
endgenerate

endmodule

/*
When a high pulse is received stay high until reset


*/
module pixel_cluster_capture
(
    clk,
    reset,

    in,
    out
);

input logic clk, reset;

input logic in;
output logic out;

always_ff @(posedge clk) begin
    if (reset) begin
        out <= 0;
    end
    else begin
        if (in)
            out <= 1;
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
