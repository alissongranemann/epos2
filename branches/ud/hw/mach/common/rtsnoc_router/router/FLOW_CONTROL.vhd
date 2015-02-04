----------------------------------------------------------------------------------
-- Company: 
-- Engineer: Marcelo Daniel Berejuck
-- 
-- Create Date:    15:59:28 03/07/2011 
-- Design Name: 
-- Module Name:    FLOW_CONTROL - Behavioral 
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

entity FLOW_CONTROL is
    Generic (
					p_SIZE_X		: integer := 4;
					p_SIZE_Y		: integer := 4;
					
					p_X			: integer := 2;
					p_Y			: integer := 2
	 );
    Port ( 
					i_CLK 	: in  STD_LOGIC;
					i_RST 	: in  STD_LOGIC;
					-- Signals that controls the Input flow.
					i_SEND 	: in  STD_LOGIC;
					o_WAIT 	: out STD_LOGIC;
					-- Signals that controls the Output flow.
					i_GRANT 	: in  STD_LOGIC_VECTOR(2 DOWNTO 0);
					i_BSY_NN	: in  STD_LOGIC;
					i_BSY_NE	: in  STD_LOGIC;
				   i_BSY_EE	: in  STD_LOGIC;
				   i_BSY_SE	: in  STD_LOGIC;
				   i_BSY_SS	: in  STD_LOGIC;
				   i_BSY_SW	: in  STD_LOGIC;
				   i_BSY_WW	: in  STD_LOGIC;
				   i_BSY_NW	: in  STD_LOGIC;

				   i_DST		: in  STD_LOGIC_VECTOR ( ((p_SIZE_X+p_SIZE_Y + 3) - 1) downto 0);

					i_RD_NN	: in  STD_LOGIC;
					i_RD_NE	: in  STD_LOGIC;
				   i_RD_EE	: in  STD_LOGIC;
				   i_RD_SE	: in  STD_LOGIC;
				   i_RD_SS	: in  STD_LOGIC;
				   i_RD_SW	: in  STD_LOGIC;
				   i_RD_WW	: in  STD_LOGIC;
				   i_RD_NW	: in  STD_LOGIC;

               i_GNT_NN : in STD_LOGIC;
               i_GNT_NE : in STD_LOGIC;
               i_GNT_EE : in STD_LOGIC;
               i_GNT_SE : in STD_LOGIC;
               i_GNT_SS : in STD_LOGIC;
               i_GNT_SW : in STD_LOGIC;
               i_GNT_WW : in STD_LOGIC;
               i_GNT_NW : in STD_LOGIC;

				   o_REQ_NN	: out STD_LOGIC;
				   o_REQ_NE	: out STD_LOGIC;
				   o_REQ_EE	: out STD_LOGIC;
				   o_REQ_SE	: out STD_LOGIC;
				   o_REQ_SS	: out STD_LOGIC;
				   o_REQ_SW	: out STD_LOGIC;
				   o_REQ_WW	: out STD_LOGIC;
				   o_REQ_NW	: out STD_LOGIC					  
	 );
end FLOW_CONTROL;

