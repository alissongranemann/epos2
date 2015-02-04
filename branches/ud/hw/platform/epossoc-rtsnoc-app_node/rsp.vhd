library ieee;
use ieee.std_logic_1164.all;
use ieee.math_real.log2;
use ieee.math_real.ceil;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity rsp is
    generic (
        CLK_FREQ        : integer := 50_000_000
    );
    port (
        clk_i           : in    std_logic;
        reset_i         : in    std_logic;
        uart_tx_o       : out   std_logic;
        uart_rx_i       : in    std_logic;
        uart_baud_o     : out   std_logic;
        gpio_i          : in    std_logic_vector(31 downto 0);
        gpio_o          : out   std_logic_vector(31 downto 0);
        ext_int_i       : in    std_logic_vector(7 downto 0);

        s_axi_awaddr    : out   std_logic_vector(31 downto 0);
        s_axi_awvalid   : out   std_logic;
        s_axi_awready   : in    std_logic;
        s_axi_wdata     : out   std_logic_vector(31 downto 0);
        s_axi_wstrb     : out   std_logic_vector(3 downto 0);
        s_axi_wvalid    : out   std_logic;
        s_axi_wready    : in    std_logic;
        s_axi_bresp     : in    std_logic_vector(1 downto 0);
        s_axi_bvalid    : in    std_logic;
        s_axi_bready    : out   std_logic;
        s_axi_araddr    : out   std_logic_vector(31 downto 0);
        s_axi_arvalid   : out   std_logic;
        s_axi_arready   : in    std_logic;
        s_axi_rdata     : in    std_logic_vector(31 downto 0);
        s_axi_rresp     : in    std_logic_vector(1 downto 0);
        s_axi_rvalid    : in    std_logic;
        s_axi_rready    : out   std_logic;
        s_axi_awprot    : out   std_logic_vector(2 downto 0);
        s_axi_arprot    : out   std_logic_vector(2 downto 0)
    );
end rsp;

