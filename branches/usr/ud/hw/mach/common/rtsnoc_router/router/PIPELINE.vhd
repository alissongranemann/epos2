----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Marcelo Daniel Berejuck
-- 
-- Create Date:    13:36:39 11/30/2010 
-- Design Name: 
-- Module Name:    PIPELINE - Behavioral 
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

entity PIPELINE_NOC is
    Port ( i_CLK 			: in  STD_LOGIC;
           i_RST 			: in  STD_LOGIC;
           i_EN 			: in  STD_LOGIC;
           i_CHANNEL 	: in  STD_LOGIC_VECTOR (3 downto 0);
			  o_PRI_0		: out STD_LOGIC_VECTOR (2 downto 0);
			  o_PRI_1		: out STD_LOGIC_VECTOR (2 downto 0);
			  o_PRI_2		: out STD_LOGIC_VECTOR (2 downto 0);
			  o_PRI_3		: out STD_LOGIC_VECTOR (2 downto 0);
			  o_PRI_4		: out STD_LOGIC_VECTOR (2 downto 0);
			  o_PRI_5		: out STD_LOGIC_VECTOR (2 downto 0);
			  o_PRI_6		: out STD_LOGIC_VECTOR (2 downto 0);
			  o_PRI_7		: out STD_LOGIC_VECTOR (2 downto 0);			  
           o_CHANNEL 	: out STD_LOGIC_VECTOR (2 downto 0)
	 );
end PIPELINE_NOC;

architecture Behavioral of PIPELINE_NOC is
----------------------------------------------------------------------------------
-- Internal signals.
----------------------------------------------------------------------------------
	signal w_Q0			:	STD_LOGIC_VECTOR (2 downto 0);
	signal w_Q1			:	STD_LOGIC_VECTOR (2 downto 0);
	signal w_Q2			:	STD_LOGIC_VECTOR (2 downto 0);
	signal w_Q3			:	STD_LOGIC_VECTOR (2 downto 0);
	signal w_Q4			:	STD_LOGIC_VECTOR (2 downto 0);
	signal w_Q5			:	STD_LOGIC_VECTOR (2 downto 0);
	signal w_Q6			:	STD_LOGIC_VECTOR (2 downto 0);
	signal w_Q7			:	STD_LOGIC_VECTOR (2 downto 0);

	signal w_CHANNEL 	:  STD_LOGIC_VECTOR (2 downto 0);

	signal w_LOAD		:  STD_LOGIC_VECTOR (7 downto 0);
	signal w_COMP		:  STD_LOGIC_VECTOR (7 downto 0);
	
	signal w_COMP_0	:	STD_LOGIC_VECTOR (7 downto 0) := "11111111";
	signal w_COMP_1	:	STD_LOGIC_VECTOR (7 downto 0) := "11111110";
	signal w_COMP_2	:	STD_LOGIC_VECTOR (7 downto 0) := "11111100";
	signal w_COMP_3	:	STD_LOGIC_VECTOR (7 downto 0) := "11111000";
	signal w_COMP_4	:	STD_LOGIC_VECTOR (7 downto 0) := "11110000";
	signal w_COMP_5	:	STD_LOGIC_VECTOR (7 downto 0) := "11100000";
	signal w_COMP_6	:	STD_LOGIC_VECTOR (7 downto 0) := "11000000";
	signal w_COMP_7	:	STD_LOGIC_VECTOR (7 downto 0) := "10000000";
	signal w_NULL		:	STD_LOGIC_VECTOR (7 downto 0) := "00000000";
	
	signal w_ACTIVE	:  STD_LOGIC;

	
begin
----------------------------------------------------------------------------------
-- Converts the input on a valid channel number.
----------------------------------------------------------------------------------
	U_ADJ :
		for i in 0 to 2 generate
			begin
				w_CHANNEL(i) <= i_CHANNEL(i);
		end generate;
		
	
	w_ACTIVE <= i_CHANNEL(3);
	

----------------------------------------------------------------------------------
-- Compares the Priority words with the input channel to check which one is the
-- reference that will used to load the pipeline.
----------------------------------------------------------------------------------

	U0_COMPARE: entity work.COMPARE PORT MAP(
		i_ACTIVE => w_ACTIVE,
		i_DIN 	=> w_Q0,
		i_REF 	=> w_CHANNEL,
		o_DOUT 	=> w_COMP(0)
	);
	

	U1_COMPARE: entity work.COMPARE PORT MAP(
		i_ACTIVE => w_ACTIVE,
		i_DIN 	=> w_Q1,
		i_REF 	=> w_CHANNEL,
		o_DOUT 	=> w_COMP(1)
	);


	U2_COMPARE: entity work.COMPARE PORT MAP(
		i_ACTIVE => w_ACTIVE,
		i_DIN 	=> w_Q2,
		i_REF 	=> w_CHANNEL,
		o_DOUT 	=> w_COMP(2)
	);
	

	U3_COMPARE: entity work.COMPARE PORT MAP(
		i_ACTIVE => w_ACTIVE,
		i_DIN 	=> w_Q3,
		i_REF 	=> w_CHANNEL,
		o_DOUT 	=> w_COMP(3)
	);


	U4_COMPARE: entity work.COMPARE PORT MAP(
		i_ACTIVE => w_ACTIVE,
		i_DIN 	=> w_Q4,
		i_REF 	=> w_CHANNEL,
		o_DOUT 	=> w_COMP(4)
	);

	U5_COMPARE: entity work.COMPARE PORT MAP(
		i_ACTIVE => w_ACTIVE,
		i_DIN 	=> w_Q5,
		i_REF 	=> w_CHANNEL,
		o_DOUT 	=> w_COMP(5)
	);
	

	U6_COMPARE: entity work.COMPARE PORT MAP(
		i_ACTIVE => w_ACTIVE,
		i_DIN 	=> w_Q6,
		i_REF 	=> w_CHANNEL,
		o_DOUT 	=> w_COMP(6)
	);


	U7_COMPARE: entity work.COMPARE PORT MAP(
		i_ACTIVE => w_ACTIVE,
		i_DIN 	=> w_Q7,
		i_REF 	=> w_CHANNEL,
		o_DOUT 	=> w_COMP(7)
	);


	--
	-- Set the correct LOAD to the pipeline.
	--
	U_LOAD: process (w_COMP,w_COMP_0,w_COMP_1,w_COMP_2,w_COMP_3,w_COMP_4,w_COMP_5,w_COMP_6,w_COMP_7,w_NULL)
	begin
		case w_COMP is
			when "00000001" => w_LOAD <= w_COMP_0;
			when "00000010" => w_LOAD <= w_COMP_1;
			when "00000100" => w_LOAD <= w_COMP_2;
			when "00001000" => w_LOAD <= w_COMP_3;
			when "00010000" => w_LOAD <= w_COMP_4;
			when "00100000" => w_LOAD <= w_COMP_5;
			when "01000000" => w_LOAD <= w_COMP_6;
			when "10000000" => w_LOAD <= w_COMP_7;
			when others 	 => w_LOAD <= w_NULL;
		end case;
	end process U_LOAD;

