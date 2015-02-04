----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Marcelo Daniel Berejuck
-- 
-- Create Date:    07:59:48 12/02/2010 
-- Design Name: 
-- Module Name:    ROUTER - Behavioral 
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

entity ROUTER is
	 Generic (
					--
					-- This is the identification of the router on the network.
					--
					p_X			: integer := 0;
					p_Y			: integer := 0;
					--
					-- Size of  the bus data, without payload.
					--
					p_DATA		: integer := 16;					
					--
					-- Dimension of the network.
					--
					p_SIZE_X		: integer := 1;
					p_SIZE_Y		: integer := 1
	 );
    Port ( 
				o_TESTE	 	: out STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	 
				i_CLK 		: in  STD_LOGIC;
				i_RST 		: in  STD_LOGIC;
				--
				-- North.
				--
				i_DIN_NN 	: in  STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
				o_DOUT_NN 	: out STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
				i_WR_NN 		: in  STD_LOGIC;
				i_RD_NN 		: in  STD_LOGIC;
				o_WAIT_NN 	: out STD_LOGIC;
				o_ND_NN 		: out STD_LOGIC;
				--
				-- NorthEast.
				--
				i_DIN_NE 	: in  STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
				o_DOUT_NE 	: out STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
				i_WR_NE 		: in  STD_LOGIC;
				i_RD_NE 		: in  STD_LOGIC;
				o_WAIT_NE	: out STD_LOGIC;
				o_ND_NE 		: out STD_LOGIC;
				--
				-- East.
				--
				i_DIN_EE 	: in  STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
				o_DOUT_EE 	: out STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
				i_WR_EE 		: in  STD_LOGIC;
				i_RD_EE 		: in  STD_LOGIC;
				o_WAIT_EE 	: out STD_LOGIC;
				o_ND_EE 		: out STD_LOGIC;
				--
				-- SouthEast.
				--
				i_DIN_SE 	: in  STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
				o_DOUT_SE 	: out STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
				i_WR_SE 		: in  STD_LOGIC;
				i_RD_SE 		: in  STD_LOGIC;
				o_WAIT_SE 	: out STD_LOGIC;
				o_ND_SE 		: out STD_LOGIC;
				--
				-- South.
				--
				i_DIN_SS 	: in  STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
				o_DOUT_SS 	: out STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
				i_WR_SS 		: in  STD_LOGIC;
				i_RD_SS 		: in  STD_LOGIC;
				o_WAIT_SS	: out STD_LOGIC;
				o_ND_SS 		: out STD_LOGIC;
				--
				-- SouthWest.
				--
				i_DIN_SW 	: in  STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
				o_DOUT_SW 	: out STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
				i_WR_SW 		: in  STD_LOGIC;
				i_RD_SW 		: in  STD_LOGIC;
				o_WAIT_SW	: out STD_LOGIC;
				o_ND_SW 		: out STD_LOGIC;
				--
				-- West.
				--
				i_DIN_WW 	: in  STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
				o_DOUT_WW 	: out STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
				i_WR_WW 		: in  STD_LOGIC;
				i_RD_WW 		: in  STD_LOGIC;
				o_WAIT_WW	: out STD_LOGIC;
				o_ND_WW 		: out STD_LOGIC;
				--
				-- NorthWest.
				--
				i_DIN_NW 	: in  STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
				o_DOUT_NW 	: out STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
				i_WR_NW 		: in  STD_LOGIC;
				i_RD_NW 		: in  STD_LOGIC;
				o_WAIT_NW 	: out STD_LOGIC;
				o_ND_NW 		: out STD_LOGIC				
	);
end ROUTER;

architecture Behavioral of ROUTER is
----------------------------------------------------------------------------------
-- Internal signals.
----------------------------------------------------------------------------------
	
	signal w_DIN_NN 			: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_DIN_NE 			: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_DIN_EE 			: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_DIN_SE 			: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_DIN_SS 			: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_DIN_SW 			: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_DIN_WW 			: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_DIN_NW 			: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);

	signal w_DOUT_NN 			: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_DOUT_NE 			: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_DOUT_EE 			: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_DOUT_SE 			: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_DOUT_SS 			: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_DOUT_SW 			: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_DOUT_WW 			: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	signal w_DOUT_NW 			: STD_LOGIC_VECTOR (((p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6) - 1) downto 0);
	
	signal w_BUSY_NN 			: STD_LOGIC;
	signal w_BUSY_NE 			: STD_LOGIC;
	signal w_BUSY_EE 			: STD_LOGIC;
	signal w_BUSY_SE 			: STD_LOGIC;
	signal w_BUSY_SS 			: STD_LOGIC;
	signal w_BUSY_SW 			: STD_LOGIC;
	signal w_BUSY_WW 			: STD_LOGIC;
	signal w_BUSY_NW 			: STD_LOGIC;
		
	constant c_NN        	: STD_LOGIC_VECTOR(3 downto 0) := "0000"; 
	constant c_NE        	: STD_LOGIC_VECTOR(3 downto 0) := "0001"; 
	constant c_EE        	: STD_LOGIC_VECTOR(3 downto 0) := "0010"; 
	constant c_SE        	: STD_LOGIC_VECTOR(3 downto 0) := "0011"; 
	constant c_SS        	: STD_LOGIC_VECTOR(3 downto 0) := "0100"; 
	constant c_SW        	: STD_LOGIC_VECTOR(3 downto 0) := "0101"; 
	constant c_WW        	: STD_LOGIC_VECTOR(3 downto 0) := "0110"; 
	constant c_NW        	: STD_LOGIC_VECTOR(3 downto 0) := "0111"; 
	   
   signal w_REQ_NN2NN 	: STD_LOGIC;
   signal w_REQ_NN2NE 	: STD_LOGIC;
   signal w_REQ_NN2EE 	: STD_LOGIC;
   signal w_REQ_NN2SE 	: STD_LOGIC;
   signal w_REQ_NN2SS 	: STD_LOGIC;
   signal w_REQ_NN2SW 	: STD_LOGIC;
   signal w_REQ_NN2WW 	: STD_LOGIC;
   signal w_REQ_NN2NW 	: STD_LOGIC;

   signal w_REQ_NE2NN 	: STD_LOGIC;
   signal w_REQ_NE2NE 	: STD_LOGIC;
   signal w_REQ_NE2EE 	: STD_LOGIC;
   signal w_REQ_NE2SE 	: STD_LOGIC;
   signal w_REQ_NE2SS 	: STD_LOGIC;
   signal w_REQ_NE2SW 	: STD_LOGIC;
   signal w_REQ_NE2WW 	: STD_LOGIC;
   signal w_REQ_NE2NW 	: STD_LOGIC;
			  
   signal w_REQ_EE2NN 	: STD_LOGIC;
   signal w_REQ_EE2NE 	: STD_LOGIC;
   signal w_REQ_EE2EE 	: STD_LOGIC;
   signal w_REQ_EE2SE 	: STD_LOGIC;
   signal w_REQ_EE2SS 	: STD_LOGIC;
   signal w_REQ_EE2SW 	: STD_LOGIC;
   signal w_REQ_EE2WW 	: STD_LOGIC;
   signal w_REQ_EE2NW 	: STD_LOGIC;
			  
   signal w_REQ_SE2NN 	: STD_LOGIC;
   signal w_REQ_SE2NE 	: STD_LOGIC;
   signal w_REQ_SE2EE 	: STD_LOGIC;
   signal w_REQ_SE2SE 	: STD_LOGIC;
   signal w_REQ_SE2SS 	: STD_LOGIC;
   signal w_REQ_SE2SW 	: STD_LOGIC;
   signal w_REQ_SE2WW 	: STD_LOGIC;
   signal w_REQ_SE2NW 	: STD_LOGIC;

   signal w_REQ_SS2NN 	: STD_LOGIC;
   signal w_REQ_SS2NE 	: STD_LOGIC;
   signal w_REQ_SS2EE 	: STD_LOGIC;
   signal w_REQ_SS2SE 	: STD_LOGIC;
   signal w_REQ_SS2SS 	: STD_LOGIC;
   signal w_REQ_SS2SW 	: STD_LOGIC;
   signal w_REQ_SS2WW 	: STD_LOGIC;
   signal w_REQ_SS2NW 	: STD_LOGIC;

   signal w_REQ_SW2NN 	: STD_LOGIC;
   signal w_REQ_SW2NE 	: STD_LOGIC;
   signal w_REQ_SW2EE 	: STD_LOGIC;
   signal w_REQ_SW2SE 	: STD_LOGIC;
   signal w_REQ_SW2SS 	: STD_LOGIC;
   signal w_REQ_SW2SW 	: STD_LOGIC;
   signal w_REQ_SW2WW 	: STD_LOGIC;
   signal w_REQ_SW2NW 	: STD_LOGIC;

   signal w_REQ_WW2NN 	: STD_LOGIC;
   signal w_REQ_WW2NE 	: STD_LOGIC;
   signal w_REQ_WW2EE 	: STD_LOGIC;
   signal w_REQ_WW2SE 	: STD_LOGIC;
   signal w_REQ_WW2SS 	: STD_LOGIC;
   signal w_REQ_WW2SW 	: STD_LOGIC;
   signal w_REQ_WW2WW 	: STD_LOGIC;
   signal w_REQ_WW2NW 	: STD_LOGIC;

   signal w_REQ_NW2NN 	: STD_LOGIC;
   signal w_REQ_NW2NE 	: STD_LOGIC;
   signal w_REQ_NW2EE 	: STD_LOGIC;
   signal w_REQ_NW2SE 	: STD_LOGIC;
   signal w_REQ_NW2SS 	: STD_LOGIC;
   signal w_REQ_NW2SW 	: STD_LOGIC;
   signal w_REQ_NW2WW 	: STD_LOGIC;
   signal w_REQ_NW2NW 	: STD_LOGIC;
