----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Marcelo Daniel Berejuck
-- 
-- Create Date:    13:38:54 12/01/2010 
-- Design Name: 
-- Module Name:    INPUT_INTERFACE - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: FLIT structure.
--
--					 Xori  Yori  Hori  Xdst  Ydst  Hdst            data
--				   |_____|_____|_____|_____|_____|_____|__________________________|
--				   |     |     |     |     |     |     |     							|
--                ^		^		^		^     ^     ^					^
--	p_SIZE_X	>-----|-----|-----|-----|	   |     |					|
--	p_SIZE_Y	>-----------|-----|-----------|	   |					|
--		3		>-----------------|-----------------|					|
--	p_DATA	>--------------------------------------------------|	
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

---- Uncomment the following library declaration if instantiating
---- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity INPUT_INTERFACE is
    Generic (
					p_DATA		: integer := 16;
					p_SIZE_X		: integer := 4;
					p_SIZE_Y		: integer := 4
	 );
    Port ( i_CLK 		: in  STD_LOGIC;
           i_RST 		: in  STD_LOGIC;
           i_DIN 		: in  STD_LOGIC_VECTOR ( ((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6)-1) downto 0);
           i_WR 		: in  STD_LOGIC;
			  o_DST		: out STD_LOGIC_VECTOR ( ((p_SIZE_X+p_SIZE_Y + 3) - 1) downto 0)
	 );
end INPUT_INTERFACE;

architecture Behavioral of INPUT_INTERFACE is
----------------------------------------------------------------------------------
-- Internal signals.
----------------------------------------------------------------------------------
	signal w_DST		: STD_LOGIC_VECTOR ( ((p_SIZE_X+p_SIZE_Y + 3) - 1) downto 0);
	
begin
	
----------------------------------------------------------------------------------
-- Extracts the destination address from the input data.
----------------------------------------------------------------------------------
	U_ADD :
		for i in 0 to ((p_SIZE_X + p_SIZE_Y + 3) - 1) generate
			begin
				w_DST(i) <= i_DIN(i+p_DATA);
		end generate;
			
			
	o_DST <= w_DST;

----------------------------------------------------------------------------------
end Behavioral;