architecture rtl of rsp is
    -- RTSNoC constants
    -- Dimension of the network and router address.
    constant NET_SIZE_X                 : integer := 1;
    constant NET_SIZE_Y                 : integer := 1;
    constant NET_SIZE_X_LOG2            : integer := 1; -- it should be "integer(ceil(log2(real(NET_SIZE_X))))" when NET_SIZE_X >= 2
    constant NET_SIZE_Y_LOG2            : integer := 1; -- it should be "integer(ceil(log2(real(NET_SIZE_Y))))" when NET_SIZE_Y >= 2
    constant ROUTER_X                   : integer := 0;
    constant ROUTER_Y                   : integer := 0;
    constant NET_DATA_WIDTH             : integer := 56;
    constant ROUTER_N_PORTS             : integer := 8;
    constant NET_BUS_SIZE               : integer := (NET_DATA_WIDTH+(2*NET_SIZE_X_LOG2)+(2*NET_SIZE_Y_LOG2)+6);
    constant RMI_MSG_SIZE               : integer := 80;
    constant IID_SIZE                   : integer := 8;

    -- Local addresses (clockwise: NN, NE, EE, SE, SS, SW, WW, NW)
    type router_addr_type is array (0 to ROUTER_N_PORTS-1) of std_logic_vector(2 downto 0);

    constant ROUTER_ADDRS               : router_addr_type := ("000", "001", "010", "011", "100", "101", "110", "111");
    constant ROUTER_NN                  : integer := 0;
    constant ROUTER_NE                  : integer := 1;
    constant ROUTER_EE                  : integer := 2;
    constant ROUTER_SE                  : integer := 3;
    constant ROUTER_SS                  : integer := 4;
    constant ROUTER_SW                  : integer := 5;
    constant ROUTER_WW                  : integer := 6;
    constant ROUTER_NW                  : integer := 7;

    -- NoC node addressess
    constant NOC_SW_NODE_ADDR_GENERIC   : integer := ROUTER_NN;
    constant NOC_SW_NODE_ADDR_MANAGER   : integer := ROUTER_NE;

    constant NOC_FREE_NODE_0            : integer := ROUTER_NW;
    constant NOC_FREE_NODE_1            : integer := ROUTER_SS;
    constant NOC_FREE_NODE_2            : integer := ROUTER_WW;
    constant NOC_FREE_NODE_3            : integer := ROUTER_SE;
    constant NOC_FREE_NODE_4            : integer := ROUTER_SW;
    constant NOC_FREE_NODE_5            : integer := ROUTER_EE;

    constant NODE_RSP_CONTROLLER_ADDR   : integer := NOC_FREE_NODE_1;
    constant NODE_RSP_DTMF_ADDR         : integer := NOC_FREE_NODE_3;
    constant NODE_RSP_ADPCM_ADDR        : integer := NOC_FREE_NODE_4;
    constant NODE_RSP_AES_ADDR          : integer := NOC_FREE_NODE_5;
    constant NODE_ADD_ADDR              : integer := NOC_FREE_NODE_0;

    -- IIDs
    constant RECFG_NODE_NIID            : integer := 1;

    -- Declarations
    component rtsnoc_axi4lite_reset is
        port (
            axi_rst_i   : in    std_logic;
            noc_reset_o : out   std_logic
        );
    end component;

    component ram_amba_1024k is
        port (
            s_aclk        : in  std_logic;
            s_aresetn     : in  std_logic;
            s_axi_awaddr  : in  std_logic_vector(31 downto 0);
            s_axi_awvalid : in  std_logic;
            s_axi_awready : out std_logic;
            s_axi_wdata   : in  std_logic_vector(31 downto 0);
            s_axi_wstrb   : in  std_logic_vector(3 downto 0);
            s_axi_wvalid  : in  std_logic;
            s_axi_wready  : out std_logic;
            s_axi_bresp   : out std_logic_vector(1 downto 0);
            s_axi_bvalid  : out std_logic;
            s_axi_bready  : in  std_logic;
            s_axi_araddr  : in  std_logic_vector(31 downto 0);
            s_axi_arvalid : in  std_logic;
            s_axi_arready : out std_logic;
            s_axi_rdata   : out std_logic_vector(31 downto 0);
            s_axi_rresp   : out std_logic_vector(1 downto 0);
            s_axi_rvalid  : out std_logic;
            s_axi_rready  : in  std_logic
        );
    end component;

    component recfg_node is
        generic (
            X               : integer;
            Y               : integer;
            LOCAL_ADDR      : integer;
            SIZE_X          : integer;
            SIZE_Y          : integer;
            SIZE_DATA       : integer;
            RMI_MSG_SIZE    : integer;
            IID_SIZE        : integer
        );
        port (
            clk_i           : in  std_logic;
            rst_i           : in  std_logic;
            din_o           : out std_logic_vector(NET_BUS_SIZE-1 downto 0);
            wr_o            : out std_logic;
            rd_o            : out std_logic;
            dout_i          : in  std_logic_vector(NET_BUS_SIZE-1 downto 0);
            wait_i          : in  std_logic;
            nd_i            : in  std_logic;
            iid_i           : in  std_logic_vector(IID_SIZE-1 downto 0);
            db_rx_vz        : out std_logic;
            db_rx_lz        : out std_logic;
            db_tx_vz        : out std_logic;
            db_tx_lz        : out std_logic
        );
    end component;

    attribute black_box                 : string;
    attribute black_box of recfg_node   : component is "yes";

    -- RTSNoC signals
    signal sig_noc_reset        : std_logic;

    -- Router ports (clockwise: NN, NE, EE, SE, SS, SW, WW, NW)
    type noc_array_of_stdlogic is array(0 to ROUTER_N_PORTS-1) of std_logic;
    type noc_array_of_stdvec38 is array(0 to ROUTER_N_PORTS-1) of std_logic_vector(NET_BUS_SIZE-1 downto 0);

    signal sig_noc_din          : noc_array_of_stdvec38;
    signal sig_noc_dout         : noc_array_of_stdvec38;
    signal sig_noc_wr           : noc_array_of_stdlogic;
    signal sig_noc_rd           : noc_array_of_stdlogic;
    signal sig_noc_wait         : noc_array_of_stdlogic;
    signal sig_noc_nd           : noc_array_of_stdlogic;

    -- DD3 AXI4 signals
    signal sig_ddr3_awvalid     : std_logic;
    signal sig_ddr3_awready     : std_logic;
    signal sig_ddr3_awaddr      : std_logic_vector(31 downto 0);
    signal sig_ddr3_awprot      : std_logic_vector(2 downto 0);

    signal sig_ddr3_wvalid      : std_logic;
    signal sig_ddr3_wready      : std_logic;
    signal sig_ddr3_wdata       : std_logic_vector(31 downto 0);
    signal sig_ddr3_wstrb       : std_logic_vector(3 downto 0);

    signal sig_ddr3_bvalid      : std_logic;
    signal sig_ddr3_bready      : std_logic;
    signal sig_ddr3_bresp       : std_logic_vector(1 downto 0);

    signal sig_ddr3_arvalid     : std_logic;
    signal sig_ddr3_arready     : std_logic;
    signal sig_ddr3_araddr      : std_logic_vector(31 downto 0);
    signal sig_ddr3_arprot      : std_logic_vector(2 downto 0);

    signal sig_ddr3_rvalid      : std_logic;
    signal sig_ddr3_rready      : std_logic;
    signal sig_ddr3_rdata       : std_logic_vector(31 downto 0);
    signal sig_ddr3_rresp       : std_logic_vector(1 downto 0);

    -- HLS nodes signals
    signal recfg_node_iid       : std_logic_vector((IID_SIZE*RECFG_NODE_NIID)-1 downto 0);

    -- Debug signals
    signal sig_db_noc_irq       : std_logic;
    signal sig_db_plasma_int    : std_logic;
    signal sig_db_rx_lz         : std_logic;
    signal sig_db_rx_vz         : std_logic;
    signal sig_db_tx_lz         : std_logic;
    signal sig_db_tx_vz         : std_logic;
    signal sib_db_cpu_int_en    : std_logic;
    signal sig_tsc              : std_logic_vector(31 downto 0);
    signal sig_db_pic_irqs      : std_logic_vector(2 downto 0);
    signal sig_db_pic_pol       : std_logic_vector(2 downto 0);
    signal sig_db_pic_edgen     : std_logic_vector(2 downto 0);
    signal sig_db_pic_pend      : std_logic_vector(2 downto 0);
    signal sig_db_pic_mask      : std_logic_vector(2 downto 0);
    signal sig_db_pic_event     : std_logic_vector(2 downto 0);

    signal recfg_noc_din_s      : std_logic_vector(NET_BUS_SIZE-1 downto 0);
    signal recfg_noc_wr_s       : std_logic;
    signal recfg_noc_rd_s       : std_logic;

    signal gpio_o_s             : std_logic_vector(31 downto 0);
