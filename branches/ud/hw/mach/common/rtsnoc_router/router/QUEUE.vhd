----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Marcelo Daniel Berejuck
-- 
-- Create Date:    13:19:18 03/07/2011 
-- Design Name: 
-- Module Name:    QUEUE - Behavioral 
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

entity QUEUE is
    Port ( i_CLK 			: in  STD_LOGIC;
           i_RST 			: in  STD_LOGIC;
			  
			  i_REQ_NN 		: in  STD_LOGIC;
			  i_REQ_NE 		: in  STD_LOGIC;
			  i_REQ_EE 		: in  STD_LOGIC;
			  i_REQ_SE 		: in  STD_LOGIC;
			  i_REQ_SS 		: in  STD_LOGIC;
			  i_REQ_SW 		: in  STD_LOGIC;
			  i_REQ_WW 		: in  STD_LOGIC;
			  i_REQ_NW 		: in  STD_LOGIC;
			  
			  i_BUSY   		: in  STD_LOGIC;

			  o_GNT_NN  	: out STD_LOGIC;
			  o_GNT_NE  	: out STD_LOGIC;
			  o_GNT_EE  	: out STD_LOGIC;
			  o_GNT_SE  	: out STD_LOGIC;
			  o_GNT_SS  	: out STD_LOGIC;
			  o_GNT_SW  	: out STD_LOGIC;
			  o_GNT_WW  	: out STD_LOGIC;
			  o_GNT_NW  	: out STD_LOGIC;
			  
			  o_GNT  		: out STD_LOGIC;
			  o_CHANNEL 	: out STD_LOGIC_VECTOR(2 DOWNTO 0)
	 );
end QUEUE;

architecture Behavioral of QUEUE is
----------------------------------------------------------------------------------
-- Internal signals.
----------------------------------------------------------------------------------
	constant c_NN     		: STD_LOGIC_VECTOR (2 downto 0) := "000"; 
	constant c_NE     		: STD_LOGIC_VECTOR (2 downto 0) := "001"; 
	constant c_EE     		: STD_LOGIC_VECTOR (2 downto 0) := "010"; 
	constant c_SE     		: STD_LOGIC_VECTOR (2 downto 0) := "011"; 
	constant c_SS     		: STD_LOGIC_VECTOR (2 downto 0) := "100"; 
	constant c_SW     		: STD_LOGIC_VECTOR (2 downto 0) := "101"; 
	constant c_WW     		: STD_LOGIC_VECTOR (2 downto 0) := "110"; 
	constant c_NW     		: STD_LOGIC_VECTOR (2 downto 0) := "111"; 

	signal w_NEW_REQ			: STD_LOGIC;
	signal w_EN_SCH			: STD_LOGIC;

	signal w_CHANNEL 			:  STD_LOGIC_VECTOR (2 downto 0) := (others => '0');
	signal w_PRI_0				:  STD_LOGIC_VECTOR (2 downto 0);
	signal w_PRI_1				:  STD_LOGIC_VECTOR (2 downto 0);
	signal w_PRI_2				:  STD_LOGIC_VECTOR (2 downto 0);
	signal w_PRI_3				:  STD_LOGIC_VECTOR (2 downto 0);
	signal w_PRI_4				:  STD_LOGIC_VECTOR (2 downto 0);
	signal w_PRI_5				:  STD_LOGIC_VECTOR (2 downto 0);
	signal w_PRI_6				:  STD_LOGIC_VECTOR (2 downto 0);
	signal w_PRI_7				:  STD_LOGIC_VECTOR (2 downto 0);
   signal w_PRI_CHANNEL		:  STD_LOGIC_VECTOR (3 downto 0);
	signal w_REQ 				:  STD_LOGIC_VECTOR (7 downto 0);
	signal w_GRANT 			:  STD_LOGIC;
   signal w_UPDATE 			:  STD_LOGIC;
	
   signal w_GNT_NN         :  STD_LOGIC;
   signal w_GNT_NE         :  STD_LOGIC;
   signal w_GNT_EE         :  STD_LOGIC;
   signal w_GNT_SE         :  STD_LOGIC;
   signal w_GNT_SS         :  STD_LOGIC;
   signal w_GNT_SW         :  STD_LOGIC;
   signal w_GNT_WW         :  STD_LOGIC;
   signal w_GNT_NW         :  STD_LOGIC;
   
