----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Marcelo Daniel Berejuck
-- 
-- Create Date:    13:20:22 03/07/2011 
-- Design Name: 
-- Module Name:    ARBITER_MACHINE - Behavioral 
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

entity ARBITER_MACHINE is
    Port ( i_CLK 		: in  STD_LOGIC;
           i_RST 		: in  STD_LOGIC;
			  i_REQ		: in  STD_LOGIC;
           i_BUSY    : in  STD_LOGIC;
			  o_EN		: out STD_LOGIC
	 );
end ARBITER_MACHINE;

architecture Behavioral of ARBITER_MACHINE is
----------------------------------------------------------------------------------
-- Internal signals.
----------------------------------------------------------------------------------
 type w_State_Type is (st_IDLE, st_UPDATE); 

	attribute syn_encoding : string;
	attribute syn_encoding of w_State_Type : type is "safe";
 
   signal w_STATE 		: w_State_Type;
	
   signal w_EN 	   	: std_logic;  
 
	signal w_BUSY			: std_logic;
	signal w_CLR_BUSY		: std_logic;
	
begin
   --
   -- Memorizes the READ signal.
   --
	U_BUSY : process (i_CLK)
	begin
		if falling_edge (i_CLK) then
			if (w_CLR_BUSY = '1') then
				w_BUSY <= '0';
			else
				if (i_BUSY = '1') then
					w_BUSY <= '1';
				end if;
			end if;
		end if;
	end process U_BUSY;

----------------------------------------------------------------------------------
-- State machine.
----------------------------------------------------------------------------------
	U_MACHINE : process(i_CLK, i_RST)          			
	begin    																						
		if (i_RST = '1') then			
			w_EN	   	<= '0';
			w_CLR_BUSY 	<= '1';
			w_STATE		<= st_IDLE;				
			
		elsif rising_edge (i_CLK) then														
			case w_STATE is	

					when st_IDLE =>					 												
						w_CLR_BUSY 	<= '0';
						
						if (i_REQ = '1') then
							w_EN     <= '1';
							w_STATE  <= st_UPDATE;
						else
							w_STATE  <= st_IDLE;
						end if;

					when st_UPDATE =>					 												
						w_EN  <= '0';
                  
						if (w_BUSY = '1') then
                     w_CLR_BUSY 	<= '1';
							w_STATE <= st_IDLE;
						else
                     w_STATE <= st_UPDATE;
                  end if;
					when others => 																
						w_STATE <= st_IDLE;														
						
			end case;																				

		end if;																						
	end process U_MACHINE;																	


----------------------------------------------------------------------------------
-- Controls the enable signal that will be sent to the Scheduler.
----------------------------------------------------------------------------------
	o_EN <= w_EN;

	
end Behavioral;

