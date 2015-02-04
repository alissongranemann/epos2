module axi4lite_ddr3_ctrl(
    clk_i, 
    
    axi_rst_i,
    
    axi_awaddr_i, axi_awvalid_i, axi_awready_o, axi_wdata_i,
    axi_wstrb_i, axi_wvalid_i, axi_wready_o, axi_bresp_o,
    axi_bvalid_o, axi_bready_i, axi_araddr_i, axi_arvalid_i,
    axi_arready_o, axi_rdata_o, axi_rresp_o, axi_rvalid_o, axi_rready_i,
    
    
    
);
       
        
    // Clk
    input clk_i;        
    
    // AXI signals
    input axi_rst_i;
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
    
    
    axi4_ddr3_contrl # (
        .REFCLK_FREQ                      (200),
                                        // # = 200 when design frequency <= 533 MHz,
                                        //   = 300 when design frequency > 533 MHz.
        .MMCM_ADV_BANDWIDTH  ("OPTIMIZED"),
                                        // MMCM programming algorithm
        .CLKFBOUT_MULT_F     (6),
                                        // write PLL VCO multiplier.
        .DIVCLK_DIVIDE       (2),
                                        // write PLL VCO divisor.
        .CLKOUT_DIVIDE       (3),
                                        // VCO output divisor for fast (memory) clocks.

        .nCK_PER_CLK         (2),
                                        // # of memory CKs per fabric clock.
        .tCK                 (2500),
                                        // memory tCK paramter.
                                        // # = Clock Period.

        .DEBUG_PORT          ("OFF"),
                                        // # = "ON" Enable debug signals/controls.
                                        //   = "OFF" Disable debug signals/controls.
        .SIM_BYPASS_INIT_CAL ("OFF"),
                                        // # = "OFF" -  Complete memory init &
                                        //              calibration sequence
                                        // # = "SKIP" - Skip memory init &
                                        //              calibration sequence
                                        // # = "FAST" - Skip memory init & use
                                        //              abbreviated calib sequence
        .nCS_PER_RANK        (1),
                                        // # of unique CS outputs per Rank for
                                        // phy.
        .DQS_CNT_WIDTH       (3),
                                        // # = ceil(log2(DQS_WIDTH)).
        .RANK_WIDTH          (1),
                                        // # = ceil(log2(RANKS)).
        .BANK_WIDTH          (3),
                                        // # of memory Bank Address bits.
        .CK_WIDTH            (1),
                                        // # of CK/CK# outputs to memory.
        .CKE_WIDTH           (1),
                                        // # of CKE outputs to memory.
        .COL_WIDTH           (10),
                                        // # of memory Column Address bits.
        .CS_WIDTH            (1),
                                        // # of unique CS outputs to memory.
        .DM_WIDTH            (8),
                                        // # of Data Mask bits.
        .DQ_WIDTH            (64),
        .DQS_WIDTH           (8),
                                        // # of DQS/DQS# bits.
        .ROW_WIDTH           (13),
                                        // # of memory Row Address bits.
        .BURST_MODE          ("8"),
                                        // Burst Length (Mode Register 0).
                                        // # = "8", "4", "OTF".
        .BM_CNT_WIDTH        (2),
                                        // # = ceil(log2(nBANK_MACHS)).
        .ADDR_CMD_MODE       ("1T" ),
                                        // # = "2T", "1T".
        .ORDERING            ("STRICT"),
                                        // # = "NORM", "STRICT", "RELAXED".
        .WRLVL               ("ON"),
                                        // # = "ON" - DDR3 SDRAM
                                        //   = "OFF" - DDR2 SDRAM.
        .PHASE_DETECT        ("ON"),
                                        // # = "ON", "OFF".
        .RTT_NOM             ("60"),
                                        // RTT_NOM (ODT) (Mode Register 1).
                                        // # = "DISABLED" - RTT_NOM disabled,
                                        //   = "120" - RZQ/2,
                                        //   = "60"  - RZQ/4,
                                        //   = "40"  - RZQ/6.
        .RTT_WR              ("OFF"),
                                        // RTT_WR (ODT) (Mode Register 2).
                                        // # = "OFF" - Dynamic ODT off,
                                        //   = "120" - RZQ/2,
                                        //   = "60"  - RZQ/4,
        .OUTPUT_DRV          ("HIGH"),
                                        // Output Driver Impedance Control (Mode Register 1).
                                        // # = "HIGH" - RZQ/7,
                                        //   = "LOW" - RZQ/6.
        .REG_CTRL            ("OFF"),
                                        // # = "ON" - RDIMMs,
                                        //   = "OFF" - Components, SODIMMs, UDIMMs.
        .nDQS_COL0           (6),
                                        // Number of DQS groups in I/O column #1.
        .nDQS_COL1           (2),
                                        // Number of DQS groups in I/O column #2.
        .nDQS_COL2           (0),
                                        // Number of DQS groups in I/O column #3.
        .nDQS_COL3           (0),
                                        // Number of DQS groups in I/O column #4.
        .DQS_LOC_COL0        (48'h050403020100),
                                        // DQS groups in column #1.
        .DQS_LOC_COL1        (16'h0706),
                                        // DQS groups in column #2.
        .DQS_LOC_COL2        (0),
                                        // DQS groups in column #3.
        .DQS_LOC_COL3        (0),
                                        // DQS groups in column #4.
        .tPRDI               (1_000_000),
                                        // memory tPRDI paramter.
        .tREFI               (7800000),
                                        // memory tREFI paramter.
        .tZQI                (128_000_000),
                                        // memory tZQI paramter.
        .ADDR_WIDTH          (27),
                                        // # = RANK_WIDTH + BANK_WIDTH
                                        //     + ROW_WIDTH + COL_WIDTH;
        .ECC_TEST            ("OFF"),
        .TCQ                 (100),
        .DATA_WIDTH          (64),
                                        // # of Data (DQ) bits.
        .PAYLOAD_WIDTH       (64),
        .INTERFACE           ("AXI4"),
                                        // Port Interface.
                                        // # = UI - User Interface,
                                        //   = AXI4 - AXI4 Interface.
        // AXI related parameters
        .C_S_AXI_ID_WIDTH    (1),
                                        // Width of all master and slave ID signals.
        .C_S_AXI_ADDR_WIDTH  (32),
                                        // Width of S_AXI_AWADDR, S_AXI_ARADDR, M_AXI_AWADDR and 
                                        // M_AXI_ARADDR for all SI/MI slots.
                                        // # = 32.
        .C_S_AXI_DATA_WIDTH  (32),
                                        // Width of WDATA and RDATA on SI slot.
                                        // Must be less or equal to APP_DATA_WIDTH.
                                        // # = 32, 64, 128, 256.
        .C_S_AXI_SUPPORTS_NARROW_BURST  (1),
                                       // Indicates whether to instatiate upsizer
                                       // Range: 0, 1
        .CALIB_ROW_ADD      (16'h0000),// Calibration row address
        .CALIB_COL_ADD      (12'h000), // Calibration column address
        .CALIB_BA_ADD       (3'h0),    // Calibration bank address
        .RST_ACT_LOW             (1),
                           // =1 for active low reset,
                           // =0 for active high.
        .IODELAY_GRP          ("IODELAY_MIG"),
                           //to phy_top
        .INPUT_CLK_TYPE          ("SINGLE_ENDED"),
                           // input clock type DIFFERENTIAL or SINGLE_ENDED
        .STARVE_LIMIT            (2)
                           // # = 2,3,4.
  )
  u_axi4_ddr3_contrl (

    .sys_clk           (sys_clk),
    .clk_ref               (clk_ref),
    .ddr3_dq                     (ddr3_dq),
    .ddr3_addr                   (ddr3_addr),
    .ddr3_ba                     (ddr3_ba),
    .ddr3_ras_n                  (ddr3_ras_n),
    .ddr3_cas_n                  (ddr3_cas_n),
    .ddr3_we_n                   (ddr3_we_n),
    .ddr3_reset_n                (ddr3_reset_n),
    .ddr3_cs_n                   (ddr3_cs_n),
    .ddr3_odt                    (ddr3_odt),
    .ddr3_cke                    (ddr3_cke),
    .ddr3_dm            (ddr3_dm),
    .ddr3_dqs_p                  (ddr3_dqs_p),
    .ddr3_dqs_n                  (ddr3_dqs_n),
    .ddr3_ck_p                   (ddr3_ck_p),
    .ddr3_ck_n                   (ddr3_ck_n),
    .sda      (sda),
    .scl      (scl),
    .phy_init_done     (phy_init_done),
    .aresetn             (aresetn),
    .s_axi_awid          (s_axi_awid),
    .s_axi_awaddr        (s_axi_awaddr),
    .s_axi_awlen         (s_axi_awlen),
    .s_axi_awsize        (s_axi_awsize),
    .s_axi_awburst       (s_axi_awburst),
    .s_axi_awlock        (s_axi_awlock),
    .s_axi_awcache       (s_axi_awcache),
    .s_axi_awprot        (s_axi_awprot),
    .s_axi_awqos         (s_axi_awqos),
    .s_axi_awvalid       (s_axi_awvalid),
    .s_axi_awready       (s_axi_awready),
    .s_axi_wdata         (s_axi_wdata),
    .s_axi_wstrb         (s_axi_wstrb),
    .s_axi_wlast         (s_axi_wlast),
    .s_axi_wvalid        (s_axi_wvalid),
    .s_axi_wready        (s_axi_wready),
    .s_axi_bid           (s_axi_bid),
    .s_axi_bresp         (s_axi_bresp),
    .s_axi_bvalid        (s_axi_bvalid),
    .s_axi_bready        (s_axi_bready),
    .s_axi_arid          (s_axi_arid),
    .s_axi_araddr        (s_axi_araddr),
    .s_axi_arlen         (s_axi_arlen),
    .s_axi_arsize        (s_axi_arsize),
    .s_axi_arburst       (s_axi_arburst),
    .s_axi_arlock        (s_axi_arlock),
    .s_axi_arcache       (s_axi_arcache),
    .s_axi_arprot        (s_axi_arprot),
    .s_axi_arqos         (s_axi_arqos),
    .s_axi_arvalid       (s_axi_arvalid),
    .s_axi_arready       (s_axi_arready),
    .s_axi_rid           (s_axi_rid),
    .s_axi_rdata         (s_axi_rdata),
    .s_axi_rresp         (s_axi_rresp),
    .s_axi_rlast         (s_axi_rlast),
    .s_axi_rvalid        (s_axi_rvalid),
    .s_axi_rready        (s_axi_rready),
    .tb_rst              (tb_rst),
    .tb_clk              (tb_clk),
    .sys_rst           (sys_rst)
    );
    
   
endmodule
