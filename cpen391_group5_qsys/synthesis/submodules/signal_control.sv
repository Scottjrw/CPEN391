module signal_control (
    input logic clk, reset,
	 input logic [31:0] mm_writedata,
	 output logic [31:0] mm_readdata,
	 input logic [23:0] mm_address,
	 input logic mm_write,
	 input logic mm_read,
    output logic write_master,
	 input logic wait_request,
    output logic [3:0] byteenable,
	 output logic [31:0] address_out,
    output logic [31:0] data_out
    );
	 
	 // draw pixel states
	 parameter pixel_idle = 1'b0;
	 parameter pixel_write = 1'b1;
	 
	 logic pixel_state = pixel_idle;
	 
	 // draw rec states
	 parameter [5:0] rec_idle = 6'b000_000;
	 parameter [5:0] rec_init = 6'b000_001;
	 parameter [5:0] rec_write = 6'b100_010;
	 parameter [5:0] rec_cmp_x = 6'b000_011;
	 parameter [5:0] rec_inc_x = 6'b001_100;
	 parameter [5:0] rec_cmp_y = 6'b000_101;
	 parameter [5:0] rec_inc_y = 6'b010_110;
	 
	 logic [5:0] rec_state = rec_idle;
	 
	 wire [31:0] buffer_base = 32'h80000;
	 wire inc_x, inc_y;
	 
	 logic trigger;
	 logic [32*7-1:0] wr_regs_out;
	 logic [31:0] x1, x2, y1, y2, width, command, cur_x, cur_y;
	 
	 pixel_cluster_regs
	 #(
		.N(7),
		.ADDR_BITS(24),
		.REG_BITS(32),
		.TRIGGER_REG(6)
		) regs_inst (
		.clk(clk),
		.reset(reset),
		.mm_address(mm_address),
		.mm_byteenable(4'b1111),
		.mm_read(mm_read),
		.mm_write(mm_write),
		.mm_readdata(mm_readdata),
		.mm_writedata(mm_writedata),

		.read_regs({rec_state, address_out, x2, y2, cur_x, cur_y}),
		.write_regs(wr_regs_out),
		
		.trigger(trigger)
		);
		
	 // command control
	 always @(posedge clk or posedge reset) begin
		if(reset) command <= 0;
		else begin
			if(trigger==1)
				command <= wr_regs_out[7*32-1:6*32];
			else
				command <= 0;
		end
	 end
	 
	 // draw pixel
	 always @(posedge clk or posedge reset) begin 
		if(reset)
			pixel_state = pixel_idle;
		else begin
			case(pixel_state)
			
				pixel_idle: begin
					if (command[0]==1) pixel_state <= pixel_write;
					else pixel_state <= pixel_idle;
				end
				
				pixel_write: begin

					if(wait_request) pixel_state <= pixel_write;
					else pixel_state <= pixel_idle;
				end
				
				default: pixel_state <= pixel_idle;
			endcase
		end
	 end
	 
	 //draw rec
	 always @(posedge clk or posedge reset) begin
		if (reset) rec_state <= rec_idle;
		else begin
			case (rec_state)
			
				rec_idle: begin
					if(command[1]==1) rec_state <= rec_init;
					else rec_state <= rec_idle;
				end
				
				rec_init: begin
					rec_state <= rec_write;
				end
				
				rec_write: begin
					if(wait_request) rec_state <= rec_write;
					else rec_state <= rec_cmp_x;
				end
				
				rec_cmp_x: begin
					if(cur_x<x2) rec_state <= rec_inc_x;
					else rec_state <= rec_cmp_y;
				end
				
				rec_inc_x: begin
					rec_state <= rec_write;
				end
				
				rec_cmp_y: begin
					if(cur_y<y2) rec_state <= rec_inc_y;
					else rec_state <= rec_idle;
				end
				
				rec_inc_y: begin
					rec_state <= rec_write;
				end
				
				default: rec_state <= rec_idle;
				
			endcase
		
		end
	 
	 
	 
	 end
	 
	 
	 // process control
	 always @(posedge clk or posedge reset) begin
		if(reset) begin
			x1 <= 0;
			x2 <= 0;
			y1 <= 0;
			y2 <= 0;
			width <= 0;
			cur_x <= 0;
			cur_y <= 0;		
		end
		else begin
			if(command>0) begin
				cur_x <= wr_regs_out [1*32-1:0];
				x1 <= wr_regs_out [1*32-1:0];
				x2 <= wr_regs_out [2*32-1:1*32];
				cur_y <= wr_regs_out [3*32-1:2*32];
				y1 <= wr_regs_out [3*32-1:2*32];
				y2 <= wr_regs_out [4*32-1:3*32];
				width <= wr_regs_out [5*32-1:4*32];
				data_out <= wr_regs_out [6*32-1:5*32];
			end
			else if (inc_x)
				cur_x = cur_x + 1;
			else if (inc_y) begin
				cur_y = cur_y + 1;
				cur_x = x1;
			end
		end
	 
	 end
	 
	 assign inc_x = rec_state[3];
	 assign inc_y = rec_state[4];
	 assign write_master = pixel_state | rec_state[5];
	 assign byteenable = 4'b1111;
	 assign address_out = buffer_base + (cur_y*width + cur_x)*2;

    
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
