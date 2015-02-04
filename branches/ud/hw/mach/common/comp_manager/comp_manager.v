module comp_manager (
    clk_i,
    rst_i,
    wb_cyc_i,
    wb_stb_i,
    wb_adr_i,
    wb_sel_i,
    wb_we_i,
    wb_dat_i,
    wb_dat_o,
    wb_ack_o,
    noc_int_o,
    noc_din_o,
    noc_wr_o,
    noc_rd_o,
    noc_dout_i,
    noc_wait_i,
    noc_nd_i
);
    parameter NOC_X             = 0;
    parameter NOC_Y             = 0;
    parameter NOC_LOCAL_ADR     = 0;
    parameter NOC_DATA_WIDTH    = 56;
    parameter SOC_SIZE_X        = 1; // Log2
    parameter SOC_SIZE_Y        = 1; // Log2
    parameter BUFFER_SIZE       = 4;
    parameter BUFFER_SIZE_LOG2  = 2;

    localparam SOC_XY_SIZE      = (2*SOC_SIZE_Y)+(2*SOC_SIZE_X);
    localparam NOC_HEADER_SIZE  = SOC_XY_SIZE+6;
    localparam NOC_BUS_SIZE     = NOC_DATA_WIDTH+NOC_HEADER_SIZE;
    localparam WB_ADDR_WIDTH    = 32;
    localparam WB_DATA_WIDTH    = 32;

    input                           clk_i;
    input                           rst_i;
    input                           wb_cyc_i;
    input                           wb_stb_i;
    input [WB_ADDR_WIDTH-1:0]       wb_adr_i;
    input [3:0]                     wb_sel_i;
    input                           wb_we_i;
    input [WB_DATA_WIDTH-1:0]       wb_dat_i;
    output reg [WB_DATA_WIDTH-1:0]  wb_dat_o;
    output reg                      wb_ack_o;
    output reg                      noc_int_o;
    output [NOC_BUS_SIZE-1:0]       noc_din_o;
    output reg                      noc_wr_o;
    output reg                      noc_rd_o;
    input [NOC_BUS_SIZE-1:0]        noc_dout_i;
    input                           noc_wait_i;
    input                           noc_nd_i;

    // Memory mapped regs
    // All regs are 32 bits (however, some regs may be smaller: reads to undef
    // bits return 0 e writes are ignored)

    // Address space logic:
    // b7 -> select internal space: 0 control space
    //                              1 buffers space

    // For control space:
    // b6..b0 -> select register

    // For buffers:
    // b6..b3 -> select internal buffer register (up to 15 registers)
    // b2..b0 -> select buffer (up to 8 buffers)

    // Obs.: Some read-only regs are defined as wire
    //       The process that drives the data is indicated
    localparam ADDR_SPACE_CTRL = 1'h0;
    localparam ADDR_SPACE_BUFF = 1'h1;

    localparam ADDR_CMD                 = 8'h00;
    localparam ADDR_CMD_RESULT          = 8'h04;
    localparam ADDR_CMD_IDLE            = 8'h08;
    localparam ADDR_STATUS_AGENT_INT    = 8'h0C;
    localparam ADDR_STATUS_AGENT_BUFFER = 8'h10;
    localparam ADDR_INFO_NOC_X          = 8'h14;
    localparam ADDR_INFO_NOC_Y          = 8'h18;
    localparam ADDR_INFO_NOC_LOCAL      = 8'h1C;
    localparam ADDR_INFO_BUFFER_SIZE    = 8'h20;

    localparam ADDR_PROXY_BUFF_PHY_X        = 4'h0;
    localparam ADDR_PROXY_BUFF_PHY_Y        = 4'h1;
    localparam ADDR_PROXY_BUFF_PHY_LOCAL    = 4'h2;
    localparam ADDR_BUFF_MSG_TYPE           = 4'h3;
    // Map to the same address
    localparam ADDR_BUFF_INSTANCE_ID        = 4'h4;
    localparam ADDR_BUFF_TYPE_ID            = 4'h5;
    localparam ADDR_BUFF_DATA               = 4'h6;
    // Map to the same address
    localparam ADDR_BUFF_TX                 = 4'h7;
    // Map to the same address
    localparam ADDR_BUFF_RX                 = 4'h8;
    // Map to the same address
    localparam ADDR_AGENT_DISP_ADDR         = 4'h9;
    localparam ADDR_AGENT_DISP_OBJ_ADDR     = 4'hA;

    wire       addr_space_sel       = wb_adr_i[9];
    wire [6:0] addr_ctrl_reg_sel    = wb_adr_i[8:0];
    wire [3:0] addr_buffer_reg_sel  = wb_adr_i[8:5];
    wire [2:0] addr_buffer_sel      = wb_adr_i[4:2];

    // Control space
    wire[1:0]               cmd;
    wire[2:0]               buf_id;
    reg [WB_DATA_WIDTH-1:0] cmd_result;
    wire                    status_cmd_idle;
    reg                     status_agent_int;
    reg [WB_DATA_WIDTH-1:0] status_agent_buffer;

    // TODO: Add regs to keep the last msg that caused an error
    reg [SOC_SIZE_X-1:0]    buff_proxy_phy_X[BUFFER_SIZE-1:0];
    reg [SOC_SIZE_Y-1:0]    buff_proxy_phy_Y[BUFFER_SIZE-1:0];
    reg [2:0]               buff_proxy_phy_LOCAL[BUFFER_SIZE-1:0];
    reg [2:0]               buff_tx_msg_type[BUFFER_SIZE-1:0];
    reg [2:0]               buff_rx_msg_type[BUFFER_SIZE-1:0];
    reg [7:0]               buff_instance_id[BUFFER_SIZE-1:0];
    reg [7:0]               buff_type_id[BUFFER_SIZE-1:0];
    reg [WB_DATA_WIDTH-1:0] buff_data_tx[BUFFER_SIZE-1:0];
    reg [WB_DATA_WIDTH-1:0] buff_data_rx[BUFFER_SIZE-1:0];
    reg [BUFFER_SIZE-1:0]   buff_tx;
    reg [BUFFER_SIZE-1:0]   buff_tx_wait;
    reg [BUFFER_SIZE-1:0]   buff_rx;
    reg [BUFFER_SIZE-1:0]   buff_rx_ack;
    reg [WB_DATA_WIDTH-1:0] buff_agent_disp_addr[BUFFER_SIZE-1:0];
    reg [WB_DATA_WIDTH-1:0] buff_agent_disp_obj_addr[BUFFER_SIZE-1:0];
    reg [SOC_SIZE_X-1:0]    buff_agent_phy_X[BUFFER_SIZE-1:0];
    reg [SOC_SIZE_Y-1:0]    buff_agent_phy_Y[BUFFER_SIZE-1:0];
    reg [2:0]               buff_agent_phy_LOCAL[BUFFER_SIZE-1:0];
    reg [BUFFER_SIZE-1:0]   buff_alloc_free;
    reg [BUFFER_SIZE-1:0]   buff_alloc_to_proxy_or_agent; // 0 = proxy, 1 = agent

    // 'Global' constants and helpers
    localparam MSG_TYPE_CALL        = 3'd0;
    localparam MSG_TYPE_RESP        = 3'd1;
    localparam MSG_TYPE_CALL_DATA   = 3'd2;
    localparam MSG_TYPE_RESP_DATA   = 3'd3;
    localparam MSG_TYPE_ERROR       = 3'd4;

    // Main and bus process
    integer j;

    // cmd related
    localparam CMD_ALLOC_PROXY_BUFFER   = 2'd0;
    localparam CMD_ALLOC_AGENT_BUFFER   = 2'd1;
    localparam CMD_FREE_BUFFER          = 2'd2;
    localparam CMD_RESULT_ERR           = 32'hFFFFFFFF;

    assign status_cmd_idle  = 1'b1;
    assign cmd              = wb_dat_i[1:0];
    assign buf_id           = wb_dat_i[4:2];

    wire cmd_search_found;
    wire[BUFFER_SIZE_LOG2-1:0] cmd_search_result;

    parallel_bit_search #(
        .ADDR_WIDTH(BUFFER_SIZE_LOG2)
    )
    search_buffer (
        .cam_data_in(buff_alloc_free),
        .cam_hit_out(cmd_search_found),
        .cam_addr_out(cmd_search_result)
    );

    always @(posedge clk_i)
    begin: process_main
        if(rst_i) begin
            wb_dat_o <= 0;
            wb_ack_o <= 0;

            for(j = 0; j < BUFFER_SIZE; j = j + 1)
            begin
                buff_proxy_phy_X[j]         <= 0;
                buff_proxy_phy_Y[j]         <= 0;
                buff_proxy_phy_LOCAL[j]     <= 0;
                buff_tx_msg_type[j]         <= 0;
                buff_instance_id[j]         <= 0;
                buff_type_id[j]             <= 0;
                buff_data_tx[j]             <= 0;
                buff_agent_disp_addr[j]     <= 0;
                buff_agent_disp_obj_addr[j] <= 0;
            end

            buff_tx     <= 0;
            buff_rx_ack <= 0;

            // cmd related
            cmd_result                      <= 0;
            buff_alloc_free                 <= {{BUFFER_SIZE{1'b1}}};
            buff_alloc_to_proxy_or_agent    <= 0;
        end
        else begin
            wb_ack_o    <= 1'b0;
            buff_tx     <= 0;
            buff_rx_ack <= 0;

            if(wb_stb_i & wb_cyc_i & ~wb_ack_o) begin
                case (addr_space_sel)
                ADDR_SPACE_CTRL: begin
                    case (addr_ctrl_reg_sel)
                    ADDR_CMD: begin
                        if(wb_we_i) begin
                            if(cmd_search_found) begin
                                if(cmd == CMD_ALLOC_PROXY_BUFFER) begin
                                    buff_alloc_free[cmd_search_result]                                      <= 0;
                                    cmd_result                                                              <= cmd_search_result;
                                    buff_alloc_to_proxy_or_agent[cmd_search_result[BUFFER_SIZE_LOG2-1:0]]   <= 0;
                                end
                                else if(cmd == CMD_ALLOC_AGENT_BUFFER) begin
                                    buff_alloc_free[cmd_search_result]                                      <= 0;
                                    cmd_result                                                              <= cmd_search_result;
                                    buff_alloc_to_proxy_or_agent[cmd_search_result[BUFFER_SIZE_LOG2-1:0]]   <= 1;
                                end
                                else if(cmd == CMD_FREE_BUFFER) begin
                                    buff_alloc_free[buf_id] <= 1;
                                    cmd_result              <= buf_id;
                                end
                            end
                            else begin
                                cmd_result <= CMD_RESULT_ERR;
                            end
                        end
                        else begin
                            wb_dat_o[WB_DATA_WIDTH-1:0] <= 32'b0;
                        end
                    end
                    ADDR_CMD_RESULT: begin
                        //Read-only
                        if(!wb_we_i) begin
                            wb_dat_o <= cmd_result;
                        end
                    end
                    ADDR_CMD_IDLE: begin
                        //Read-only
                        if(!wb_we_i) begin
                            wb_dat_o <= {31'b0, status_cmd_idle};
                        end
                    end
                    ADDR_STATUS_AGENT_INT: begin
                        //Read-only
                        if(!wb_we_i) begin
                            wb_dat_o <= {31'b0, status_agent_int};
                        end
                    end
                    ADDR_STATUS_AGENT_BUFFER: begin
                        //Read-only
                        if(!wb_we_i) begin
                            wb_dat_o <= status_agent_buffer;
                        end
                    end
                    ADDR_INFO_NOC_X: begin
                        //Read-only
                        if(!wb_we_i) begin
                            wb_dat_o <= NOC_X;
                        end
                    end
                    ADDR_INFO_NOC_Y: begin
                        //Read-only
                        if(!wb_we_i) begin
                            wb_dat_o <= NOC_Y;
                        end
                    end
                    ADDR_INFO_NOC_LOCAL: begin
                        //Read-only
                        if(!wb_we_i) begin
                            wb_dat_o <= NOC_LOCAL_ADR;
                        end
                    end
                    ADDR_INFO_BUFFER_SIZE: begin
                        //Read-only
                        if(!wb_we_i) begin
                            wb_dat_o <= BUFFER_SIZE;
                        end
                    end
                    default: begin
                        wb_dat_o <= 0;
                    end
                    endcase
                end
                ADDR_SPACE_BUFF: begin
                    case (addr_buffer_reg_sel)
                    ADDR_PROXY_BUFF_PHY_X: begin
                        if(wb_we_i) begin
                            buff_proxy_phy_X[addr_buffer_sel] <= wb_dat_i[SOC_SIZE_X-1:0];
                        end
                        else begin
                            wb_dat_o <= {{(WB_DATA_WIDTH-SOC_SIZE_X){1'b0}},
                            buff_proxy_phy_X[addr_buffer_sel]};
                        end
                    end
                    ADDR_PROXY_BUFF_PHY_Y: begin
                        if(wb_we_i) begin
                            buff_proxy_phy_Y[addr_buffer_sel] <= wb_dat_i[SOC_SIZE_Y-1:0];
                        end
                        else begin
                            wb_dat_o <= {{(WB_DATA_WIDTH-SOC_SIZE_Y){1'b0}},
                            buff_proxy_phy_Y[addr_buffer_sel]};
                        end
                    end
                    ADDR_PROXY_BUFF_PHY_LOCAL: begin
                        if(wb_we_i) begin
                            buff_proxy_phy_LOCAL[addr_buffer_sel] <= wb_dat_i[2:0];
                        end
                        else begin
                            wb_dat_o <= {{(WB_DATA_WIDTH-3){1'b0}},
                            buff_proxy_phy_LOCAL[addr_buffer_sel]};
                        end
                    end
                    ADDR_BUFF_MSG_TYPE: begin
                        if(wb_we_i) begin
                            buff_tx_msg_type[addr_buffer_sel] <= wb_dat_i[2:0];
                        end
                        else begin
                            wb_dat_o <= {{(WB_DATA_WIDTH-3){1'b0}},
                            buff_rx_msg_type[addr_buffer_sel]};
                        end
                    end
                    ADDR_BUFF_INSTANCE_ID: begin
                        if(wb_we_i) begin
                            buff_instance_id[addr_buffer_sel] <= wb_dat_i[7:0];
                        end
                        else begin
                            wb_dat_o <= {{(WB_DATA_WIDTH-8){1'b0}},
                            buff_instance_id[addr_buffer_sel]};
                        end
                    end
                    ADDR_BUFF_TYPE_ID: begin
                        if(wb_we_i) begin
                            buff_type_id[addr_buffer_sel] <= wb_dat_i[7:0];
                        end
                        else begin
                            wb_dat_o <= {{(WB_DATA_WIDTH-8){1'b0}},
                            buff_type_id[addr_buffer_sel]};
                        end
                    end
                    ADDR_BUFF_DATA: begin
                        if(wb_we_i) begin
                            buff_data_tx[addr_buffer_sel] <= wb_dat_i;
                        end
                        else begin
                            wb_dat_o <= buff_data_rx[addr_buffer_sel];
                        end
                    end
                    ADDR_BUFF_TX: begin
                        if(wb_we_i) begin
                            if(~buff_tx_wait[addr_buffer_sel])
                                buff_tx[addr_buffer_sel] <= wb_dat_i[0];
                        end
                        else begin
                            wb_dat_o <= {31'b0, buff_tx_wait[addr_buffer_sel]};
                        end
                    end
                    ADDR_BUFF_RX: begin
                        if(wb_we_i) begin
                            if(buff_rx[addr_buffer_sel])
                                buff_rx_ack[addr_buffer_sel] <= ~wb_dat_i[0];
                        end
                        else begin
                            wb_dat_o <= {31'b0, buff_rx[addr_buffer_sel]};
                        end
                    end
                    ADDR_AGENT_DISP_ADDR: begin
                        if(wb_we_i) begin
                            buff_agent_disp_addr[addr_buffer_sel] <= wb_dat_i;
                        end
                        else begin
                            wb_dat_o <= buff_agent_disp_addr[addr_buffer_sel];
                        end
                    end
                    ADDR_AGENT_DISP_OBJ_ADDR: begin
                        if(wb_we_i) begin
                            buff_agent_disp_obj_addr[addr_buffer_sel] <= wb_dat_i;
                        end
                        else begin
                            wb_dat_o <= buff_agent_disp_obj_addr[addr_buffer_sel];
                        end
                    end
                    default: begin
                        wb_dat_o <= 0;
                    end
                    endcase
                end
                default: begin
                    wb_dat_o <= 0;
                end
                endcase
                wb_ack_o <= 1'b1;
            end
        end
    end

    // NoC tx process
    wire [NOC_DATA_WIDTH-1:0]   noc_tx_data;
    reg [7:0]                   noc_tx_msg_msg_type;
    reg [7:0]                   noc_tx_msg_instance_id;
    reg [7:0]                   noc_tx_msg_type_id;
    reg [31:0]                  noc_tx_msg_payload;

    assign noc_tx_data = {noc_tx_msg_type_id, noc_tx_msg_instance_id,
        noc_tx_msg_msg_type, noc_tx_msg_payload};

    reg [2:0]               noc_tx_local_dst;
    reg [SOC_SIZE_Y-1:0]    noc_tx_Y_dst;
    reg [SOC_SIZE_X-1:0]    noc_tx_X_dst;
    wire [2:0]              noc_tx_local_orig   = NOC_LOCAL_ADR;
    wire [SOC_SIZE_Y-1:0]   noc_tx_Y_orig       = NOC_Y;
    wire [SOC_SIZE_X-1:0]   noc_tx_X_orig       = NOC_X;

    assign noc_din_o[NOC_BUS_SIZE-1:0] = {noc_tx_X_orig, noc_tx_Y_orig,
        noc_tx_local_orig, noc_tx_X_dst, noc_tx_Y_dst, noc_tx_local_dst,
        noc_tx_data};

    reg [1:0] tx_state;
    localparam TX_STATE_SEARCH  = 2'd0;
    localparam TX_STATE_MSG     = 2'd1;
    localparam TX_STATE_TX      = 2'd2;

    wire                        tx_search_found;
    wire[BUFFER_SIZE_LOG2-1:0]  tx_search_result;
    reg[BUFFER_SIZE_LOG2-1:0]   tx_search_result_reg;

    parallel_bit_search #(
        .ADDR_WIDTH(BUFFER_SIZE_LOG2)
    )
    search_tx(
        .cam_data_in(buff_tx_wait),
        .cam_hit_out(tx_search_found),
        .cam_addr_out(tx_search_result)
    );

    wire [SOC_SIZE_X-1:0]    tx_sel_buff_proxy_phy_X                = buff_proxy_phy_X[tx_search_result_reg];
    wire [SOC_SIZE_Y-1:0]    tx_sel_buff_proxy_phy_Y                = buff_proxy_phy_Y[tx_search_result_reg];
    wire [2:0]               tx_sel_buff_proxy_phy_LOCAL            = buff_proxy_phy_LOCAL[tx_search_result_reg];
    wire [SOC_SIZE_X-1:0]    tx_sel_buff_agent_phy_X                = buff_agent_phy_X[tx_search_result_reg];
    wire [SOC_SIZE_Y-1:0]    tx_sel_buff_agent_phy_Y                = buff_agent_phy_Y[tx_search_result_reg];
    wire [2:0]               tx_sel_buff_agent_phy_LOCAL            = buff_agent_phy_LOCAL[tx_search_result_reg];
    wire [2:0]               tx_sel_buff_msg_type                   = buff_tx_msg_type[tx_search_result_reg];
    wire [7:0]               tx_sel_buff_instance_id                = buff_instance_id[tx_search_result_reg];
    wire [7:0]               tx_sel_buff_type_id                    = buff_type_id[tx_search_result_reg];
    wire [WB_DATA_WIDTH-1:0] tx_sel_buff_data_tx                    = buff_data_tx[tx_search_result_reg];
    wire                     tx_sel_buff_alloc_to_proxy_or_agent    = buff_alloc_to_proxy_or_agent[tx_search_result_reg];

    integer l;

    always @(posedge clk_i)
    begin: process_noc_tx
        if(rst_i) begin
            noc_wr_o                <= 0;
            noc_tx_msg_msg_type     <= 0;
            noc_tx_msg_instance_id  <= 0;
            noc_tx_msg_type_id      <= 0;
            noc_tx_msg_payload      <= 0;
            noc_tx_local_dst        <= 0;
            noc_tx_Y_dst            <= 0;
            noc_tx_X_dst            <= 0;
            buff_tx_wait            <= 0;
            tx_state                <= TX_STATE_SEARCH;
            tx_search_result_reg    <= 0;
        end
        else begin
            for(l = 0; l < BUFFER_SIZE; l = l + 1)
            begin
                if(buff_tx[l]) buff_tx_wait[l] <= 1'b1;
            end

            case (tx_state)
            TX_STATE_SEARCH: begin
                noc_wr_o <= 0;
                if(tx_search_found) begin
                    tx_state                <= TX_STATE_MSG;
                    tx_search_result_reg    <= tx_search_result;
                end
                else begin
                    tx_state <= TX_STATE_SEARCH;
                end
            end
            TX_STATE_MSG: begin
                tx_state                            <= TX_STATE_TX;
                buff_tx_wait[tx_search_result_reg]  <= 1'b0;

                if(tx_sel_buff_alloc_to_proxy_or_agent) begin
                    // To agent
                    noc_tx_X_dst        <= tx_sel_buff_agent_phy_X;
                    noc_tx_Y_dst        <= tx_sel_buff_agent_phy_Y;
                    noc_tx_local_dst    <= tx_sel_buff_agent_phy_LOCAL;
                end
                else begin
                    // To proxy
                    noc_tx_X_dst        <= tx_sel_buff_proxy_phy_X;
                    noc_tx_Y_dst        <= tx_sel_buff_proxy_phy_Y;
                    noc_tx_local_dst    <= tx_sel_buff_proxy_phy_LOCAL;
                end

                noc_tx_msg_msg_type     <= tx_sel_buff_msg_type;
                noc_tx_msg_instance_id  <= tx_sel_buff_instance_id;
                noc_tx_msg_type_id      <= tx_sel_buff_type_id;
                noc_tx_msg_payload      <= tx_sel_buff_data_tx;
            end
            TX_STATE_TX: begin
                if(noc_wait_i) begin
                    tx_state <= TX_STATE_TX;
                end
                else begin
                    noc_wr_o <= 1;
                    tx_state <= TX_STATE_SEARCH;
                end
            end
            default: begin
                tx_state <= TX_STATE_SEARCH;
                noc_wr_o <= 0;
            end
            endcase
        end
    end

    // NoC rx process
    wire [NOC_DATA_WIDTH-1:0]   noc_rx_data;
    wire [2:0]                  noc_rx_local_dst;
    wire [SOC_SIZE_Y-1:0]       noc_rx_Y_dst;
    wire [SOC_SIZE_X-1:0]       noc_rx_X_dst;
    wire [2:0]                  noc_rx_local_orig;
    wire [SOC_SIZE_Y-1:0]       noc_rx_Y_orig;
    wire [SOC_SIZE_X-1:0]       noc_rx_X_orig;

    assign {noc_rx_X_orig, noc_rx_Y_orig, noc_rx_local_orig, noc_rx_X_dst,
        noc_rx_Y_dst, noc_rx_local_dst, noc_rx_data}
        = noc_dout_i[NOC_BUS_SIZE-1:0];

    wire [7:0]  noc_rx_msg_msg_type;
    wire [7:0]  noc_rx_msg_instance_id;
    wire [7:0]  noc_rx_msg_type_id;
    wire [31:0] noc_rx_msg_payload;

    assign {noc_rx_msg_type_id, noc_rx_msg_instance_id, noc_rx_msg_msg_type,
        noc_rx_msg_payload} = noc_rx_data;

    reg [2:0] rx_state;
    localparam RX_STATE_RESET   = 3'd0;
    localparam RX_STATE_RX      = 3'd1;
    localparam RX_STATE_SEARCH  = 3'd2;
    localparam RX_STATE_PROXY   = 3'd3;
    localparam RX_STATE_AGENT   = 3'd4;

    reg[BUFFER_SIZE_LOG2-1:0] rx_idx;

    wire rx_buffer_avaliable = (buff_rx[rx_idx] & buff_rx_ack[rx_idx]) |
        ~buff_rx[rx_idx];

    integer m;

    task task_rx_next_buffer;
    begin
        rx_state    <= RX_STATE_SEARCH;
        rx_idx      <= rx_idx + 1;
    end
    endtask

    task task_rx_sm_reset;
    begin
        rx_state    <= RX_STATE_RESET;
        rx_idx      <= 0;
        noc_rd_o    <= 1;
    end
    endtask

    wire                    rx_sel_buff_allocated               = ~buff_alloc_free[rx_idx];
    wire                    rx_sel_buff_alloc_to_proxy_or_agent = buff_alloc_to_proxy_or_agent[rx_idx];
    wire [7:0]              rx_sel_buff_instance_id             = buff_instance_id[rx_idx];
    wire [7:0]              rx_sel_buff_type_id                 = buff_type_id[rx_idx];
    wire [SOC_SIZE_X-1:0]   rx_sel_buff_agent_phy_X             = buff_proxy_phy_X[rx_idx];
    wire [SOC_SIZE_Y-1:0]   rx_sel_buff_agent_phy_Y             = buff_proxy_phy_Y[rx_idx];
    wire [2:0]              rx_sel_buff_agent_phy_LOCAL         = buff_proxy_phy_LOCAL[rx_idx];
    wire                    rx_sel_buff_match                   = (rx_sel_buff_instance_id == noc_rx_msg_instance_id) &&
        (rx_sel_buff_type_id == noc_rx_msg_type_id) && rx_sel_buff_allocated;

    always @(posedge clk_i)
    begin: process_noc_rx
        if(rst_i) begin
            for(m = 0; m < BUFFER_SIZE; m = m + 1)
            begin
                buff_data_rx[m]         <= 0;
                buff_rx_msg_type[m]     <= 0;
                buff_agent_phy_LOCAL[m] <= 0;
                buff_agent_phy_X[m]     <= 0;
                buff_agent_phy_Y[m]     <= 0;
            end

            buff_rx <= 0;
            task_rx_sm_reset();
        end
        else begin
            noc_rd_o    <= 0;
            buff_rx     <= buff_rx & ~buff_rx_ack;

            case (rx_state)
            RX_STATE_RESET: begin
                rx_state <= RX_STATE_RX;
            end
            RX_STATE_RX: begin
                if(noc_nd_i)
                    rx_state <= RX_STATE_SEARCH;
                else
                    rx_state <= RX_STATE_RX;
            end
            RX_STATE_SEARCH: begin
                if(rx_sel_buff_match) begin
                    if(rx_sel_buff_alloc_to_proxy_or_agent)
                        rx_state <= RX_STATE_AGENT;
                    else
                        rx_state <= RX_STATE_PROXY;
                end
                else begin
                    if(rx_idx == (BUFFER_SIZE-1)) begin
                        task_rx_sm_reset();
                        // TODO: Notify and buffer_not_found error and store the msg
                    end
                    else begin
                        task_rx_next_buffer();
                    end
                end
            end
            RX_STATE_PROXY: begin
                if(rx_buffer_avaliable) begin
                    buff_rx[rx_idx]             <= 1;
                    buff_data_rx[rx_idx]        <= noc_rx_msg_payload;
                    buff_rx_msg_type[rx_idx]    <= noc_rx_msg_msg_type[2:0];
                    task_rx_sm_reset();
                end
                else begin
                    rx_state <= RX_STATE_PROXY;
                end
            end
            RX_STATE_AGENT: begin
                if(rx_buffer_avaliable) begin
                    buff_rx[rx_idx]             <= 1;
                    buff_data_rx[rx_idx]        <= noc_rx_msg_payload;
                    buff_rx_msg_type[rx_idx]    <= noc_rx_msg_msg_type[2:0];

                    if(noc_rx_msg_msg_type == MSG_TYPE_CALL) begin
                        buff_agent_phy_X[rx_idx]        <= noc_rx_X_orig;
                        buff_agent_phy_Y[rx_idx]        <= noc_rx_Y_orig;
                        buff_agent_phy_LOCAL[rx_idx]    <= noc_rx_local_orig;
                    end

                    task_rx_sm_reset();
                end
                else begin
                    rx_state <= RX_STATE_AGENT;
                end
            end
            default: begin
                task_rx_sm_reset();
            end
            endcase
        end
    end

    // Agent notification process: Iterates through the agent buffers and checks
    // which ones have a pending msg. Updates the ctrl status registers and
    // generates interrupts.
    wire                        agent_ntf_search_found;
    wire[BUFFER_SIZE-1:0]       agent_ntf_search_in = buff_rx & ~buff_alloc_free & buff_alloc_to_proxy_or_agent;
    wire[BUFFER_SIZE_LOG2-1:0]  agent_ntf_search_result;

    parallel_bit_search #(
        .ADDR_WIDTH(BUFFER_SIZE_LOG2)
    )
    search_agent_ntf(
        .cam_data_in(agent_ntf_search_in),
        .cam_hit_out(agent_ntf_search_found),
        .cam_addr_out(agent_ntf_search_result)
    );

    always @(posedge clk_i)
    begin: process_agent_notification
        if(rst_i) begin
            status_agent_int    <= 0;
            status_agent_buffer <= 0;
            noc_int_o           <= 1'b0;
        end
        else begin
            status_agent_int    <= agent_ntf_search_found;
            status_agent_buffer <= agent_ntf_search_found
                ? {{(WB_DATA_WIDTH-BUFFER_SIZE_LOG2){1'b0}},agent_ntf_search_result}
                : status_agent_buffer;
            noc_int_o           <= agent_ntf_search_found & ~status_agent_int;
        end
    end
endmodule
