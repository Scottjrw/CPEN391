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

// Pixel Cluster Regs
parameter N_REGS = 7;
parameter ADDR_BITS = 4;
parameter REG_BITS = 32;

// 24 bit color
parameter N_COLORS = 3;
parameter COLOR_BITS = 8;

parameter X_MAX = 320;
parameter Y_MAX = 240;

parameter N_CLUSTERS = 4;

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

MMRegister register_control;
MMRegister register_compare_en;
MMRegister register_compare_color;
MMRegister register_range;
MMRegister register_max_x;
MMRegister register_max_y;
MMRegister register_max_count;

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

logic [COLOR_BITS-1:0] color_filter_b_cmp;
logic [COLOR_BITS-1:0] color_filter_g_cmp;
logic [COLOR_BITS-1:0] color_filter_r_cmp;
logic [N_COLORS-1:0] color_filter_compare;
logic [N_COLORS-1:0] color_filter_less_than;
logic color_filter_pixel_valid_out;

pixel_cluster_color_filter
#(
    .N_COLORS(N_COLORS),
    .COLOR_BITS(COLOR_BITS)
) color_filter_inst (
    .pixel_stream_in(avalon_st_pixel_stream),
    .pixel_valid_in(avalon_st_pixel_valid),
    .b_cmp(color_filter_b_cmp), 
    .g_cmp(color_filter_g_cmp), 
    .r_cmp(color_filter_r_cmp),
    .compare(color_filter_compare),
    .less_than(color_filter_less_than),
    .pixel_valid_out(color_filter_pixel_valid_out)
);

logic clusterer_reset;
logic [X_Y_BITS-1:0] clusterer_range;
logic [X_Y_BITS*N_CLUSTERS-1:0] clusterer_X_clusters;
logic [X_Y_BITS*N_CLUSTERS-1:0] clusterer_Y_clusters;
logic [COUNTER_BITS*N_CLUSTERS-1:0] clusterer_cluster_counters;

pixel_cluster_x_y_clusterer
#(
    .N_CLUSTERS(N_CLUSTERS),
    .X_Y_BITS(X_Y_BITS),
    .COUNTER_BITS(COUNTER_BITS)
) clusterer_inst (
    .clk(clk),
    .reset(clusterer_reset),
    .X(avalon_st_pixel_x),
    .Y(avalon_st_pixel_y),
    .valid(color_filter_pixel_valid_out),
    .range(clusterer_range),
    .X_clusters(clusterer_X_clusters),
    .Y_clusters(clusterer_Y_clusters),
    .cluster_counters(clusterer_cluster_counters)
);

logic find_max_start;

pixel_cluster_delay
#(
    .DELAY(N_CLUSTERS + 2)
) delay_start_find (
    .clk(clk),
    .reset(reset),

    .in(avalon_st_pixel_end),
    .out(find_max_start)
);

logic find_max_finish;

logic [$clog2(N_CLUSTERS)-1:0] find_max_max_index;

pixel_cluster_find_max
#(
    .VALUE_BITS(COUNTER_BITS),
    .N_VALUES(N_CLUSTERS)
) find_max_inst (
    .clk(clk),
    .reset(reset),

    .start(find_max_start),
    .finish(find_max_finish),

    .values(clusterer_cluster_counters),
    .max_index(find_max_max_index)
);

logic find_max_finish_capture;

logic irq_reset;

pixel_cluster_capture
cap_finish (
    .clk(clk),
    .reset(irq_reset),

    .in(find_max_finish),
    .out(find_max_finish_capture)
);

// IRQ Send after max is found
assign irq_send_irq = find_max_finish_capture;

// -- Avalon MM Register Map --

localparam Control_Register_Num = 0;
localparam Compare_En_Register_Num = 1;
localparam Compare_Color_Register_Num = 2;
localparam Range_Register_Num = 3;
localparam Max_X_Register_Num = 4;
localparam Max_Y_Register_Num = 5;
localparam Max_Count_Register_Num = 6;

// Control Register
assign register_control       = {{(REG_BITS-1){1'b0}}, 1'b1} & regs_write_regs[Control_Register_Num*REG_BITS +: REG_BITS];
assign regs_read_regs[Control_Register_Num*REG_BITS +: REG_BITS] = register_control;
// Reset clusterer and irq when written to
assign clusterer_reset = regs_write_trigger[Control_Register_Num];
assign irq_reset = regs_write_trigger[Control_Register_Num];

// Compare Enable
assign register_compare_en    = {{(REG_BITS-3){1'b0}}, 3'b1} & regs_write_regs[Compare_En_Register_Num*REG_BITS +: REG_BITS];
assign regs_read_regs[Compare_En_Register_Num*REG_BITS +: REG_BITS] = register_compare_en;
assign color_filter_compare =   register_compare_en[0 +: 8];
assign color_filter_less_than = register_compare_en[8 +: 8];

// Compare Color
assign register_compare_color = {{(REG_BITS-24){1'b0}}, 24'b1} & regs_write_regs[Compare_Color_Register_Num*REG_BITS +: REG_BITS];
assign regs_read_regs[Compare_Color_Register_Num*REG_BITS +: REG_BITS] = register_compare_color;
assign color_filter_b_cmp = register_compare_color[0*COLOR_BITS +: COLOR_BITS];
assign color_filter_g_cmp = register_compare_color[1*COLOR_BITS +: COLOR_BITS];
assign color_filter_r_cmp = register_compare_color[2*COLOR_BITS +: COLOR_BITS];

// Range
assign register_range = regs_write_regs[Range_Register_Num*REG_BITS +: REG_BITS];
assign regs_read_regs[Range_Register_Num*REG_BITS +: REG_BITS] = register_range;
assign clusterer_range = register_range[X_Y_BITS-1:0];

// Max X
assign register_max_x         = {{X_Y_BITS{1'b0}}, clusterer_X_clusters[find_max_max_index]};
assign regs_read_regs[Max_X_Register_Num*REG_BITS +: REG_BITS] = register_max_x;

// Max Y
assign register_max_y         = {{X_Y_BITS{1'b0}}, clusterer_Y_clusters[find_max_max_index]};
assign regs_read_regs[Max_Y_Register_Num*REG_BITS +: REG_BITS] = register_max_y;

// Max Count
assign register_max_count     = {{X_Y_BITS{1'b0}}, clusterer_cluster_counters[find_max_max_index]};
assign regs_read_regs[Max_Count_Register_Num*REG_BITS +: REG_BITS] = register_max_count;


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

if multiple max values exist, the first one is returned

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