-----------
   signal w_GNT_NN2NN 	: STD_LOGIC;
   signal w_GNT_NN2NE 	: STD_LOGIC;
   signal w_GNT_NN2EE 	: STD_LOGIC;
   signal w_GNT_NN2SE 	: STD_LOGIC;
   signal w_GNT_NN2SS 	: STD_LOGIC;
   signal w_GNT_NN2SW 	: STD_LOGIC;
   signal w_GNT_NN2WW 	: STD_LOGIC;
   signal w_GNT_NN2NW 	: STD_LOGIC;

   signal w_GNT_NE2NN 	: STD_LOGIC;
   signal w_GNT_NE2NE 	: STD_LOGIC;
   signal w_GNT_NE2EE 	: STD_LOGIC;
   signal w_GNT_NE2SE 	: STD_LOGIC;
   signal w_GNT_NE2SS 	: STD_LOGIC;
   signal w_GNT_NE2SW 	: STD_LOGIC;
   signal w_GNT_NE2WW 	: STD_LOGIC;
   signal w_GNT_NE2NW 	: STD_LOGIC;
			  
   signal w_GNT_EE2NN 	: STD_LOGIC;
   signal w_GNT_EE2NE 	: STD_LOGIC;
   signal w_GNT_EE2EE 	: STD_LOGIC;
   signal w_GNT_EE2SE 	: STD_LOGIC;
   signal w_GNT_EE2SS 	: STD_LOGIC;
   signal w_GNT_EE2SW 	: STD_LOGIC;
   signal w_GNT_EE2WW 	: STD_LOGIC;
   signal w_GNT_EE2NW 	: STD_LOGIC;
			  
   signal w_GNT_SE2NN 	: STD_LOGIC;
   signal w_GNT_SE2NE 	: STD_LOGIC;
   signal w_GNT_SE2EE 	: STD_LOGIC;
   signal w_GNT_SE2SE 	: STD_LOGIC;
   signal w_GNT_SE2SS 	: STD_LOGIC;
   signal w_GNT_SE2SW 	: STD_LOGIC;
   signal w_GNT_SE2WW 	: STD_LOGIC;
   signal w_GNT_SE2NW 	: STD_LOGIC;

   signal w_GNT_SS2NN 	: STD_LOGIC;
   signal w_GNT_SS2NE 	: STD_LOGIC;
   signal w_GNT_SS2EE 	: STD_LOGIC;
   signal w_GNT_SS2SE 	: STD_LOGIC;
   signal w_GNT_SS2SS 	: STD_LOGIC;
   signal w_GNT_SS2SW 	: STD_LOGIC;
   signal w_GNT_SS2WW 	: STD_LOGIC;
   signal w_GNT_SS2NW 	: STD_LOGIC;

   signal w_GNT_SW2NN 	: STD_LOGIC;
   signal w_GNT_SW2NE 	: STD_LOGIC;
   signal w_GNT_SW2EE 	: STD_LOGIC;
   signal w_GNT_SW2SE 	: STD_LOGIC;
   signal w_GNT_SW2SS 	: STD_LOGIC;
   signal w_GNT_SW2SW 	: STD_LOGIC;
   signal w_GNT_SW2WW 	: STD_LOGIC;
   signal w_GNT_SW2NW 	: STD_LOGIC;

   signal w_GNT_WW2NN 	: STD_LOGIC;
   signal w_GNT_WW2NE 	: STD_LOGIC;
   signal w_GNT_WW2EE 	: STD_LOGIC;
   signal w_GNT_WW2SE 	: STD_LOGIC;
   signal w_GNT_WW2SS 	: STD_LOGIC;
   signal w_GNT_WW2SW 	: STD_LOGIC;
   signal w_GNT_WW2WW 	: STD_LOGIC;
   signal w_GNT_WW2NW 	: STD_LOGIC;

   signal w_GNT_NW2NN 	: STD_LOGIC;
   signal w_GNT_NW2NE 	: STD_LOGIC;
   signal w_GNT_NW2EE 	: STD_LOGIC;
   signal w_GNT_NW2SE 	: STD_LOGIC;
   signal w_GNT_NW2SS 	: STD_LOGIC;
   signal w_GNT_NW2SW 	: STD_LOGIC;
   signal w_GNT_NW2WW 	: STD_LOGIC;
   signal w_GNT_NW2NW 	: STD_LOGIC;
	
   signal w_EXCHANGE_NN : STD_LOGIC;
   signal w_CMD_SWT_NN  : STD_LOGIC_VECTOR (3 downto 0) := "1000";
   
   signal w_EXCHANGE_NE : STD_LOGIC;
   signal w_CMD_SWT_NE  : STD_LOGIC_VECTOR (3 downto 0) := "1000";

   signal w_EXCHANGE_EE :  STD_LOGIC;
   signal w_CMD_SWT_EE  :  STD_LOGIC_VECTOR (3 downto 0) := "1000";

   signal w_EXCHANGE_SE : STD_LOGIC;
   signal w_CMD_SWT_SE  : STD_LOGIC_VECTOR (3 downto 0) := "1000";

   signal w_EXCHANGE_SS : STD_LOGIC;
   signal w_CMD_SWT_SS  : STD_LOGIC_VECTOR (3 downto 0) := "1000";

   signal w_EXCHANGE_SW : STD_LOGIC;
   signal w_CMD_SWT_SW  : STD_LOGIC_VECTOR (3 downto 0) := "1000";

   signal w_EXCHANGE_WW : STD_LOGIC;
   signal w_CMD_SWT_WW  : STD_LOGIC_VECTOR (3 downto 0) := "1000";

   signal w_EXCHANGE_NW : STD_LOGIC;
   signal w_CMD_SWT_NW  : STD_LOGIC_VECTOR (3 downto 0) := "1000";
	
	signal w_CTR_NN		: STD_LOGIC_VECTOR (2 DOWNTO 0) := "000";
	signal w_CTR_NE		: STD_LOGIC_VECTOR (2 DOWNTO 0) := "001";
	signal w_CTR_EE		: STD_LOGIC_VECTOR (2 DOWNTO 0) := "010";
	signal w_CTR_SE		: STD_LOGIC_VECTOR (2 DOWNTO 0) := "011";
	signal w_CTR_SS		: STD_LOGIC_VECTOR (2 DOWNTO 0) := "100";
	signal w_CTR_SW		: STD_LOGIC_VECTOR (2 DOWNTO 0) := "101";
	signal w_CTR_WW		: STD_LOGIC_VECTOR (2 DOWNTO 0) := "110";
	signal w_CTR_NW		: STD_LOGIC_VECTOR (2 DOWNTO 0) := "111";

   signal w_CLEAR_NN    : STD_LOGIC;
   signal w_CLEAR_NE    : STD_LOGIC;
   signal w_CLEAR_EE    : STD_LOGIC;
   signal w_CLEAR_SE    : STD_LOGIC;
   signal w_CLEAR_SS    : STD_LOGIC;
   signal w_CLEAR_SW    : STD_LOGIC;
   signal w_CLEAR_WW    : STD_LOGIC;
   signal w_CLEAR_NW    : STD_LOGIC;
	
   signal w_GNT_NN   	: STD_LOGIC;
   signal w_GNT_NE   	: STD_LOGIC;
   signal w_GNT_EE   	: STD_LOGIC;
   signal w_GNT_SE   	: STD_LOGIC;
   signal w_GNT_SS   	: STD_LOGIC;
   signal w_GNT_SW   	: STD_LOGIC;
   signal w_GNT_WW   	: STD_LOGIC;
   signal w_GNT_NW   	: STD_LOGIC;

	signal w_DST_NN		: STD_LOGIC_VECTOR ( ((p_SIZE_X+p_SIZE_Y + 3) - 1) downto 0);
	signal w_DST_NE		: STD_LOGIC_VECTOR ( ((p_SIZE_X+p_SIZE_Y + 3) - 1) downto 0);
	signal w_DST_EE		: STD_LOGIC_VECTOR ( ((p_SIZE_X+p_SIZE_Y + 3) - 1) downto 0);
	signal w_DST_SE		: STD_LOGIC_VECTOR ( ((p_SIZE_X+p_SIZE_Y + 3) - 1) downto 0);
	signal w_DST_SS		: STD_LOGIC_VECTOR ( ((p_SIZE_X+p_SIZE_Y + 3) - 1) downto 0);
	signal w_DST_SW		: STD_LOGIC_VECTOR ( ((p_SIZE_X+p_SIZE_Y + 3) - 1) downto 0);
	signal w_DST_WW		: STD_LOGIC_VECTOR ( ((p_SIZE_X+p_SIZE_Y + 3) - 1) downto 0);
	signal w_DST_NW		: STD_LOGIC_VECTOR ( ((p_SIZE_X+p_SIZE_Y + 3) - 1) downto 0);
	
   signal w_SEQUENCY_NN : STD_LOGIC_VECTOR (7 DOWNTO 0);
   signal w_SEQUENCY_NE : STD_LOGIC_VECTOR (7 DOWNTO 0);
   signal w_SEQUENCY_EE : STD_LOGIC_VECTOR (7 DOWNTO 0);
   signal w_SEQUENCY_SE : STD_LOGIC_VECTOR (7 DOWNTO 0);
   signal w_SEQUENCY_SS : STD_LOGIC_VECTOR (7 DOWNTO 0);
   signal w_SEQUENCY_SW : STD_LOGIC_VECTOR (7 DOWNTO 0);
   signal w_SEQUENCY_WW : STD_LOGIC_VECTOR (7 DOWNTO 0);
   signal w_SEQUENCY_NW : STD_LOGIC_VECTOR (7 DOWNTO 0);
   
