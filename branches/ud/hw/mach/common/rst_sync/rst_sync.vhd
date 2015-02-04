library ieee;
use ieee.std_logic_1164.all;

entity rst_sync is
    port (
      clk       : in    std_logic;
      arst_n    : in    std_logic;
      rst_n     : out   std_logic
    );
end rst_sync;

architecture rtl of rst_sync is
    signal q : std_logic;
begin
    process (clk, arst_n)
    begin
        if (arst_n = '0') then
            q       <= '0';
            rst_n   <= '0';
        elsif (rising_edge(clk)) then
            q       <= '1';
            rst_n   <= q;
        end if;
 end process;
end rtl;