architecture Behavioral of FLOW_CONTROL is
----------------------------------------------------------------------------------
-- Internal signals.
----------------------------------------------------------------------------------
 type w_State_Type is (st_START, st_IDLE, st_REQ_NN, st_REQ_NE, st_REQ_EE, st_REQ_SE,
							  st_REQ_SS, st_REQ_SW, st_REQ_WW, st_REQ_NW,
                       st_WAIT_NN, st_WAIT_NE, st_WAIT_EE, st_WAIT_SE, 
                       st_WAIT_SS, st_WAIT_SW, st_WAIT_WW, st_WAIT_NW); 

	attribute syn_encoding : string;
	attribute syn_encoding of w_State_Type : type is "safe";
 
   signal w_STATE 	: w_State_Type;
	
   signal w_EN_A2B 	: std_logic;  
   signal w_EN_B2A 	: std_logic;  

	signal w_REQ		: STD_LOGIC;
	signal w_DST		: STD_LOGIC;
	signal w_NET_X		: STD_LOGIC_VECTOR ( (p_SIZE_X - 1) downto 0);	
	signal w_NET_Y		: STD_LOGIC_VECTOR ( (p_SIZE_Y - 1) downto 0);	
	signal w_HST		: STD_LOGIC_VECTOR (2 downto 0);
	signal w_CTR_MUX	: STD_LOGIC_VECTOR (2 downto 0);
	
	constant c_NN     : STD_LOGIC_VECTOR (2 downto 0) := "000"; 
	constant c_NE     : STD_LOGIC_VECTOR (2 downto 0) := "001"; 
	constant c_EE     : STD_LOGIC_VECTOR (2 downto 0) := "010"; 
	constant c_SE     : STD_LOGIC_VECTOR (2 downto 0) := "011"; 
	constant c_SS     : STD_LOGIC_VECTOR (2 downto 0) := "100"; 
	constant c_SW     : STD_LOGIC_VECTOR (2 downto 0) := "101"; 
	constant c_WW     : STD_LOGIC_VECTOR (2 downto 0) := "110"; 
	constant c_NW     : STD_LOGIC_VECTOR (2 downto 0) := "111"; 

	signal w_DST_NN	: STD_LOGIC;
	signal w_DST_NE	: STD_LOGIC;
	signal w_DST_EE	: STD_LOGIC;
	signal w_DST_SE	: STD_LOGIC;
	signal w_DST_SS	: STD_LOGIC;
	signal w_DST_SW	: STD_LOGIC;
	signal w_DST_WW	: STD_LOGIC;
	signal w_DST_NW	: STD_LOGIC;			  

	signal v_X   		: integer := p_X;
	signal v_Y   		: integer := p_Y;
   
   signal w_GNT      : STD_LOGIC;

	signal w_REQ_NN	: STD_LOGIC;
	signal w_REQ_NE	: STD_LOGIC;
	signal w_REQ_EE	: STD_LOGIC;
	signal w_REQ_SE	: STD_LOGIC;
	signal w_REQ_SS	: STD_LOGIC;
	signal w_REQ_SW	: STD_LOGIC;
	signal w_REQ_WW	: STD_LOGIC;
	signal w_REQ_NW	: STD_LOGIC;

	signal w_SEND   	: STD_LOGIC;
	signal w_CLR_SEND	: STD_LOGIC;

	signal w_RD_NN   	: STD_LOGIC;
	signal w_CLR_NN	: STD_LOGIC;
			
	signal w_RD_NE   	: STD_LOGIC;
	signal w_CLR_NE	: STD_LOGIC;

	signal w_RD_EE   	: STD_LOGIC;
	signal w_CLR_EE	: STD_LOGIC;

	signal w_RD_SE   	: STD_LOGIC;
	signal w_CLR_SE	: STD_LOGIC;

	signal w_RD_SS   	: STD_LOGIC;
	signal w_CLR_SS	: STD_LOGIC;

	signal w_RD_SW   	: STD_LOGIC;
	signal w_CLR_SW	: STD_LOGIC;

	signal w_RD_WW   	: STD_LOGIC;
	signal w_CLR_WW	: STD_LOGIC;

	signal w_RD_NW   	: STD_LOGIC;
	signal w_CLR_NW	: STD_LOGIC;

begin
	--
	-- Splits the addresses.
	--
	U_ADD_HST :
   for i in 0 to 2 generate
      begin
         w_HST(i) <= i_DST(i);
   end generate;
	
	
	U_ADD_Y :
   for j in 0 to (p_SIZE_Y - 1) generate
      begin
         w_NET_Y(j) <= i_DST(j+3);
   end generate;


	U_ADD_X :
   for k in 0 to (p_SIZE_X - 1) generate
      begin
         w_NET_X(k) <= i_DST(k+(p_SIZE_Y+3));
   end generate;


	v_X <= conv_integer(w_NET_X);
	v_Y <= conv_integer(w_NET_Y);

