-- FSM that performs RTSNoC transmit handshake

library ieee;
use ieee.std_logic_1164.all;

entity cctl_tx_fsm is
    port (
        clk     : in    std_logic;
        rst_n   : in    std_logic;
        start   : in    std_logic;
        hold    : in    std_logic;
        wr      : out   std_logic
    );
end cctl_tx_fsm;

architecture rtl of cctl_tx_fsm is
    type state_type is (st_rst, st_idle, st_hold, st_tx);

    signal state, next_state : state_type;
    signal wr_s : std_logic;
begin
    sync_proc: process (clk)
    begin
        if (rising_edge (clk)) then
            if (rst_n = '0') then
                state   <= st_rst;
                wr_s    <= '0';
            else
                state   <= next_state;
                wr      <= wr_s;
            end if;
        end if;
    end process;

    -- Moore State-Machine, outputs are based only on the current state
    output_decode: process (state)
    begin
        if (state = st_idle) then
            wr_s <= '0';
        elsif (state = st_hold) then
            wr_s <= '0';
        elsif (state = st_tx) then
            wr_s <= '1';
        end if;
    end process;

    next_state_decode: process (state, start, hold)
    begin
        -- Declare default state for next_state to avoid latches
        next_state <= state;
        case (state) is
            when st_idle =>
                if (start = '1') then
                    next_state <= st_hold;
                end if;
            when st_hold =>
                if (hold = '0') then
                    next_state <= st_tx;
                end if;
            when st_tx =>
                next_state <= st_idle;
            when others =>
                next_state <= st_idle;
        end case;
    end process;
end rtl;
