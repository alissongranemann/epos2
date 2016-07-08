----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Marcelo Daniel Berejuck
-- 
-- Create Date:    16:45:15 11/30/2010 
-- Design Name: 
-- Module Name:    SCHEDULER - Behavioral 
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

entity SCHEDULER is
    Port ( i_CLK 		: in  STD_LOGIC;
           i_RST 		: in  STD_LOGIC;
           -- Request from inputs.
			  i_REQ		: in  STD_LOGIC_VECTOR(7 downto 0);
			  i_EN		: in  STD_LOGIC;
			  o_EN		: out STD_LOGIC;
			  -- Priority level of the input.
			  i_PRI_0	: in  STD_LOGIC_VECTOR (2 downto 0);
			  i_PRI_1	: in  STD_LOGIC_VECTOR (2 downto 0);           
			  i_PRI_2	: in  STD_LOGIC_VECTOR (2 downto 0);           
			  i_PRI_3	: in  STD_LOGIC_VECTOR (2 downto 0);           
			  i_PRI_4	: in  STD_LOGIC_VECTOR (2 downto 0);           
			  i_PRI_5	: in  STD_LOGIC_VECTOR (2 downto 0);           
			  i_PRI_6	: in  STD_LOGIC_VECTOR (2 downto 0);           
			  i_PRI_7	: in  STD_LOGIC_VECTOR (2 downto 0);           
			  -- Priority that was choosen.
			  o_GRANT	: out STD_LOGIC;
			  o_CHANNEL : out  STD_LOGIC_VECTOR (3 downto 0)
	);
end SCHEDULER;

architecture Behavioral of SCHEDULER is
----------------------------------------------------------------------------------
-- Internal signals.
----------------------------------------------------------------------------------
   signal w_PRI    			: STD_LOGIC_VECTOR (7 downto 0) := (others => '0');

	signal w_PRI_0				: STD_LOGIC;
	signal w_PRI_1				: STD_LOGIC;
	signal w_PRI_2				: STD_LOGIC;
	signal w_PRI_3				: STD_LOGIC;
	signal w_PRI_4				: STD_LOGIC;
	signal w_PRI_5				: STD_LOGIC;
	signal w_PRI_6				: STD_LOGIC;
	signal w_PRI_7				: STD_LOGIC;
	
	signal w_CHANNEL     	: STD_LOGIC_VECTOR (3 downto 0); 
	signal w_NONE				: STD_LOGIC_VECTOR (3 downto 0) := "1000";
	signal w_EN					: STD_LOGIC;

begin

	--
	-- Input. Enables when detect that some request happened.
	--
	w_EN <= i_EN;

----------------------------------------------------------------------------------
-- Priority selector.
----------------------------------------------------------------------------------

	w_PRI_0 <= '1' when ((i_REQ(CONV_INTEGER(i_PRI_0)) = '1') 
			   		) else '0';
						
	
	w_PRI_1 <= '1' when ((i_REQ(CONV_INTEGER(i_PRI_1)) = '1') and 
												         (w_PRI_0 = '0')
						) else '0';
						

	w_PRI_2 <= '1' when ((i_REQ(CONV_INTEGER(i_PRI_2)) = '1') and 
														   (w_PRI_1 = '0') and
															(w_PRI_0 = '0')
						) else '0';
						

	w_PRI_3 <= '1' when ((i_REQ(CONV_INTEGER(i_PRI_3)) = '1') and 
														   (w_PRI_2 = '0') and
															(w_PRI_1 = '0') and
															(w_PRI_0 = '0') 
					   ) else '0';
						
	
	w_PRI_4 <= '1' when ((i_REQ(CONV_INTEGER(i_PRI_4)) = '1') and 
														   (w_PRI_3 = '0') and
															(w_PRI_2 = '0') and
															(w_PRI_1 = '0') and
															(w_PRI_0 = '0') 
						) else '0';
						

	w_PRI_5 <= '1' when ((i_REQ(CONV_INTEGER(i_PRI_5)) = '1') and 
														   (w_PRI_4 = '0') and
															(w_PRI_3 = '0') and
															(w_PRI_2 = '0') and
															(w_PRI_1 = '0') and
															(w_PRI_0 = '0') 
				      ) else '0';
						

	w_PRI_6 <= '1' when ((i_REQ(CONV_INTEGER(i_PRI_6)) = '1') and 
														   (w_PRI_5 = '0') and
															(w_PRI_4 = '0') and
															(w_PRI_3 = '0') and
															(w_PRI_2 = '0') and
															(w_PRI_1 = '0') and
															(w_PRI_0 = '0') 
						) else '0';
						

	w_PRI_7 <= '1' when ((i_REQ(CONV_INTEGER(i_PRI_7)) = '1') and 
													  	   (w_PRI_6 = '0') and
															(w_PRI_5 = '0') and
															(w_PRI_4 = '0') and
															(w_PRI_3 = '0') and
															(w_PRI_2 = '0') and
															(w_PRI_1 = '0') and
															(w_PRI_0 = '0') 
						) else '0';

	--
	-- Concatenation.
	--
	w_PRI <= w_PRI_7 & w_PRI_6 & w_PRI_5 & w_PRI_4 & w_PRI_3 & w_PRI_2 & w_PRI_1 & w_PRI_0;
	
----------------------------------------------------------------------------------
-- Converts the request choosen on a valid channel number.
----------------------------------------------------------------------------------
	U_CONV: process (i_CLK)
	begin
		if falling_edge (i_CLK) then
			if (w_PRI  = "00000001") then
				w_CHANNEL <= '0' & i_PRI_0;
			elsif (w_PRI  = "00000010") then
				w_CHANNEL <= '0' & i_PRI_1;
			elsif (w_PRI  = "00000100") then 
				w_CHANNEL <= '0' & i_PRI_2;
			elsif (w_PRI  = "00001000") then
				w_CHANNEL <= '0' & i_PRI_3;
			elsif (w_PRI  = "00010000") then
				w_CHANNEL <= '0' & i_PRI_4;
			elsif (w_PRI  = "00100000") then
				w_CHANNEL <= '0' & i_PRI_5;
			elsif (w_PRI  = "01000000") then
				w_CHANNEL <= '0' & i_PRI_6;
			elsif (w_PRI  = "10000000") then
				w_CHANNEL <= '0' & i_PRI_7;
			else
				w_CHANNEL <= w_NONE;
			end if;
		end if;
	end process U_CONV;	
	
----------------------------------------------------------------------------------
-- Register.
----------------------------------------------------------------------------------	
	U_NEW_REQ : process (i_CLK)
	begin
		if rising_edge (i_CLK) then
			if (i_RST = '1') then
				o_CHANNEL <= (others => '0');
				o_GRANT <= '0';
				o_EN    <= '0';
			else
				if (w_EN = '1') then
					o_CHANNEL <= w_CHANNEL;
					o_GRANT <= '1';
					o_EN    <= '1';
				else
					o_GRANT <= '0';
					o_EN    <= '0';
				end if;
			end if;
		end if;
	end process U_NEW_REQ;
	
----------------------------------------------------------------------------------
end Behavioral;

