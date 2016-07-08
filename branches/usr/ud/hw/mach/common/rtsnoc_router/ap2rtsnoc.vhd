library ieee;
use ieee.std_logic_1164.all;

entity ap2rtsnoc is
    port (
        clk         : in    std_logic;
        rst_n       : in    std_logic;
        ap_vld      : in    std_logic;
        ap_ack      : out   std_logic;
        rtsnoc_wr   : out   std_logic;
        rtsnoc_wait : in    std_logic
    );
end ap2rtsnoc;

architecture structural of ap2rtsnoc is
    type state_type is (idle, tx, ack);
    signal state, next_state : state_type;

    -- Internal signals for all state-machine outputs
    signal rtsnoc_wr_s       : std_logic;
    signal ap_ack_s          : std_logic;
begin
    sync_proc: process (clk)
    begin
        if (rising_edge(clk)) then
            if (rst_n = '0') then
                state       <= idle;
                rtsnoc_wr   <= '0';
                ap_ack      <= '0';
            else
                state       <= next_state;
                rtsnoc_wr   <= rtsnoc_wr_s;
                ap_ack      <= ap_ack_s;
            end if;
        end if;
    end process;

    -- Moore State-Machine: Outputs based on current state only
    output_decode: process (state)
    begin
        if (state = idle) then
            rtsnoc_wr_s <= '0';
            ap_ack_s    <= '0';
        elsif (state = tx) then
            rtsnoc_wr_s <= '1';
            ap_ack_s    <= '0';
        elsif (state = ack) then
            rtsnoc_wr_s <= '0';
            ap_ack_s    <= '1';
        else
            rtsnoc_wr_s <= '0';
            ap_ack_s    <= '0';
        end if;
    end process;

    next_state_decode: process (state, rtsnoc_wait, ap_vld)
    begin
        -- Declare default state for next_state to avoid latches
        next_state <= state;
        case (state) is
            when idle =>
                if (rtsnoc_wait = '0' and ap_vld = '1')  then
                    next_state <= tx;
                end if;
            when tx =>
                next_state <= ack;
            -- Not sure if the 2 cycles delay between ap_vld deassertion and
            -- ap_ack assertion might be harmful. If so, use a mealy state
            -- machine instead.
            when ack =>
                if (ap_vld = '0')  then
                    next_state <= idle;
                end if;
            when others =>
                next_state <= idle;
        end case;
    end process;
end architecture structural;