begin
----------------------------------------------------------------------------------
-- DEBUG!!!
----------------------------------------------------------------------------------
	o_TESTE(0) <= w_GNT_NE2NN;
--	o_TESTE(1) <= w_DOUT_NW(1);
--	o_TESTE(2) <= w_DOUT_NW(2);
--	o_TESTE(3) <= w_DOUT_NW(3);
--	o_TESTE(34) <= w_REQ_NN2SE;
--	o_TESTE(35) <= w_CTR_SE(0);
--	o_TESTE(36) <= w_CTR_SE(1);
--	o_TESTE(37) <= w_CTR_SE(2);
	
--	o_TESTE <= w_DOUT_NW;
	

----------------------------------------------------------------------------------
-- NORTH.
----------------------------------------------------------------------------------
	--
	-- Imput buffer.
	--
	UNN_INPUT_INTERFACE: entity work.INPUT_INTERFACE 
   GENERIC MAP(
		p_DATA		=> p_DATA,
		p_SIZE_X		=> p_SIZE_X,
		p_SIZE_Y		=> p_SIZE_Y
	)
	PORT MAP(
		i_CLK 	=> i_CLK,
		i_RST 	=> i_RST,
		i_DIN 	=> i_DIN_NN,
		i_WR 		=> i_WR_NN,
		o_DST 	=> w_DST_NN
	);

	--
	-- Flow controller.
	--
	UNN_FLOW_CONTROL: entity work.FLOW_CONTROL 
   GENERIC MAP(
		p_SIZE_X		=> p_SIZE_X,
		p_SIZE_Y		=> p_SIZE_Y,
		p_X			=> p_X,
		p_Y			=> p_Y
	)
	PORT MAP(
		i_CLK 		=> i_CLK,
		i_RST 		=> i_RST,
		i_SEND 		=> i_WR_NN,
		o_WAIT 		=> o_WAIT_NN,
		i_GRANT		=> w_CTR_NN,
		i_BSY_NN 	=> w_BUSY_NN,
		i_BSY_NE 	=> w_BUSY_NE,
		i_BSY_EE 	=> w_BUSY_EE,
		i_BSY_SE 	=> w_BUSY_SE,
		i_BSY_SS 	=> w_BUSY_SS,
		i_BSY_SW 	=> w_BUSY_SW,
		i_BSY_WW 	=> w_BUSY_WW,
		i_BSY_NW 	=> w_BUSY_NW,
		i_DST 		=> w_DST_NN,
		i_RD_NN 		=> i_RD_NN,
		i_RD_NE 		=> i_RD_NE,
		i_RD_EE 		=> i_RD_EE,
		i_RD_SE 		=> i_RD_SE,
		i_RD_SS 		=> i_RD_SS,
		i_RD_SW 		=> i_RD_SW,
		i_RD_WW 		=> i_RD_WW,
		i_RD_NW 		=> i_RD_NW,

      i_GNT_NN    => w_GNT_NN2NN,
      i_GNT_NE    => w_GNT_NE2NN,
      i_GNT_EE    => w_GNT_EE2NN,
      i_GNT_SE    => w_GNT_SE2NN,
      i_GNT_SS    => w_GNT_SS2NN,
      i_GNT_SW    => w_GNT_SW2NN,
      i_GNT_WW    => w_GNT_WW2NN,
      i_GNT_NW    => w_GNT_NW2NN,

		o_REQ_NN 	=> w_REQ_NN2NN,
		o_REQ_NE 	=> w_REQ_NN2NE,
		o_REQ_EE 	=> w_REQ_NN2EE,
		o_REQ_SE 	=> w_REQ_NN2SE,
		o_REQ_SS 	=> w_REQ_NN2SS,
		o_REQ_SW 	=> w_REQ_NN2SW,
		o_REQ_WW 	=> w_REQ_NN2WW,
		o_REQ_NW 	=> w_REQ_NN2NW
	);

	
	U_MUX_NN : entity work.CROSSBAR 
   GENERIC MAP(
					p_DATA		=> p_DATA,
					p_SIZE_X		=> p_SIZE_X,
					p_SIZE_Y		=> p_SIZE_Y
	)
	PORT MAP(
					i_GNT 		=> w_CTR_NN,
					i_NORTH 		=> i_DIN_NN,
					i_NORTHEAST => i_DIN_NE,
					i_EAST 		=> i_DIN_EE,
					i_SOUTHEAST => i_DIN_SE,
					i_SOUTH 		=> i_DIN_SS,
					i_SOUTHWEST => i_DIN_SW,
					i_WEST 		=> i_DIN_WW,
					i_NORTHWEST => i_DIN_NW,
					o_DOUT 		=> w_DOUT_NN
	);