----------------------------------------------------------------------------------
-- Register the new transmition request.
----------------------------------------------------------------------------------
   U_REG : process (i_CLK)
   begin
      if falling_edge (i_CLK) then
         if (w_CLR_SEND = '1') then
            w_SEND <= '0';
         else
            if (i_SEND = '1') then
               w_SEND <= '1';
            end if;
         end if;
      end if;
   end process U_REG;

----------------------------------------------------------------------------------
-- Register the new data received.
----------------------------------------------------------------------------------
   U_REG_NN : process (i_CLK)
   begin
      if falling_edge (i_CLK) then
         if (w_CLR_NN = '1') then
            w_RD_NN <= '0';
         else
            if (i_RD_NN = '1') then
               w_RD_NN <= '1';
            end if;
         end if;
      end if;
   end process U_REG_NN;
   
   U_REG_NE : process (i_CLK)
   begin
      if falling_edge (i_CLK) then
         if (w_CLR_NE = '1') then
            w_RD_NE <= '0';
         else
            if (i_RD_NE = '1') then
               w_RD_NE <= '1';
            end if;
         end if;
      end if;
   end process U_REG_NE;
   
   U_REG_EE : process (i_CLK)
   begin
      if falling_edge (i_CLK) then
         if (w_CLR_EE = '1') then
            w_RD_EE <= '0';
         else
            if (i_RD_EE = '1') then
               w_RD_EE <= '1';
            end if;
         end if;
      end if;
   end process U_REG_EE;
   
   U_REG_SE : process (i_CLK)
   begin
      if falling_edge (i_CLK) then
         if (w_CLR_SE = '1') then
            w_RD_SE <= '0';
         else
            if (i_RD_SE = '1') then
               w_RD_SE <= '1';
            end if;
         end if;
      end if;
   end process U_REG_SE;
   
   U_REG_SS : process (i_CLK)
   begin
      if falling_edge (i_CLK) then
         if (w_CLR_SS = '1') then
            w_RD_SS <= '0';
         else
            if (i_RD_SS = '1') then
               w_RD_SS <= '1';
            end if;
         end if;
      end if;
   end process U_REG_SS;
   
   U_REG_SW : process (i_CLK)
   begin
      if falling_edge (i_CLK) then
         if (w_CLR_SW = '1') then
            w_RD_SW <= '0';
         else
            if (i_RD_SW = '1') then
               w_RD_SW <= '1';
            end if;
         end if;
      end if;
   end process U_REG_SW;
   
   U_REG_WW : process (i_CLK)
   begin
      if falling_edge (i_CLK) then
         if (w_CLR_WW = '1') then
            w_RD_WW <= '0';
         else
            if (i_RD_WW = '1') then
               w_RD_WW <= '1';
            end if;
         end if;
      end if;
   end process U_REG_WW;
   
   U_REG_NW : process (i_CLK)
   begin
      if falling_edge (i_CLK) then
         if (w_CLR_NW = '1') then
            w_RD_NW <= '0';
         else
            if (i_RD_NW = '1') then
               w_RD_NW <= '1';
            end if;
         end if;
      end if;
   end process U_REG_NW;
   

----------------------------------------------------------------------------------
-- State machine.
----------------------------------------------------------------------------------
	U_MACHINE : process(i_CLK, i_RST)          			
	begin    																						
		if (i_RST = '1') then			
			w_REQ_NN	   <= '0';
			w_REQ_NE	   <= '0';
			w_REQ_EE	   <= '0';
			w_REQ_SE	   <= '0';
			w_REQ_SS	   <= '0';
			w_REQ_SW	   <= '0';
			w_REQ_WW	   <= '0';
			w_REQ_NW	   <= '0';
         w_CLR_SEND  <= '1';
         w_CLR_NN    <= '1';
         w_CLR_NE    <= '1';
         w_CLR_EE    <= '1';
         w_CLR_SE    <= '1';
         w_CLR_SS    <= '1';
         w_CLR_SW    <= '1';
         w_CLR_WW    <= '1';
         w_CLR_NW    <= '1';
			o_WAIT      <= '0';
			w_STATE	   <= st_START;				
			
		elsif rising_edge (i_CLK) then														
