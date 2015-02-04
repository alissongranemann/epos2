library ieee;
use ieee.std_logic_1164.all;
use ieee.math_real.log2;
use ieee.math_real.ceil;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- Memory Map (X:Y:LOCAL):
--  Proc node   0:0:0x000
--  Echo P0     0:0:0x110
--  Echo P1     0:0:0x100

entity Unified_Tests_Comp_Manager_tb is

end Unified_Tests_Comp_Manager_tb;

architecture RTL of Unified_Tests_Comp_Manager_tb is
    
    
    constant CLK_FREQ : integer := 50_000_000;
    constant PERIOD : time := 20 ns;
    constant DUTY_CYCLE : real := 0.5;
    constant OFFSET : time := 200 ns;

    
    -- RTSNoC constants
        -- Dimension of the network and router address.
    constant NET_SIZE_X         : integer := 1;
    constant NET_SIZE_Y         : integer := 1;
    constant NET_SIZE_X_LOG2    : integer := 1; -- it should be "integer(ceil(log2(real(NET_SIZE_X))))" when NET_SIZE_X >= 2
    constant NET_SIZE_Y_LOG2    : integer := 1; -- it should be "integer(ceil(log2(real(NET_SIZE_Y))))" when NET_SIZE_Y >= 2
    constant ROUTER_X           : std_logic_vector(NET_SIZE_X_LOG2-1 downto 0) := "0";
    constant ROUTER_Y           : std_logic_vector(NET_SIZE_Y_LOG2-1 downto 0) := "0";
    constant NET_DATA_WIDTH     : integer := 56;
    constant ROUTER_N_PORTS     : integer := 8;
    constant NET_BUS_SIZE       : integer := NET_DATA_WIDTH+(2*NET_SIZE_X_LOG2)+(2*NET_SIZE_Y_LOG2)+6;
    constant RMI_MSG_SIZE     : integer := 80;
    constant IID_SIZE     : integer := 8;
        -- Local addresses (clockwise: nn, ne, ee, se, ss, sw, ww, nw)
    type router_addr_type is array (0 to ROUTER_N_PORTS-1) of std_logic_vector(2 downto 0);
    constant ROUTER_ADDRS   : router_addr_type := 
            ("000", "001", "010", "011", "100", "101", "110", "111");
    constant ROUTER_NN  : integer := 0;
    constant ROUTER_NE  : integer := 1;
    constant ROUTER_EE  : integer := 2;
    constant ROUTER_SE  : integer := 3;
    constant ROUTER_SS  : integer := 4;
    constant ROUTER_SW  : integer := 5;
    constant ROUTER_WW  : integer := 6;
    constant ROUTER_NW  : integer := 7;
        -- NoC node addressess
    constant NOC_SW_NODE_ADDR       : integer := ROUTER_NE;
    constant NODE_DUMMY_CALLEE_NODE_ADDR    : integer := ROUTER_SS;
    constant NODE_DUMMY_CALLER_NODE_ADDR    : integer := ROUTER_EE;
    
    -- IIDs
    constant NODE_DUMMY_CALLEE_NIID    : integer := 1;
    constant NODE_DUMMY_CALLER_NIID    : integer := NODE_DUMMY_CALLEE_NIID+1;
    
    -- 
    -- Declarations
    --

    COMPONENT ROUTER
        GENERIC (
            p_X         : integer := conv_integer(ROUTER_X);
            p_Y         : integer := conv_integer(ROUTER_Y);
            p_DATA      : integer := NET_DATA_WIDTH;                    
            p_SIZE_X    : integer := NET_SIZE_X_LOG2;
            p_SIZE_Y    : integer := NET_SIZE_Y_LOG2);  
        PORT(
            o_TESTE     : out std_logic_vector(NET_BUS_SIZE-1 downto 0);
            i_CLK       : IN std_logic;
            i_RST       : IN std_logic;
            i_DIN_NN    : IN std_logic_vector(NET_BUS_SIZE-1 downto 0);
            i_WR_NN     : IN std_logic;
            i_RD_NN     : IN std_logic;
            i_DIN_NE    : IN std_logic_vector(NET_BUS_SIZE-1 downto 0);
            i_WR_NE     : IN std_logic;
            i_RD_NE     : IN std_logic;
            i_DIN_EE    : IN std_logic_vector(NET_BUS_SIZE-1 downto 0);
            i_WR_EE     : IN std_logic;
            i_RD_EE     : IN std_logic;
            i_DIN_SE    : IN std_logic_vector(NET_BUS_SIZE-1 downto 0);
            i_WR_SE     : IN std_logic;
            i_RD_SE     : IN std_logic;
            i_DIN_SS    : IN std_logic_vector(NET_BUS_SIZE-1 downto 0);
            i_WR_SS     : IN std_logic;
            i_RD_SS     : IN std_logic;
            i_DIN_SW    : IN std_logic_vector(NET_BUS_SIZE-1 downto 0);
            i_WR_SW     : IN std_logic;
            i_RD_SW     : IN std_logic;
            i_DIN_WW    : IN std_logic_vector(NET_BUS_SIZE-1 downto 0);
            i_WR_WW     : IN std_logic;
            i_RD_WW     : IN std_logic;
            i_DIN_NW    : IN std_logic_vector(NET_BUS_SIZE-1 downto 0);
            i_WR_NW     : IN std_logic;
            i_RD_NW     : IN std_logic;          
            o_DOUT_NN   : OUT std_logic_vector(NET_BUS_SIZE-1 downto 0);
            o_WAIT_NN   : OUT std_logic;
            o_ND_NN     : OUT std_logic;
            o_DOUT_NE   : OUT std_logic_vector(NET_BUS_SIZE-1 downto 0);
            o_WAIT_NE   : OUT std_logic;
            o_ND_NE     : OUT std_logic;
            o_DOUT_EE   : OUT std_logic_vector(NET_BUS_SIZE-1 downto 0);
            o_WAIT_EE   : OUT std_logic;
            o_ND_EE     : OUT std_logic;
            o_DOUT_SE   : OUT std_logic_vector(NET_BUS_SIZE-1 downto 0);
            o_WAIT_SE   : OUT std_logic;
            o_ND_SE     : OUT std_logic;
            o_DOUT_SS   : OUT std_logic_vector(NET_BUS_SIZE-1 downto 0);
            o_WAIT_SS   : OUT std_logic;
            o_ND_SS     : OUT std_logic;
            o_DOUT_SW   : OUT std_logic_vector(NET_BUS_SIZE-1 downto 0);
            o_WAIT_SW   : OUT std_logic;
            o_ND_SW     : OUT std_logic;
            o_DOUT_WW   : OUT std_logic_vector(NET_BUS_SIZE-1 downto 0);
            o_WAIT_WW   : OUT std_logic;
            o_ND_WW     : OUT std_logic;
            o_DOUT_NW   : OUT std_logic_vector(NET_BUS_SIZE-1 downto 0);
            o_WAIT_NW   : OUT std_logic;
            o_ND_NW     : OUT std_logic
        );
    END COMPONENT;
    
    component rtsnoc_axi4lite_reset is
        port (
            axi_rst_i   : in std_logic;
            noc_reset_o : out std_logic    
        );
    end component;
    
    component cpu_io_node_comp_manager_tb is
        generic(
            CLK_FREQ : integer := 50_000_000;
            -- RTSNoC generics
            NET_SIZE_X         : integer := 1;
            NET_SIZE_Y         : integer := 1;
            NET_SIZE_X_LOG2    : integer := 1; -- it should be "integer(ceil(log2(real(NET_SIZE_X))))" when NET_SIZE_X >= 2
            NET_SIZE_Y_LOG2    : integer := 1; -- it should be "integer(ceil(log2(real(NET_SIZE_Y))))" when NET_SIZE_Y >= 2
            NET_DATA_WIDTH     : integer := NET_DATA_WIDTH;
            NET_BUS_SIZE       : integer := NET_BUS_SIZE;
            ROUTER_X_ADDR      : integer := 0;
            ROUTER_Y_ADDR      : integer := 0;
            ROUTER_LOCAL_ADDR  : std_logic_vector(2 downto 0) := ROUTER_ADDRS(NOC_SW_NODE_ADDR));
        port(clk_i       : in std_logic;
            reset_axi_i     : in std_logic;
            reset_noc_i     : in std_logic;
    
            uart_tx_o   : out std_logic;
            uart_rx_i   : in std_logic;
            uart_baud_o : out std_logic;
        
            gpio_i      : in  std_logic_vector(31 downto 0);
            gpio_o      : out  std_logic_vector(31 downto 0);
            ext_int_i   : in std_logic_vector(7 downto 0);
        
            noc_din_o   : out std_logic_vector(NET_BUS_SIZE-1 downto 0);
            noc_wr_o    : out std_logic;
            noc_rd_o    : out std_logic;
            noc_dout_i  : in std_logic_vector(NET_BUS_SIZE-1 downto 0);
            noc_wait_i  : in std_logic;
            noc_nd_i    : in std_logic;
            
            ext_ram_awaddr_o  : out std_logic_vector(31 DOWNTO 0);
            ext_ram_awvalid_o : out std_logic;
            ext_ram_awready_i : in std_logic;
            ext_ram_wdata_o   : out std_logic_vector(31 DOWNTO 0);
            ext_ram_wstrb_o   : out std_logic_vector(3 DOWNTO 0);
            ext_ram_wvalid_o  : out std_logic;
            ext_ram_wready_i  : in std_logic;
            ext_ram_bresp_i   : in std_logic_vector(1 DOWNTO 0);
            ext_ram_bvalid_i  : in std_logic;
            ext_ram_bready_o  : out std_logic;
            ext_ram_araddr_o  : out std_logic_vector(31 DOWNTO 0);
            ext_ram_arvalid_o : out std_logic;
            ext_ram_arready_i : in std_logic;
            ext_ram_rdata_i   : in std_logic_vector(31 DOWNTO 0);
            ext_ram_rresp_i   : in std_logic_vector(1 DOWNTO 0);
            ext_ram_rvalid_i  : in std_logic;
            ext_ram_rready_o  : out std_logic;
            
            db_noc_proxy_irq_o : out std_logic;
            db_pic_int_o : out std_logic);
    end component;
      
    component ram_amba_1024k is
        port(
            s_aclk        : in std_logic;
            s_aresetn     : in std_logic;
            s_axi_awaddr  : in std_logic_vector(31 downto 0);
            s_axi_awvalid : in std_logic;
            s_axi_awready : out std_logic;
            s_axi_wdata   : in std_logic_vector(31 downto 0);
            s_axi_wstrb   : in std_logic_vector(3 downto 0);
            s_axi_wvalid  : in std_logic;
            s_axi_wready  : out std_logic;
            s_axi_bresp   : out std_logic_vector(1 downto 0);
            s_axi_bvalid  : out std_logic;
            s_axi_bready  : in std_logic;
            s_axi_araddr  : in std_logic_vector(31 downto 0);
            s_axi_arvalid : in std_logic;
            s_axi_arready : out std_logic;
            s_axi_rdata   : out std_logic_vector(31 downto 0);
            s_axi_rresp   : out std_logic_vector(1 downto 0);
            s_axi_rvalid  : out std_logic;
            s_axi_rready  : in std_logic);
    end component;
    
    component Dummy_Callee_Node_RTL is
        generic (
            X           : std_logic_vector(NET_SIZE_Y_LOG2-1 downto 0) := "0";
            Y           : std_logic_vector(NET_SIZE_Y_LOG2-1 downto 0) := "0";
            LOCAL_ADDR  : std_logic_vector(2 downto 0) := ROUTER_ADDRS(NODE_DUMMY_CALLEE_NODE_ADDR);
            SIZE_X      : integer := NET_SIZE_X_LOG2;
            SIZE_Y      : integer := NET_SIZE_Y_LOG2;
            SIZE_DATA   : integer := NET_DATA_WIDTH;
            RMI_MSG_SIZE: integer := RMI_MSG_SIZE;
            IID_SIZE    : integer := IID_SIZE);
            
       port(
            clk_i   : in std_logic;
            rst_i   : in std_logic;
            din_o   : out std_logic_vector(NET_BUS_SIZE-1 downto 0);
            dout_i  : in std_logic_vector(NET_BUS_SIZE-1 downto 0);
            wr_o    : out std_logic;
            rd_o    : out std_logic;
            wait_i  : in std_logic;
            nd_i    : in std_logic;
            iid_i   : in std_logic_vector((IID_SIZE*NODE_DUMMY_CALLEE_NIID)-1 downto 0);
            
            db_rx_vz : out std_logic;
            db_rx_lz : out std_logic;
            db_tx_vz : out std_logic;
            db_tx_lz : out std_logic);
    end component Dummy_Callee_Node_RTL;
    
    component Dummy_Caller_Node_RTL is
        generic (
            X           : std_logic_vector(NET_SIZE_Y_LOG2-1 downto 0) := "0";
            Y           : std_logic_vector(NET_SIZE_Y_LOG2-1 downto 0) := "0";
            LOCAL_ADDR  : std_logic_vector(2 downto 0) := ROUTER_ADDRS(NODE_DUMMY_CALLER_NODE_ADDR);
            SIZE_X      : integer := NET_SIZE_X_LOG2;
            SIZE_Y      : integer := NET_SIZE_Y_LOG2;
            SIZE_DATA   : integer := NET_DATA_WIDTH;
            RMI_MSG_SIZE: integer := RMI_MSG_SIZE;
            IID_SIZE    : integer := IID_SIZE);
            
       port(
            clk_i   : in std_logic;
            rst_i   : in std_logic;
            din_o   : out std_logic_vector(NET_BUS_SIZE-1 downto 0);
            dout_i  : in std_logic_vector(NET_BUS_SIZE-1 downto 0);
            wr_o    : out std_logic;
            rd_o    : out std_logic;
            wait_i  : in std_logic;
            nd_i    : in std_logic;
            iid_i   : in std_logic_vector((IID_SIZE*NODE_DUMMY_CALLER_NIID)-1 downto 0);
            
            db_rx_vz : out std_logic;
            db_rx_lz : out std_logic;
            db_tx_vz : out std_logic;
            db_tx_lz : out std_logic);
    end component Dummy_Caller_Node_RTL;
   
  

    --  
    -- Signals
    --

   signal clk_i      : std_logic;
   signal    reset_i     : std_logic;

   signal   uart_tx_o   : std_logic;
   signal    uart_rx_i   : std_logic;
   signal   uart_baud_o : std_logic;
        
   signal    gpio_i      : std_logic_vector(31 downto 0);
   signal   gpio_o      : std_logic_vector(31 downto 0);
   signal  ext_int_i   : std_logic_vector(7 downto 0);
    
    -- 
    -- 
    
    --GND mng
    signal sig_GND : std_logic;
    
    -- RTSNoC signals
    signal sig_noc_reset    : std_logic;
        --Ports of the router (clockwise: nn, ne, ee, se, ss, sw, ww, nw) 
    type noc_array_of_stdlogic is array(0 to ROUTER_N_PORTS-1) of std_logic;
    type noc_array_of_stdvec38 is array(0 to ROUTER_N_PORTS-1) of std_logic_vector(NET_BUS_SIZE-1 downto 0);
    signal sig_noc_din   : noc_array_of_stdvec38;
    signal sig_noc_dout  : noc_array_of_stdvec38;
    signal sig_noc_wr    : noc_array_of_stdlogic;
    signal sig_noc_rd    : noc_array_of_stdlogic;
    signal sig_noc_wait  : noc_array_of_stdlogic;
    signal sig_noc_nd    : noc_array_of_stdlogic;
    
    -- DD3 AXI4 signals
    signal sig_ddr3_awvalid : std_logic;
    signal sig_ddr3_awready : std_logic;
    signal sig_ddr3_awaddr  : std_logic_vector(31 downto 0);
    signal sig_ddr3_awprot  : std_logic_vector(2 downto 0);

    signal sig_ddr3_wvalid  : std_logic;
    signal sig_ddr3_wready  : std_logic;
    signal sig_ddr3_wdata   : std_logic_vector(31 downto 0);
    signal sig_ddr3_wstrb   : std_logic_vector(3 downto 0);

    signal sig_ddr3_bvalid  : std_logic;
    signal sig_ddr3_bready  : std_logic;
    signal sig_ddr3_bresp   : std_logic_vector(1 downto 0);

    signal sig_ddr3_arvalid : std_logic;
    signal sig_ddr3_arready : std_logic;
    signal sig_ddr3_araddr  : std_logic_vector(31 downto 0);
    signal sig_ddr3_arprot  : std_logic_vector(2 downto 0);

    signal sig_ddr3_rvalid  : std_logic;
    signal sig_ddr3_rready  : std_logic;
    signal sig_ddr3_rdata   : std_logic_vector(31 downto 0);
    signal sig_ddr3_rresp   : std_logic_vector(1 downto 0);
    
    -- HLS nodes signals
    signal dummy_callee_node_iid   :  std_logic_vector((IID_SIZE*NODE_DUMMY_CALLEE_NIID)-1 downto 0);
    signal dummy_caller_node_iid   :  std_logic_vector((IID_SIZE*NODE_DUMMY_CALLER_NIID)-1 downto 0);
    
    --Debug signals
    signal sig_chipscope_ctrl       : std_logic_vector(35 downto 0);
    signal sig_chipscope_data  : std_logic_vector(255 downto 0);
    signal sig_chipscope_trig0  : std_logic_vector(0 downto 0);
    signal sig_chipscope_trig1  : std_logic_vector(0 downto 0);
    signal sig_chipscope_trig2  : std_logic_vector(0 downto 0);
    signal sig_chipscope_trig3  : std_logic_vector(0 downto 0);
    signal sig_chipscope_trig4  : std_logic_vector(0 downto 0);
    signal sig_chipscope_trig5  : std_logic_vector(0 downto 0);
    signal sig_chipscope_trig6  : std_logic_vector(0 downto 0);
    signal sig_chipscope_trig7  : std_logic_vector(0 downto 0);
    
    signal sig_db_noc_irq : std_logic;
    signal sig_db_plasma_int : std_logic;
    signal sig_db_rsp_eth_rx_lz : std_logic;
    signal sig_db_rsp_eth_rx_vz : std_logic;
    signal sig_db_rsp_eth_tx_lz : std_logic;
    signal sig_db_rsp_eth_tx_vz : std_logic;
    
    signal sig_tsc : std_logic_vector(31 downto 0);
    
