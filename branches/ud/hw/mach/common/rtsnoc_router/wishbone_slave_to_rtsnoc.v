

module wishbone_slave_to_rtsnoc (
    clk_i, rst_i,
     
    wb_cyc_i, wb_stb_i, wb_adr_i, wb_sel_i, wb_we_i, wb_dat_i, wb_dat_o, wb_ack_o,
        
    noc_din_o, noc_wr_o, noc_rd_o, noc_dout_i, noc_wait_i, noc_nd_i
    );
   
    //Parameters
    parameter WB_ADDR_WIDTH = 6;
    parameter WB_NOC_DATA_WIDTH = 32;
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
    localparam NOC_BUS_SIZE = WB_NOC_DATA_WIDTH + NOC_HEADER_SIZE;
    
              
                    
    //Ports
    input clk_i;
    input rst_i;
    input wb_cyc_i;
    input wb_stb_i;
    input [WB_ADDR_WIDTH-1:0] wb_adr_i;
    input [3:0] wb_sel_i;
    input wb_we_i;
    input [WB_NOC_DATA_WIDTH-1:0] wb_dat_i;
    output reg [WB_NOC_DATA_WIDTH-1:0] wb_dat_o;
    output reg wb_ack_o;
    
    output [NOC_BUS_SIZE-1:0] noc_din_o;
    output reg noc_wr_o;
    output reg noc_rd_o;
    input [NOC_BUS_SIZE-1:0] noc_dout_i;
    input noc_wait_i;
    input noc_nd_i;
    

    wire [WB_NOC_DATA_WIDTH-1:0] noc_rx_data;
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
    
    reg [WB_NOC_DATA_WIDTH-1:0] noc_tx_data;
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
    
     
    
    localparam PKT_SIZE = 3;
    localparam PKT_WRITE    = 3'h0;
    localparam PKT_READ     = 3'h1;
    localparam PKT_INT      = 3'h2;//handled by rtsnoc_int_rx
    localparam PKT_ERR      = 3'h3;//not used
    localparam PKT_OK       = 3'h4;//not used
    
    localparam PKT_FILL_SIZE = WB_NOC_DATA_WIDTH - WB_ADDR_WIDTH - PKT_SIZE;
   
    
    localparam STATE_SIZE = 3;
    localparam MAIN_SM_WAIT_OP          = 3'h0;
    localparam MAIN_SM_TX_WRITE_CMD     = 3'h1;
    localparam MAIN_SM_TX_WRITE_DATA    = 3'h2;
    localparam MAIN_SM_TX_READ_CMD      = 3'h3;
    localparam MAIN_SM_WAIT_DATA        = 3'h4;
    reg [STATE_SIZE-1:0]  MAIN_SM_STATE; 
 
    
    
    always @(posedge clk_i)
        if(rst_i) begin
	       wb_ack_o <= 0;
	       noc_wr_o <= 0;
	       
	       noc_tx_data <= 0;
	       
	       wb_dat_o <= 0;
           
           noc_rd_o <= 0;
           
           MAIN_SM_STATE <= MAIN_SM_WAIT_OP;
           
        end
        else begin
            case(MAIN_SM_STATE)
            MAIN_SM_WAIT_OP: begin
                wb_ack_o <= 1'b0;
                if(wb_stb_i & wb_cyc_i) begin
                    if(wb_we_i) begin
                        noc_tx_data <= {PKT_WRITE, {PKT_FILL_SIZE{1'b0}}, wb_adr_i[WB_ADDR_WIDTH-1:0]};
                        noc_wr_o <= 1'b1;
                        MAIN_SM_STATE <= MAIN_SM_TX_WRITE_CMD;
                    end
                    else begin
                        noc_tx_data <= {PKT_READ, {PKT_FILL_SIZE{1'b0}}, wb_adr_i[WB_ADDR_WIDTH-1:0]};
                        noc_wr_o <= 1'b1;
                        MAIN_SM_STATE <= MAIN_SM_TX_READ_CMD;
                    end
                end
                else
                    MAIN_SM_STATE <= MAIN_SM_WAIT_OP;
            end
            MAIN_SM_TX_WRITE_CMD: begin
                noc_wr_o <= 1'b0;
                if(noc_wait_i) begin
                    MAIN_SM_STATE <= MAIN_SM_TX_WRITE_CMD;
                end
                else begin
                    noc_tx_data <= wb_dat_i;
                    noc_wr_o <= 1'b1;
                    MAIN_SM_STATE <= MAIN_SM_TX_WRITE_DATA;
                end
            end
            MAIN_SM_TX_WRITE_DATA: begin
                noc_wr_o <= 1'b0;
                if(noc_wait_i) begin
                    MAIN_SM_STATE <= MAIN_SM_TX_WRITE_DATA;
                end
                else begin
                    MAIN_SM_STATE <= MAIN_SM_WAIT_OP;
                    wb_ack_o <= 1'b1;
                end
            end
            MAIN_SM_TX_READ_CMD: begin
                noc_wr_o <= 1'b0;
                if(noc_nd_i) begin
                    noc_rd_o <= 1'b1;      
                    wb_dat_o <= noc_rx_data;
                    MAIN_SM_STATE <= MAIN_SM_WAIT_DATA;
                end
                else
                    MAIN_SM_STATE <= MAIN_SM_TX_READ_CMD;
            end
            MAIN_SM_WAIT_DATA: begin
                wb_ack_o <= 1'b1;
                noc_rd_o <= 1'b0;
                MAIN_SM_STATE <= MAIN_SM_WAIT_OP;
            end
            default: begin
                MAIN_SM_STATE <= MAIN_SM_WAIT_OP;
                wb_dat_o <= 0;
                wb_ack_o <= 0;
                noc_wr_o <= 0;
                noc_rd_o <= 0;
            end
            endcase
	       
        end
        
   
endmodule // timer

