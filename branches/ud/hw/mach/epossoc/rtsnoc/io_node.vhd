library ieee;
use ieee.std_logic_1164.all;
use ieee.math_real.log2;
use ieee.math_real.ceil;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- Memory Map:
--   0x00000000  UART
--   0x00000400  GPIO
-- 
-- Interrupt Map
-- 0        UART rx
-- 1        UART tx
-- 2 -  7   Reserved
-- 8 - 15   External interrupts

entity io_node is
   generic(
        CLK_FREQ : integer := 50_000_000;
        
        -- RTSNoC generics
        NET_SIZE_X         : integer := 1;
        NET_SIZE_Y         : integer := 1;
        NET_SIZE_X_LOG2    : integer := 1; -- it should be "integer(ceil(log2(real(NET_SIZE_X))))" when NET_SIZE_X >= 2
        NET_SIZE_Y_LOG2    : integer := 1; -- it should be "integer(ceil(log2(real(NET_SIZE_Y))))" when NET_SIZE_Y >= 2
        NET_DATA_WIDTH     : integer := 32;
        NET_BUS_SIZE       : integer := 27;
        ROUTER_X_ADDR      : integer := 0;
        ROUTER_Y_ADDR      : integer := 0;
        ROUTER_LOCAL_ADDR  : std_logic_vector(2 downto 0) := "0";
        PN_ROUTER_X_ADDR      : integer := 0;
        PN_ROUTER_Y_ADDR      : integer := 0;
        PN_ROUTER_LOCAL_ADDR  : std_logic_vector(2 downto 0) := "0");

   port(clk_i       : in std_logic;
        reset_axi_i     : in std_logic;
        reset_noc_i     : in std_logic;

        uart_tx_o   : out std_logic;
        uart_rx_i   : in std_logic;
        uart_baud_o : out std_logic;
        
        gpio_i      : in  std_logic_vector(31 downto 0);
        gpio_o      : out  std_logic_vector(31 downto 0);
        ext_int_i   : in std_logic_vector(7 downto 0);
        
        io_int_o    : out std_logic_vector(15 downto 0);
        
        noc_din_o   : out std_logic_vector(NET_BUS_SIZE-1 downto 0);
        noc_wr_o    : out std_logic;
        noc_rd_o    : out std_logic;
        noc_dout_i  : in std_logic_vector(NET_BUS_SIZE-1 downto 0);
        noc_wait_i  : in std_logic;
        noc_nd_i    : in std_logic);
end io_node;

