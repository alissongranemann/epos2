library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity platform is
    port (
        rst                 : in    std_logic;
        uart_rx             : in    std_logic;
        uart_tx             : out   std_logic;
        gpio_leds           : out   std_logic_vector(7 downto 0);
        switches            : in    std_logic_vector(7 downto 0);
        leds                : out   std_logic_vector(3 downto 0);
        buttons             : in    std_logic_vector(3 downto 0);
        ddr_addr            : inout std_logic_vector(14 downto 0);
        ddr_ba              : inout std_logic_vector(2 downto 0);
        ddr_cas_n           : inout std_logic;
        ddr_ck_n            : inout std_logic;
        ddr_ck_p            : inout std_logic;
        ddr_cke             : inout std_logic;
        ddr_cs_n            : inout std_logic;
        ddr_dm              : inout std_logic_vector(3 downto 0);
        ddr_dq              : inout std_logic_vector(31 downto 0);
        ddr_dqs_n           : inout std_logic_vector(3 downto 0);
        ddr_dqs_p           : inout std_logic_vector(3 downto 0);
        ddr_odt             : inout std_logic;
        ddr_ras_n           : inout std_logic;
        ddr_reset_n         : inout std_logic;
        ddr_we_n            : inout std_logic;
        fixed_io_ddr_vrn    : inout std_logic;
        fixed_io_ddr_vrp    : inout std_logic;
        fixed_io_mio        : inout std_logic_vector(53 downto 0);
        fixed_io_ps_clk     : inout std_logic;
        fixed_io_ps_porb    : inout std_logic;
        fixed_io_ps_srstb   : inout std_logic
    );
end platform;