----------------------------------------------------------------------------------
-- NORTHEAST.
----------------------------------------------------------------------------------
	--
	-- Imput buffer.
	--
	UNE_INPUT_INTERFACE: entity work.INPUT_INTERFACE 
   GENERIC MAP(
		p_DATA		=> p_DATA,
		p_SIZE_X		=> p_SIZE_X,
		p_SIZE_Y		=> p_SIZE_Y
	)
	PORT MAP(
		i_CLK 	=> i_CLK,
		i_RST 	=> i_RST,
		i_DIN 	=> i_DIN_NE,
		i_WR 		=> i_WR_NE,
		o_DST 	=> w_DST_NE
	);

	--
	-- Flow controller.
	--
	UNE_FLOW_CONTROL: entity work.FLOW_CONTROL 
   GENERIC MAP(
		p_SIZE_X		=> p_SIZE_X,
		p_SIZE_Y		=> p_SIZE_Y,
		p_X			=> p_X,
		p_Y			=> p_Y
	)
	PORT MAP(
		i_CLK 		=> i_CLK,
		i_RST 		=> i_RST,
		i_SEND 		=> i_WR_NE,
		o_WAIT		=> o_WAIT_NE,
		i_GRANT		=> w_CTR_NE,
		i_BSY_NN 	=> w_BUSY_NN,
		i_BSY_NE 	=> w_BUSY_NE,
		i_BSY_EE 	=> w_BUSY_EE,
		i_BSY_SE 	=> w_BUSY_SE,
		i_BSY_SS 	=> w_BUSY_SS,
		i_BSY_SW 	=> w_BUSY_SW,
		i_BSY_WW 	=> w_BUSY_WW,
		i_BSY_NW 	=> w_BUSY_NW,
		i_DST 		=> w_DST_NE,
		i_RD_NN 		=> i_RD_NN,
		i_RD_NE 		=> i_RD_NE,
		i_RD_EE 		=> i_RD_EE,
		i_RD_SE 		=> i_RD_SE,
		i_RD_SS 		=> i_RD_SS,
		i_RD_SW 		=> i_RD_SW,
		i_RD_WW 		=> i_RD_WW,
		i_RD_NW 		=> i_RD_NW,

      i_GNT_NN    => w_GNT_NN2NE,
      i_GNT_NE    => w_GNT_NE2NE,
      i_GNT_EE    => w_GNT_EE2NE,
      i_GNT_SE    => w_GNT_SE2NE,
      i_GNT_SS    => w_GNT_SS2NE,
      i_GNT_SW    => w_GNT_SW2NE,
      i_GNT_WW    => w_GNT_WW2NE,
      i_GNT_NW    => w_GNT_NW2NE,

		o_REQ_NN 	=> w_REQ_NE2NN,
		o_REQ_NE 	=> w_REQ_NE2NE,
		o_REQ_EE 	=> w_REQ_NE2EE,
		o_REQ_SE 	=> w_REQ_NE2SE,
		o_REQ_SS 	=> w_REQ_NE2SS,
		o_REQ_SW 	=> w_REQ_NE2SW,
		o_REQ_WW 	=> w_REQ_NE2WW,
		o_REQ_NW 	=> w_REQ_NE2NW
	);

	U_MUX_NE : entity work.CROSSBAR 
   GENERIC MAP(
					p_DATA		=> p_DATA,
					p_SIZE_X		=> p_SIZE_X,
					p_SIZE_Y		=> p_SIZE_Y
	)
	PORT MAP(
					i_GNT 		=> w_CTR_NE,
					i_NORTH 		=> i_DIN_NN,
					i_NORTHEAST => i_DIN_NE,
					i_EAST 		=> i_DIN_EE,
					i_SOUTHEAST => i_DIN_SE,
					i_SOUTH 		=> i_DIN_SS,
					i_SOUTHWEST => i_DIN_SW,
					i_WEST 		=> i_DIN_WW,
					i_NORTHWEST => i_DIN_NW,
					o_DOUT 		=> w_DOUT_NE
	);


----------------------------------------------------------------------------------
-- EAST.
----------------------------------------------------------------------------------
	--
	-- Imput buffer.
	--
	UEE_INPUT_INTERFACE: entity work.INPUT_INTERFACE 
   GENERIC MAP(
		p_DATA		=> p_DATA,
		p_SIZE_X		=> p_SIZE_X,
		p_SIZE_Y		=> p_SIZE_Y
	)
	PORT MAP(
		i_CLK 	=> i_CLK,
		i_RST 	=> i_RST,
		i_DIN 	=> i_DIN_EE,
		i_WR 		=> i_WR_EE,
		o_DST 	=> w_DST_EE
	);

	--
	-- Flow controller.
	--
	UEE_FLOW_CONTROL: entity work.FLOW_CONTROL 
   GENERIC MAP(
		p_SIZE_X		=> p_SIZE_X,
		p_SIZE_Y		=> p_SIZE_Y,
		p_X			=> p_X,
		p_Y			=> p_Y
	)
	PORT MAP(
		i_CLK 		=> i_CLK,
		i_RST 		=> i_RST,
		i_SEND 		=> i_WR_EE,
		o_WAIT		=> o_WAIT_EE,
		i_GRANT		=> w_CTR_EE,
		i_BSY_NN 	=> w_BUSY_NN,
		i_BSY_NE 	=> w_BUSY_NE,
		i_BSY_EE 	=> w_BUSY_EE,
		i_BSY_SE 	=> w_BUSY_SE,
		i_BSY_SS 	=> w_BUSY_SS,
		i_BSY_SW 	=> w_BUSY_SW,
		i_BSY_WW 	=> w_BUSY_WW,
		i_BSY_NW 	=> w_BUSY_NW,
		i_DST 		=> w_DST_EE,
		i_RD_NN 		=> i_RD_NN,
		i_RD_NE 		=> i_RD_NE,
		i_RD_EE 		=> i_RD_EE,
		i_RD_SE 		=> i_RD_SE,
		i_RD_SS 		=> i_RD_SS,
		i_RD_SW 		=> i_RD_SW,
		i_RD_WW 		=> i_RD_WW,
		i_RD_NW 		=> i_RD_NW,

      i_GNT_NN    => w_GNT_NN2EE,
      i_GNT_NE    => w_GNT_NE2EE,
      i_GNT_EE    => w_GNT_EE2EE,
      i_GNT_SE    => w_GNT_SE2EE,
      i_GNT_SS    => w_GNT_SS2EE,
      i_GNT_SW    => w_GNT_SW2EE,
      i_GNT_WW    => w_GNT_WW2EE,
      i_GNT_NW    => w_GNT_NW2EE,

		o_REQ_NN 	=> w_REQ_EE2NN,
		o_REQ_NE 	=> w_REQ_EE2NE,
		o_REQ_EE 	=> w_REQ_EE2EE,
		o_REQ_SE 	=> w_REQ_EE2SE,
		o_REQ_SS 	=> w_REQ_EE2SS,
		o_REQ_SW 	=> w_REQ_EE2SW,
		o_REQ_WW 	=> w_REQ_EE2WW,
		o_REQ_NW 	=> w_REQ_EE2NW
		
	);

	U_MUX_EE : entity work.CROSSBAR 
   GENERIC MAP(
					p_DATA		=> p_DATA,
					p_SIZE_X		=> p_SIZE_X,
					p_SIZE_Y		=> p_SIZE_Y
	)
	PORT MAP(
					i_GNT 		=> w_CTR_EE,
					i_NORTH 		=> i_DIN_NN,
					i_NORTHEAST => i_DIN_NE,
					i_EAST 		=> i_DIN_EE,
					i_SOUTHEAST => i_DIN_SE,
					i_SOUTH 		=> i_DIN_SS,
					i_SOUTHWEST => i_DIN_SW,
					i_WEST 		=> i_DIN_WW,
					i_NORTHWEST => i_DIN_NW,
					o_DOUT 		=> w_DOUT_EE
	);


----------------------------------------------------------------------------------
-- SOUTHEAST.
----------------------------------------------------------------------------------
	--
	-- Imput buffer.
	--
	USE_INPUT_INTERFACE: entity work.INPUT_INTERFACE 
   GENERIC MAP(
		p_DATA		=> p_DATA,
		p_SIZE_X		=> p_SIZE_X,
		p_SIZE_Y		=> p_SIZE_Y
	)
	PORT MAP(
		i_CLK 	=> i_CLK,
		i_RST 	=> i_RST,
		i_DIN 	=> i_DIN_SE,
		i_WR 		=> i_WR_SE,
		o_DST 	=> w_DST_SE
	);

	--
	-- Flow controller.
	--
	USE_FLOW_CONTROL: entity work.FLOW_CONTROL 
   GENERIC MAP(
		p_SIZE_X		=> p_SIZE_X,
		p_SIZE_Y		=> p_SIZE_Y,
		p_X			=> p_X,
		p_Y			=> p_Y
	)
	PORT MAP(
		i_CLK 		=> i_CLK,
		i_RST 		=> i_RST,
		i_SEND 		=> i_WR_SE,
		o_WAIT		=> o_WAIT_SE,
		i_GRANT		=> w_CTR_SE,
		i_BSY_NN 	=> w_BUSY_NN,
		i_BSY_NE 	=> w_BUSY_NE,
		i_BSY_EE 	=> w_BUSY_EE,
		i_BSY_SE 	=> w_BUSY_SE,
		i_BSY_SS 	=> w_BUSY_SS,
		i_BSY_SW 	=> w_BUSY_SW,
		i_BSY_WW 	=> w_BUSY_WW,
		i_BSY_NW 	=> w_BUSY_NW,
		i_DST 		=> w_DST_SE,
		i_RD_NN 		=> i_RD_NN,
		i_RD_NE 		=> i_RD_NE,
		i_RD_EE 		=> i_RD_EE,
		i_RD_SE 		=> i_RD_SE,
		i_RD_SS 		=> i_RD_SS,
		i_RD_SW 		=> i_RD_SW,
		i_RD_WW 		=> i_RD_WW,
		i_RD_NW 		=> i_RD_NW,

      i_GNT_NN    => w_GNT_NN2SE,
      i_GNT_NE    => w_GNT_NE2SE,
      i_GNT_EE    => w_GNT_EE2SE,
      i_GNT_SE    => w_GNT_SE2SE,
      i_GNT_SS    => w_GNT_SS2SE,
      i_GNT_SW    => w_GNT_SW2SE,
      i_GNT_WW    => w_GNT_WW2SE,
      i_GNT_NW    => w_GNT_NW2SE,

		o_REQ_NN 	=> w_REQ_SE2NN,
		o_REQ_NE 	=> w_REQ_SE2NE,
		o_REQ_EE 	=> w_REQ_SE2EE,
		o_REQ_SE 	=> w_REQ_SE2SE,
		o_REQ_SS 	=> w_REQ_SE2SS,
		o_REQ_SW 	=> w_REQ_SE2SW,
		o_REQ_WW 	=> w_REQ_SE2WW,
		o_REQ_NW 	=> w_REQ_SE2NW
	);

	U_MUX_SE : entity work.CROSSBAR 
   GENERIC MAP(
					p_DATA		=> p_DATA,
					p_SIZE_X		=> p_SIZE_X,
					p_SIZE_Y		=> p_SIZE_Y
	)
	PORT MAP(
					i_GNT 		=> w_CTR_SE,
					i_NORTH 		=> i_DIN_NN,
					i_NORTHEAST => i_DIN_NE,
					i_EAST 		=> i_DIN_EE,
					i_SOUTHEAST => i_DIN_SE,
					i_SOUTH 		=> i_DIN_SS,
					i_SOUTHWEST => i_DIN_SW,
					i_WEST 		=> i_DIN_WW,
					i_NORTHWEST => i_DIN_NW,
					o_DOUT 		=> w_DOUT_SE
	);


