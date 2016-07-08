----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Marcelo Daniel Berejuck
-- 
-- Create Date:    09:20:22 03/08/2010 
-- Design Name: 
-- Module Name:    CROSSBAR - Behavioral 
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

entity CROSSBAR is
  	 generic ( 
					p_DATA		: integer := 16;					               
					p_SIZE_X		: integer := 4;
					p_SIZE_Y		: integer := 4
	 ); 
    Port ( 
-- 			  i_CLK 				: in  STD_LOGIC;
--           i_RST 				: in  STD_LOGIC;
			  
--			  i_EN				: in STD_LOGIC;
--			  i_DST        	: in STD_LOGIC_VECTOR (3 downto 0);
			  i_GNT        	: in STD_LOGIC_VECTOR (2 downto 0);
			  -- Input ports.
			  i_NORTH		   : in STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
			  i_NORTHEAST	   : in STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
			  i_EAST 		   : in STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
			  i_SOUTHEAST	   : in STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
			  i_SOUTH		   : in STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
			  i_SOUTHWEST	   : in STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
			  i_WEST 		   : in STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
			  i_NORTHWEST	   : in STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
			  -- Output port.
			  o_DOUT 		   : out STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0)
	 );
end CROSSBAR;

architecture Behavioral of CROSSBAR is
----------------------------------------------------------------------------------
-- Internal signals.
----------------------------------------------------------------------------------
	signal w_MUX_2_DEMUX	   : STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0) := (OTHERS => '0');
	signal w_DST        		: STD_LOGIC_VECTOR (2 downto 0) := "000";
	
	signal w_NORTH		   	: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_NORTHEAST	   : STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_EAST 		   	: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_SOUTHEAST	   : STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_SOUTH		   	: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_SOUTHWEST	   : STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_WEST 		   	: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_NORTHWEST	   : STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	
	constant c_NN        	: STD_LOGIC_VECTOR(3 downto 0) := "0000"; 
	constant c_NE        	: STD_LOGIC_VECTOR(3 downto 0) := "0001"; 
	constant c_EE        	: STD_LOGIC_VECTOR(3 downto 0) := "0010"; 
	constant c_SE        	: STD_LOGIC_VECTOR(3 downto 0) := "0011"; 
	constant c_SS        	: STD_LOGIC_VECTOR(3 downto 0) := "0100"; 
	constant c_SW        	: STD_LOGIC_VECTOR(3 downto 0) := "0101"; 
	constant c_WW        	: STD_LOGIC_VECTOR(3 downto 0) := "0110"; 
	constant c_NW        	: STD_LOGIC_VECTOR(3 downto 0) := "0111"; 
	
	signal w_GNT        		: STD_LOGIC_VECTOR (2 downto 0);
	
begin

--	U_SPLIT :
--		for i in 0 to 2 generate
--			begin
--				w_DST(i) <= i_DST(i);
--				
--		end generate;

--	U_REG : process (i_CLK)
--	begin
--		if falling_edge(i_CLK) then
--			if (i_EN = '1') then
				w_GNT <= i_GNT;
--			end if;
--		end if;
--	end process U_REG;
	
----------------------------------------------------------------------------------
-- MUX.
----------------------------------------------------------------------------------
	U00 : process (w_GNT, i_NORTH, i_NORTHEAST, i_EAST, i_SOUTHEAST, 
						i_SOUTH, i_SOUTHWEST, i_WEST, i_NORTHWEST)
	begin
		case w_GNT is
			when "000" => o_DOUT  <= i_NORTH;
			when "001" => o_DOUT  <= i_NORTHEAST;
			when "010" => o_DOUT  <= i_EAST;
			when "011" => o_DOUT  <= i_SOUTHEAST;
			when "100" => o_DOUT  <= i_SOUTH;
			when "101" => o_DOUT  <= i_SOUTHWEST;
			when "110" => o_DOUT  <= i_WEST;
			when "111" => o_DOUT  <= i_NORTHWEST;
			when others => o_DOUT <= i_NORTH;
		end case;
	end process U00;
	
			
----------------------------------------------------------------------------------	
end Behavioral;

