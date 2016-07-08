----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Marcelo Daniel Berejuck
-- 
-- Create Date:    16:11:00 12/03/2010 
-- Design Name: 
-- Module Name:    COMPARE - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
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

entity COMPARE is
    Port ( 	i_ACTIVE : in  STD_LOGIC;
				i_DIN 	: in  STD_LOGIC_VECTOR (2 downto 0);
            i_REF 	: in  STD_LOGIC_VECTOR (2 downto 0);
            o_DOUT	: out  STD_LOGIC
	 );
end COMPARE;

architecture Behavioral of COMPARE is
----------------------------------------------------------------------------------
-- Internal signals.
----------------------------------------------------------------------------------


begin

	o_DOUT <= '1' when ((i_DIN = i_REF) and (i_ACTIVE = '0')) else '0';
	
	
----------------------------------------------------------------------------------
end Behavioral;

