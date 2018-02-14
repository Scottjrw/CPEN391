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
	 
	 parameter [4:0] idle = 5'b00000;
	 parameter [4:0] writeVal = 5'b00111;
	 parameter [4:0] writeVal_wait = 5'b01101;
	 
	 logic [4:0] state = idle;
	 
	 logic trigger;
	 logic [32*5-1:0] wr_regs_out;
	 logic [31:0] x, y, buffer_base, width;
	 
	 pixel_cluster_regs
		#(
			 .N(5),
			 .ADDR_BITS(24),
			 .REG_BITS(32),
			 .TRIGGER_REG(4)
		) regs_inst (
			 .clk(clk),
			 .reset(reset),
			 .mm_address(mm_address),
			 .mm_byteenable(4'b1111),
			 .mm_read(mm_read),
			 .mm_write(mm_write),
			 .mm_readdata(mm_readdata),
			 .mm_writedata(mm_writedata),

			 .read_regs({state, address_out, buffer_base, y, x}),
			 .write_regs(wr_regs_out),
			 
			 .trigger(trigger)
		);
	 
	 always @(posedge clk or posedge reset) begin 
		if(reset)
			state = idle;
		else begin
			case(state)
				idle: begin
					write_master = 1'b0;
					if (trigger==1) begin 
						state <= writeVal;
						x <= wr_regs_out [1*32-1:0];
						y <= wr_regs_out [2*32-1:1*32];
						width <= wr_regs_out [3*32-1:2*32];
						buffer_base <= wr_regs_out [4*32-1:3*32];
						data_out <= wr_regs_out [5*32-1:4*32];
					end
					else state <= idle;
				end
				
				writeVal: begin
					write_master = 1'b1;
					if(wait_request) state <= writeVal;
					else state <= writeVal_wait;
				end
				
				writeVal_wait: begin
					write_master = 1'b1;
					state <= idle;
				end
				
				
				
				default: state <= idle;
			endcase
		end
	 end
	 
	assign byteenable = 4'b1111;
	assign address_out = buffer_base + (y*width + x)*2;

    
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