architecture RTL of io_node is
    -- AXI4 constants
    constant N_SLAVES       : integer := 2;
    constant N_SLAVES_LOG   : integer := integer(ceil(log2(real(N_SLAVES))));

    constant SLAVE0_ADDR_W  : integer := 22;
    constant SLAVE0_ADDR    : std_logic_vector(SLAVE0_ADDR_W-1 downto 0) 
        := b"0000" & b"0000" & b"0000" & b"0000" & b"0000" & b"00";
        
    constant SLAVE1_ADDR_W  : integer := 22;
    constant SLAVE1_ADDR    : std_logic_vector(SLAVE1_ADDR_W-1 downto 0) 
        := b"0000" & b"0000" & b"0000" & b"0000" & b"0000" & b"01";
  
    -- 
    -- Declarations
    -- 
    component axi4lite_decoder is
        generic(
            sw          : integer;
            sw_log2     : integer;
            s0_addr_w   : integer; 
            s0_addr     : std_logic_vector(SLAVE0_ADDR_W-1 downto 0);
            s1_addr_w   : integer; 
            s1_addr     : std_logic_vector(SLAVE1_ADDR_W-1 downto 0));
        port(
            -- master write address channel
            m_awvalid_i    : in std_logic;
            m_awready_o    : out std_logic;
            m_awaddr_i     : in std_logic_vector(31 downto 0);
            m_awprot_i     : in std_logic_vector(2 downto 0);
            -- master write data channel
            m_wvalid_i     : in std_logic;
            m_wready_o     : out std_logic;
            m_wdata_i      : in std_logic_vector(31 downto 0);
            m_wstrb_i      : in std_logic_vector(3 downto 0);
            -- master write response channel
            m_bvalid_o     : out std_logic;
            m_bready_i     : in std_logic;
            m_bresp_o      : out std_logic_vector(1 downto 0);
            -- master read address channel
            m_arvalid_i    : in std_logic;
            m_arready_o    : out std_logic;
            m_araddr_i     : in std_logic_vector(31 downto 0);
            m_arprot_i     : in std_logic_vector(2 downto 0);
            -- master read data channel
            m_rvalid_o     : out std_logic;
            m_rready_i     : in std_logic;
            m_rdata_o      : out std_logic_vector(31 downto 0);
            m_rresp_o      : out std_logic_vector(1 downto 0);
            -- slaves write address channel
            s_awvalid_o    : out std_logic_vector(N_SLAVES-1 downto 0);
            s_awready_i    : in std_logic_vector(N_SLAVES-1 downto 0);
            s_awaddr_o     : out std_logic_vector((N_SLAVES*32)-1 downto 0);
            s_awprot_o     : out std_logic_vector((N_SLAVES*3)-1 downto 0);
            -- slaves write data channel
            s_wvalid_o     : out std_logic_vector(N_SLAVES-1 downto 0);
            s_wready_i     : in std_logic_vector(N_SLAVES-1 downto 0);
            s_wdata_o      : out std_logic_vector((N_SLAVES*32)-1 downto 0);
            s_wstrb_o      : out std_logic_vector((N_SLAVES*4)-1 downto 0);
            -- slaves write response channel
            s_bvalid_i     : in std_logic_vector(N_SLAVES-1 downto 0);
            s_bready_o     : out std_logic_vector(N_SLAVES-1 downto 0);
            s_bresp_i      : in std_logic_vector((N_SLAVES*2)-1 downto 0);
            -- slaves read address channel
            s_arvalid_o    : out std_logic_vector(N_SLAVES-1 downto 0);
            s_arready_i    : in std_logic_vector(N_SLAVES-1 downto 0);
            s_araddr_o     : out std_logic_vector((N_SLAVES*32)-1 downto 0);
            s_arprot_o     : out std_logic_vector((N_SLAVES*3)-1 downto 0);
            -- slaves read data channel
            s_rvalid_i     : in std_logic_vector(N_SLAVES-1 downto 0);
            s_rready_o     : out std_logic_vector(N_SLAVES-1 downto 0);
            s_rdata_i      : in std_logic_vector((N_SLAVES*32)-1 downto 0);
            s_rresp_i      : in std_logic_vector((N_SLAVES*2)-1 downto 0));
    end component axi4lite_decoder;

    component rtsnoc_to_axi4lite_master is
        generic (
            NOC_LOCAL_ADR   : std_logic_vector(2 downto 0) := ROUTER_LOCAL_ADDR; 
            NOC_X           : std_logic_vector(NET_SIZE_X_LOG2-1 downto 0) := conv_std_logic_vector(ROUTER_X_ADDR, NET_SIZE_X_LOG2);
            NOC_Y           : std_logic_vector(NET_SIZE_Y_LOG2-1 downto 0) := conv_std_logic_vector(ROUTER_Y_ADDR, NET_SIZE_Y_LOG2);
            NOC_LOCAL_ADR_TGT   : std_logic_vector(2 downto 0) := PN_ROUTER_LOCAL_ADDR; 
            NOC_X_TGT       : std_logic_vector(NET_SIZE_X_LOG2-1 downto 0) := conv_std_logic_vector(PN_ROUTER_X_ADDR, NET_SIZE_X_LOG2);
            NOC_Y_TGT       : std_logic_vector(NET_SIZE_Y_LOG2-1 downto 0) := conv_std_logic_vector(PN_ROUTER_Y_ADDR, NET_SIZE_Y_LOG2);
            SOC_SIZE_X      : integer := NET_SIZE_X_LOG2;
            SOC_SIZE_Y      : integer := NET_SIZE_Y_LOG2;
            NOC_DATA_WIDTH  : integer := NET_DATA_WIDTH);
        port(
            -- System signals
            clk_i       : in  std_logic;
            axi_rst_i   : in  std_logic;
            noc_rst_i   : in  std_logic;
            -- AXI signals
            axi_awaddr_o          : out  std_logic_vector(31 downto 0);
            axi_awvalid_o         : out  std_logic;
            axi_awready_i         : in std_logic;
            axi_wdata_o           : out  std_logic_vector(31 downto 0);
            axi_wstrb_o           : out  std_logic_vector(3 downto 0);
            axi_wvalid_o          : out  std_logic;
            axi_wready_i          : in std_logic;
            axi_bresp_i           : in std_logic_vector(1 downto 0);
            axi_bvalid_i          : in std_logic;
            axi_bready_o          : out  std_logic;
            axi_araddr_o          : out  std_logic_vector(31 downto 0);
            axi_arvalid_o         : out  std_logic;
            axi_arready_i         : in std_logic;
            axi_rdata_i           : in std_logic_vector(31 downto 0);
            axi_rresp_i           : in std_logic_vector(1 downto 0);
            axi_rvalid_i          : in std_logic;
            axi_rready_o          : out  std_logic;
            -- NoC signals
            noc_din_o   : out std_logic_vector(NET_BUS_SIZE-1 downto 0);
            noc_dout_i  : in std_logic_vector(NET_BUS_SIZE-1 downto 0);
            noc_wr_o    : out std_logic;
            noc_rd_o    : out std_logic;
            noc_wait_i  : in std_logic;
            noc_nd_i    : in std_logic);
    end component;
    
    component simple_uart_axi4lite is
        generic(
            TXDEPTH : integer;
            RXDEPTH : integer);
        port(
            -- System signals
            clk_i       : in  std_logic;
            axi_rst_i   : in  std_logic;
            rx_int_o    : out std_logic;
            tx_int_o    : out std_logic;
            -- AXI signals
            axi_awaddr_i          : in  std_logic_vector(31 downto 0);
            axi_awvalid_i         : in  std_logic;
            axi_awready_o         : out std_logic;
            axi_wdata_i           : in  std_logic_vector(31 downto 0);
            axi_wstrb_i           : in  std_logic_vector(3 downto 0);
            axi_wvalid_i          : in  std_logic;
            axi_wready_o          : out std_logic;
            axi_bresp_o           : out std_logic_vector(1 downto 0);
            axi_bvalid_o          : out std_logic;
            axi_bready_i          : in  std_logic;
            axi_araddr_i          : in  std_logic_vector(31 downto 0);
            axi_arvalid_i         : in  std_logic;
            axi_arready_o         : out std_logic;
            axi_rdata_o           : out std_logic_vector(31 downto 0);
            axi_rresp_o           : out std_logic_vector(1 downto 0);
            axi_rvalid_o          : out std_logic;
            axi_rready_i          : in  std_logic;
            -- UARTLite Interface Signals
            rx_i    : in  std_logic;
            tx_o    : out std_logic;
            baud_o  : out std_logic);
    end component;
    
    component gpio_axi4lite is
        port(
            -- AXI signals
            clk_i       : in  std_logic;
            axi_rst_i   : in  std_logic;
            axi_awaddr_i          : in  std_logic_vector(31 downto 0);
            axi_awvalid_i         : in  std_logic;
            axi_awready_o         : out std_logic;
            axi_wdata_i           : in  std_logic_vector(31 downto 0);
            axi_wstrb_i           : in  std_logic_vector(3 downto 0);
            axi_wvalid_i          : in  std_logic;
            axi_wready_o          : out std_logic;
            axi_bresp_o           : out std_logic_vector(1 downto 0);
            axi_bvalid_o          : out std_logic;
            axi_bready_i          : in  std_logic;
            axi_araddr_i          : in  std_logic_vector(31 downto 0);
            axi_arvalid_i         : in  std_logic;
            axi_arready_o         : out std_logic;
            axi_rdata_o           : out std_logic_vector(31 downto 0);
            axi_rresp_o           : out std_logic_vector(1 downto 0);
            axi_rvalid_o          : out std_logic;
            axi_rready_i          : in  std_logic;
            -- GPIO
            gpio_i  : in  std_logic_vector(31 downto 0);
            gpio_o  : out  std_logic_vector(31 downto 0));
    end component;
          
    --  
    -- Signals
    --
    
    --GND mng
    signal sig_GND : std_logic;
    --signal sig_GND64 : std_logic_vector(63 downto 0);  

    -- AMBA AXI4 signals
    -- Master
    signal sig_m0_awvalid : std_logic;
    signal sig_m0_awready : std_logic;
    signal sig_m0_awaddr  : std_logic_vector(31 downto 0);
    signal sig_m0_awprot  : std_logic_vector(2 downto 0);

    signal sig_m0_wvalid  : std_logic;
    signal sig_m0_wready  : std_logic;
    signal sig_m0_wdata   : std_logic_vector(31 downto 0);
    signal sig_m0_wstrb   : std_logic_vector(3 downto 0);

    signal sig_m0_bvalid  : std_logic;
    signal sig_m0_bready  : std_logic;
    signal sig_m0_bresp   : std_logic_vector(1 downto 0);

    signal sig_m0_arvalid : std_logic;
    signal sig_m0_arready : std_logic;
    signal sig_m0_araddr  : std_logic_vector(31 downto 0);
    signal sig_m0_arprot  : std_logic_vector(2 downto 0);

    signal sig_m0_rvalid  : std_logic;
    signal sig_m0_rready  : std_logic;
    signal sig_m0_rdata   : std_logic_vector(31 downto 0);
    signal sig_m0_rresp   : std_logic_vector(1 downto 0);


    -- AMBA AXI4 signals
    -- Slaves
    signal sig_awvalid  : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_awready  : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_awaddr   : std_logic_vector((N_SLAVES*32)-1 downto 0);
    signal sig_awprot   : std_logic_vector((N_SLAVES*3)-1 downto 0);

    signal sig_wvalid   : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_wready   : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_wdata    : std_logic_vector((N_SLAVES*32)-1 downto 0);
    signal sig_wstrb    : std_logic_vector((N_SLAVES*4)-1 downto 0);

    signal sig_bvalid   : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_bready   : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_bresp    : std_logic_vector((N_SLAVES*2)-1 downto 0);

    signal sig_arvalid  : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_arready  : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_araddr   : std_logic_vector((N_SLAVES*32)-1 downto 0);
    signal sig_arprot   : std_logic_vector((N_SLAVES*3)-1 downto 0);

    signal sig_rvalid   : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_rready   : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_rdata    : std_logic_vector((N_SLAVES*32)-1 downto 0);
    signal sig_rresp    : std_logic_vector((N_SLAVES*2)-1 downto 0);

    type axi_array_of_stdlogic is array(0 to N_SLAVES-1) of std_logic;
    type axi_array_of_stdvec2 is array(0 to N_SLAVES-1) of std_logic_vector(1 downto 0);
    type axi_array_of_stdvec3 is array(0 to N_SLAVES-1) of std_logic_vector(2 downto 0);
    type axi_array_of_stdvec4 is array(0 to N_SLAVES-1) of std_logic_vector(3 downto 0);
    type axi_array_of_stdvec32 is array(0 to N_SLAVES-1) of std_logic_vector(31 downto 0);

    signal sig_slaves_awvalid : axi_array_of_stdlogic;
    signal sig_slaves_awready : axi_array_of_stdlogic;
    signal sig_slaves_awaddr  : axi_array_of_stdvec32;
    signal sig_slaves_awprot  : axi_array_of_stdvec3;

    signal sig_slaves_wvalid  : axi_array_of_stdlogic;
    signal sig_slaves_wready  : axi_array_of_stdlogic;
    signal sig_slaves_wdata   : axi_array_of_stdvec32;
    signal sig_slaves_wstrb   : axi_array_of_stdvec4;

    signal sig_slaves_bready  : axi_array_of_stdlogic;
    signal sig_slaves_bvalid  : axi_array_of_stdlogic;
    signal sig_slaves_bresp   : axi_array_of_stdvec2;

    signal sig_slaves_arvalid : axi_array_of_stdlogic;
    signal sig_slaves_arready : axi_array_of_stdlogic;
    signal sig_slaves_araddr  : axi_array_of_stdvec32;
    signal sig_slaves_arprot  : axi_array_of_stdvec3;

    signal sig_slaves_rready  : axi_array_of_stdlogic;
    signal sig_slaves_rdata   : axi_array_of_stdvec32;
    signal sig_slaves_rresp   : axi_array_of_stdvec2;
    signal sig_slaves_rvalid  : axi_array_of_stdlogic;
    
    -- Interrupts
    signal sig_uart_rx_int  : std_logic;
    signal sig_uart_tx_int  : std_logic;
                
    