begin
    rtsnoc_router: entity work.router
        generic map (
            p_X         => ROUTER_X,
            p_Y         => ROUTER_Y,
            p_DATA      => NET_DATA_WIDTH,
            p_SIZE_X    => NET_SIZE_X_LOG2,
            p_SIZE_Y    => NET_SIZE_Y_LOG2
        )
        port map (
            o_TESTE     => open,
            i_CLK       => clk_i,
            i_RST       => sig_noc_reset,
            -- NORTH
            i_DIN_NN    => sig_noc_din(ROUTER_NN),
            o_DOUT_NN   => sig_noc_dout(ROUTER_NN),
            i_WR_NN     => sig_noc_wr(ROUTER_NN),
            i_RD_NN     => sig_noc_rd(ROUTER_NN),
            o_WAIT_NN   => sig_noc_wait(ROUTER_NN),
            o_ND_NN     => sig_noc_nd(ROUTER_NN),
            -- NORTHEAST
            i_DIN_NE    => sig_noc_din(ROUTER_NE),
            o_DOUT_NE   => sig_noc_dout(ROUTER_NE),
            i_WR_NE     => sig_noc_wr(ROUTER_NE),
            i_RD_NE     => sig_noc_rd(ROUTER_NE),
            o_WAIT_NE   => sig_noc_wait(ROUTER_NE),
            o_ND_NE     => sig_noc_nd(ROUTER_NE),
            -- EAST
            i_DIN_EE    => sig_noc_din(ROUTER_EE),
            o_DOUT_EE   => sig_noc_dout(ROUTER_EE),
            i_WR_EE     => sig_noc_wr(ROUTER_EE),
            i_RD_EE     => sig_noc_rd(ROUTER_EE),
            o_WAIT_EE   => sig_noc_wait(ROUTER_EE),
            o_ND_EE     => sig_noc_nd(ROUTER_EE),
            -- SOUTHEAST
            i_DIN_SE    => sig_noc_din(ROUTER_SE),
            o_DOUT_SE   => sig_noc_dout(ROUTER_SE),
            i_WR_SE     => sig_noc_wr(ROUTER_SE),
            i_RD_SE     => sig_noc_rd(ROUTER_SE),
            o_WAIT_SE   => sig_noc_wait(ROUTER_SE),
            o_ND_SE     => sig_noc_nd(ROUTER_SE),
            -- SOUTH
            i_DIN_SS    => sig_noc_din(ROUTER_SS),
            o_DOUT_SS   => sig_noc_dout(ROUTER_SS),
            i_WR_SS     => sig_noc_wr(ROUTER_SS),
            i_RD_SS     => sig_noc_rd(ROUTER_SS),
            o_WAIT_SS   => sig_noc_wait(ROUTER_SS),
            o_ND_SS     => sig_noc_nd(ROUTER_SS),
            -- SOUTHWEST
            i_DIN_SW    => sig_noc_din(ROUTER_SW),
            o_DOUT_SW   => sig_noc_dout(ROUTER_SW),
            i_WR_SW     => sig_noc_wr(ROUTER_SW),
            i_RD_SW     => sig_noc_rd(ROUTER_SW),
            o_WAIT_SW   => sig_noc_wait(ROUTER_SW),
            o_ND_SW     => sig_noc_nd(ROUTER_SW),
            -- WEST
            i_DIN_WW    => sig_noc_din(ROUTER_WW),
            o_DOUT_WW   => sig_noc_dout(ROUTER_WW),
            i_WR_WW     => sig_noc_wr(ROUTER_WW),
            i_RD_WW     => sig_noc_rd(ROUTER_WW),
            o_WAIT_WW   => sig_noc_wait(ROUTER_WW),
            o_ND_WW     => sig_noc_nd(ROUTER_WW),
            -- NORTHWEST
            i_DIN_NW    => sig_noc_din(ROUTER_NW),
            o_DOUT_NW   => sig_noc_dout(ROUTER_NW),
            i_WR_NW     => sig_noc_wr(ROUTER_NW),
            i_RD_NW     => sig_noc_rd(ROUTER_NW),
            o_WAIT_NW   => sig_noc_wait(ROUTER_NW),
            o_ND_NW     => sig_noc_nd(ROUTER_NW)
        );

    rtsnoc_reset : rtsnoc_axi4lite_reset
        port map (
            axi_rst_i   => reset_i,
            noc_reset_o => sig_noc_reset
        );

    -- NODE - Proc node + External RAM (DDR3 controller)
    proc_node : entity work.cpu_io_node
        generic map (
            CLK_FREQ                    => CLK_FREQ,
            -- RTSNoC generics
            NET_SIZE_X                  => NET_SIZE_X,
            NET_SIZE_Y                  => NET_SIZE_Y,
            NET_SIZE_X_LOG2             => NET_SIZE_X_LOG2,
            NET_SIZE_Y_LOG2             => NET_SIZE_Y_LOG2,
            NET_DATA_WIDTH              => NET_DATA_WIDTH,
            NET_BUS_SIZE                => NET_BUS_SIZE,
            ROUTER_X_ADDR               => ROUTER_X,
            ROUTER_Y_ADDR               => ROUTER_Y,
            ROUTER_LOCAL_ADDR_GENERIC   => ROUTER_ADDRS(NOC_SW_NODE_ADDR_GENERIC),
            ROUTER_LOCAL_ADDR_MANAGER   => ROUTER_ADDRS(NOC_SW_NODE_ADDR_MANAGER)
        )
        port map (
            -- System signals
            clk_i                       => clk_i,
            reset_axi_i                 => reset_i,
            reset_noc_i                 => sig_noc_reset,
            -- Peripherals
            uart_tx_o                   => uart_tx_o,
            uart_rx_i                   => uart_rx_i,
            uart_baud_o                 => uart_baud_o,
            gpio_i                      => gpio_i,
            gpio_o                      => gpio_o_s,
            ext_int_i                   => ext_int_i,
            -- NoC interface
            noc_din_o                   => sig_noc_din(NOC_SW_NODE_ADDR_GENERIC),
            noc_dout_i                  => sig_noc_dout(NOC_SW_NODE_ADDR_GENERIC),
            noc_wr_o                    => sig_noc_wr(NOC_SW_NODE_ADDR_GENERIC),
            noc_rd_o                    => sig_noc_rd(NOC_SW_NODE_ADDR_GENERIC),
            noc_wait_i                  => sig_noc_wait(NOC_SW_NODE_ADDR_GENERIC),
            noc_nd_i                    => sig_noc_nd(NOC_SW_NODE_ADDR_GENERIC),
            -- NoC interface, CPM manager
            mgn_din_o                   => sig_noc_din(NOC_SW_NODE_ADDR_MANAGER),
            mgn_dout_i                  => sig_noc_dout(NOC_SW_NODE_ADDR_MANAGER),
            mgn_wr_o                    => sig_noc_wr(NOC_SW_NODE_ADDR_MANAGER),
            mgn_rd_o                    => sig_noc_rd(NOC_SW_NODE_ADDR_MANAGER),
            mgn_wait_i                  => sig_noc_wait(NOC_SW_NODE_ADDR_MANAGER),
            mgn_nd_i                    => sig_noc_nd(NOC_SW_NODE_ADDR_MANAGER),
            -- EXT RAM
            ext_ram_awaddr_o            => sig_ddr3_awaddr,
            ext_ram_awvalid_o           => sig_ddr3_awvalid,
            ext_ram_awready_i           => sig_ddr3_awready,
            ext_ram_wdata_o             => sig_ddr3_wdata,
            ext_ram_wstrb_o             => sig_ddr3_wstrb,
            ext_ram_wvalid_o            => sig_ddr3_wvalid,
            ext_ram_wready_i            => sig_ddr3_wready,
            ext_ram_bresp_i             => sig_ddr3_bresp,
            ext_ram_bvalid_i            => sig_ddr3_bvalid,
            ext_ram_bready_o            => sig_ddr3_bready,
            ext_ram_araddr_o            => sig_ddr3_araddr,
            ext_ram_arvalid_o           => sig_ddr3_arvalid,
            ext_ram_arready_i           => sig_ddr3_arready,
            ext_ram_rdata_i             => sig_ddr3_rdata,
            ext_ram_rresp_i             => sig_ddr3_rresp,
            ext_ram_rvalid_i            => sig_ddr3_rvalid,
            ext_ram_rready_o            => sig_ddr3_rready,

            db_noc_proxy_irq_o          => sig_db_noc_irq,
            db_pic_int_o                => sig_db_plasma_int,

            db_cpu_int_en               => sib_db_cpu_int_en,

            db_edgen_o                  => sig_db_pic_edgen,
            db_event_o                  => sig_db_pic_event,
            db_irqs_o                   => sig_db_pic_irqs,
            db_mask_o                   => sig_db_pic_mask,
            db_pend_o                   => sig_db_pic_pend,
            db_pol_o                    => sig_db_pic_pol,

            s_axi_awaddr                => s_axi_awaddr,
            s_axi_awvalid               => s_axi_awvalid,
            s_axi_awready               => s_axi_awready,
            s_axi_wdata                 => s_axi_wdata,
            s_axi_wstrb                 => s_axi_wstrb,
            s_axi_wvalid                => s_axi_wvalid,
            s_axi_wready                => s_axi_wready,
            s_axi_bresp                 => s_axi_bresp,
            s_axi_bvalid                => s_axi_bvalid,
            s_axi_bready                => s_axi_bready,
            s_axi_araddr                => s_axi_araddr,
            s_axi_arvalid               => s_axi_arvalid,
            s_axi_arready               => s_axi_arready,
            s_axi_rdata                 => s_axi_rdata,
            s_axi_rresp                 => s_axi_rresp,
            s_axi_rvalid                => s_axi_rvalid,
            s_axi_rready                => s_axi_rready,
            s_axi_awprot                => s_axi_awprot,
            s_axi_arprot                => s_axi_arprot
        );

    ddr_ctrl: ram_amba_1024k
        port map (
            s_aclk        => clk_i,
            s_aresetn     => reset_i,

            s_axi_awvalid => sig_ddr3_awvalid,
            s_axi_awready => sig_ddr3_awready,
            s_axi_awaddr  => sig_ddr3_awaddr,

            s_axi_wvalid  => sig_ddr3_wvalid,
            s_axi_wready  => sig_ddr3_wready,
            s_axi_wdata   => sig_ddr3_wdata,
            s_axi_wstrb   => sig_ddr3_wstrb,

            s_axi_bvalid  => sig_ddr3_bvalid,
            s_axi_bready  => sig_ddr3_bready,
            s_axi_bresp   => sig_ddr3_bresp,

            s_axi_arvalid => sig_ddr3_arvalid,
            s_axi_arready => sig_ddr3_arready,
            s_axi_araddr  => sig_ddr3_araddr,

            s_axi_rvalid  => sig_ddr3_rvalid,
            s_axi_rready  => sig_ddr3_rready,
            s_axi_rdata   => sig_ddr3_rdata,
            s_axi_rresp   => sig_ddr3_rresp
        );

    u_recfg_node : recfg_node
        generic map (
            X               => ROUTER_X,
            Y               => ROUTER_Y,
            LOCAL_ADDR      => ROUTER_NW,
            SIZE_X          => NET_SIZE_X_LOG2,
            SIZE_Y          => NET_SIZE_Y_LOG2,
            SIZE_DATA       => NET_DATA_WIDTH,
            RMI_MSG_SIZE    => RMI_MSG_SIZE,
            IID_SIZE        => IID_SIZE
        )
        port map (
            clk_i           => clk_i,
            rst_i           => sig_noc_reset,
            din_o           => recfg_noc_din_s,
            dout_i          => sig_noc_dout(ROUTER_NW),
            wr_o            => recfg_noc_wr_s,
            rd_o            => recfg_noc_rd_s,
            wait_i          => sig_noc_wait(ROUTER_NW),
            nd_i            => sig_noc_nd(ROUTER_NW),
            iid_i           => recfg_node_iid,
            db_rx_vz        => open,
            db_rx_lz        => open,
            db_tx_vz        => open,
            db_tx_lz        => open
        );

    -- recfg_node decoupling
    sig_noc_din(ROUTER_NW)  <= recfg_noc_din_s when (gpio_o_s(31) = '1') else (others => '0');
    sig_noc_wr(ROUTER_NW)   <= recfg_noc_wr_s when (gpio_o_s(31) = '1') else '0';
    sig_noc_rd(ROUTER_NW)   <= recfg_noc_rd_s when (gpio_o_s(31) = '1') else '0';

    recfg_node_iid          <= (others => '0');

    gpio_o                  <= gpio_o_s;
end rtl;
