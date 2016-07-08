library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

-- Limitations: Address size.

entity cctl is
    generic (
        X                   : integer := 0;
        Y                   : integer := 0;
        LOCAL               : integer := 0;
        DATA_WIDTH          : integer := 56;
        X_WIDTH             : integer := 1;
        Y_WIDTH             : integer := 1;
        LOCAL_WIDTH         : integer := 3;
        C_S_AXI_DATA_WIDTH  : integer := 32;
        C_S_AXI_ADDR_WIDTH  : integer := 4
    );
    port (
        noc_int         : out   std_logic;
        noc_din         : out   std_logic_vector(DATA_WIDTH+2*(X_WIDTH+Y_WIDTH+LOCAL_WIDTH)-1 downto 0);
        noc_wr          : out   std_logic;
        noc_rd          : out   std_logic;
        noc_dout        : in    std_logic_vector(DATA_WIDTH+2*(X_WIDTH+Y_WIDTH+LOCAL_WIDTH)-1 downto 0);
        noc_wait        : in    std_logic;
        noc_nd          : in    std_logic;
        s_axi_aclk      : in    std_logic;
        s_axi_aresetn   : in    std_logic;
        s_axi_awaddr    : in    std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
        s_axi_awprot    : in    std_logic_vector(2 downto 0);
        s_axi_awvalid   : in    std_logic;
        s_axi_awready   : out   std_logic;
        s_axi_wdata     : in    std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
        s_axi_wstrb     : in    std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0);
        s_axi_wvalid    : in    std_logic;
        s_axi_wready    : out   std_logic;
        s_axi_bresp     : out   std_logic_vector(1 downto 0);
        s_axi_bvalid    : out   std_logic;
        s_axi_bready    : in    std_logic;
        s_axi_araddr    : in    std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
        s_axi_arprot    : in    std_logic_vector(2 downto 0);
        s_axi_arvalid   : in    std_logic;
        s_axi_arready   : out   std_logic;
        s_axi_rdata     : out   std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
        s_axi_rresp     : out   std_logic_vector(1 downto 0);
        s_axi_rvalid    : out   std_logic;
        s_axi_rready    : in    std_logic
    );
end cctl;