begin

    sig_GND <= '0';
    
    -- -----------------------------------------------------
    -- AMBA interconnect
    -- -----------------------------------------------------
    
    axi4lite_interconnect: axi4lite_decoder
        generic map(
            sw          => N_SLAVES,
            sw_log2     => N_SLAVES_LOG,
            s0_addr_w   => SLAVE0_ADDR_W, 
            s0_addr     => SLAVE0_ADDR,
            s1_addr_w   => SLAVE1_ADDR_W, 
            s1_addr     => SLAVE1_ADDR)
        port map(
            -- master write address channel
            m_awvalid_i => sig_m0_awvalid,
            m_awready_o => sig_m0_awready,
            m_awaddr_i  => sig_m0_awaddr,
            m_awprot_i  => sig_m0_awprot,
            -- master write data channel
            m_wvalid_i  => sig_m0_wvalid,
            m_wready_o  => sig_m0_wready,
            m_wdata_i   => sig_m0_wdata,
            m_wstrb_i   => sig_m0_wstrb,
            -- master write response channel
            m_bvalid_o  => sig_m0_bvalid,
            m_bready_i  => sig_m0_bready,
            m_bresp_o   => sig_m0_bresp,
            -- master read address channel
            m_arvalid_i => sig_m0_arvalid,
            m_arready_o => sig_m0_arready,
            m_araddr_i  => sig_m0_araddr,
            m_arprot_i  => sig_m0_arprot,
            -- master read data channel
            m_rvalid_o  => sig_m0_rvalid,
            m_rready_i  => sig_m0_rready,
            m_rdata_o   => sig_m0_rdata,
            m_rresp_o   => sig_m0_rresp,
            -- slaves write address channel
            s_awvalid_o =>  sig_awvalid,
            s_awready_i =>  sig_awready,
            s_awaddr_o  =>  sig_awaddr,
            s_awprot_o  =>  sig_awprot,
            -- slaves write data channel
            s_wvalid_o  =>  sig_wvalid,
            s_wready_i  =>  sig_wready,
            s_wdata_o   =>  sig_wdata,
            s_wstrb_o   =>  sig_wstrb,
            -- slaves write response channel
            s_bvalid_i  =>  sig_bvalid,
            s_bready_o  =>  sig_bready,
            s_bresp_i   =>  sig_bresp,
            -- slaves read address channel
            s_arvalid_o =>  sig_arvalid,
            s_arready_i =>  sig_arready,
            s_araddr_o  =>  sig_araddr,
            s_arprot_o  =>  sig_arprot,
            -- slaves read data channel
            s_rvalid_i  =>  sig_rvalid,
            s_rready_o  =>  sig_rready,
            s_rdata_i   =>  sig_rdata,
            s_rresp_i   =>  sig_rresp);


    slaves_outputs_to_interconnect: for s in 0 to (N_SLAVES-1) generate
        sig_awready(s) <= sig_slaves_awready(s);
        sig_wready(s)  <= sig_slaves_wready(s);
        sig_bvalid(s)  <= sig_slaves_bvalid(s);
        sig_arready(s) <= sig_slaves_arready(s);
        sig_rvalid(s)  <= sig_slaves_rvalid(s);
        sig_bresp( ((s*2)+1) downto (s*2) )    <= sig_slaves_bresp(s);
        sig_rdata( ((s*32)+31) downto (s*32) ) <= sig_slaves_rdata(s);
        sig_rresp( ((s*2)+1) downto (s*2) )    <= sig_slaves_rresp(s);
    end generate slaves_outputs_to_interconnect;

    slaves_inputs_from_interconnect: for s in 0 to (N_SLAVES-1) generate
        sig_slaves_awvalid(s) <= sig_awvalid(s);
        sig_slaves_awaddr(s)  <= sig_awaddr( ((s*32)+31) downto (s*32) );
        sig_slaves_awprot(s)  <= sig_awprot( ((s*3)+2) downto (s*3) );
        sig_slaves_wvalid(s)  <= sig_wvalid(s);
        sig_slaves_wdata(s)   <= sig_wdata( ((s*32)+31) downto (s*32) );
        sig_slaves_wstrb(s)   <= sig_wstrb( ((s*4)+3) downto (s*4) );
        sig_slaves_bready(s)  <= sig_bready(s);
        sig_slaves_arvalid(s) <= sig_arvalid(s);
        sig_slaves_araddr(s)  <= sig_araddr( ((s*32)+31) downto (s*32) );
        sig_slaves_arprot(s)  <= sig_arprot( ((s*3)+2) downto (s*3) );
        sig_slaves_rready(s)  <= sig_rready(s);
    end generate slaves_inputs_from_interconnect;

    -- -----------------------------------------------------
    -- Master - RTSNOC
    -- -----------------------------------------------------

    rtsnoc_master: rtsnoc_to_axi4lite_master
        generic map (
            NOC_LOCAL_ADR   => ROUTER_LOCAL_ADDR, 
            NOC_X           => conv_std_logic_vector(ROUTER_X_ADDR, NET_SIZE_X_LOG2),
            NOC_Y           => conv_std_logic_vector(ROUTER_Y_ADDR, NET_SIZE_Y_LOG2),
            NOC_LOCAL_ADR_TGT   => PN_ROUTER_LOCAL_ADDR, 
            NOC_X_TGT       => conv_std_logic_vector(PN_ROUTER_X_ADDR, NET_SIZE_X_LOG2),
            NOC_Y_TGT       => conv_std_logic_vector(PN_ROUTER_Y_ADDR, NET_SIZE_Y_LOG2),
            SOC_SIZE_X      => NET_SIZE_X_LOG2,
            SOC_SIZE_Y      => NET_SIZE_Y_LOG2,
            NOC_DATA_WIDTH  => NET_DATA_WIDTH)
        port map(
            -- System signals
            clk_i       => clk_i,
            axi_rst_i   => reset_axi_i,
            noc_rst_i   => reset_noc_i,
            -- AXI signals
            axi_awaddr_o    => sig_m0_awaddr,
            axi_awvalid_o   => sig_m0_awvalid,
            axi_awready_i   => sig_m0_awready,
            axi_wdata_o     => sig_m0_wdata,
            axi_wstrb_o     => sig_m0_wstrb,
            axi_wvalid_o    => sig_m0_wvalid,
            axi_wready_i    => sig_m0_wready,
            axi_bresp_i     => sig_m0_bresp,
            axi_bvalid_i    => sig_m0_bvalid,
            axi_bready_o    => sig_m0_bready,
            axi_araddr_o    => sig_m0_araddr,
            axi_arvalid_o   => sig_m0_arvalid,
            axi_arready_i   => sig_m0_arready,
            axi_rdata_i     => sig_m0_rdata,
            axi_rresp_i     => sig_m0_rresp,
            axi_rvalid_i    => sig_m0_rvalid,
            axi_rready_o    => sig_m0_rready,
            -- NoC interface
            noc_din_o   => noc_din_o,
            noc_dout_i  => noc_dout_i,
            noc_wr_o    => noc_wr_o,
            noc_rd_o    => noc_rd_o,
            noc_wait_i  => noc_wait_i,
            noc_nd_i    => noc_nd_i
        );
            
    -- Interrupts
    io_int_o(0) <= sig_uart_rx_int;
    io_int_o(1) <= sig_uart_tx_int;
    io_int_o(7 downto 2) <= (others => sig_GND);
    io_int_o(15 downto 8) <= ext_int_i; 
            
    -- -----------------------------------------------------
    -- Slave0 - UART
    -- -----------------------------------------------------

    uart: simple_uart_axi4lite 
        generic map(
            TXDEPTH => 3,
            RXDEPTH => 3)
        port map(
            -- System signals
            clk_i       => clk_i,
            axi_rst_i   => reset_axi_i,
            rx_int_o    => sig_uart_rx_int,
            tx_int_o    => sig_uart_tx_int,
            -- AXI signals
            axi_awaddr_i    => sig_slaves_awaddr(0),
            axi_awvalid_i   => sig_slaves_awvalid(0),
            axi_awready_o   => sig_slaves_awready(0),
            axi_wdata_i     => sig_slaves_wdata(0),
            axi_wstrb_i     => sig_slaves_wstrb(0),
            axi_wvalid_i    => sig_slaves_wvalid(0),
            axi_wready_o    => sig_slaves_wready(0),
            axi_bresp_o     => sig_slaves_bresp(0),
            axi_bvalid_o    => sig_slaves_bvalid(0),
            axi_bready_i    => sig_slaves_bready(0),
            axi_araddr_i    => sig_slaves_araddr(0),
            axi_arvalid_i   => sig_slaves_arvalid(0),
            axi_arready_o   => sig_slaves_arready(0),
            axi_rdata_o     => sig_slaves_rdata(0),
            axi_rresp_o     => sig_slaves_rresp(0),
            axi_rvalid_o    => sig_slaves_rvalid(0),
            axi_rready_i    => sig_slaves_rready(0),
            -- UARTLite Interface Signals
            rx_i    => uart_rx_i,
            tx_o    => uart_tx_o,
            baud_o  => uart_baud_o);


    -- -----------------------------------------------------
    -- Slave1 - GPIO
    -- -----------------------------------------------------
            
    gpio: gpio_axi4lite 
        port map(
            -- System signals
            clk_i       => clk_i,
            axi_rst_i   => reset_axi_i,
            -- AXI signals
            axi_awaddr_i    => sig_slaves_awaddr(1),
            axi_awvalid_i   => sig_slaves_awvalid(1),
            axi_awready_o   => sig_slaves_awready(1),
            axi_wdata_i     => sig_slaves_wdata(1),
            axi_wstrb_i     => sig_slaves_wstrb(1),
            axi_wvalid_i    => sig_slaves_wvalid(1),
            axi_wready_o    => sig_slaves_wready(1),
            axi_bresp_o     => sig_slaves_bresp(1),
            axi_bvalid_o    => sig_slaves_bvalid(1),
            axi_bready_i    => sig_slaves_bready(1),
            axi_araddr_i    => sig_slaves_araddr(1),
            axi_arvalid_i   => sig_slaves_arvalid(1),
            axi_arready_o   => sig_slaves_arready(1),
            axi_rdata_o     => sig_slaves_rdata(1),
            axi_rresp_o     => sig_slaves_rresp(1),
            axi_rvalid_o    => sig_slaves_rvalid(1),
            axi_rready_i    => sig_slaves_rready(1),
            -- GPIO
            gpio_o  => gpio_o,
            gpio_i  => gpio_i);
            
end RTL;