----------------------------------------------------------------------------------
-- SOUTH.
----------------------------------------------------------------------------------
	--
	-- Imput buffer.
	--
	USS_INPUT_INTERFACE: entity work.INPUT_INTERFACE 
   GENERIC MAP(
		p_DATA		=> p_DATA,
		p_SIZE_X		=> p_SIZE_X,
		p_SIZE_Y		=> p_SIZE_Y
	)
	PORT MAP(
		i_CLK 	=> i_CLK,
		i_RST 	=> i_RST,
		i_DIN 	=> i_DIN_SS,
		i_WR 		=> i_WR_SS,
		o_DST 	=> w_DST_SS
	);

	--
	-- Flow controller.
	--
	USS_FLOW_CONTROL: entity work.FLOW_CONTROL 
   GENERIC MAP(
		p_SIZE_X		=> p_SIZE_X,
		p_SIZE_Y		=> p_SIZE_Y,
		p_X			=> p_X,
		p_Y			=> p_Y
	)
	PORT MAP(
		i_CLK 		=> i_CLK,
		i_RST 		=> i_RST,
		i_SEND 		=> i_WR_SS,
		o_WAIT		=> o_WAIT_SS,
		i_GRANT		=> w_CTR_SS,
		i_BSY_NN 	=> w_BUSY_NN,
		i_BSY_NE 	=> w_BUSY_NE,
		i_BSY_EE 	=> w_BUSY_EE,
		i_BSY_SE 	=> w_BUSY_SE,
		i_BSY_SS 	=> w_BUSY_SS,
		i_BSY_SW 	=> w_BUSY_SW,
		i_BSY_WW 	=> w_BUSY_WW,
		i_BSY_NW 	=> w_BUSY_NW,
		i_DST 		=> w_DST_SS,
		i_RD_NN 		=> i_RD_NN,
		i_RD_NE 		=> i_RD_NE,
		i_RD_EE 		=> i_RD_EE,
		i_RD_SE 		=> i_RD_SE,
		i_RD_SS 		=> i_RD_SS,
		i_RD_SW 		=> i_RD_SW,
		i_RD_WW 		=> i_RD_WW,
		i_RD_NW 		=> i_RD_NW,

      i_GNT_NN    => w_GNT_NN2SS,
      i_GNT_NE    => w_GNT_NE2SS,
      i_GNT_EE    => w_GNT_EE2SS,
      i_GNT_SE    => w_GNT_SE2SS,
      i_GNT_SS    => w_GNT_SS2SS,
      i_GNT_SW    => w_GNT_SW2SS,
      i_GNT_WW    => w_GNT_WW2SS,
      i_GNT_NW    => w_GNT_NW2SS,

		o_REQ_NN 	=> w_REQ_SS2NN,
		o_REQ_NE 	=> w_REQ_SS2NE,
		o_REQ_EE 	=> w_REQ_SS2EE,
		o_REQ_SE 	=> w_REQ_SS2SE,
		o_REQ_SS 	=> w_REQ_SS2SS,
		o_REQ_SW 	=> w_REQ_SS2SW,
		o_REQ_WW 	=> w_REQ_SS2WW,
		o_REQ_NW 	=> w_REQ_SS2NW
	);

	U_MUX_SS : entity work.CROSSBAR 
   GENERIC MAP(
					p_DATA		=> p_DATA,
					p_SIZE_X		=> p_SIZE_X,
					p_SIZE_Y		=> p_SIZE_Y
	)
	PORT MAP(
					i_GNT 		=> w_CTR_SS,
					i_NORTH 		=> i_DIN_NN,
					i_NORTHEAST => i_DIN_NE,
					i_EAST 		=> i_DIN_EE,
					i_SOUTHEAST => i_DIN_SE,
					i_SOUTH 		=> i_DIN_SS,
					i_SOUTHWEST => i_DIN_SW,
					i_WEST 		=> i_DIN_WW,
					i_NORTHWEST => i_DIN_NW,
					o_DOUT 		=> w_DOUT_SS
	);


----------------------------------------------------------------------------------
-- SOUTHWEST.
----------------------------------------------------------------------------------
	--
	-- Imput buffer.
	--
	USW_INPUT_INTERFACE: entity work.INPUT_INTERFACE 
   GENERIC MAP(
		p_DATA		=> p_DATA,
		p_SIZE_X		=> p_SIZE_X,
		p_SIZE_Y		=> p_SIZE_Y
	)
	PORT MAP(
		i_CLK 	=> i_CLK,
		i_RST 	=> i_RST,
		i_DIN 	=> i_DIN_SW,
		i_WR 		=> i_WR_SW,
		o_DST 	=> w_DST_SW
	);

	--
	-- Flow controller.
	--
	USW_FLOW_CONTROL: entity work.FLOW_CONTROL 
   GENERIC MAP(
		p_SIZE_X		=> p_SIZE_X,
		p_SIZE_Y		=> p_SIZE_Y,
		p_X			=> p_X,
		p_Y			=> p_Y
	)
	PORT MAP(
		i_CLK 		=> i_CLK,
		i_RST 		=> i_RST,
		i_SEND 		=> i_WR_SW,
		o_WAIT		=> o_WAIT_SW,
		i_GRANT		=> w_CTR_SW,
		i_BSY_NN 	=> w_BUSY_NN,
		i_BSY_NE 	=> w_BUSY_NE,
		i_BSY_EE 	=> w_BUSY_EE,
		i_BSY_SE 	=> w_BUSY_SE,
		i_BSY_SS 	=> w_BUSY_SS,
		i_BSY_SW 	=> w_BUSY_SW,
		i_BSY_WW 	=> w_BUSY_WW,
		i_BSY_NW 	=> w_BUSY_NW,
		i_DST 		=> w_DST_SW,
		i_RD_NN 		=> i_RD_NN,
		i_RD_NE 		=> i_RD_NE,
		i_RD_EE 		=> i_RD_EE,
		i_RD_SE 		=> i_RD_SE,
		i_RD_SS 		=> i_RD_SS,
		i_RD_SW 		=> i_RD_SW,
		i_RD_WW 		=> i_RD_WW,
		i_RD_NW 		=> i_RD_NW,

      i_GNT_NN    => w_GNT_NN2SW,
      i_GNT_NE    => w_GNT_NE2SW,
      i_GNT_EE    => w_GNT_EE2SW,
      i_GNT_SE    => w_GNT_SE2SW,
      i_GNT_SS    => w_GNT_SS2SW,
      i_GNT_SW    => w_GNT_SW2SW,
      i_GNT_WW    => w_GNT_WW2SW,
      i_GNT_NW    => w_GNT_NW2SW,

		o_REQ_NN 	=> w_REQ_SW2NN,
		o_REQ_NE 	=> w_REQ_SW2NE,
		o_REQ_EE 	=> w_REQ_SW2EE,
		o_REQ_SE 	=> w_REQ_SW2SE,
		o_REQ_SS 	=> w_REQ_SW2SS,
		o_REQ_SW 	=> w_REQ_SW2SW,
		o_REQ_WW 	=> w_REQ_SW2WW,
		o_REQ_NW 	=> w_REQ_SW2NW
	);

	U_MUX_SW : entity work.CROSSBAR 
   GENERIC MAP(
					p_DATA		=> p_DATA,
					p_SIZE_X		=> p_SIZE_X,
					p_SIZE_Y		=> p_SIZE_Y
	)
	PORT MAP(
					i_GNT 		=> w_CTR_SW,
					i_NORTH 		=> i_DIN_NN,
					i_NORTHEAST => i_DIN_NE,
					i_EAST 		=> i_DIN_EE,
					i_SOUTHEAST => i_DIN_SE,
					i_SOUTH 		=> i_DIN_SS,
					i_SOUTHWEST => i_DIN_SW,
					i_WEST 		=> i_DIN_WW,
					i_NORTHWEST => i_DIN_NW,
					o_DOUT 		=> w_DOUT_SW
	);


