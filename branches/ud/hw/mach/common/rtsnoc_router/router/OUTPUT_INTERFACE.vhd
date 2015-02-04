----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Marcelo Daniel Berejuck
-- 
-- Create Date:    13:22:26 12/06/2010 
-- Design Name: 
-- Module Name:    OUTPUT_INTERFACE - Behavioral 
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

entity OUTPUT_INTERFACE is
    Generic(
			  p_WIDTH			: integer := 37;
			  p_PORT				: STD_LOGIC_VECTOR (3 downto 0) := "0000"
	 );
    Port ( i_CLK 				: in  STD_LOGIC;
           i_RST 				: in  STD_LOGIC;
           i_ND				: in  STD_LOGIC;
			  i_RD 				: in  STD_LOGIC;
  			  i_DIN     	   : in  STD_LOGIC_VECTOR (p_WIDTH-1 downto 0);
			  o_DOUT 		   : out STD_LOGIC_VECTOR (p_WIDTH-1 downto 0);
			  o_ND   			: out STD_LOGIC
	 );
end OUTPUT_INTERFACE;

architecture Behavioral of OUTPUT_INTERFACE is
----------------------------------------------------------------------------------
-- Internal signals.
----------------------------------------------------------------------------------
 type w_State_Type is (st_IDLE, st_WAIT_READ, st_START); 

	attribute syn_encoding : string;
	attribute syn_encoding of w_State_Type : type is "safe";
 
   signal w_STATE 	: w_State_Type;

	
   signal w_CLR_ND 	: std_logic;  
   signal w_ND    	: std_logic;  
 
	signal w_DOUT 		: STD_LOGIC_VECTOR (p_WIDTH-1 downto 0);
   signal w_SEQUENCY : STD_LOGIC_VECTOR (7 DOWNTO 0);
   signal w_UPDATE   : std_logic;

   signal w_UPDATE_A2B  : std_logic;
   signal w_ND_A2B      : STD_LOGIC;
   signal w_UPDATE_B2A  : std_logic;
   signal w_ND_B2A      : STD_LOGIC;

   signal w_CLR_RD 	: std_logic;  
   signal w_RD    	: std_logic;  

begin
	--
	-- Input signal.
	--
	w_DOUT <= i_DIN;
	--
	-- Saves the READ signal.
	--
	U_RD : process (i_CLK)
	begin
		if falling_edge (i_CLK) then
			if (w_CLR_RD = '1') then
				w_RD <= '0';
			else
				if (i_RD = '1') then
					w_RD <= '1';
				end if;
			end if;
		end if;
	end process U_RD;

	--
	-- Saves the data in the output.
	--
	U_REG : process (i_CLK)
	begin
		if falling_edge (i_CLK) then
			if (i_RST = '1') then
				o_DOUT <= (others => '0');
			else
				if (i_ND = '1') then
					o_DOUT <= w_DOUT;
				end if;
			end if;
		end if;
	end process U_REG;

   --
   -- Memorizes a new data pulse.
   --
	U_ND : process (i_CLK)
	begin
		if falling_edge (i_CLK) then
			if (w_CLR_ND = '1') then
				w_ND <= '0';
			else
				if (i_ND = '1') then
					w_ND <= '1';
				end if;
			end if;
		end if;
	end process U_ND;

----------------------------------------------------------------------------------
-- State machine for new output.
----------------------------------------------------------------------------------
	U_MACHINE : process(i_CLK, i_RST)          			
	begin    																						
		if (i_RST = '1') then			
			o_ND <= '0';
			w_CLR_ND <= '1';
			w_CLR_RD <= '1';
			w_STATE	<= st_START;				
			
		elsif rising_edge (i_CLK) then														
			case w_STATE is	

					when st_START =>	
						w_CLR_ND <= '0';
						w_CLR_RD <= '0';
						w_STATE	<= st_IDLE;

					when st_IDLE =>					 												
						w_CLR_RD <= '0';
						if (w_ND = '1') then
							o_ND <= '1';
							w_CLR_ND <= '1';
							w_STATE <= st_WAIT_READ;
						else
							w_STATE <= st_IDLE;
						end if;

					when st_WAIT_READ =>					 												
						w_CLR_ND <= '0';
						
						if (w_RD = '1') then
							o_ND <= '0';
							w_CLR_RD <= '1';
							w_STATE <= st_IDLE;
						else
							w_STATE <= st_WAIT_READ;
						end if;
							
					when others => 																
						w_STATE <= st_IDLE;														
						
			end case;																				

		end if;																						
	end process U_MACHINE;																	

----------------------------------------------------------------------------------
end Behavioral;