architecture rtl of cctl is
    -- Example-specific design signals
    -- local parameter for addressing 32 bit / 64 bit C_S_AXI_DATA_WIDTH
    -- ADDR_LSB is used for addressing 32/64 bit registers/memories
    -- ADDR_LSB = 2 for 32 bits (n downto 2)
    -- ADDR_LSB = 3 for 64 bits (n downto 3)
    constant ADDR_LSB           : integer := (C_S_AXI_DATA_WIDTH/32)+ 1;
    constant OPT_MEM_ADDR_BITS  : integer := 1;

    -- Width for a pair of (x, y, local)
    --constant NOC_HEAD_WIDTH : integer := 2*(X_WIDTH+Y_WIDTH+LOCAL_WIDTH);
    constant NOC_PKT_WIDTH  : integer := DATA_WIDTH+2*(X_WIDTH+Y_WIDTH+LOCAL_WIDTH);

    -- AXI4LITE signals
    signal axi_awaddr   : std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
    signal axi_awready  : std_logic;
    signal axi_wready   : std_logic;
    signal axi_bresp    : std_logic_vector(1 downto 0);
    signal axi_bvalid   : std_logic;
    signal axi_araddr   : std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
    signal axi_arready  : std_logic;
    signal axi_rdata    : std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
    signal axi_rresp    : std_logic_vector(1 downto 0);
    signal axi_rvalid   : std_logic;

    -- Number of Slave Registers 4
    signal slv_reg_rd_data  : std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
    signal slv_reg_wr_data  : std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
    signal slv_reg_rd_addr  : std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
    signal slv_reg_wr_addr  : std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
    signal slv_reg_rden     : std_logic;
    signal slv_reg_wren     : std_logic;
    signal reg_data_out     : std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
    signal byte_index       : integer;

    signal wr_x_src     : std_logic_vector(X_WIDTH-1 downto 0);
    signal wr_y_src     : std_logic_vector(Y_WIDTH-1 downto 0);
    signal wr_local_src : std_logic_vector(LOCAL_WIDTH-1 downto 0);
    signal wr_x_dst     : std_logic_vector(X_WIDTH-1 downto 0);
    signal wr_y_dst     : std_logic_vector(Y_WIDTH-1 downto 0);
    signal wr_local_dst : std_logic_vector(LOCAL_WIDTH-1 downto 0);
    signal wr_type      : std_logic_vector(7 downto 0);
    signal wr_unit      : std_logic_vector(7 downto 0);
    signal wr_msg       : std_logic_vector(2 downto 0);
    signal rd_x_src     : std_logic_vector(X_WIDTH-1 downto 0);
    signal rd_y_src     : std_logic_vector(Y_WIDTH-1 downto 0);
    signal rd_local_src : std_logic_vector(LOCAL_WIDTH-1 downto 0);
    signal rd_x_dst     : std_logic_vector(X_WIDTH-1 downto 0);
    signal rd_y_dst     : std_logic_vector(Y_WIDTH-1 downto 0);
    signal rd_local_dst : std_logic_vector(LOCAL_WIDTH-1 downto 0);
    signal rd_type      : std_logic_vector(7 downto 0);
    signal rd_unit      : std_logic_vector(7 downto 0);
    signal rd_msg       : std_logic_vector(2 downto 0);

    -- tx_start_s triggers the transmission of a packet through the NoC when
    -- data is written to slv_reg_wr_data
    signal tx_start_s       : std_logic;
    signal tx_start_d       : std_logic;
    signal tx_start_rise_s  : std_logic;
