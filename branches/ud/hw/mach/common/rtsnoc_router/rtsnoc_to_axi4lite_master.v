module rtsnoc_to_axi4lite_master(
    clk_i, 
    
    axi_rst_i,
    noc_rst_i,
    
    axi_awaddr_o, axi_awvalid_o, axi_awready_i, axi_wdata_o,
    axi_wstrb_o, axi_wvalid_o, axi_wready_i, axi_bresp_i,
    axi_bvalid_i, axi_bready_o, axi_araddr_o, axi_arvalid_o,
    axi_arready_i, axi_rdata_i, axi_rresp_i, axi_rvalid_i, axi_rready_o,
    
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
    output [ADDR_WIDTH-1:0] axi_awaddr_o;
    output axi_awvalid_o;
    input axi_awready_i;
    output [DATA_WIDTH-1:0] axi_wdata_o;
    output [(DATA_WIDTH/8)-1:0] axi_wstrb_o;
    output axi_wvalid_o;
    input axi_wready_i;
    input [1:0] axi_bresp_i;
    input axi_bvalid_i;
    output axi_bready_o;
    output [ADDR_WIDTH-1:0] axi_araddr_o;
    output axi_arvalid_o;
    input axi_arready_i;
    input [DATA_WIDTH-1:0] axi_rdata_i;
    input [1:0] axi_rresp_i;
    input axi_rvalid_i;
    output axi_rready_o;
    
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
    
    rtsnoc_to_wishbone_master #(
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
    rtsnoc_to_wishbone_master(
    	.clk_i(clk_i),
    	.rst_i(wb_rst),
    	
		.wb_cyc_o(wb_cyc),
    	.wb_stb_o(wb_stb),
    	.wb_adr_o(wb_adr[WB_ADDR_WIDTH-1:0]),
    	.wb_we_o(wb_we),
		.wb_sel_o(wb_sel),
    	.wb_dat_o(wb_dat_o),
    	.wb_dat_i(wb_dat_i),
    	.wb_ack_i(wb_ack),
    	
    	.noc_din_o(noc_din_o),
    	.noc_wr_o(noc_wr_o),
    	.noc_rd_o(noc_rd_o),
    	.noc_dout_i(noc_dout_i),
    	.noc_wait_i(noc_wait_i),
    	.noc_nd_i(noc_nd_i)
    );
    
    assign wb_rst = ~axi_rst_i;
    
    wishbone_to_axi4lite #(
        .ADDR_WIDTH(32),
        .DATA_WIDTH(32)
    )
    wishbone_to_axi4lite(
        .clk_i(clk_i),
        
        .wb_rst_i(1'b0),
        .wb_dat_i(wb_dat_o),
        .wb_dat_o(wb_dat_i),
        .wb_adr_i(wb_adr),
        .wb_sel_i(wb_sel),
        .wb_we_i(wb_we),
        .wb_cyc_i(wb_cyc),
        .wb_stb_i(wb_stb),
        .wb_ack_o(wb_ack),
        
        .axi_rst_o(),
        .axi_awaddr_o(axi_awaddr_o),
        .axi_awvalid_o(axi_awvalid_o),
        .axi_awready_i(axi_awready_i),
        .axi_wdata_o(axi_wdata_o),
        .axi_wstrb_o(axi_wstrb_o),
        .axi_wvalid_o(axi_wvalid_o),
        .axi_wready_i(axi_wready_i),
        .axi_bresp_i(axi_bresp_i),
        .axi_bvalid_i(axi_bvalid_i),
        .axi_bready_o(axi_bready_o),
        .axi_araddr_o(axi_araddr_o),
        .axi_arvalid_o(axi_arvalid_o),
        .axi_arready_i(axi_arready_i),
        .axi_rdata_i(axi_rdata_i),
        .axi_rresp_i(axi_rresp_i),
        .axi_rvalid_i(axi_rvalid_i),
        .axi_rready_o(axi_rready_o)
    );
    
endmodule