--		elsif falling_edge (i_CLK) then														
			case w_STATE is	

					when st_START =>					 												
                  w_CLR_SEND  <= '0';
						w_CLR_NN    <= '0';
						w_CLR_NE    <= '0';
						w_CLR_EE    <= '0';
						w_CLR_SE    <= '0';
						w_CLR_SS    <= '0';
						w_CLR_SW    <= '0';
						w_CLR_WW    <= '0';
						w_CLR_NW    <= '0';						
                  w_STATE	   <= st_IDLE;			
                  
					when st_IDLE =>					 												
						
						if (w_SEND = '1') then
								w_CLR_SEND  <= '1';
                        o_WAIT      <= '1';
										
								if ((v_X = p_X) and (v_Y = p_Y)) then								
									--
									-- Routing inside the router.
									--
									if (w_HST = c_NN) then
										o_WAIT   <= '1';
										w_REQ_NN <= '1';
										w_STATE	<= st_REQ_NN;
									elsif (w_HST = c_NE) then
										w_REQ_NE <= '1';
										w_STATE	<= st_REQ_NE;
									elsif (w_HST = c_EE) then
										w_REQ_EE <= '1';
										w_STATE	<= st_REQ_EE;
									elsif (w_HST = c_SE) then
										w_REQ_SE <= '1';
										w_STATE	<= st_REQ_SE;
									elsif (w_HST = c_SS) then
										w_REQ_SS <= '1';
										w_STATE	<= st_REQ_SS;
									elsif (w_HST = c_SW) then
										w_REQ_SW <= '1';
										w_STATE	<= st_REQ_SW;
									elsif (w_HST = c_WW) then
										w_REQ_WW <= '1';
										w_STATE	<= st_REQ_WW;