----------------------------------------------------------------------------------
-- WEST.
----------------------------------------------------------------------------------
	--
	-- Imput buffer.
	--
	UWW_INPUT_INTERFACE: entity work.INPUT_INTERFACE 
   GENERIC MAP(
		p_DATA		=> p_DATA,
		p_SIZE_X		=> p_SIZE_X,
		p_SIZE_Y		=> p_SIZE_Y
	)
	PORT MAP(
		i_CLK 	=> i_CLK,
		i_RST 	=> i_RST,
		i_DIN 	=> i_DIN_WW,
		i_WR 		=> i_WR_WW,
		o_DST 	=> w_DST_WW
	);

	--
	-- Flow controller.
	--
	UWW_FLOW_CONTROL: entity work.FLOW_CONTROL 
   GENERIC MAP(
		p_SIZE_X		=> p_SIZE_X,
		p_SIZE_Y		=> p_SIZE_Y,
		p_X			=> p_X,
		p_Y			=> p_Y
	)
	PORT MAP(
		i_CLK 		=> i_CLK,
		i_RST 		=> i_RST,
		i_SEND 		=> i_WR_WW,
		o_WAIT		=> o_WAIT_WW,
		i_GRANT		=> w_CTR_WW,
		i_BSY_NN 	=> w_BUSY_NN,
		i_BSY_NE 	=> w_BUSY_NE,
		i_BSY_EE 	=> w_BUSY_EE,
		i_BSY_SE 	=> w_BUSY_SE,
		i_BSY_SS 	=> w_BUSY_SS,
		i_BSY_SW 	=> w_BUSY_SW,
		i_BSY_WW 	=> w_BUSY_WW,
		i_BSY_NW 	=> w_BUSY_NW,
		i_DST 		=> w_DST_WW,
		i_RD_NN 		=> i_RD_NN,
		i_RD_NE 		=> i_RD_NE,
		i_RD_EE 		=> i_RD_EE,
		i_RD_SE 		=> i_RD_SE,
		i_RD_SS 		=> i_RD_SS,
		i_RD_SW 		=> i_RD_SW,
		i_RD_WW 		=> i_RD_WW,
		i_RD_NW 		=> i_RD_NW,

      i_GNT_NN    => w_GNT_NN2WW,
      i_GNT_NE    => w_GNT_NE2WW,
      i_GNT_EE    => w_GNT_EE2WW,
      i_GNT_SE    => w_GNT_SE2WW,
      i_GNT_SS    => w_GNT_SS2WW,
      i_GNT_SW    => w_GNT_SW2WW,
      i_GNT_WW    => w_GNT_WW2WW,
      i_GNT_NW    => w_GNT_NW2WW,

		o_REQ_NN 	=> w_REQ_WW2NN,
		o_REQ_NE 	=> w_REQ_WW2NE,
		o_REQ_EE 	=> w_REQ_WW2EE,
		o_REQ_SE 	=> w_REQ_WW2SE,
		o_REQ_SS 	=> w_REQ_WW2SS,
		o_REQ_SW 	=> w_REQ_WW2SW,
		o_REQ_WW 	=> w_REQ_WW2WW,
		o_REQ_NW 	=> w_REQ_WW2NW
	);

	U_MUX_WW : entity work.CROSSBAR 
   GENERIC MAP(
					p_DATA		=> p_DATA,
					p_SIZE_X		=> p_SIZE_X,
					p_SIZE_Y		=> p_SIZE_Y
	)
	PORT MAP(
					i_GNT 		=> w_CTR_WW,
					i_NORTH 		=> i_DIN_NN,
					i_NORTHEAST => i_DIN_NE,
					i_EAST 		=> i_DIN_EE,
					i_SOUTHEAST => i_DIN_SE,
					i_SOUTH 		=> i_DIN_SS,
					i_SOUTHWEST => i_DIN_SW,
					i_WEST 		=> i_DIN_WW,
					i_NORTHWEST => i_DIN_NW,
					o_DOUT 		=> w_DOUT_WW
	);


----------------------------------------------------------------------------------
-- NORTHWEST.
----------------------------------------------------------------------------------
	--
	-- Imput buffer.
	--
	UNW_INPUT_INTERFACE: entity work.INPUT_INTERFACE 
   GENERIC MAP(
		p_DATA		=> p_DATA,
		p_SIZE_X		=> p_SIZE_X,
		p_SIZE_Y		=> p_SIZE_Y
	)
	PORT MAP(
		i_CLK 	=> i_CLK,
		i_RST 	=> i_RST,
		i_DIN 	=> i_DIN_NW,
		i_WR 		=> i_WR_NW,
		o_DST 	=> w_DST_NW
	);

	--
	-- Flow controller.
	--
	UNW_FLOW_CONTROL: entity work.FLOW_CONTROL 
   GENERIC MAP(
		p_SIZE_X		=> p_SIZE_X,
		p_SIZE_Y		=> p_SIZE_Y,
		p_X			=> p_X,
		p_Y			=> p_Y
	)
	PORT MAP(
		i_CLK 		=> i_CLK,
		i_RST 		=> i_RST,
		i_SEND 		=> i_WR_NW,
		o_WAIT		=> o_WAIT_NW,
		i_GRANT		=> w_CTR_NW,
		i_BSY_NN 	=> w_BUSY_NN,
		i_BSY_NE 	=> w_BUSY_NE,
		i_BSY_EE 	=> w_BUSY_EE,
		i_BSY_SE 	=> w_BUSY_SE,
		i_BSY_SS 	=> w_BUSY_SS,
		i_BSY_SW 	=> w_BUSY_SW,
		i_BSY_WW 	=> w_BUSY_WW,
		i_BSY_NW 	=> w_BUSY_NW,
		i_DST 		=> w_DST_NW,
		i_RD_NN 		=> i_RD_NN,
		i_RD_NE 		=> i_RD_NE,
		i_RD_EE 		=> i_RD_EE,
		i_RD_SE 		=> i_RD_SE,
		i_RD_SS 		=> i_RD_SS,
		i_RD_SW 		=> i_RD_SW,
		i_RD_WW 		=> i_RD_WW,
		i_RD_NW 		=> i_RD_NW,

      i_GNT_NN    => w_GNT_NN2NW,
      i_GNT_NE    => w_GNT_NE2NW,
      i_GNT_EE    => w_GNT_EE2NW,
      i_GNT_SE    => w_GNT_SE2NW,
      i_GNT_SS    => w_GNT_SS2NW,
      i_GNT_SW    => w_GNT_SW2NW,
      i_GNT_WW    => w_GNT_WW2NW,
      i_GNT_NW    => w_GNT_NW2NW,

		o_REQ_NN 	=> w_REQ_NW2NN,
		o_REQ_NE 	=> w_REQ_NW2NE,
		o_REQ_EE 	=> w_REQ_NW2EE,
		o_REQ_SE 	=> w_REQ_NW2SE,
		o_REQ_SS 	=> w_REQ_NW2SS,
		o_REQ_SW 	=> w_REQ_NW2SW,
		o_REQ_WW 	=> w_REQ_NW2WW,
		o_REQ_NW 	=> w_REQ_NW2NW
	);

	U_MUX_NW : entity work.CROSSBAR 
   GENERIC MAP(
					p_DATA		=> p_DATA,
					p_SIZE_X		=> p_SIZE_X,
					p_SIZE_Y		=> p_SIZE_Y
	)
	PORT MAP(
					i_GNT 		=> w_CTR_NW,
					i_NORTH 		=> i_DIN_NN,
					i_NORTHEAST => i_DIN_NE,
					i_EAST 		=> i_DIN_EE,
					i_SOUTHEAST => i_DIN_SE,
					i_SOUTH 		=> i_DIN_SS,
					i_SOUTHWEST => i_DIN_SW,
					i_WEST 		=> i_DIN_WW,
					i_NORTHWEST => i_DIN_NW,
					o_DOUT 		=> w_DOUT_NW
	);