----------------------------------------------------------------------------------
-- Output.
----------------------------------------------------------------------------------
  o_PRI_0		<= w_Q0;
  o_PRI_1		<= w_Q1;
  o_PRI_2		<= w_Q2;
  o_PRI_3		<= w_Q3;
  o_PRI_4		<= w_Q4;
  o_PRI_5		<= w_Q5;
  o_PRI_6		<= w_Q6;
  o_PRI_7		<= w_Q7;
  	
  o_CHANNEL <= w_CHANNEL;

	
----------------------------------------------------------------------------------
-- FLIP-FLOP T.
----------------------------------------------------------------------------------
	U_T7 : process (i_CLK)
	begin
		if falling_edge (i_CLK) then
			if (i_RST = '1') then
				w_Q7 <= "111";
			elsif ((w_LOAD(7) = '1') and (i_EN = '1')) then
				w_Q7 <= w_CHANNEL;
			end if;
		end if;
	end process U_T7;
		
----------------------------------------------------------------------------------
-- FLIP-FLOP T.
----------------------------------------------------------------------------------
	U_T6 : process (i_CLK)
	begin
		if falling_edge (i_CLK) then
			if (i_RST = '1') then
				w_Q6 <= "110";
			elsif ((w_LOAD(6) = '1') and (i_EN = '1')) then
				w_Q6 <= w_Q7;
			end if;
		end if;
	end process U_T6;
		
----------------------------------------------------------------------------------
-- FLIP-FLOP T.
----------------------------------------------------------------------------------
	U_T5 : process (i_CLK)
	begin
		if falling_edge (i_CLK) then
			if (i_RST = '1') then
				w_Q5 <= "101";
			elsif ((w_LOAD(5) = '1') and (i_EN = '1')) then
				w_Q5 <= w_Q6;
			end if;
		end if;
	end process U_T5;
	
----------------------------------------------------------------------------------
-- FLIP-FLOP T.
----------------------------------------------------------------------------------
	U_T4 : process (i_CLK)
	begin
		if falling_edge (i_CLK) then
			if (i_RST = '1') then
				w_Q4 <= "100";
			elsif ((w_LOAD(4) = '1') and (i_EN = '1')) then
				w_Q4 <= w_Q5;
			end if;
		end if;
	end process U_T4;
	
----------------------------------------------------------------------------------
-- FLIP-FLOP T.
----------------------------------------------------------------------------------
	U_T3 : process (i_CLK)
	begin
		if falling_edge (i_CLK) then
			if (i_RST = '1') then
				w_Q3 <= "011";
			elsif ((w_LOAD(3) = '1') and (i_EN = '1')) then
				w_Q3 <= w_Q4;
			end if;
		end if;
	end process U_T3;
	
----------------------------------------------------------------------------------
-- FLIP-FLOP T.
----------------------------------------------------------------------------------
	U_T2 : process (i_CLK)
	begin
		if falling_edge (i_CLK) then
			if (i_RST = '1') then
				w_Q2 <= "010";
			elsif ((w_LOAD(2) = '1') and (i_EN = '1')) then
				w_Q2 <= w_Q3;
			end if;
		end if;
	end process U_T2;
	
----------------------------------------------------------------------------------
-- FLIP-FLOP T.
----------------------------------------------------------------------------------
	U_T1 : process (i_CLK)
	begin
		if falling_edge (i_CLK) then
			if (i_RST = '1') then
				w_Q1 <= "001";
			elsif ((w_LOAD(1) = '1') and (i_EN = '1')) then
				w_Q1 <= w_Q2;
			end if;
		end if;
	end process U_T1;
	
----------------------------------------------------------------------------------
-- FLIP-FLOP T.
----------------------------------------------------------------------------------
	U_T0 : process (i_CLK)
	begin
		if falling_edge (i_CLK) then
			if (i_RST = '1') then
				w_Q0 <= "000";
			elsif ((w_LOAD(0) = '1') and (i_EN = '1')) then
				w_Q0 <= w_Q1;
			end if;
		end if;
	end process U_T0;
	
----------------------------------------------------------------------------------
end Behavioral;

