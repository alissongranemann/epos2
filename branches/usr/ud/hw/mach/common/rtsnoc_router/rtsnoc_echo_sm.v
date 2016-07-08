
module rtsnoc_echo_sm (
    clk_i, rst_i,
    
    din_o, wr_o, rd_o, dout_i, wait_i, nd_i
    );
   
   
    parameter SOC_SIZE_X = 1; //Log2
    parameter SOC_SIZE_Y = 1; //Log2
    parameter NOC_DATA_WIDTH = 16; 
    parameter RX_ADDR = 3'b0;
    parameter TX_ADDR = 3'b0;
    
    localparam SOC_XY_SIZE = (2*SOC_SIZE_Y)+(2*SOC_SIZE_X);
    localparam NOC_HEADER_SIZE = SOC_XY_SIZE + 6;
    localparam NOC_BUS_SIZE = NOC_DATA_WIDTH + NOC_HEADER_SIZE;
    
             
    //Ports
    input clk_i;
    input rst_i;
    
    output [NOC_BUS_SIZE-1:0] din_o;
    output reg wr_o;
    output reg rd_o;
    input [NOC_BUS_SIZE-1:0] dout_i;
    input wait_i;
    input nd_i;
    
    wire [NOC_DATA_WIDTH-1:0] rx_data;
    wire [2:0] rx_local_dst;
    wire [SOC_SIZE_Y-1:0] rx_Y_dst;
    wire [SOC_SIZE_X-1:0] rx_X_dst;
    wire [2:0] rx_local_orig;
    wire [SOC_SIZE_Y-1:0] rx_Y_orig;
    wire [SOC_SIZE_X-1:0] rx_X_orig;
    assign {rx_X_orig,
            rx_Y_orig,
            rx_local_orig, 
            rx_X_dst,
            rx_Y_dst,
            rx_local_dst,
            rx_data} = dout_i[NOC_BUS_SIZE-1:0];
    
    reg [NOC_DATA_WIDTH-1:0] tx_data;
    reg [2:0] tx_local_dst;
    reg [SOC_SIZE_Y-1:0] tx_Y_dst;
    reg [SOC_SIZE_X-1:0] tx_X_dst;
    reg [2:0] tx_local_orig;
    reg [SOC_SIZE_Y-1:0] tx_Y_orig;
    reg [SOC_SIZE_X-1:0] tx_X_orig;
    assign din_o[NOC_BUS_SIZE-1:0] = {tx_X_orig,
                                      tx_Y_orig,
                                      tx_local_orig, 
                                      tx_X_dst,
                                      tx_Y_dst,
                                      tx_local_dst,
                                      tx_data};
    
   
   localparam STATE_READING = 2'd0;
   localparam STATE_WAITING = 2'd1;
   localparam STATE_WRITING = 2'd2;
   reg [1:0] state;
   
    always @(posedge clk_i)
        if(rst_i) begin
	       wr_o <= 0;
	       rd_o <= 0; 
	       tx_data <= 0;
           tx_local_dst <= 0;
           tx_Y_dst <= 0;
           tx_X_dst <= 0;
           tx_local_orig <= 0;
           tx_Y_orig <= 0;
           tx_X_orig <= 0;
           
           state <= STATE_READING;

        end
        else begin
	       case (state)
	       STATE_READING: begin
	           if(nd_i) begin
	               state <= STATE_WAITING;
	               
	               tx_X_orig <= rx_X_dst;
                   tx_Y_orig <= rx_Y_dst;
                   tx_local_orig <= TX_ADDR;//tx_local_orig <= rx_local_dst;
                   tx_X_dst <= rx_X_orig;
                   tx_Y_dst <= rx_Y_orig;
                   tx_local_dst <= rx_local_orig;
                   tx_data <= rx_data;
                   
                   rd_o <= 1'b1;
	           end
	       end
	       STATE_WAITING: begin
	           rd_o <= 1'b0;
	           if(~wait_i) begin
	               state <= STATE_WRITING;
	               wr_o <= 1'b1;
	           end
           end
           STATE_WRITING: begin
               wr_o <= 1'b0;
               state <= STATE_READING;
           end
           default: begin
               wr_o <= 0;
               rd_o <= 0; 
               tx_data <= 0;
               tx_local_dst <= 0;
               tx_Y_dst <= 0;
               tx_X_dst <= 0;
               tx_local_orig <= 0;
               tx_Y_orig <= 0;
               tx_X_orig <= 0;
           
               state <= STATE_READING;
           end
	       endcase
        end


   
endmodule