architecture rtl of platform is
    component axi4_reset_control is
        port (
            clk_i       : in    std_logic;
            ext_reset_i : in    std_logic;
            axi_reset_o : out   std_logic
        );
    end component;

    component ps is
        port (
            ddr_cas_n           : inout std_logic;
            ddr_cke             : inout std_logic;
            ddr_ck_n            : inout std_logic;
            ddr_ck_p            : inout std_logic;
            ddr_cs_n            : inout std_logic;
            ddr_reset_n         : inout std_logic;
            ddr_odt             : inout std_logic;
            ddr_ras_n           : inout std_logic;
            ddr_we_n            : inout std_logic;
            ddr_ba              : inout std_logic_vector(2 downto 0);
            ddr_addr            : inout std_logic_vector(14 downto 0);
            ddr_dm              : inout std_logic_vector(3 downto 0);
            ddr_dq              : inout std_logic_vector(31 downto 0);
            ddr_dqs_n           : inout std_logic_vector(3 downto 0);
            ddr_dqs_p           : inout std_logic_vector(3 downto 0);
            fixed_io_mio        : inout std_logic_vector(53 downto 0);
            fixed_io_ddr_vrn    : inout std_logic;
            fixed_io_ddr_vrp    : inout std_logic;
            fixed_io_ps_srstb   : inout std_logic;
            fixed_io_ps_clk     : inout std_logic;
            fixed_io_ps_porb    : inout std_logic;
            fclk_clk0           : out   std_logic;
            fclk_rst0_n         : out   std_logic;
            m_axi_arvalid       : out   std_logic;
            m_axi_awvalid       : out   std_logic;
            m_axi_bready        : out   std_logic;
            m_axi_rready        : out   std_logic;
            m_axi_wvalid        : out   std_logic;
            m_axi_arprot        : out   std_logic_vector(2 downto 0);
            m_axi_awprot        : out   std_logic_vector(2 downto 0);
            m_axi_araddr        : out   std_logic_vector(31 downto 0);
            m_axi_awaddr        : out   std_logic_vector(31 downto 0);
            m_axi_wdata         : out   std_logic_vector(31 downto 0);
            m_axi_wstrb         : out   std_logic_vector(3 downto 0);
            m_axi_arready       : in    std_logic;
            m_axi_awready       : in    std_logic;
            m_axi_bvalid        : in    std_logic;
            m_axi_rvalid        : in    std_logic;
            m_axi_wready        : in    std_logic;
            m_axi_bresp         : in    std_logic_vector(1 downto 0);
            m_axi_rresp         : in    std_logic_vector(1 downto 0);
            m_axi_rdata         : in    std_logic_vector(31 downto 0);
            s_axi_awaddr        : in    std_logic_vector(31 downto 0);
            s_axi_awprot        : in    std_logic_vector(2 downto 0);
            s_axi_awvalid       : in    std_logic;
            s_axi_awready       : out   std_logic;
            s_axi_wdata         : in    std_logic_vector (31 downto 0);
            s_axi_wstrb         : in    std_logic_vector (3 downto 0);
            s_axi_wvalid        : in    std_logic;
            s_axi_wready        : out   std_logic;
            s_axi_bresp         : out   std_logic_vector (1 downto 0);
            s_axi_bvalid        : out   std_logic;
            s_axi_bready        : in    std_logic;
            s_axi_araddr        : in    std_logic_vector (31 downto 0);
            s_axi_arprot        : in    std_logic_vector (2 downto 0);
            s_axi_arvalid       : in    std_logic;
            s_axi_arready       : out   std_logic;
            s_axi_rdata         : out   std_logic_vector (31 downto 0);
            s_axi_rresp         : out   std_logic_vector (1 downto 0);
            s_axi_rvalid        : out   std_logic;
            s_axi_rready        : in    std_logic;
            gpio_emio_tri_i     : in    std_logic_vector(63 downto 0);
            gpio_emio_tri_o     : out   std_logic_vector(63 downto 0);
            gpio_emio_tri_t     : out   std_logic_vector(63 downto 0)
        );
    end component;

    signal gpio_emio_tri_i_s    : std_logic_vector(63 downto 0);
    signal gpio_emio_tri_o_s    : std_logic_vector(63 downto 0);
    signal gpio_emio_tri_t_s    : std_logic_vector(63 downto 0);

    signal fclk_clk0_s      : std_logic;
    signal fclk_rst0_n_s    : std_logic;

    signal clk_50m_s    : std_logic;
    signal rst_s        : std_logic;
    signal gpio_i_s     : std_logic_vector(31 downto 0);
    signal gpio_o_s     : std_logic_vector(31 downto 0);
    signal ext_int_s    : std_logic_vector(7 downto 0);

    signal m_axi_awaddr_s       : std_logic_vector(31 downto 0);
    signal m_axi_awvalid_s      : std_logic;
    signal m_axi_awready_s      : std_logic;
    signal m_axi_wdata_s        : std_logic_vector(31 downto 0);
    signal m_axi_wstrb_s        : std_logic_vector(3 downto 0);
    signal m_axi_wvalid_s       : std_logic;
    signal m_axi_wready_s       : std_logic;
    signal m_axi_bresp_s        : std_logic_vector(1 downto 0);
    signal m_axi_bvalid_s       : std_logic;
    signal m_axi_bready_s       : std_logic;
    signal m_axi_araddr_s       : std_logic_vector(31 downto 0);
    signal m_axi_arvalid_s      : std_logic;
    signal m_axi_arready_s      : std_logic;
    signal m_axi_rdata_s        : std_logic_vector(31 downto 0);
    signal m_axi_rresp_s        : std_logic_vector(1 downto 0);
    signal m_axi_rvalid_s       : std_logic;
    signal m_axi_rready_s       : std_logic;
    signal m_axi_arprot_s       : std_logic_vector(2 downto 0);
    signal m_axi_awprot_s       : std_logic_vector(2 downto 0);

    signal ps_s_axi_awaddr_s    : std_logic_vector(31 downto 0);
    signal ps_s_axi_awvalid_s   : std_logic;
    signal ps_s_axi_awready_s   : std_logic;
    signal ps_s_axi_wdata_s     : std_logic_vector(31 downto 0);
    signal ps_s_axi_wstrb_s     : std_logic_vector(3 downto 0);
    signal ps_s_axi_wvalid_s    : std_logic;
    signal ps_s_axi_wready_s    : std_logic;
    signal ps_s_axi_bresp_s     : std_logic_vector(1 downto 0);
    signal ps_s_axi_bvalid_s    : std_logic;
    signal ps_s_axi_bready_s    : std_logic;
    signal ps_s_axi_araddr_s    : std_logic_vector(31 downto 0);
    signal ps_s_axi_arvalid_s   : std_logic;
    signal ps_s_axi_arready_s   : std_logic;
    signal ps_s_axi_rdata_s     : std_logic_vector(31 downto 0);
    signal ps_s_axi_rresp_s     : std_logic_vector(1 downto 0);
    signal ps_s_axi_rvalid_s    : std_logic;
    signal ps_s_axi_rready_s    : std_logic;
    signal ps_s_axi_arprot_s    : std_logic_vector(2 downto 0);
    signal ps_s_axi_awprot_s    : std_logic_vector(2 downto 0);
