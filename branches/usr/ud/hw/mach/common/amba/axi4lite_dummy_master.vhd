library ieee;
use ieee.std_logic_1164.all; 


entity axi4lite_dummy_master is
    port(
        aclk       : in std_logic;
        areset     : in std_logic;
        -- write address channel
        awvalid    : out std_logic;
        awready    : in std_logic;
        awaddr     : out std_logic_vector(31 downto 0);
        awprot     : out std_logic_vector(2 downto 0);
        -- write data channel
        wvalid     : out std_logic;
        wready     : in std_logic;
        wdata      : out std_logic_vector(31 downto 0);
        wstrb      : out std_logic_vector(3 downto 0);
        -- write response channel
        bvalid     : in std_logic;
        bready     : out std_logic;
        bresp      : in std_logic_vector(1 downto 0);
        -- read address channel
        arvalid    : out std_logic;
        arready    : in std_logic;
        araddr     : out std_logic_vector(31 downto 0);
        arprot     : out std_logic_vector(2 downto 0);
        -- read data channel
        rvalid     : in std_logic;
        rready     : out std_logic;
        rdata      : in std_logic_vector(31 downto 0);
        rresp      : in std_logic_vector(1 downto 0);
        
        -- interrupt, externalized
        intr       : in std_logic);
end axi4lite_dummy_master;


architecture RTL of axi4lite_dummy_master is

    type STATE_TYPE is (RST1, RST2, IDLE, ADDR_READ, DATA_READ, 
                        ADDR_WRITE, DATA_WRITE, WRITE_RESPONSE);
    signal state : STATE_TYPE;
    
    signal last_address : std_logic_vector(31 downto 0);
    signal last_data : std_logic_vector(31 downto 0);

    constant ZERO_32BITS : std_logic_vector(31 downto 0) := "00000000000000000000000000000000";

begin

 

    -- leave write and read protections ALL LOW
    awprot <= "000";
    arprot <= "000";

    
    -- permanently tied signals
    araddr           <= ZERO_32BITS;
    awaddr           <= last_address;
    wdata            <= last_data;
    rready           <= '1';
    bready           <= '1';
    wstrb            <= "1111";


    -- Moore machine, the inputs are Plasma's external memory signals
    -- and the outputs are all AXI4Lite channels
    fsm_proc: process(aclk, areset)
    begin
        if areset = '0' then
            state <= RST1;

            -- must drive these LOW on reset (AXI Protocol Specification sec. 11.1.2)
            arvalid <= '0';
            awvalid <= '0';
            wvalid  <= '0';
            last_address <= ZERO_32BITS;
            last_data <= ZERO_32BITS;

        elsif rising_edge(aclk) then
            case state is
                when RST1 =>
                    state <= RST2;
                    arvalid <= '0'; awvalid <= '0'; wvalid  <= '0';
                    last_address <= ZERO_32BITS;
                    last_data <= ZERO_32BITS;

                when RST2 =>
                    state <= IDLE;
                    arvalid <= '0'; awvalid <= '0'; wvalid  <= '0';
                    last_address <= ZERO_32BITS;
                    last_data <= ZERO_32BITS;

                when IDLE =>
                    if intr='1' then
                        state   <= ADDR_READ;
                        arvalid <= '1';
                        awvalid <= '0';
                    else
                        state <= IDLE;
                        arvalid             <= '0';
                        awvalid             <= '0';
                    end if;
                    wvalid <= '0';

                when ADDR_READ =>
                    if arready = '1' then
                        state <= DATA_READ;
                    else
                        state <= ADDR_READ;
                    end if;
                    arvalid             <= '1';
                    awvalid             <= '0';
                    wvalid              <= '0';
                
                when DATA_READ =>
                    if rvalid = '1' then
                        state   <= ADDR_WRITE;
                        last_address <= rdata;
                        last_data <= rdata;
                        awvalid <= '1';
                    else
                        state <= DATA_READ;
                        awvalid             <= '0';
                    end if;
                    arvalid             <= '0';
                    wvalid              <= '0';
                    
                
                when ADDR_WRITE =>
                    if awready = '1' then
                        state <= DATA_WRITE;
                    else
                        state <= ADDR_WRITE;
                    end if;
                    arvalid             <= '0';
                    awvalid             <= '1';
                    wvalid              <= '0';

                when DATA_WRITE =>
                    state <= WRITE_RESPONSE;

                    arvalid             <= '0';
                    awvalid             <= '0';
                    wvalid              <= '1';

                when WRITE_RESPONSE =>
                    if bvalid = '1' then
                        state <= IDLE;
                    else
                        state <= WRITE_RESPONSE;
                    end if;
                    arvalid             <= '0';
                    awvalid             <= '0';
                    wvalid              <= '0';
            end case;

        end if;
    end process;

end RTL;

