library ieee;
use ieee.std_logic_1164.all;

entity tb_cctl is
end tb_cctl;

architecture tb of tb_cctl is
    constant CLK_PERIOD : time := 10 ns;

    constant X                  : integer := 0;
    constant Y                  : integer := 0;
    constant LOCAL              : integer := 3;
    constant NOC_DATA_WIDTH     : integer := 56;
    constant X_WIDTH            : integer := 1;
    constant Y_WIDTH            : integer := 1;
    constant LOCAL_WIDTH        : integer := 3;
    constant NOC_HEAD_WIDTH     : integer := 2*(Y_WIDTH+X_WIDTH+LOCAL_WIDTH);
    constant NOC_PKT_WIDTH      : integer := NOC_DATA_WIDTH+NOC_HEAD_WIDTH;
    constant C_S_AXI_DATA_WIDTH : integer := 32;
    constant C_S_AXI_ADDR_WIDTH : integer := 4;

    signal clk_s : std_logic := '0';

    signal noc_int_s        : std_logic;
    signal noc_din_s        : std_logic_vector(NOC_PKT_WIDTH-1 downto 0);
    signal noc_wr_s         : std_logic;
    signal noc_rd_s         : std_logic;
    signal noc_dout_s       : std_logic_vector(NOC_PKT_WIDTH-1 downto 0);
    signal noc_wait_s       : std_logic;
    signal noc_nd_s         : std_logic;
    signal s_axi_aclk_s     : std_logic;
    signal s_axi_aresetn_s  : std_logic;
    signal s_axi_awaddr_s   : std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
    signal s_axi_awprot_s   : std_logic_vector(2 downto 0);
    signal s_axi_awvalid_s  : std_logic;
    signal s_axi_awready_s  : std_logic;
    signal s_axi_wdata_s    : std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
    signal s_axi_wstrb_s    : std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0);
    signal s_axi_wvalid_s   : std_logic;
    signal s_axi_wready_s   : std_logic;
    signal s_axi_bresp_s    : std_logic_vector(1 downto 0);
    signal s_axi_bvalid_s   : std_logic;
    signal s_axi_bready_s   : std_logic;
    signal s_axi_araddr_s   : std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
    signal s_axi_arprot_s   : std_logic_vector(2 downto 0);
    signal s_axi_arvalid_s  : std_logic;
    signal s_axi_arready_s  : std_logic;
    signal s_axi_rdata_s    : std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
    signal s_axi_rresp_s    : std_logic_vector(1 downto 0);
    signal s_axi_rvalid_s   : std_logic;
    signal s_axi_rready_s   : std_logic;

    signal m_axi_lite_ch1_awready_s : std_logic;
    signal m_axi_lite_ch1_awaddr_s  : std_logic_vector(31 downto 0);
    signal m_axi_lite_ch1_wdata_s   : std_logic_vector(31 downto 0);
    signal m_axi_lite_ch1_awvalid_s : std_logic;
    signal m_axi_lite_ch1_bready_s  : std_logic;
    signal m_axi_lite_ch1_bvalid_s  : std_logic;
    signal m_axi_lite_ch1_wstrb_s   : std_logic_vector(3 downto 0);
    signal m_axi_lite_ch1_bresp_s   : std_logic_vector(1 downto 0);
    signal m_axi_lite_ch1_awprot_s  : std_logic_vector(2 downto 0);
    signal m_axi_lite_ch1_wvalid_s  : std_logic;
    signal m_axi_lite_ch1_wready_s  : std_logic;
    signal m_axi_lite_ch1_arready_s : std_logic;
    signal m_axi_lite_ch1_rdata_s   : std_logic_vector(31 downto 0);
    signal m_axi_lite_ch1_araddr_s  : std_logic_vector(31 downto 0);
    signal m_axi_lite_ch1_rresp_s   : std_logic_vector(1 downto 0);
    signal m_axi_lite_ch1_rready_s  : std_logic;
    signal m_axi_lite_ch1_arvalid_s : std_logic;
    signal m_axi_lite_ch1_rvalid_s  : std_logic;
    signal done_s                   : std_logic;
    signal status_s                 : std_logic_vector(31 downto 0);