begin
    -- I/O Connections assignments

    s_axi_awready   <= axi_awready;
    s_axi_wready    <= axi_wready;
    s_axi_bresp     <= axi_bresp;
    s_axi_bvalid    <= axi_bvalid;
    s_axi_arready   <= axi_arready;
    s_axi_rdata     <= axi_rdata;
    s_axi_rresp     <= axi_rresp;
    s_axi_rvalid    <= axi_rvalid;

    -- Implement axi_awready generation
    -- axi_awready is asserted for one s_axi_aclk clock cycle when both
    -- s_axi_awvalid and s_axi_wvalid are asserted. axi_awready is
    -- de-asserted when reset is low.
    process (s_axi_aclk)
    begin
        if (rising_edge(s_axi_aclk)) then
            if (s_axi_aresetn = '0') then
                axi_awready <= '0';
            else
                if (axi_awready = '0' and s_axi_awvalid = '1' and s_axi_wvalid = '1') then
                    -- Slave is ready to accept write address when
                    -- there is a valid write address and write data
                    -- on the write address and data bus. This design
                    -- expects no outstanding transactions.
                    axi_awready <= '1';
                else
                    axi_awready <= '0';
                end if;
            end if;
        end if;
    end process;

    -- Implement axi_awaddr latching
    -- This process is used to latch the address when both
    -- s_axi_awvalid and s_axi_wvalid are valid.
    process (s_axi_aclk)
    begin
        if (rising_edge(s_axi_aclk)) then
            if (s_axi_aresetn = '0') then
                axi_awaddr <= (others => '0');
            else
                if (axi_awready = '0' and s_axi_awvalid = '1' and s_axi_wvalid = '1') then
                    -- Write Address latching
                    axi_awaddr <= s_axi_awaddr;
                end if;
            end if;
        end if;
    end process;

    -- Implement axi_wready generation
    -- axi_wready is asserted for one s_axi_aclk clock cycle when both
    -- s_axi_awvalid and s_axi_wvalid are asserted. axi_wready is
    -- de-asserted when reset is low.
    process (s_axi_aclk)
    begin
        if (rising_edge(s_axi_aclk)) then
            if (s_axi_aresetn = '0') then
                axi_wready <= '0';
            else
                if (axi_wready = '0' and s_axi_wvalid = '1' and s_axi_awvalid = '1') then
                        -- Slave is ready to accept write data when
                        -- there is a valid write address and write data
                        -- on the write address and data bus. This design
                        -- expects no outstanding transactions.
                        axi_wready <= '1';
                else
                    axi_wready <= '0';
                end if;
            end if;
        end if;
    end process;

    -- Implement memory mapped register select and write logic generation. The
    -- write data is accepted and written to memory mapped registers when
    -- axi_awready, s_axi_wvalid, axi_wready and s_axi_wvalid are asserted. Write
    -- strobes are used to select byte enables of slave registers while writing.
    -- These registers are cleared when reset (active low) is applied. Slave
    -- register write enable is asserted when valid address and data are available
    -- and the slave is ready to accept the write address and write data.
    slv_reg_wren <= axi_wready and s_axi_wvalid and axi_awready and s_axi_awvalid ;

    process (s_axi_aclk)
    variable loc_addr :std_logic_vector(OPT_MEM_ADDR_BITS downto 0);
    begin
        if (rising_edge(s_axi_aclk)) then
            if (s_axi_aresetn = '0') then
                --slv_reg_rd_data <= (others => '0');
                slv_reg_wr_data <= (others => '0');
                --slv_reg_rd_addr <= (others => '0');
                slv_reg_wr_addr <= (others => '0');
            else
                loc_addr := axi_awaddr(ADDR_LSB + OPT_MEM_ADDR_BITS downto ADDR_LSB);
                if (slv_reg_wren = '1') then
                    case loc_addr is
                        when b"00" =>
                            for byte_index in 0 to (C_S_AXI_DATA_WIDTH/8-1) loop
                                if (s_axi_wstrb(byte_index) = '1') then
                                    -- Respective byte enables are asserted as per write strobes
                                    -- slave registor 0
                                    --slv_reg_rd_data(byte_index*8+7 downto byte_index*8) <= s_axi_wdata(byte_index*8+7 downto byte_index*8);
                                end if;
                            end loop;
                        when b"01" =>
                            for byte_index in 0 to (C_S_AXI_DATA_WIDTH/8-1) loop
                                if (s_axi_wstrb(byte_index) = '1') then
                                    -- Respective byte enables are asserted as per write strobes
                                    -- slave registor 1
                                    slv_reg_wr_data(byte_index*8+7 downto byte_index*8) <= s_axi_wdata(byte_index*8+7 downto byte_index*8);
                                end if;
                            end loop;
                        when b"10" =>
                            for byte_index in 0 to (C_S_AXI_DATA_WIDTH/8-1) loop
                                if (s_axi_wstrb(byte_index) = '1') then
                                    -- Respective byte enables are asserted as per write strobes
                                    -- slave registor 2
                                    --slv_reg_rd_addr(byte_index*8+7 downto byte_index*8) <= s_axi_wdata(byte_index*8+7 downto byte_index*8);
                                end if;
                            end loop;
                        when b"11" =>
                            for byte_index in 0 to (C_S_AXI_DATA_WIDTH/8-1) loop
                                if (s_axi_wstrb(byte_index) = '1') then
                                    -- Respective byte enables are asserted as per write strobes
                                    -- slave registor 3
                                    slv_reg_wr_addr(byte_index*8+7 downto byte_index*8) <= s_axi_wdata(byte_index*8+7 downto byte_index*8);
                                end if;
                            end loop;
                        when others =>
                            --slv_reg_rd_data <= slv_reg_rd_data;
                            slv_reg_wr_data <= slv_reg_wr_data;
                            --slv_reg_rd_addr <= slv_reg_rd_addr;
                            slv_reg_wr_addr <= slv_reg_wr_addr;
                    end case;
                end if;
            end if;
        end if;
    end process;

    -- Implement write response logic generation. The write response and response
    -- valid signals are asserted by the slave when axi_wready, s_axi_wvalid,
    -- axi_wready and s_axi_wvalid are asserted. This marks the acceptance of
    -- address and indicates the status of write transaction.
    process (s_axi_aclk)
    begin
        if (rising_edge(s_axi_aclk)) then
            if (s_axi_aresetn = '0') then
                axi_bvalid  <= '0';
                axi_bresp   <= "00"; --need to work more on the responses
            else
                if (axi_awready = '1' and s_axi_awvalid = '1' and axi_wready = '1' and s_axi_wvalid = '1' and axi_bvalid = '0') then
                    axi_bvalid <= '1';
                    axi_bresp  <= "00";
                -- Check if bready is asserted while bvalid is high ,there is a
                -- possibility that bready is always asserted high
                elsif (s_axi_bready = '1' and axi_bvalid = '1') then
                    axi_bvalid <= '0';
                end if;
            end if;
        end if;
    end process;

    -- Implement axi_arready generation. axi_arready is asserted for one s_axi_aclk
    -- clock cycle when s_axi_arvalid is asserted. axi_awready is de-asserted when
    -- reset (active low) is asserted. The read address is also latched when
    -- s_axi_arvalid is asserted. axi_araddr is reset to zero on reset assertion.
    process (s_axi_aclk)
    begin
        if (rising_edge(s_axi_aclk)) then
            if (s_axi_aresetn = '0') then
                axi_arready <= '0';
                axi_araddr  <= (others => '1');
            else
                if (axi_arready = '0' and s_axi_arvalid = '1') then
                    -- Indicates that the slave has acceped the valid read address
                    axi_arready <= '1';
                    -- Read Address latching
                    axi_araddr  <= s_axi_araddr;
                else
                    axi_arready <= '0';
                end if;
            end if;
        end if;
    end process;

    -- Implement axi_arvalid generation.  axi_rvalid is asserted for one s_axi_aclk
    -- clock cycle when both s_axi_arvalid and axi_arready are asserted. The slave
    -- registers data are available on the axi_rdata bus at this instance. The
    -- assertion of axi_rvalid marks the validity of read data on the bus and
    -- axi_rresp indicates the status of read transaction.axi_rvalid is deasserted
    -- on reset (active low). axi_rresp and axi_rdata are cleared to zero on reset
    -- (active low).
    process (s_axi_aclk)
    begin
        if (rising_edge(s_axi_aclk)) then
            if (s_axi_aresetn = '0') then
                axi_rvalid <= '0';
                axi_rresp  <= "00";
            else
                if (axi_arready = '1' and s_axi_arvalid = '1' and axi_rvalid = '0') then
                    -- Valid read data is available at the read data bus
                    axi_rvalid <= '1';
                    axi_rresp  <= "00"; -- 'OKAY' response
                elsif (axi_rvalid = '1' and s_axi_rready = '1') then
                    -- Read data is accepted by the master
                    axi_rvalid <= '0';
                end if;
            end if;
        end if;
    end process;

    -- Implement memory mapped register select and read logic generation.  Slave
    -- register read enable is asserted when valid address is available and the
    -- slave is ready to accept the read address.
    slv_reg_rden <= axi_arready and s_axi_arvalid and (not axi_rvalid) ;

    process (slv_reg_rd_data, slv_reg_wr_data, slv_reg_rd_addr, slv_reg_wr_addr, axi_araddr, s_axi_aresetn, slv_reg_rden)
    variable loc_addr : std_logic_vector(OPT_MEM_ADDR_BITS downto 0);
    begin
        if (s_axi_aresetn = '0') then
            reg_data_out  <= (others => '1');
        else
            -- Address decoding for reading registers
            loc_addr := axi_araddr(ADDR_LSB + OPT_MEM_ADDR_BITS downto ADDR_LSB);
            case loc_addr is
                when b"00" =>
                    reg_data_out <= slv_reg_rd_data;
                when b"01" =>
                    reg_data_out <= slv_reg_wr_data;
                when b"10" =>
                    reg_data_out <= slv_reg_rd_addr;
                when b"11" =>
                    reg_data_out <= slv_reg_wr_addr;
                when others =>
                    reg_data_out <= (others => '0');
            end case;
        end if;
    end process;

    -- Output register or memory read data
    process(s_axi_aclk) is
    begin
        if (rising_edge (s_axi_aclk)) then
            if (s_axi_aresetn = '0') then
                axi_rdata  <= (others => '0');
            else
                if (slv_reg_rden = '1') then
                    -- When there is a valid read address (s_axi_arvalid) with
                    -- acceptance of read address by the slave (axi_arready),
                    -- output the read data.
                    axi_rdata <= reg_data_out;
                end if;
            end if;
        end if;
    end process;

    -- Detect rising edge on tx_start_s
    tx_start_s <= '1' when ((slv_reg_wren = '1') and (axi_awaddr(ADDR_LSB +
                  OPT_MEM_ADDR_BITS downto ADDR_LSB) = b"01")) else '0';

    process (s_axi_aclk)
    begin
        if (rising_edge(s_axi_aclk)) then
            tx_start_d <= tx_start_s;
        end if;
    end process;

    tx_start_rise_s <= (not tx_start_d) and tx_start_s;

    u_cctl_tx_fsm : entity work.cctl_tx_fsm
        port map (
            clk     => s_axi_aclk,
            rst_n   => s_axi_aresetn,
            start   => tx_start_rise_s,
            hold    => noc_wait,
            wr      => noc_wr
        );

    u_cctl_rx_fsm : entity work.cctl_rx_fsm
        port map (
            clk     => s_axi_aclk,
            rst_n   => s_axi_aresetn,
            -- Start should be tied to a FIFO not full signal?
            start   => noc_nd,
            rd      => noc_rd
        );

    -- Assemble data to be written into to the NoC
    wr_x_src        <= std_logic_vector(to_unsigned(X, wr_x_src'length));
    wr_y_src        <= std_logic_vector(to_unsigned(Y, wr_y_src'length));
    wr_local_src    <= std_logic_vector(to_unsigned(LOCAL, wr_local_src'length));
    -- Ignoring 4 bits
    wr_x_dst        <= slv_reg_wr_addr(27 downto 27);
    -- Ignoring 4 bits
    wr_y_dst        <= slv_reg_wr_addr(22 downto 22);
    wr_local_dst    <= slv_reg_wr_addr(21 downto 19);
    wr_type         <= slv_reg_wr_addr(18 downto 11);
    wr_unit         <= slv_reg_wr_addr(10 downto 3);
    -- Ignoring 4 bits
    wr_msg          <= slv_reg_wr_addr(2 downto 0);
    noc_din         <= wr_x_src & wr_y_src & wr_local_src & wr_x_dst & wr_y_dst
                       & wr_local_dst & wr_type & wr_unit & "00000" & wr_msg &
                       slv_reg_wr_data;

    -- Assemble data read from the NoC
    slv_reg_rd_addr <= "0000" & rd_x_src & "0000" & rd_y_src & rd_local_src &
                       rd_type & rd_unit & rd_msg;
    rd_x_src        <= noc_dout(65 downto 65);
    rd_y_src        <= noc_dout(64 downto 64);
    rd_local_src    <= noc_dout(63 downto 61);
    rd_x_dst        <= noc_dout(60 downto 60);
    rd_y_dst        <= noc_dout(59 downto 59);
    rd_local_dst    <= noc_dout(58 downto 56);
    rd_type         <= noc_dout(55 downto 48);
    rd_unit         <= noc_dout(47 downto 40);
    -- Ignoring 4 bits
    rd_msg          <= noc_dout(34 downto 32);
    slv_reg_rd_data <= noc_dout(31 downto 0);
end rtl;