begin

    sig_GND <= '0';

    -- -----------------------------------------------------
    -- RTSNoC router
    -- -----------------------------------------------------
    
    rtsnoc_router: ROUTER 
        GENERIC MAP (
            p_X         => conv_integer(ROUTER_X),
            p_Y         => conv_integer(ROUTER_Y),
            p_DATA      => NET_DATA_WIDTH,                    
            p_SIZE_X    => NET_SIZE_X_LOG2,
            p_SIZE_Y    => NET_SIZE_Y_LOG2)
        PORT MAP(
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
     
    -- ------------------------------------------------------------
    -- NODE - Proc node + External RAM (DDR3 controller)
    -- ------------------------------------------------------------
    --     
    proc_node: cpu_io_node_comp_manager_tb
        generic map (
            CLK_FREQ    => CLK_FREQ,
            -- RTSNoC generics
            NET_SIZE_X          => NET_SIZE_X,
            NET_SIZE_Y          => NET_SIZE_Y,
            NET_SIZE_X_LOG2     => NET_SIZE_X_LOG2,
            NET_SIZE_Y_LOG2     => NET_SIZE_Y_LOG2,
            NET_DATA_WIDTH      => NET_DATA_WIDTH,
            NET_BUS_SIZE        => NET_BUS_SIZE,
            ROUTER_X_ADDR       => conv_integer(ROUTER_X),
            ROUTER_Y_ADDR       => conv_integer(ROUTER_Y),
            ROUTER_LOCAL_ADDR   => ROUTER_ADDRS(NOC_SW_NODE_ADDR))
        port map(
            -- System signals
            clk_i       => clk_i,
            reset_axi_i   => reset_i,
            reset_noc_i   => sig_noc_reset,
            -- Peripherals
            uart_tx_o   => uart_tx_o,
            uart_rx_i   => uart_rx_i,
            uart_baud_o => uart_baud_o,
            gpio_i      => gpio_i,
            gpio_o      => gpio_o,
            ext_int_i   => ext_int_i,
            -- NoC interface
            noc_din_o   => sig_noc_din(NOC_SW_NODE_ADDR),
            noc_dout_i  => sig_noc_dout(NOC_SW_NODE_ADDR),
            noc_wr_o    => sig_noc_wr(NOC_SW_NODE_ADDR),
            noc_rd_o    => sig_noc_rd(NOC_SW_NODE_ADDR),
            noc_wait_i  => sig_noc_wait(NOC_SW_NODE_ADDR),
            noc_nd_i    => sig_noc_nd(NOC_SW_NODE_ADDR),
            -- EXT RAM
            ext_ram_awaddr_o    => sig_ddr3_awaddr,
            ext_ram_awvalid_o   => sig_ddr3_awvalid,
            ext_ram_awready_i   => sig_ddr3_awready,
            ext_ram_wdata_o     => sig_ddr3_wdata,
            ext_ram_wstrb_o     => sig_ddr3_wstrb,
            ext_ram_wvalid_o    => sig_ddr3_wvalid,
            ext_ram_wready_i    => sig_ddr3_wready,
            ext_ram_bresp_i     => sig_ddr3_bresp,
            ext_ram_bvalid_i    => sig_ddr3_bvalid,
            ext_ram_bready_o    => sig_ddr3_bready,
            ext_ram_araddr_o    => sig_ddr3_araddr,
            ext_ram_arvalid_o   => sig_ddr3_arvalid,
            ext_ram_arready_i   => sig_ddr3_arready,
            ext_ram_rdata_i     => sig_ddr3_rdata,
            ext_ram_rresp_i     => sig_ddr3_rresp,
            ext_ram_rvalid_i    => sig_ddr3_rvalid,
            ext_ram_rready_o    => sig_ddr3_rready,
            
            db_noc_proxy_irq_o => sig_db_noc_irq,
            db_pic_int_o => sig_db_plasma_int
        );
        
    ddr_ctrl: ram_amba_1024k
        port map(
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
            s_axi_rresp   => sig_ddr3_rresp);


    -- ------------------------------------------------------------
    -- HLS nodes
    -- ------------------------------------------------------------
    dummy_callee_node_iid <= (others => sig_GND);
    dummy_callee_node : Dummy_Callee_Node_RTL
        generic map (
            X           => "0",
            Y           => "0",
            LOCAL_ADDR  => ROUTER_ADDRS(NODE_DUMMY_CALLEE_NODE_ADDR),
            SIZE_X      => NET_SIZE_X_LOG2,
            SIZE_Y      => NET_SIZE_Y_LOG2,
            SIZE_DATA   => NET_DATA_WIDTH,
            RMI_MSG_SIZE => RMI_MSG_SIZE,
            IID_SIZE    => IID_SIZE)
        port map(
            -- System signals
            clk_i       => clk_i,
            rst_i       => sig_noc_reset,
            -- NoC signals
            din_o   => sig_noc_din(NODE_DUMMY_CALLEE_NODE_ADDR),
            dout_i  => sig_noc_dout(NODE_DUMMY_CALLEE_NODE_ADDR),
            wr_o    => sig_noc_wr(NODE_DUMMY_CALLEE_NODE_ADDR),
            rd_o    => sig_noc_rd(NODE_DUMMY_CALLEE_NODE_ADDR),
            wait_i  => sig_noc_wait(NODE_DUMMY_CALLEE_NODE_ADDR),
            nd_i    => sig_noc_nd(NODE_DUMMY_CALLEE_NODE_ADDR),
            
            -- IID
            iid_i  => dummy_callee_node_iid,
            
            --DB
            db_rx_vz => sig_db_rsp_eth_rx_vz,
            db_rx_lz => sig_db_rsp_eth_rx_lz,
            db_tx_vz => sig_db_rsp_eth_tx_vz,
            db_tx_lz => sig_db_rsp_eth_tx_lz
        );
        
    dummy_caller_node_iid <= (others => sig_GND);
    dummy_caller_node : Dummy_Caller_Node_RTL
        generic map (
            X           => "0",
            Y           => "0",
            LOCAL_ADDR  => ROUTER_ADDRS(NODE_DUMMY_CALLER_NODE_ADDR),
            SIZE_X      => NET_SIZE_X_LOG2,
            SIZE_Y      => NET_SIZE_Y_LOG2,
            SIZE_DATA   => NET_DATA_WIDTH,
            RMI_MSG_SIZE => RMI_MSG_SIZE,
            IID_SIZE    => IID_SIZE)
        port map(
            -- System signals
            clk_i       => clk_i,
            rst_i       => sig_noc_reset,
            -- NoC signals
            din_o   => sig_noc_din(NODE_DUMMY_CALLER_NODE_ADDR),
            dout_i  => sig_noc_dout(NODE_DUMMY_CALLER_NODE_ADDR),
            wr_o    => sig_noc_wr(NODE_DUMMY_CALLER_NODE_ADDR),
            rd_o    => sig_noc_rd(NODE_DUMMY_CALLER_NODE_ADDR),
            wait_i  => sig_noc_wait(NODE_DUMMY_CALLER_NODE_ADDR),
            nd_i    => sig_noc_nd(NODE_DUMMY_CALLER_NODE_ADDR),
            
            -- IID
            iid_i  => dummy_caller_node_iid,
            
            --DB
            db_rx_vz => open,
            db_rx_lz => open,
            db_tx_vz => open,
            db_tx_lz => open
        );
                
   --Clock Generator.
   clock_process: process
   begin
        wait for OFFSET;
        clock_loop : loop
             clk_i <= '0';
             wait for (PERIOD - (PERIOD * DUTY_CYCLE));
             clk_i <= '1';
             wait for (PERIOD * DUTY_CYCLE);
        end loop clock_loop;
   end process;
   
   reset_process: process
    begin
        reset_i <= '0';
        -- Wait 100 ns for global reset to finish
        wait for OFFSET;
        wait for 5 ns;
        
        -- Place stimulus here
        reset_i <= '0';
        wait for PERIOD; -- Wait next cycle
        wait for PERIOD; -- Wait next cycle
        wait for PERIOD; -- Wait next cycle
        wait for PERIOD; -- Wait next cycle
        reset_i <= '1';
        wait;         
    end process;
    
end RTL;