----------------------------------------------------------------------------------
-- Queue NORTH.
----------------------------------------------------------------------------------
	U_QUEUE_NN : entity work.QUEUE 
	PORT MAP(
					i_CLK 		=> i_CLK,
					i_RST 		=> i_RST,
					i_REQ_NN 	=> w_REQ_NN2NN,
					i_REQ_NE 	=> w_REQ_NE2NN,
					i_REQ_EE 	=> w_REQ_EE2NN,
					i_REQ_SE 	=> w_REQ_SE2NN,
					i_REQ_SS 	=> w_REQ_SS2NN,
					i_REQ_SW 	=> w_REQ_SW2NN,
					i_REQ_WW 	=> w_REQ_WW2NN,
					i_REQ_NW 	=> w_REQ_NW2NN,
               i_BUSY      => i_RD_NN,
					o_GNT_NN		=> w_GNT_NN2NN,
					o_GNT_NE		=> w_GNT_NN2NE,
					o_GNT_EE		=> w_GNT_NN2EE,
					o_GNT_SE		=> w_GNT_NN2SE,
					o_GNT_SS		=> w_GNT_NN2SS,
					o_GNT_SW		=> w_GNT_NN2SW,
					o_GNT_WW		=> w_GNT_NN2WW,
					o_GNT_NW		=> w_GNT_NN2NW,
					o_GNT 		=> w_GNT_NN,
					o_CHANNEL 	=> w_CTR_NN
	);

----------------------------------------------------------------------------------
-- Queue NORTHEAST.
----------------------------------------------------------------------------------
	U_QUEUE_NE : entity work.QUEUE 
	PORT MAP(
					i_CLK 		=> i_CLK,
					i_RST 		=> i_RST,
					i_REQ_NN 	=> w_REQ_NN2NE,
					i_REQ_NE 	=> w_REQ_NE2NE,
					i_REQ_EE 	=> w_REQ_EE2NE,
					i_REQ_SE 	=> w_REQ_SE2NE,
					i_REQ_SS 	=> w_REQ_SS2NE,
					i_REQ_SW 	=> w_REQ_SW2NE,
					i_REQ_WW 	=> w_REQ_WW2NE,
					i_REQ_NW 	=> w_REQ_NW2NE,
               i_BUSY      => i_RD_NE,
					o_GNT_NN		=> w_GNT_NE2NN,
					o_GNT_NE		=> w_GNT_NE2NE,
					o_GNT_EE		=> w_GNT_NE2EE,
					o_GNT_SE		=> w_GNT_NE2SE,
					o_GNT_SS		=> w_GNT_NE2SS,
					o_GNT_SW		=> w_GNT_NE2SW,
					o_GNT_WW		=> w_GNT_NE2WW,
					o_GNT_NW		=> w_GNT_NE2NW,
					o_GNT 		=> w_GNT_NE,
					o_CHANNEL 	=> w_CTR_NE
	);


----------------------------------------------------------------------------------
-- Queue EAST.
----------------------------------------------------------------------------------
	U_QUEUE_EE : entity work.QUEUE 
	PORT MAP(
					i_CLK 		=> i_CLK,
					i_RST 		=> i_RST,
					i_REQ_NN 	=> w_REQ_NN2EE,
					i_REQ_NE 	=> w_REQ_NE2EE,
					i_REQ_EE 	=> w_REQ_EE2EE,
					i_REQ_SE 	=> w_REQ_SE2EE,
					i_REQ_SS 	=> w_REQ_SS2EE,
					i_REQ_SW 	=> w_REQ_SW2EE,
					i_REQ_WW 	=> w_REQ_WW2EE,
					i_REQ_NW 	=> w_REQ_NW2EE,
               i_BUSY      => i_RD_EE,
					o_GNT_NN		=> w_GNT_EE2NN,
					o_GNT_NE		=> w_GNT_EE2NE,
					o_GNT_EE		=> w_GNT_EE2EE,
					o_GNT_SE		=> w_GNT_EE2SE,
					o_GNT_SS		=> w_GNT_EE2SS,
					o_GNT_SW		=> w_GNT_EE2SW,
					o_GNT_WW		=> w_GNT_EE2WW,
					o_GNT_NW		=> w_GNT_EE2NW,
					o_GNT 		=> w_GNT_EE,
					o_CHANNEL 	=> w_CTR_EE
	);


----------------------------------------------------------------------------------
-- Queue SOUTHEAST.
----------------------------------------------------------------------------------
	U_QUEUE_SE : entity work.QUEUE 
	PORT MAP(
					i_CLK 		=> i_CLK,
					i_RST 		=> i_RST,
					i_REQ_NN 	=> w_REQ_NN2SE,
					i_REQ_NE 	=> w_REQ_NE2SE,
					i_REQ_EE 	=> w_REQ_EE2SE,
					i_REQ_SE 	=> w_REQ_SE2SE,
					i_REQ_SS 	=> w_REQ_SS2SE,
					i_REQ_SW 	=> w_REQ_SW2SE,
					i_REQ_WW 	=> w_REQ_WW2SE,
					i_REQ_NW 	=> w_REQ_NW2SE,
               i_BUSY      => i_RD_SE,
					o_GNT_NN		=> w_GNT_SE2NN,
					o_GNT_NE		=> w_GNT_SE2NE,
					o_GNT_EE		=> w_GNT_SE2EE,
					o_GNT_SE		=> w_GNT_SE2SE,
					o_GNT_SS		=> w_GNT_SE2SS,
					o_GNT_SW		=> w_GNT_SE2SW,
					o_GNT_WW		=> w_GNT_SE2WW,
					o_GNT_NW		=> w_GNT_SE2NW,
					o_GNT 		=> w_GNT_SE,
					o_CHANNEL 	=> w_CTR_SE
	);


----------------------------------------------------------------------------------
-- Queue SOUTH.
----------------------------------------------------------------------------------
	U_QUEUE_SS : entity work.QUEUE 
	PORT MAP(
					i_CLK 		=> i_CLK,
					i_RST 		=> i_RST,
					i_REQ_NN 	=> w_REQ_NN2SS,
					i_REQ_NE 	=> w_REQ_NE2SS,
					i_REQ_EE 	=> w_REQ_EE2SS,
					i_REQ_SE 	=> w_REQ_SE2SS,
					i_REQ_SS 	=> w_REQ_SS2SS,
					i_REQ_SW 	=> w_REQ_SW2SS,
					i_REQ_WW 	=> w_REQ_WW2SS,
					i_REQ_NW 	=> w_REQ_NW2SS,
               i_BUSY      => i_RD_SS,
					o_GNT_NN		=> w_GNT_SS2NN,
					o_GNT_NE		=> w_GNT_SS2NE,
					o_GNT_EE		=> w_GNT_SS2EE,
					o_GNT_SE		=> w_GNT_SS2SE,
					o_GNT_SS		=> w_GNT_SS2SS,
					o_GNT_SW		=> w_GNT_SS2SW,
					o_GNT_WW		=> w_GNT_SS2WW,
					o_GNT_NW		=> w_GNT_SS2NW,
					o_GNT 		=> w_GNT_SS,
					o_CHANNEL 	=> w_CTR_SS
	);


----------------------------------------------------------------------------------
-- Queue SOUTHWEST.
----------------------------------------------------------------------------------
	U_QUEUE_SW : entity work.QUEUE 
	PORT MAP(
					i_CLK 		=> i_CLK,
					i_RST 		=> i_RST,
					i_REQ_NN 	=> w_REQ_NN2SW,
					i_REQ_NE 	=> w_REQ_NE2SW,
					i_REQ_EE 	=> w_REQ_EE2SW,
					i_REQ_SE 	=> w_REQ_SE2SW,
					i_REQ_SS 	=> w_REQ_SS2SW,
					i_REQ_SW 	=> w_REQ_SW2SW,
					i_REQ_WW 	=> w_REQ_WW2SW,
					i_REQ_NW 	=> w_REQ_NW2SW,
               i_BUSY      => i_RD_SW,               
					o_GNT_NN		=> w_GNT_SW2NN,
					o_GNT_NE		=> w_GNT_SW2NE,
					o_GNT_EE		=> w_GNT_SW2EE,
					o_GNT_SE		=> w_GNT_SW2SE,
					o_GNT_SS		=> w_GNT_SW2SS,
					o_GNT_SW		=> w_GNT_SW2SW,
					o_GNT_WW		=> w_GNT_SW2WW,
					o_GNT_NW		=> w_GNT_SW2NW,
					o_GNT 		=> w_GNT_SW,
					o_CHANNEL 	=> w_CTR_SW
	);


