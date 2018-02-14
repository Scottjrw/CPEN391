module pixel_cluster_color_filter_tb();
parameter COLOR_BITS = 8;
parameter N_COLORS = 3;

logic [N_COLORS*COLOR_BITS-1:0] pixel_stream_in;

// Avalon ST valid signal
logic pixel_valid_in;

// Whether or not to compare this color
logic [N_COLORS-1:0] compare;

// 0 is stream >= cmp, 1 is stream < cmp
logic [N_COLORS-1:0] less_than;

logic pixel_valid_out;

logic [N_COLORS*COLOR_BITS-1:0] rgb_cmp;

pixel_cluster_color_filter #(
    .COLOR_BITS(COLOR_BITS),
    .N_COLORS(N_COLORS)
) inst (
    .pixel_stream_in(pixel_stream_in),
    .pixel_valid_in(pixel_valid_in),
    .b_cmp(rgb_cmp[0*COLOR_BITS +: COLOR_BITS]),
    .g_cmp(rgb_cmp[1*COLOR_BITS +: COLOR_BITS]),
    .r_cmp(rgb_cmp[2*COLOR_BITS +: COLOR_BITS]),
    .compare(compare),
    .less_than(less_than),
    .pixel_valid_out(pixel_valid_out)
);

parameter NUM_TESTS = 6;
int ok = 0;

initial begin
    pixel_stream_in = 0; pixel_valid_in = 0;
    compare = 0; less_than = 0; rgb_cmp = 0;
    #10;
    assert(!pixel_valid_out) ok++;

    pixel_valid_in = 1;
    pixel_stream_in = {8'd255, 8'd255, 8'd255};
    rgb_cmp =         {8'd255, 8'd255, 8'd255};
    compare =         {  1'b1,   1'b1,   1'b1};
    less_than =       {  1'b1,   1'b1,   1'b1};
    #10;
    assert(!pixel_valid_out) ok++;

    pixel_stream_in = {8'd255, 8'd255, 8'd255};
    rgb_cmp =         {8'd255, 8'd255, 8'd255};
    compare =         {  1'b1,   1'b1,   1'b1};
    less_than =       {  1'b0,   1'b0,   1'b0};
    #10;
    assert(pixel_valid_out) ok++;
    
    pixel_stream_in = {8'd255, 8'd255, 8'd255};
    rgb_cmp =         {8'd255, 8'd255, 8'd255};
    compare =         {  1'b1,   1'b1,   1'b1};
    less_than =       {  1'b0,   1'b0,   1'b0};
    #10;
    assert(pixel_valid_out) ok++;

    pixel_stream_in = {8'd025, 8'd255, 8'd060};
    rgb_cmp =         {8'd105, 8'd000, 8'd245};
    compare =         {  1'b1,   1'b1,   1'b1};
    less_than =       {  1'b1,   1'b1,   1'b1};
    #10;
    assert(!pixel_valid_out) ok++;

    pixel_stream_in = {8'd025, 8'd255, 8'd060};
    rgb_cmp =         {8'd105, 8'd000, 8'd245};
    compare =         {  1'b1,   1'b1,   1'b1};
    less_than =       {  1'b1,   1'b0,   1'b1};
    #10;
    assert(pixel_valid_out) ok++;

    if (ok == NUM_TESTS)
        $display("All Tests passed");
    else
        $display("%d of %d tests passed", ok, NUM_TESTS);

    $stop;

end


endmodule


module pixel_cluster_x_y_clusterer_tb();
parameter N_CLUSTERS = 4;
parameter X_Y_BITS = 16;
parameter COUNTER_BITS = 16;

logic clk, reset;

logic [X_Y_BITS-1:0] X, Y;

logic valid;

logic [X_Y_BITS-1:0] range;

logic [X_Y_BITS*N_CLUSTERS-1:0] X_clusters, Y_clusters;
logic [COUNTER_BITS*N_CLUSTERS-1:0] cluster_counters;

pixel_cluster_x_y_clusterer #(
    .N_CLUSTERS(N_CLUSTERS),
    .X_Y_BITS(X_Y_BITS),
    .COUNTER_BITS(COUNTER_BITS)
) inst (
    .clk(clk),
    .reset(reset),
    .X(X),
    .Y(Y),
    .valid(valid),
    .range(range),
    .X_clusters(X_clusters),
    .Y_clusters(Y_clusters),
    .cluster_counters(cluster_counters)
);

initial forever begin
    clk = 0; #5;
    clk = 1; #5;
end

parameter NUM_TESTS = 11;
int ok = 0;

initial begin
    reset = 1; X = 0; Y = 0;
    valid = 0; range = 0;
    #10;
    assert(cluster_counters == {16'd0, 16'd0, 16'd0, 16'd0}) ok++;

    reset = 0;
    #10;
    assert(cluster_counters == {16'd0, 16'd0, 16'd0, 16'd0}) ok++;

    // Start test
    range = 50; valid = 1;
    
    X = 100; Y = 100;
    #10;

    X = 125; Y = 125;
    #10;

    X = 75; Y = 100;
    #10;

    valid = 0;
    #10;

    // Wait for pipeline
    #50;
    assert(X_clusters == {16'd0, 16'd0, 16'd0, 16'd93}) ok++;
    assert(Y_clusters == {16'd0, 16'd0, 16'd0, 16'd106}) ok++;
    assert(cluster_counters == {16'd0, 16'd0, 16'd0, 16'd3}) ok++;

    valid = 1;
    X = 10; Y = 10;
    #10;

    X = 20; Y = 20;
    #10

    valid = 0;
    
    #50;
    assert(X_clusters == {16'd0, 16'd0, 16'd15, 16'd93}) ok++;
    assert(Y_clusters == {16'd0, 16'd0, 16'd15, 16'd106}) ok++;
    assert(cluster_counters == {16'd0, 16'd0, 16'd2, 16'd3}) ok++;

    valid = 1;
    X = 200; Y = 200;
    #10;

    X = 300; Y = 200;
    #10;

    X = 100; Y = 200;
    #10;

    valid = 0;

    #50;
    assert(X_clusters == {16'd300, 16'd200, 16'd15, 16'd93}) ok++;
    assert(Y_clusters == {16'd200, 16'd200, 16'd15, 16'd106}) ok++;
    assert(cluster_counters == {16'd1, 16'd1, 16'd2, 16'd3}) ok++;


    if (ok == NUM_TESTS)
        $display("All Tests passed");
    else
        $display("%d of %d tests passed", ok, NUM_TESTS);

    $stop;
end

endmodule
