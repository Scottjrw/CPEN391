module signal_control (
    input logic clk, reset,
	 input logic [31:0] mm_writedata,
	 output logic [31:0] mm_readdata,
	 input logic [23:0] mm_address,
	 input logic mm_write,
	 input logic mm_read,
    output logic write_master,
	 input logic wait_request,
    output logic [1:0] byteenable,
	 output logic [31:0] address_out,
    output logic [15:0] data_out,
	 output logic irq
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
	 
	 // hard coded parameters
	 wire [31:0] buffer_base = 32'h80000;
	 wire [31:0] width = 32'd160;
	 
	 wire inc_x, inc_y;
	 
	 logic trigger, trigger_irq, rec_irq, pixel_irq;
	 logic [32*9-1:0] write_regs;
	 logic [31:0] x1, x2, y1, y2, command, cur_x, cur_y;
	 logic [8:0] trigger_reg;
	 
	 // set to 1 to disable the irq
	 logic [31:0] irq_disable;
	 
	 pixel_cluster_regs
	 #(
		.N(9),
		.ADDR_BITS(24),
		.REG_BITS(32)
		) regs_inst (
		.clk(clk),
		.reset(reset),
		.mm_address(mm_address),
		.mm_byteenable(2'b11),
		.mm_read(mm_read),
		.mm_write(mm_write),
		.mm_readdata(mm_readdata),
		.mm_writedata(mm_writedata),

		.read_regs({rec_state, address_out, x2, y2, cur_x, rec_available}),
		.write_regs(write_regs),
		
		.read_trigger(),
		.write_trigger(trigger_reg)
		);
		
	 // command control
	 always @(posedge clk or posedge reset) begin
		if(reset) command <= 0;
		else begin
			if(trigger==1)
				command <= write_regs[7*32-1:6*32];
			else
				command <= 0;
		end
	 end
	 
	 // draw pixel (not using)
	 always @(posedge clk or posedge reset) begin 
		if(reset)
			pixel_state = pixel_idle;
		else begin
			case(pixel_state)
			
				pixel_idle: begin
					pixel_irq <= 0;
					if (command[0]==1) pixel_state <= pixel_write;
					else pixel_state <= pixel_idle;
				end
				
				pixel_write: begin
					if(wait_request) pixel_state <= pixel_write;
					else begin 
						pixel_irq <= 1;
						pixel_state <= pixel_idle;
					end
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
					rec_irq <= 0;
					if(command[1]==1) rec_state <= rec_init;
					else rec_state <= rec_idle;
				end
				
				rec_init: begin
					rec_available = 0;
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
					else begin 
						rec_irq = 1;
						rec_state <= rec_idle;
					end
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
			cur_x <= 0;
			cur_y <= 0;		
		end
		else begin
			if(command>0) begin
				cur_x <= write_regs [1*32-1:0];
				x1 <= write_regs [1*32-1:0];
				x2 <= write_regs [2*32-1:1*32];
				cur_y <= write_regs [3*32-1:2*32];
				y1 <= write_regs [3*32-1:2*32];
				y2 <= write_regs [4*32-1:3*32];
				data_out <= write_regs [5*32+15:5*32];
			end
			else if (inc_x)
				cur_x = cur_x + 1;
			else if (inc_y) begin
				cur_y = cur_y + 1;
				cur_x = x1;
			end
			
			if(irq_disable) irq <= 0;
			else begin
				if(rec_irq | pixel_irq) irq <= 1;
				else if(!trigger_irq) irq <= irq;
				else irq <= 0;
			end
		end
	 
	 end
	 
	 // variable assignments
	 assign inc_x = rec_state[3];
	 assign inc_y = rec_state[4];
	 assign write_master = pixel_state | rec_state[5];
	 assign byteenable = 2'b11;
	 assign address_out = buffer_base + (cur_y*width + cur_x)*2;
	 assign trigger = trigger_reg[6];
	 assign trigger_irq = trigger_reg[7];
	 assign irq_disable = write_regs[32*9-1:32*8];

    
endmodule

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
