

module rtsnoc_int_tx (
    clk_i, rst_i,
     
    int_i,
    
    noc_din_o, noc_wr_o, noc_rd_o, noc_dout_i, noc_wait_i, noc_nd_i
    );
   
    //Parameters
    parameter NOC_DATA_WIDTH = 32;
    parameter NOC_LOCAL_ADR = 0;
    parameter NOC_X = 0;
    parameter NOC_Y = 0;
    parameter NOC_LOCAL_ADR_TGT = 0;
    parameter NOC_X_TGT = 0;
    parameter NOC_Y_TGT = 0;
    parameter SOC_SIZE_X = 1; //Log2
    parameter SOC_SIZE_Y = 1; //Log2 
    
    localparam SOC_XY_SIZE = (2*SOC_SIZE_Y)+(2*SOC_SIZE_X);
    localparam NOC_HEADER_SIZE = SOC_XY_SIZE + 6;
    localparam NOC_BUS_SIZE = NOC_DATA_WIDTH + NOC_HEADER_SIZE;
    
              
                    
    //Ports
    input clk_i;
    input rst_i;
   
    input int_i;
  
    output [NOC_BUS_SIZE-1:0] noc_din_o;
    output reg noc_wr_o;
    output reg noc_rd_o;
    input [NOC_BUS_SIZE-1:0] noc_dout_i;
    input noc_wait_i;
    input noc_nd_i;
    

    wire [NOC_DATA_WIDTH-1:0] noc_rx_data;
    wire [2:0] noc_rx_local_dst;
    wire [SOC_SIZE_Y-1:0] noc_rx_Y_dst;
    wire [SOC_SIZE_X-1:0] noc_rx_X_dst;
    wire [2:0] noc_rx_local_orig;
    wire [SOC_SIZE_Y-1:0] noc_rx_Y_orig;
    wire [SOC_SIZE_X-1:0] noc_rx_X_orig;
    assign {noc_rx_X_orig,
            noc_rx_Y_orig,
            noc_rx_local_orig, 
            noc_rx_X_dst,
            noc_rx_Y_dst,
            noc_rx_local_dst,
            noc_rx_data} = noc_dout_i[NOC_BUS_SIZE-1:0];
    
    reg [NOC_DATA_WIDTH-1:0] noc_tx_data;
    wire [2:0] noc_tx_local_dst = NOC_LOCAL_ADR_TGT;
    wire [SOC_SIZE_Y-1:0] noc_tx_Y_dst = NOC_Y_TGT;
    wire [SOC_SIZE_X-1:0] noc_tx_X_dst = NOC_X_TGT;
    wire [2:0] noc_tx_local_orig = NOC_LOCAL_ADR;
    wire [SOC_SIZE_Y-1:0] noc_tx_Y_orig = NOC_Y;
    wire [SOC_SIZE_X-1:0] noc_tx_X_orig = NOC_X;
    assign noc_din_o[NOC_BUS_SIZE-1:0] = {noc_tx_X_orig,
                                          noc_tx_Y_orig,
                                          noc_tx_local_orig, 
                                          noc_tx_X_dst,
                                          noc_tx_Y_dst,
                                          noc_tx_local_dst,
                                          noc_tx_data};
    
     
    
    localparam PKT_SIZE = 2;
    localparam PKT_INT_UP       = 2'h1;
    localparam PKT_INT_DOWN     = 2'h2;
    localparam PKT_INT_PULSE    = 2'h3;
    
       
    
    always @(posedge clk_i)
        if(rst_i) begin
         
           noc_wr_o <= 0;
           noc_tx_data <= 0;
           noc_rd_o <= 0;
           
           
        end
        else begin
            //TODO
            //wait int
            //if int goes down on next cycle tx PKT_INT_PULSE
            //else tx PKT_INT_UP then wait for int=0 the tx PKT_INT_DOWN
            
        end
        
   
endmodule