----------------------------------------------------------------------------------
-- Queue WEST.
----------------------------------------------------------------------------------
	U_QUEUE_WW : entity work.QUEUE 
	PORT MAP(
					i_CLK 		=> i_CLK,
					i_RST 		=> i_RST,
					i_REQ_NN 	=> w_REQ_NN2WW,
					i_REQ_NE 	=> w_REQ_NE2WW,
					i_REQ_EE 	=> w_REQ_EE2WW,
					i_REQ_SE 	=> w_REQ_SE2WW,
					i_REQ_SS 	=> w_REQ_SS2WW,
					i_REQ_SW 	=> w_REQ_SW2WW,
					i_REQ_WW 	=> w_REQ_WW2WW,
					i_REQ_NW 	=> w_REQ_NW2WW,
               i_BUSY      => i_RD_WW,
					o_GNT_NN		=> w_GNT_WW2NN,
					o_GNT_NE		=> w_GNT_WW2NE,
					o_GNT_EE		=> w_GNT_WW2EE,
					o_GNT_SE		=> w_GNT_WW2SE,
					o_GNT_SS		=> w_GNT_WW2SS,
					o_GNT_SW		=> w_GNT_WW2SW,
					o_GNT_WW		=> w_GNT_WW2WW,
					o_GNT_NW		=> w_GNT_WW2NW,
					o_GNT 		=> w_GNT_WW,
					o_CHANNEL 	=> w_CTR_WW
	);


----------------------------------------------------------------------------------
-- Queue NORTHWEST.
----------------------------------------------------------------------------------
	U_QUEUE_NW : entity work.QUEUE 
	PORT MAP(
					i_CLK 		=> i_CLK,
					i_RST 		=> i_RST,
					i_REQ_NN 	=> w_REQ_NN2NW,
					i_REQ_NE 	=> w_REQ_NE2NW,
					i_REQ_EE 	=> w_REQ_EE2NW,
					i_REQ_SE 	=> w_REQ_SE2NW,
					i_REQ_SS 	=> w_REQ_SS2NW,
					i_REQ_SW 	=> w_REQ_SW2NW,
					i_REQ_WW 	=> w_REQ_WW2NW,
					i_REQ_NW 	=> w_REQ_NW2NW,
               i_BUSY      => i_RD_NW,
					o_GNT_NN		=> w_GNT_NW2NN,
					o_GNT_NE		=> w_GNT_NW2NE,
					o_GNT_EE		=> w_GNT_NW2EE,
					o_GNT_SE		=> w_GNT_NW2SE,
					o_GNT_SS		=> w_GNT_NW2SS,
					o_GNT_SW		=> w_GNT_NW2SW,
					o_GNT_WW		=> w_GNT_NW2WW,
					o_GNT_NW		=> w_GNT_NW2NW,
					o_GNT 		=> w_GNT_NW,
					o_CHANNEL 	=> w_CTR_NW
	);

----------------------------------------------------------------------------------
-- Output signaling.
----------------------------------------------------------------------------------
   --
	-- NORTH.
	--
	UNN_OUTPUT_INTERFACE: entity work.OUTPUT_INTERFACE 
	GENERIC MAP(
			p_WIDTH		=> (p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6),
			p_PORT		=> c_NN
	)		
	PORT MAP(
			i_CLK 		=> i_CLK,
			i_RST 		=> i_RST,
			i_ND			=> w_GNT_NN,
			i_RD  		=> i_RD_NN,
			i_DIN 		=> w_DOUT_NN,
			o_DOUT 		=> o_DOUT_NN,
			o_ND 			=> w_BUSY_NN
   );
		
	o_ND_NN <= w_BUSY_NN;
	
   --
	-- NORTHEAST.
	--
	UNE_OUTPUT_INTERFACE: entity work.OUTPUT_INTERFACE 
	GENERIC MAP(
			p_WIDTH		=> (p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6),
			p_PORT		=> c_NE
	)
	PORT MAP(
			i_CLK 		=> i_CLK,
			i_RST 		=> i_RST,
			i_ND			=> w_GNT_NE,
			i_RD  		=> i_RD_NE,
			i_DIN 		=> w_DOUT_NE,
			o_DOUT 		=> o_DOUT_NE,
			o_ND 			=> w_BUSY_NE
	);

	o_ND_NE <= w_BUSY_NE;

   --
	-- EAST.
	--
	UEE_OUTPUT_INTERFACE: entity work.OUTPUT_INTERFACE 
	GENERIC MAP(
			p_WIDTH		=> (p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6),
			p_PORT		=> c_EE
	)
	PORT MAP(
			i_CLK 		=> i_CLK,
			i_RST 		=> i_RST,
			i_ND			=> w_GNT_EE,
			i_RD  		=> i_RD_EE,
			i_DIN 		=> w_DOUT_EE,
			o_DOUT 		=> o_DOUT_EE,
			o_ND 			=> w_BUSY_EE
	);	

	o_ND_EE <= w_BUSY_EE;

   --
	-- SOUTHEAST.
	--
	USE_OUTPUT_INTERFACE: entity work.OUTPUT_INTERFACE 
	GENERIC MAP(
			p_WIDTH		=> (p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6),
			p_PORT		=> c_SE
	)
	PORT MAP(
			i_CLK 		=> i_CLK,
			i_RST 		=> i_RST,
			i_ND			=> w_GNT_SE,
			i_RD  		=> i_RD_SE,
			i_DIN 		=> w_DOUT_SE,
			o_DOUT 		=> o_DOUT_SE,
			o_ND 			=> w_BUSY_SE
	);	

	o_ND_SE <= w_BUSY_SE;

   --
	-- SOUTH.
	--
	USS_OUTPUT_INTERFACE: entity work.OUTPUT_INTERFACE 
	GENERIC MAP(
			p_WIDTH		=> (p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6),
			p_PORT		=> c_SS
	)
	PORT MAP(
			i_CLK 		=> i_CLK,
			i_RST 		=> i_RST,
			i_ND			=> w_GNT_SS,
			i_RD  		=> i_RD_SS,
			i_DIN 		=> w_DOUT_SS,
			o_DOUT 		=> o_DOUT_SS,
			o_ND 			=> w_BUSY_SS
	);	

	o_ND_SS <= w_BUSY_SS;

   --
	-- SOUTHWEST.
	--
	USW_OUTPUT_INTERFACE: entity work.OUTPUT_INTERFACE 
	GENERIC MAP(
			p_WIDTH		=> (p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6),
			p_PORT		=> c_SW
	)
	PORT MAP(
			i_CLK 		=> i_CLK,
			i_RST 		=> i_RST,
			i_ND			=> w_GNT_SW,
			i_RD  		=> i_RD_SW,
			i_DIN 		=> w_DOUT_SW,
			o_DOUT 		=> o_DOUT_SW,
			o_ND 			=> w_BUSY_SW
	);	

	o_ND_SW <= w_BUSY_SW;

   --
	-- WEST.
	--
	UWW_OUTPUT_INTERFACE: entity work.OUTPUT_INTERFACE 
	GENERIC MAP(
			p_WIDTH		=> (p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6),
			p_PORT		=> c_WW
	)
	PORT MAP(
			i_CLK 		=> i_CLK,
			i_RST 		=> i_RST,
			i_ND			=> w_GNT_WW,
			i_RD  		=> i_RD_WW,
			i_DIN 		=> w_DOUT_WW,
			o_DOUT 		=> o_DOUT_WW,
			o_ND 			=> w_BUSY_WW
	);	

	o_ND_WW <= w_BUSY_WW;

   --
	-- NORTHWEST.
	--
	UNW_OUTPUT_INTERFACE: entity work.OUTPUT_INTERFACE 
	GENERIC MAP(
			p_WIDTH		=> (p_DATA+2*p_SIZE_X+2*p_SIZE_Y+6),
			p_PORT		=> c_NW
	)
	PORT MAP(
			i_CLK 		=> i_CLK,
			i_RST 		=> i_RST,
			i_ND			=> w_GNT_NW,			
			i_RD  		=> i_RD_NW,
			i_DIN 		=> w_DOUT_NW,
			o_DOUT 		=> o_DOUT_NW,
			o_ND 			=> w_BUSY_NW
	);	

	o_ND_NW <= w_BUSY_NW;

               
----------------------------------------------------------------------------------
end Behavioral;

