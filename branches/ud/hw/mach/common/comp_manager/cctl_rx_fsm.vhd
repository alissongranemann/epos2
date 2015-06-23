-- FSM that performs RTSNoC receive handshake

library ieee;
use ieee.std_logic_1164.all;

entity cctl_rx_fsm is
    port (
        clk     : in    std_logic;
        rst_n   : in    std_logic;
        start   : in    std_logic;
        rd      : out   std_logic
    );
end cctl_rx_fsm;

architecture rtl of cctl_rx_fsm is
    type state_type is (st_rst, st_req, st_hold, st_rx);

    signal state, next_state : state_type;
    signal rd_s : std_logic;
begin
    sync_proc: process (clk)
    begin
        if (rising_edge (clk)) then
            if (rst_n = '0') then
                state   <= st_rst;
                rd_s    <= '0';
            else
                state   <= next_state;
                rd      <= rd_s;
            end if;
        end if;
    end process;

    -- Moore State-Machine, outputs are based only on the current state
    output_decode: process (state)
    begin
        if (state = st_req) then
            rd_s <= '1';
        elsif (state = st_hold) then
            rd_s <= '0';
        elsif (state = st_rx) then
            rd_s <= '0';
        end if;
    end process;

    next_state_decode: process (state, start)
    begin
        -- Declare default state for next_state to avoid latches
        next_state <= state;
        case (state) is
            when st_req =>
                next_state <= st_hold;
            when st_hold =>
                if (start = '0') then
                    next_state <= st_rx;
                end if;
            when st_rx =>
                next_state <= st_req;
            when others =>
                next_state <= st_req;
        end case;
    end process;
end rtl;