begin
    dut : entity work.cctl
    generic map (
        X                   => X;
        Y                   => Y;
        LOCAL               => LOCAL;
        NOC_DATA_WIDTH      => NOC_DATA_WIDTH;
        X_WIDTH             => X_WIDTH;
        Y_WIDTH             => Y_WIDTH;
        LOCAL_WIDTH         => LOCAL_WIDTH;
        C_S_AXI_DATA_WIDTH  => C_S_AXI_DATA_WIDTH;
        C_S_AXI_ADDR_WIDTH  => C_S_AXI_ADDR_WIDTH
    );
    port map (
        noc_int       => noc_int_s,
        noc_din       => noc_din_s,
        noc_wr        => noc_wr_s,
        noc_rd        => noc_rd_s,
        noc_dout      => noc_dout_s,
        noc_wait      => noc_wait_s,
        noc_nd        => noc_nd_s,
        s_axi_aclk    => s_axi_aclk_s,
        s_axi_aresetn => s_axi_aresetn_s,
        s_axi_awaddr  => s_axi_awaddr_s,
        s_axi_awprot  => s_axi_awprot_s,
        s_axi_awvalid => s_axi_awvalid_s,
        s_axi_awready => s_axi_awready_s,
        s_axi_wdata   => s_axi_wdata_s,
        s_axi_wstrb   => s_axi_wstrb_s,
        s_axi_wvalid  => s_axi_wvalid_s,
        s_axi_wready  => s_axi_wready_s,
        s_axi_bresp   => s_axi_bresp_s,
        s_axi_bvalid  => s_axi_bvalid_s,
        s_axi_bready  => s_axi_bready_s,
        s_axi_araddr  => s_axi_araddr_s,
        s_axi_arprot  => s_axi_arprot_s,
        s_axi_arvalid => s_axi_arvalid_s,
        s_axi_arready => s_axi_arready_s,
        s_axi_rdata   => s_axi_rdata_s,
        s_axi_rresp   => s_axi_rresp_s,
        s_axi_rvalid  => s_axi_rvalid_s,
        s_axi_rready  => s_axi_rready_s
    );

    u_axi_traffic_gen : entity work.axi_traffic_gen
    port map (
        s_axi_aclk              => s_axi_aclk_s,
        s_axi_aresetn           => s_axi_aresetn_s,
        m_axi_lite_ch1_araddr   => m_axi_lite_ch1_araddr_s,
        m_axi_lite_ch1_arready  => m_axi_lite_ch1_arready_s,
        m_axi_lite_ch1_arvalid  => m_axi_lite_ch1_arvalid_s,
        m_axi_lite_ch1_awaddr   => m_axi_lite_ch1_awaddr_s,
        m_axi_lite_ch1_awprot   => m_axi_lite_ch1_awprot_s,
        m_axi_lite_ch1_awready  => m_axi_lite_ch1_awready_s,
        m_axi_lite_ch1_awvalid  => m_axi_lite_ch1_awvalid_s,
        m_axi_lite_ch1_bready   => m_axi_lite_ch1_bready_s,
        m_axi_lite_ch1_bresp    => m_axi_lite_ch1_bresp_s,
        m_axi_lite_ch1_bvalid   => m_axi_lite_ch1_bvalid_s,
        m_axi_lite_ch1_rdata    => m_axi_lite_ch1_rdata_s,
        m_axi_lite_ch1_rready   => m_axi_lite_ch1_rready_s,
        m_axi_lite_ch1_rresp    => m_axi_lite_ch1_rresp_s,
        m_axi_lite_ch1_rvalid   => m_axi_lite_ch1_rvalid_s,
        m_axi_lite_ch1_wdata    => m_axi_lite_ch1_wdata_s,
        m_axi_lite_ch1_wready   => m_axi_lite_ch1_wready_s,
        m_axi_lite_ch1_wstrb    => m_axi_lite_ch1_wstrb_s,
        m_axi_lite_ch1_wvalid   => m_axi_lite_ch1_wvalid_s,
        done                    => done_s,
        status                  => status_s
    );

    stimuli : process
    begin
        -- EDIT
        wait;
    end process;

    clk_s       <= not clk_s after CLK_PERIOD/2;
    s_axi_aclk  <= clk_s;
end tb;