begin
    u_ps : ps
        port map (
            ddr_addr            => ddr_addr,
            ddr_ba              => ddr_ba,
            ddr_cas_n           => ddr_cas_n,
            ddr_ck_n            => ddr_ck_n,
            ddr_ck_p            => ddr_ck_p,
            ddr_cke             => ddr_cke,
            ddr_cs_n            => ddr_cs_n,
            ddr_dm              => ddr_dm,
            ddr_dq              => ddr_dq,
            ddr_dqs_n           => ddr_dqs_n,
            ddr_dqs_p           => ddr_dqs_p,
            ddr_odt             => ddr_odt,
            ddr_ras_n           => ddr_ras_n,
            ddr_reset_n         => ddr_reset_n,
            ddr_we_n            => ddr_we_n,
            fclk_clk0           => fclk_clk0_s,
            fclk_rst0_n         => fclk_rst0_n_s,
            fixed_io_ddr_vrn    => fixed_io_ddr_vrn,
            fixed_io_ddr_vrp    => fixed_io_ddr_vrp,
            fixed_io_mio        => fixed_io_mio,
            fixed_io_ps_clk     => fixed_io_ps_clk,
            fixed_io_ps_porb    => fixed_io_ps_porb,
            fixed_io_ps_srstb   => fixed_io_ps_srstb,
            m_axi_araddr        => m_axi_araddr_s,
            m_axi_arprot        => m_axi_arprot_s,
            m_axi_arready       => m_axi_arready_s,
            m_axi_arvalid       => m_axi_arvalid_s,
            m_axi_awaddr        => m_axi_awaddr_s,
            m_axi_awprot        => m_axi_awprot_s,
            m_axi_awready       => m_axi_awready_s,
            m_axi_awvalid       => m_axi_awvalid_s,
            m_axi_bready        => m_axi_bready_s,
            m_axi_bresp         => m_axi_bresp_s,
            m_axi_bvalid        => m_axi_bvalid_s,
            m_axi_rdata         => m_axi_rdata_s,
            m_axi_rready        => m_axi_rready_s,
            m_axi_rresp         => m_axi_rresp_s,
            m_axi_rvalid        => m_axi_rvalid_s,
            m_axi_wdata         => m_axi_wdata_s,
            m_axi_wready        => m_axi_wready_s,
            m_axi_wstrb         => m_axi_wstrb_s,
            m_axi_wvalid        => m_axi_wvalid_s,
            s_axi_awaddr        => ps_s_axi_awaddr_s,
            s_axi_awprot        => ps_s_axi_awprot_s,
            s_axi_awvalid       => ps_s_axi_awvalid_s,
            s_axi_awready       => ps_s_axi_awready_s,
            s_axi_wdata         => ps_s_axi_wdata_s,
            s_axi_wstrb         => ps_s_axi_wstrb_s,
            s_axi_wvalid        => ps_s_axi_wvalid_s,
            s_axi_wready        => ps_s_axi_wready_s,
            s_axi_bresp         => ps_s_axi_bresp_s,
            s_axi_bvalid        => ps_s_axi_bvalid_s,
            s_axi_bready        => ps_s_axi_bready_s,
            s_axi_araddr        => ps_s_axi_araddr_s,
            s_axi_arprot        => ps_s_axi_arprot_s,
            s_axi_arvalid       => ps_s_axi_arvalid_s,
            s_axi_arready       => ps_s_axi_arready_s,
            s_axi_rdata         => ps_s_axi_rdata_s,
            s_axi_rresp         => ps_s_axi_rresp_s,
            s_axi_rvalid        => ps_s_axi_rvalid_s,
            s_axi_rready        => ps_s_axi_rready_s,
            gpio_emio_tri_i     => gpio_emio_tri_i_s,
            gpio_emio_tri_o     => gpio_emio_tri_o_s,
            gpio_emio_tri_t     => gpio_emio_tri_t_s
        );

    u_axi4_reset_control : axi4_reset_control
        port map (
            clk_i       => fclk_clk0_s,
            ext_reset_i => not rst,
            axi_reset_o => rst_s
        );

    u_rsp : entity work.rsp
        generic map (
            CLK_FREQ        => 50_000_000
        )
        port map (
            clk_i           => fclk_clk0_s,
            reset_i         => rst_s,
            uart_tx_o       => uart_tx,
            uart_rx_i       => uart_rx,
            uart_baud_o     => open,
            gpio_i          => gpio_i_s,
            gpio_o          => gpio_o_s,
            ext_int_i       => ext_int_s,
            s_axi_awaddr    => ps_s_axi_awaddr_s,
            s_axi_awvalid   => ps_s_axi_awvalid_s,
            s_axi_awready   => ps_s_axi_awready_s,
            s_axi_wdata     => ps_s_axi_wdata_s,
            s_axi_wstrb     => ps_s_axi_wstrb_s,
            s_axi_wvalid    => ps_s_axi_wvalid_s,
            s_axi_wready    => ps_s_axi_wready_s,
            s_axi_bresp     => ps_s_axi_bresp_s,
            s_axi_bvalid    => ps_s_axi_bvalid_s,
            s_axi_bready    => ps_s_axi_bready_s,
            s_axi_araddr    => ps_s_axi_araddr_s,
            s_axi_arvalid   => ps_s_axi_arvalid_s,
            s_axi_arready   => ps_s_axi_arready_s,
            s_axi_rdata     => ps_s_axi_rdata_s,
            s_axi_rresp     => ps_s_axi_rresp_s,
            s_axi_rvalid    => ps_s_axi_rvalid_s,
            s_axi_rready    => ps_s_axi_rready_s,
            s_axi_awprot    => ps_s_axi_awprot_s,
            s_axi_arprot    => ps_s_axi_arprot_s
        );

    gpio_i_s(11 downto 0)   <= buttons & switches;
    gpio_i_s(31 downto 12)  <= (others => '0');
    gpio_leds               <= gpio_o_s(7 downto 0);
    leds                    <= gpio_o_s(11 downto 8);
    ext_int_s(3 downto 0)   <= buttons;
    ext_int_s(7 downto 4)   <= (others => '0');
end rtl;