--									elsif (w_HST = c_NW) then
									else
										w_REQ_NW <= '1';
										w_STATE	<= st_REQ_NW;
									end if;
										
								else
									--
									-- X is different.
									--
									if (v_X /= p_X) then										
										if (v_X > p_X) then											
											w_REQ_EE <= '1';
											w_STATE	<= st_REQ_EE;
										elsif (v_X < p_X) then
											w_REQ_WW <= '1';
											w_STATE	<= st_REQ_WW;
										end if;
											
									--
									-- Y is different.
									--
									elsif (v_Y /= p_Y) then
										if (v_Y > p_Y) then
											o_WAIT   <= '1';
											w_REQ_NN <= '1';
											w_STATE	<= st_REQ_NN;
										else
											o_WAIT   <= '1';
											w_REQ_SS <= '1';
											w_STATE	<= st_REQ_SS;
										end if;
															
									end if;
								
								end if;
			
		
						else
							w_STATE	<= st_IDLE;

						end if;
					--
               -- Requests.
               --               
					when st_REQ_NN =>			
						w_CLR_SEND  <= '0';
                  if (i_GNT_NN = '1') then
							w_STATE	<= st_WAIT_NN;
						else
							w_STATE	<= st_REQ_NN;
						end if;

					when st_REQ_NE =>			
						w_CLR_SEND  <= '0';
                  if (i_GNT_NE = '1') then
							w_STATE	<= st_WAIT_NE;
						else
							w_STATE	<= st_REQ_NE;
						end if;

					when st_REQ_EE =>			
						w_CLR_SEND  <= '0';
                  if (i_GNT_EE = '1') then
							w_STATE	<= st_WAIT_EE;
						else
							w_STATE	<= st_REQ_EE;
						end if;

					when st_REQ_SE =>			
						w_CLR_SEND  <= '0';
                  if (i_GNT_SE = '1') then
							w_STATE	<= st_WAIT_SE;
						else
							w_STATE	<= st_REQ_SE;
						end if;

					when st_REQ_SS =>			
						w_CLR_SEND  <= '0';
                  if (i_GNT_SS = '1') then
							w_STATE	<= st_WAIT_SS;
						else
							w_STATE	<= st_REQ_SS;
						end if;

					when st_REQ_SW =>			
						w_CLR_SEND  <= '0';
                  if (i_GNT_SW = '1') then
							w_STATE	<= st_WAIT_SW;
						else
							w_STATE	<= st_REQ_SW;
						end if;

					when st_REQ_WW =>			
						w_CLR_SEND  <= '0';
                  if (i_GNT_WW = '1') then
							w_STATE	<= st_WAIT_WW;
						else
							w_STATE	<= st_REQ_WW;
						end if;

					when st_REQ_NW =>			
						w_CLR_SEND  <= '0';
                  if (i_GNT_NW = '1') then
							w_STATE	<= st_WAIT_NW;
						else
							w_STATE	<= st_REQ_NW;
						end if;

					--
               -- Waits.
               --               
					when st_WAIT_NN =>					 												
						if (w_RD_NN = '1') then
							w_CLR_NN <= '1';
							w_REQ_NN <= '0';
							o_WAIT 	<= '0';
							w_STATE	<= st_START;
						else
							w_STATE	<= st_WAIT_NN;
						end if;

					when st_WAIT_NE =>					 												
						if (w_RD_NE = '1') then
							w_CLR_NE <= '1';
							w_REQ_NE <= '0';
							o_WAIT 	<= '0';
							w_STATE	<= st_START;
						else
							w_STATE	<= st_WAIT_NE;
						end if;

					when st_WAIT_EE =>					 												
						if (w_RD_EE = '1') then
							w_CLR_EE <= '1';
							w_REQ_EE <= '0';
							o_WAIT 	<= '0';
							w_STATE	<= st_START;
						else
							w_STATE	<= st_WAIT_EE;
						end if;

					when st_WAIT_SE =>					 												
						if (w_RD_SE = '1') then
							w_CLR_SE <= '1';
							w_REQ_SE <= '0';
							o_WAIT 	<= '0';
							w_STATE	<= st_START;
						else
							w_STATE	<= st_WAIT_SE;
						end if;

					when st_WAIT_SS =>					 												
						if (w_RD_SS = '1') then
							w_CLR_SS <= '1';
							w_REQ_SS <= '0';
							o_WAIT 	<= '0';
							w_STATE	<= st_START;
						else
							w_STATE	<= st_WAIT_SS;
						end if;

					when st_WAIT_SW =>					 												
						if (w_RD_SW = '1') then
							w_CLR_SW <= '1';
							w_REQ_SW <= '0';
							o_WAIT 	<= '0';
							w_STATE	<= st_START;
						else
							w_STATE	<= st_WAIT_SW;
						end if;

					when st_WAIT_WW =>					 												
						if (w_RD_WW = '1') then
							w_CLR_WW <= '1';
							w_REQ_WW <= '0';
							o_WAIT 	<= '0';
							w_STATE	<= st_START;
						else
							w_STATE	<= st_WAIT_WW;
						end if;

					when st_WAIT_NW =>					 												
						if (w_RD_NW = '1') then
							w_CLR_NW <= '1';
							w_REQ_NW <= '0';
							o_WAIT 	<= '0';
							w_STATE	<= st_START;
						else
							w_STATE	<= st_WAIT_NW;
						end if;

					when others => 																
						w_STATE <= st_IDLE;														
						
			end case;																				

		end if;																						
	end process U_MACHINE;																	

----------------------------------------------------------------------------------
-- Sends requests.
----------------------------------------------------------------------------------

	o_REQ_NN <= w_REQ_NN ;
	o_REQ_NE <= w_REQ_NE ;
	o_REQ_EE <= w_REQ_EE ;
	o_REQ_SE <= w_REQ_SE ;
	o_REQ_SS <= w_REQ_SS ;
	o_REQ_SW <= w_REQ_SW ;
	o_REQ_WW <= w_REQ_WW ;
	o_REQ_NW <= w_REQ_NW ;
	

end Behavioral;

