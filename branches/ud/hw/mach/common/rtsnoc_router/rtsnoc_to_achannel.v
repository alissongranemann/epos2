module rtsnoc_to_achannel (
    clk_i, rst_i,

    din_o, wr_o, rd_o, dout_i, wait_i, nd_i,

    x, y, local_addr,

    rx_ch_z_o, rx_ch_vz_o, rx_ch_lz_i, tx_ch_z_i, tx_ch_vz_o, tx_ch_lz_i
    );

    parameter SIZE_X = 1; //Log2
    parameter SIZE_Y = 1; //Log2
    parameter SIZE_DATA = 56;
    parameter RMI_MSG_SIZE = 80;

    localparam BUS_SIZE = SIZE_DATA+(2*SIZE_X)+(2*SIZE_Y)+6;

    localparam SIZE_X_DIFF = 8 - SIZE_X;
    localparam SIZE_Y_DIFF = 8 - SIZE_Y;
    localparam SIZE_LOCAL_DIFF = 8 - 3;

    //Ports
    input clk_i;
    input rst_i;

    //RTSNoC
    output [BUS_SIZE-1:0] din_o;
    output reg wr_o;
    output reg rd_o;
    input [BUS_SIZE-1:0] dout_i;
    input wait_i;
    input nd_i;

    input [SIZE_X-1:0] x;
    input [SIZE_Y-1:0] y;
    input [2:0] local_addr;

    //Catapult achannel
    output reg [RMI_MSG_SIZE-1:0] rx_ch_z_o;
    output reg rx_ch_vz_o;
    input rx_ch_lz_i;
    input [RMI_MSG_SIZE-1:0] tx_ch_z_i;
    output reg tx_ch_vz_o;
    input tx_ch_lz_i;

    // /////////////////////////////////////////

    wire [SIZE_DATA-1:0] rx_data;
    wire [2:0] rx_local_dst;
    wire [SIZE_Y-1:0] rx_Y_dst;
    wire [SIZE_X-1:0] rx_X_dst;
    wire [2:0] rx_local_orig;
    wire [SIZE_Y-1:0] rx_Y_orig;
    wire [SIZE_X-1:0] rx_X_orig;
    assign {rx_X_orig,
            rx_Y_orig,
            rx_local_orig,
            rx_X_dst,
            rx_Y_dst,
            rx_local_dst,
            rx_data} = dout_i[BUS_SIZE-1:0];

    reg [SIZE_DATA-1:0] tx_data;
    reg [2:0] tx_local_dst;
    reg [SIZE_Y-1:0] tx_Y_dst;
    reg [SIZE_X-1:0] tx_X_dst;
    reg [2:0] tx_local_orig;
    reg [SIZE_Y-1:0] tx_Y_orig;
    reg [SIZE_X-1:0] tx_X_orig;
    assign din_o[BUS_SIZE-1:0] = {tx_X_orig,
                                  tx_Y_orig,
                                  tx_local_orig,
                                  tx_X_dst,
                                  tx_Y_dst,
                                  tx_local_dst,
                                  tx_data};

    reg [RMI_MSG_SIZE-1:0] rx_data_ac;
    wire [7:0] phyaddr_X;
    wire [7:0] phyaddr_Y;
    wire [7:0] phyaddr_local;
    assign {phyaddr_local,
            phyaddr_Y,
            phyaddr_X} = rx_data_ac[RMI_MSG_SIZE-1:SIZE_DATA];



    reg [RMI_MSG_SIZE-1:0] tx_data_ac;


    localparam STATE_AC = 1'd0;
    localparam STATE_NOC = 1'd1;
    reg tx_state;
    reg rx_state;

    //TX SM
    always @(posedge clk_i)
    begin
        if(rst_i) begin
            tx_ch_vz_o <= 0;
            wr_o <= 0;

            //din_o
            tx_data <= 0;
            tx_local_dst <= 0;
            tx_Y_dst <= 0;
            tx_X_dst <= 0;
            tx_local_orig <= 0;
            tx_Y_orig <= 0;
            tx_X_orig <= 0;

            rx_data_ac <= 0;
            tx_state <= STATE_AC;

        end
        else begin
            case (tx_state)
            STATE_AC: begin
                wr_o <= 0;
                if(!tx_ch_lz_i)begin
                    tx_state <= STATE_AC;
                end
                else begin
                    tx_ch_vz_o <= 1;
                    rx_data_ac <= tx_ch_z_i;
                    tx_state <= STATE_NOC;
                end
            end
            STATE_NOC: begin
                tx_ch_vz_o <= 0;
                if(wait_i) begin
                    tx_state <= STATE_NOC;
                end
                else begin
                    tx_data <= rx_data_ac[SIZE_DATA-1:0];
                    tx_local_orig <= local_addr;
                    tx_Y_orig <= y;
                    tx_X_orig <= x;
                    tx_local_dst <= phyaddr_local[2:0];
                    tx_Y_dst <= phyaddr_X[SIZE_Y-1:0];
                    tx_X_dst <= phyaddr_Y[SIZE_X-1:0];

                    wr_o <= 1;
                    tx_state <= STATE_AC;
                end
            end
            default: begin
            end
            endcase
        end
    end

    //RX SM
    always @(posedge clk_i)
    begin
        if(rst_i) begin
            rd_o <= 0;

            rx_ch_z_o <= 0;
            rx_ch_vz_o <= 0;

            tx_data_ac <= 0;

            rx_state <= STATE_NOC;

        end
        else begin
            case (rx_state)
            STATE_NOC: begin
                rx_ch_vz_o <= 0;
                if(!nd_i) begin
                    rx_state <= STATE_NOC;
                end
                else begin
                    tx_data_ac <= {{SIZE_LOCAL_DIFF{1'b0}},rx_local_orig,
                                   {SIZE_Y_DIFF{1'b0}},rx_Y_orig,
                                   {SIZE_X_DIFF{1'b0}},rx_X_orig,
                                   rx_data};
                    rd_o <= 1;
                    rx_state <= STATE_AC;
                end
            end
            STATE_AC: begin
                rd_o <= 0;
                if(!rx_ch_lz_i) begin
                    rx_state <= STATE_AC;
                end
                else begin
                    rx_ch_z_o <= tx_data_ac;
                    rx_ch_vz_o <= 1;
                    rx_state <= STATE_NOC;
                end
            end
            default: begin
            end
            endcase
        end
    end
endmodule