begin

	w_NEW_REQ <= i_REQ_NN or i_REQ_NE or i_REQ_EE or i_REQ_SE or 
					 i_REQ_SS or i_REQ_SW or i_REQ_WW or i_REQ_NW; 
						
	w_REQ(0) <= i_REQ_NN;
	w_REQ(1) <= i_REQ_NE;
	w_REQ(2) <= i_REQ_EE;
	w_REQ(3) <= i_REQ_SE;
	w_REQ(4) <= i_REQ_SS;
	w_REQ(5) <= i_REQ_SW;
	w_REQ(6) <= i_REQ_WW;
	w_REQ(7) <= i_REQ_NW;
	
   
	U_ARBITER_MACHINE: entity work.ARBITER_MACHINE 
	PORT MAP(
				i_CLK    => i_CLK,
				i_RST    => i_RST,
				i_REQ    => w_NEW_REQ,
            i_BUSY   => i_BUSY, 
				o_EN     => w_EN_SCH
	);
	
----------------------------------------------------------------------------------
-- Scheduler.
----------------------------------------------------------------------------------
	U_SCHEDULER: entity work.SCHEDULER 
	PORT MAP(
				i_CLK 		=> i_CLK,
				i_RST 		=> i_RST,
				i_REQ 		=> w_REQ,
				i_EN 			=> w_EN_SCH,
				o_EN			=> w_UPDATE,
				i_PRI_0 		=> w_PRI_0,
				i_PRI_1 		=> w_PRI_1,
				i_PRI_2 		=> w_PRI_2,
				i_PRI_3 		=> w_PRI_3,
				i_PRI_4 		=> w_PRI_4,
				i_PRI_5 		=> w_PRI_5,
				i_PRI_6 		=> w_PRI_6,
				i_PRI_7 		=> w_PRI_7,
				o_GRANT		=> w_GRANT,
				o_CHANNEL 	=> w_PRI_CHANNEL
	);
	
----------------------------------------------------------------------------------
-- Pipeline.
----------------------------------------------------------------------------------
	U_PIPE : entity work.PIPELINE_NOC 
	PORT MAP(
				i_CLK 		=> i_CLK,
				i_RST 		=> i_RST,
				i_EN  		=> w_UPDATE,
				i_CHANNEL 	=> w_PRI_CHANNEL,
				o_PRI_0 		=> w_PRI_0,
				o_PRI_1 		=> w_PRI_1,
				o_PRI_2 		=> w_PRI_2,
				o_PRI_3 		=> w_PRI_3,
				o_PRI_4 		=> w_PRI_4,
				o_PRI_5 		=> w_PRI_5,
				o_PRI_6 		=> w_PRI_6,
				o_PRI_7 		=> w_PRI_7,
				o_CHANNEL 	=> w_CHANNEL
	);	


----------------------------------------------------------------------------------
-- Output and Grants.
----------------------------------------------------------------------------------
	o_CHANNEL 	   <= w_CHANNEL;
	o_GNT			   <= w_GRANT;
   
   o_GNT_NN       <= w_GNT_NN;
   o_GNT_NE       <= w_GNT_NE;
   o_GNT_EE       <= w_GNT_EE;
   o_GNT_SE       <= w_GNT_SE;
   o_GNT_SS       <= w_GNT_SS;
   o_GNT_SW       <= w_GNT_SW;
   o_GNT_WW       <= w_GNT_WW;
   o_GNT_NW       <= w_GNT_NW;
   

	--
	-- DEMUX used to sends grant signals to the flow controllers.
	--
	w_GNT_NN <= '1' when ((w_CHANNEL = c_NN) and (w_GRANT = '1')) else '0';
	w_GNT_NE <= '1' when ((w_CHANNEL = c_NE) and (w_GRANT = '1')) else '0';
	w_GNT_EE <= '1' when ((w_CHANNEL = c_EE) and (w_GRANT = '1')) else '0';
	w_GNT_SE <= '1' when ((w_CHANNEL = c_SE) and (w_GRANT = '1')) else '0';
	w_GNT_SS <= '1' when ((w_CHANNEL = c_SS) and (w_GRANT = '1')) else '0';
	w_GNT_SW <= '1' when ((w_CHANNEL = c_SW) and (w_GRANT = '1')) else '0';
	w_GNT_WW <= '1' when ((w_CHANNEL = c_WW) and (w_GRANT = '1')) else '0';
	w_GNT_NW <= '1' when ((w_CHANNEL = c_NW) and (w_GRANT = '1')) else '0';
	
----------------------------------------------------------------------------------
end Behavioral;

