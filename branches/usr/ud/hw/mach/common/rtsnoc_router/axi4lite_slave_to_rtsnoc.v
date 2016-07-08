module axi4lite_slave_to_rtsnoc(
    clk_i, 
    
    axi_rst_i,
    noc_rst_i,
    
    axi_awaddr_i, axi_awvalid_i, axi_awready_o, axi_wdata_i,
    axi_wstrb_i, axi_wvalid_i, axi_wready_o, axi_bresp_o,
    axi_bvalid_o, axi_bready_i, axi_araddr_i, axi_arvalid_i,
    axi_arready_o, axi_rdata_o, axi_rresp_o, axi_rvalid_o, axi_rready_i,
    
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
    
    
    parameter WB_ADDR_WIDTH = 16;
    localparam ADDR_WIDTH = 32;
    localparam DATA_WIDTH = 32;
    
    localparam SOC_XY_SIZE = (2*SOC_SIZE_Y)+(2*SOC_SIZE_X);
    localparam NOC_HEADER_SIZE = SOC_XY_SIZE + 6;
    localparam NOC_BUS_SIZE = NOC_DATA_WIDTH + NOC_HEADER_SIZE;
        
        
    // System signals
    input clk_i;
    input axi_rst_i;
    input noc_rst_i;      
    
    // AXI signals
    input [ADDR_WIDTH-1:0] axi_awaddr_i;
    input axi_awvalid_i;
    output axi_awready_o;
    input [DATA_WIDTH-1:0] axi_wdata_i;
    input [(DATA_WIDTH/8)-1:0] axi_wstrb_i;
    input axi_wvalid_i;
    output axi_wready_o;
    output [1:0] axi_bresp_o;
    output axi_bvalid_o;
    input axi_bready_i;
    input [ADDR_WIDTH-1:0] axi_araddr_i;
    input axi_arvalid_i;
    output axi_arready_o;
    output [DATA_WIDTH-1:0] axi_rdata_o;
    output [1:0] axi_rresp_o;
    output axi_rvalid_o;
    input axi_rready_i;
    
    //NoC 
    output [NOC_BUS_SIZE-1:0] noc_din_o;
    output noc_wr_o;
    output noc_rd_o;
    input [NOC_BUS_SIZE-1:0] noc_dout_i;
    input noc_wait_i;
    input noc_nd_i;
    
    // ///////////////////////////////////////
    
    wire wb_rst;
    wire [DATA_WIDTH-1:0] wb_dat_i;
    wire [DATA_WIDTH-1:0] wb_dat_o;
    wire [ADDR_WIDTH-1:0] wb_adr;
    wire [(DATA_WIDTH/8)-1:0] wb_sel;
    wire wb_we;
    wire wb_cyc;
    wire wb_stb;
    wire wb_ack;
    
    axi4lite_to_wishbone #(
    	.ADDR_WIDTH(32),
    	.DATA_WIDTH(32)
    )
    axi4lite_to_wishbone(
    	.clk_i(clk_i),
    	.axi_rst_i(axi_rst_i),
    	.axi_awaddr_i(axi_awaddr_i),
    	.axi_awvalid_i(axi_awvalid_i),
    	.axi_awready_o(axi_awready_o),
    	.axi_wdata_i(axi_wdata_i),
    	.axi_wstrb_i(axi_wstrb_i),
    	.axi_wvalid_i(axi_wvalid_i),
    	.axi_wready_o(axi_wready_o),
    	.axi_bresp_o(axi_bresp_o),
    	.axi_bvalid_o(axi_bvalid_o),
    	.axi_bready_i(axi_bready_i),
    	.axi_araddr_i(axi_araddr_i),
    	.axi_arvalid_i(axi_arvalid_i),
    	.axi_arready_o(axi_arready_o),
    	.axi_rdata_o(axi_rdata_o),
    	.axi_rresp_o(axi_rresp_o),
    	.axi_rvalid_o(axi_rvalid_o),
    	.axi_rready_i(axi_rready_i),
    	.wb_rst_o(wb_rst),
    	.wb_dat_i(wb_dat_i),
    	.wb_dat_o(wb_dat_o),
    	.wb_adr_o(wb_adr),
    	.wb_sel_o(wb_sel),
    	.wb_we_o(wb_we),
    	.wb_cyc_o(wb_cyc),
    	.wb_stb_o(wb_stb),
    	.wb_ack_i(wb_ack),
    	.wb_err_i(1'b0),
    	.wb_rty_i(1'b0)
    );
    
    wishbone_slave_to_rtsnoc #(
        .WB_ADDR_WIDTH(WB_ADDR_WIDTH),
        .WB_NOC_DATA_WIDTH(NOC_DATA_WIDTH),
        .NOC_LOCAL_ADR(NOC_LOCAL_ADR),
        .NOC_X(NOC_X),
        .NOC_Y(NOC_Y),
        .NOC_LOCAL_ADR_TGT(NOC_LOCAL_ADR_TGT),
        .NOC_X_TGT(NOC_X_TGT),
        .NOC_Y_TGT(NOC_Y_TGT),
        .SOC_SIZE_X(SOC_SIZE_X),
        .SOC_SIZE_Y(SOC_SIZE_Y)
    )
    wishbone_slave_to_rtsnoc(
    	.clk_i(clk_i),
    	.rst_i(wb_rst),
    	
		.wb_cyc_i(wb_cyc),
    	.wb_stb_i(wb_stb),
    	.wb_adr_i(wb_adr[WB_ADDR_WIDTH-1:0]),
    	.wb_we_i(wb_we),
		.wb_sel_i(wb_sel),
    	.wb_dat_i(wb_dat_o),
    	.wb_dat_o(wb_dat_i),
    	.wb_ack_o(wb_ack),
    	
    	.noc_din_o(noc_din_o),
    	.noc_wr_o(noc_wr_o),
    	.noc_rd_o(noc_rd_o),
    	.noc_dout_i(noc_dout_i),
    	.noc_wait_i(noc_wait_i),
    	.noc_nd_i(noc_nd_i)
    );
    
endmodule
