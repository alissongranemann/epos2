library ieee;
use ieee.std_logic_1164.all; 

-- Implemented after Plasma's AXI4 wrapper

entity wishbone_to_axi4lite is
    generic(
        ADDR_WIDTH  : integer := 32;
        DATA_WIDTH  : integer := 32);
    port(
        clk_i       : in std_logic;
        
        --Wishbone
        wb_rst_i    : in std_logic;
        wb_cyc_i    : in std_logic;
        wb_stb_i    : in std_logic;
        wb_adr_i    : in std_logic_vector(ADDR_WIDTH-1 downto 0);
        wb_sel_i    : in std_logic_vector((DATA_WIDTH/8)-1 downto 0);
        wb_we_i     : in std_logic;
        wb_dat_i    : in std_logic_vector(DATA_WIDTH-1 downto 0);
        wb_dat_o    : out std_logic_vector(DATA_WIDTH-1 downto 0);
        wb_ack_o    : out std_logic;
        -- AXI
        axi_rst_o : out std_logic;
        -- write address channel
        axi_awvalid_o    : out std_logic;
        axi_awready_i    : in std_logic;
        axi_awaddr_o     : out std_logic_vector(ADDR_WIDTH-1 downto 0);
        axi_awprot_o     : out std_logic_vector(2 downto 0);
        -- write data channel
        axi_wvalid_o     : out std_logic;
        axi_wready_i     : in std_logic;
        axi_wdata_o      : out std_logic_vector(DATA_WIDTH-1 downto 0);
        axi_wstrb_o      : out std_logic_vector((DATA_WIDTH/8)-1 downto 0);
        -- write response channel
        axi_bvalid_i     : in std_logic;
        axi_bready_o     : out std_logic;
        axi_bresp_i      : in std_logic_vector(1 downto 0);
        -- read address channel
        axi_arvalid_o    : out std_logic;
        axi_arready_i    : in std_logic;
        axi_araddr_o     : out std_logic_vector(ADDR_WIDTH-1 downto 0);
        axi_arprot_o     : out std_logic_vector(2 downto 0);
        -- read data channel
        axi_rvalid_i     : in std_logic;
        axi_rready_o     : out std_logic;
        axi_rdata_i      : in std_logic_vector(DATA_WIDTH-1 downto 0);
        axi_rresp_i      : in std_logic_vector(1 downto 0));
end wishbone_to_axi4lite;


architecture RTL of wishbone_to_axi4lite is
    
    type STATE_TYPE is (RST1, RST2, IDLE, ADDR_READ, DATA_READ, 
                        ADDR_WRITE, DATA_WRITE, WRITE_RESPONSE);
    signal state : STATE_TYPE;


begin


    -- leave write and read protections ALL LOW
    axi_awprot_o <= "000";
    axi_arprot_o <= "000";

    -- wb reset is active HIGH while AXI is active LOW, damn!
    axi_rst_o <= not wb_rst_i;

    -- permanently tied signals
    axi_araddr_o         <= wb_adr_i;
    axi_awaddr_o         <= wb_adr_i;
    wb_dat_o            <= axi_rdata_i;
    axi_wdata_o          <= wb_dat_i;
    axi_wstrb_o          <= wb_sel_i;
    axi_rready_o         <= '1';
    axi_bready_o         <= '1';
    

    -- Moore machine, the inputs are WB's external memory signals
    -- and the outputs are all AXI4Lite channels
    fsm_proc: process(clk_i, wb_rst_i)
    begin
        if wb_rst_i = '1' then
            state <= RST1;

            -- must drive these LOW on reset (AXI Protocol Specification sec. 11.1.2)
            axi_arvalid_o <= '0';
            axi_awvalid_o <= '0';
            axi_wvalid_o  <= '0';
            
            wb_ack_o <= '0';

        elsif rising_edge(clk_i) then
            case state is
                when RST1 =>
                    state <= RST2;
                    axi_arvalid_o <= '0'; axi_awvalid_o <= '0'; axi_wvalid_o  <= '0';
                    wb_ack_o <= '0';

                when RST2 =>
                    state <= IDLE;
                    axi_arvalid_o <= '0'; axi_awvalid_o <= '0'; axi_wvalid_o  <= '0';
                    wb_ack_o <= '0';

                when IDLE =>
                    if (wb_stb_i and wb_cyc_i) = '1' then
                        if wb_we_i = '0' then
                            state   <= ADDR_READ;
                            axi_arvalid_o <= '1';
                            axi_awvalid_o <= '0';
                        else
                            state   <= ADDR_WRITE;
                            axi_arvalid_o <= '0';
                            axi_awvalid_o <= '1';
                        end if;
                    else
                        state <= IDLE;
                        axi_arvalid_o <= '0';
                        axi_awvalid_o <= '0';
                    end if;
                    axi_wvalid_o <= '0';
                    wb_ack_o <= '0';

                when ADDR_READ =>
                    if axi_arready_i = '1' then
                        state <= DATA_READ;
                    else
                        state <= ADDR_READ;
                    end if;
                    wb_ack_o <= '0';
                    axi_arvalid_o             <= '1';
                    axi_awvalid_o             <= '0';
                    axi_wvalid_o              <= '0';
                
                when DATA_READ =>
                    if axi_rvalid_i = '1' then
                        state <= IDLE;
                        wb_ack_o <= '1';
                    else
                        state <= DATA_READ;
                        wb_ack_o <= '0';
                    end if;
                    
                    axi_arvalid_o             <= '0';
                    axi_awvalid_o             <= '0';
                    axi_wvalid_o              <= '0';
                
                when ADDR_WRITE =>
                    if axi_awready_i = '1' then
                        state <= DATA_WRITE;
                    else
                        state <= ADDR_WRITE;
                    end if;
                    wb_ack_o <= '0';
                    axi_arvalid_o             <= '0';
                    axi_awvalid_o             <= '1';
                    axi_wvalid_o              <= '0';

                when DATA_WRITE =>
                    state <= WRITE_RESPONSE;

                    wb_ack_o <= '0';
                    axi_arvalid_o             <= '0';
                    axi_awvalid_o             <= '0';
                    axi_wvalid_o              <= '1';

                when WRITE_RESPONSE =>
                    if axi_bvalid_i = '1' then
                        state <= IDLE;
                        wb_ack_o <= '1';
                    else
                        state <= WRITE_RESPONSE;
                        wb_ack_o <= '0';
                    end if;
                    axi_arvalid_o             <= '0';
                    axi_awvalid_o             <= '0';
                    axi_wvalid_o              <= '0';
            end case;

        end if;
    end process;

end RTL;

