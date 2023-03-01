-- cpu.vhd: Simple 8-bit CPU (BrainF*ck interpreter)
-- Copyright (C) 2019 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): xmorav41
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru

   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti

   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data

   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

 -- zde dopiste potrebne deklarace signalu
 type FSM_states is (
  state_init,
  state_fetch,
  state_decode,

  state_incpt,
  state_decpt,
  state_incel, state_incel_2,
  state_decel, state_decel_2,
  state_print, state_print_2,
  state_load, state_load_2,
  state_while, state_while_2, state_while_3, state_while_4,state_while_5,
  state_while_end, state_while_2_end, state_while_3_end, state_while_4_end,state_while_5_end,
  state_save_tmp, state_save_tmp_2,
  state_save_act, state_save_act_2,state_save_act_3,
  state_end
 );

signal state_current : FSM_states;
signal state_next : FSM_states;

signal pc_inc : std_logic;
signal pc_dec : std_logic;
signal pc_reg : std_logic_vector (12 downto 0);

signal ptr_inc : std_logic;
signal ptr_dec : std_logic;
signal ptr_reg : std_logic_vector (12 downto 0);

signal cnt_inc : std_logic;
signal cnt_dec : std_logic;
signal cnt_reg : std_logic_vector (7 downto 0);

signal mx_sel_1 : std_logic;
signal mx_sel_2 : std_logic;
signal mx_sel_3 : std_logic_vector (1 downto 0);

signal mx2_res : std_logic_vector (12 downto 0);


begin

 CNT: process(CLK, RESET)
 begin
	 if (RESET = '1') then
		cnt_reg <= (others => '0');
	 ----
	 elsif (rising_edge(CLK)) then
	 if (cnt_inc = '1') then
		cnt_reg <= cnt_reg + 1;
	 elsif (cnt_dec = '1') then
		cnt_reg <= cnt_reg - 1;
	 end if;
  end if;
 end process;

 PC: process(CLK, RESET, pc_inc, pc_dec, pc_reg)
 begin
  if (RESET = '1') then
    pc_reg <= (others => '0');
  ----
  elsif (rising_edge(CLK)) then
    if (pc_inc = '1') then
      pc_reg <= pc_reg + 1;
    elsif (pc_dec = '1') then
      pc_reg <= pc_reg - 1;
    end if;
  end if;
 end process;

 PTR: process(CLK, RESET, ptr_inc, ptr_dec, ptr_reg)
 begin
  if (RESET = '1') then
    ptr_reg <= (others => '0');
  ----
  elsif (rising_edge(CLK)) then
	 if (ptr_reg = "0000000000000") then	--initne reg na zaciatku/resete
		ptr_reg <= "1000000000000";
	 end if;

    if (ptr_inc = '1') then     ---- changenut 11111111 00000000 a naopak
		 if (ptr_reg = "1111111111111") then
			ptr_reg <= "1000000000000";
		 else
			ptr_reg <= ptr_reg + 1;
		 end if;
    elsif (ptr_dec = '1') then
		if (ptr_reg = "1000000000000") then
			ptr_reg <= "1111111111111";
		else
			ptr_reg <= ptr_reg - 1;
		end if;
    end if;
  end if;
 end process;

 MX1: process(mx_sel_1,mx2_res,pc_reg)
  begin
    case mx_sel_1 is
      when '0' =>  DATA_ADDR  <=  pc_reg;
      when '1' =>  DATA_ADDR  <=  mx2_res;
		when others =>
    end case;
  end process;

 MX2: process(DATA_RDATA, mx_sel_2,ptr_reg)
   begin
     case(mx_sel_2) is
       when '0'  =>  mx2_res  <=  ptr_reg;
       when '1'  =>  mx2_res  <=  "1000000000000";
		 when others =>
     end case;
   end process;

 MX3: process(IN_DATA, DATA_RDATA, mx_sel_3)
   begin
    case(mx_sel_3) is
      when "00"  =>  DATA_WDATA  <=  IN_DATA;
      when "01"  =>  DATA_WDATA  <=  DATA_RDATA - 1;
      when "10"  =>  DATA_WDATA  <=  DATA_RDATA + 1;
      when "11"  =>  DATA_WDATA  <=  DATA_RDATA;
		when others =>
    end case;
  end process;

 fsm_pstate: process (CLK, RESET, EN)
	begin
		if (RESET = '1') then
			state_current <= state_init;
		elsif (rising_edge(CLK)) then
			if (EN = '1') then
				state_current <= state_next;
			end if;
		end if;
	end process;

 FSM : process(state_current, IN_DATA, DATA_RDATA, OUT_BUSY,IN_VLD, mx_sel_1, mx_sel_2, mx_sel_3)
 begin
   pc_inc <= '0';
   pc_dec <= '0';

   ptr_inc <= '0';
   ptr_dec <= '0';

   cnt_inc <= '0';
   cnt_dec <= '0';

   mx_sel_1 <= '0';--pc
   mx_sel_2 <= '0';--ptr
   mx_sel_3 <= "11";--ptr

   DATA_EN <= '0';
   DATA_RDWR <= '0';
   OUT_WE <= '0';
   IN_REQ <= '0';

  case(state_current) is

     when state_init =>
      state_next <= state_fetch;

     when state_fetch =>
      DATA_EN <= '1';
      state_next <= state_decode;

     when state_decode =>
		mx_sel_2 <= '1';
       case (DATA_RDATA) is
         when X"3E" => state_next <= state_incpt;
         when X"3C" => state_next <= state_decpt;
         when X"2B" => state_next <= state_incel;
         when X"2D" => state_next <= state_decel;
         when X"5B" => state_next <= state_while;
         when X"5D" => state_next <= state_while_end;
         when X"2E" => state_next <= state_print;
         when X"2C" => state_next <= state_load;
         when X"24" => state_next <= state_save_tmp;
         when X"21" => state_next <= state_save_act;
         when X"00" => state_next <= state_end;
			when others =>
       end case;
       -----------
       ---| > |---
       -----------
       when state_incpt =>
        pc_inc <= '1';
        ptr_inc <= '1';
        state_next <= state_fetch;
       -----------
       ---| < |---
       -----------
       when state_decpt =>
        pc_inc <= '1';
        ptr_dec <= '1';
        state_next <= state_fetch;
       -----------
       ---| + |---
       -----------
       when state_incel =>
        mx_sel_1 <= '1'; --ptr
        mx_sel_2 <= '0'; --ptr
        DATA_EN <= '1';
        DATA_RDWR <= '0';--cita
        state_next <= state_incel_2;

       when state_incel_2 =>
        DATA_EN <= '1';
        DATA_RDWR <= '1';--pise na RDATA
        mx_sel_1 <= '1'; --+
        mx_sel_2 <= '0'; --+
        mx_sel_3 <= "10"; --rdata + 1
        pc_inc <= '1';
        state_next <= state_fetch;
       -----------
       ---| - |---
       -----------
       when state_decel =>
        DATA_EN <= '1';
        DATA_RDWR <= '0';--citam
        mx_sel_1 <= '1'; --ptr
        state_next <= state_decel_2;

       when state_decel_2 =>
        DATA_EN <= '1';
        DATA_RDWR <= '1';--write RDATA
        mx_sel_1 <= '1'; --ptr
        mx_sel_2 <= '0'; --
        mx_sel_3 <= "01"; --rdata -1
        pc_inc <= '1';
        state_next <= state_fetch;

       -----------
       ---| . |---
       -----------
       when state_print =>
       DATA_EN <= '1';
       DATA_RDWR <= '0';
       mx_sel_1 <= '1';--ptr
       state_next <= state_print_2;

       when state_print_2 =>
       case( OUT_BUSY ) is
         when '0' =>
          OUT_WE <= '1';
          OUT_DATA <= DATA_RDATA;--displayni rdata
          pc_inc <= '1';
          state_next <= state_fetch;
         when '1' =>---opakuj dokym not busy
    			DATA_EN <= '1';
    			DATA_RDWR <= '0';
          state_next <= state_print_2;
    			mx_sel_1 <= '1';
			 when others =>
       end case;
       -----------
       ---| , |---
       -----------
       when state_load =>
        IN_REQ <= '1';
        state_next <= state_load_2;

       when state_load_2 =>
        case( IN_VLD ) is
          when '0' =>
            IN_REQ <= '1';
            state_next <= state_load_2;---requestuj a opakuj dokym vld neni 1
          when '1' =>
            mx_sel_3 <= "00";--in_data
			    	mx_sel_1 <=  '1';--ptr
            DATA_EN <= '1';
            DATA_RDWR <= '1';--write
            pc_inc <= '1';
            state_next <= state_fetch;
			    when others =>
        end case;


        -----------
        ---| $ |---
        -----------
        when state_save_tmp =>
          DATA_EN <= '1';
          DATA_RDWR <= '0';--read
			    mx_sel_2 <= '0';--ptr
          mx_sel_1 <= '1';--ptr
          state_next <= state_save_tmp_2;

        when state_save_tmp_2 =>
          mx_sel_2 <= '1';--adr
          mx_sel_1 <= '1';--adr
			    DATA_EN <= '1';
          DATA_RDWR <= '1';--wrte
          pc_inc <= '1';
          state_next <= state_fetch;


          -----------
          ---| ! |---
          -----------
          when state_save_act =>
            DATA_EN <= '1';
            DATA_RDWR <= '0';--read
  			    mx_sel_2 <= '1';--adr
            mx_sel_1 <= '1';--adr
            state_next <= state_save_act_3;

          when state_save_act_3 =>
            DATA_EN <= '1';
            DATA_RDWR <= '1';--write
  			    mx_sel_2 <= '0';--ptr
            mx_sel_1 <= '1';--ptr
  			    pc_inc <= '1';
            state_next <= state_fetch;

            -----------
            ---| [ |---
            -----------
         when state_while =>
           pc_inc <= '1';---pc + 1
           DATA_EN <= '1';
           DATA_RDWR <= '0';--read
			     mx_sel_1 <= '1';--ptr
           state_next <= state_while_2;

         when state_while_2 =>
           state_next <= state_fetch;
			     mx_sel_1 <= '1';
           DATA_EN <= '1';
           DATA_RDWR <= '0';--mem[ptr]
           if (DATA_RDATA = "00000000") then---- if mem[ptr]=0 continue while
             state_next <= state_while_3;
				     cnt_inc <= '1';
           end if;

  			when state_while_3 =>
  				DATA_EN <= '1';
  				mx_sel_1 <= '0';---c mem[PC]
  				state_next <= state_while_4;

       when state_while_4 =>
         state_next <= state_while_5;
			   pc_inc <= '1';---pc + 1
         if (DATA_RDATA = X"5D") then---if ]
           cnt_dec <= '1';
		     elsif (DATA_RDATA = X"5B") then---if [
			     cnt_inc <= '1';
         end if;

			 when state_while_5 =>-----while cnt!=0
				if cnt_reg = "00000000" then
					state_next <= state_fetch;
				else
					state_next <= state_while_3;
				end if;

        -----------
        ---| ] |---
        -----------
      when state_while_end =>
        DATA_EN <= '1';
        DATA_RDWR <= '0';---read
  			mx_sel_1 <= '1';---ptr
			  state_next <= state_while_2_end;

		 when state_while_2_end =>
        state_next <= state_fetch;
        if (DATA_RDATA = "00000000") then---if mem[ptr]=0
			      pc_inc <= '1';--pc + 1
        else
			      cnt_inc <= '1';
            pc_dec <= '1';
            state_next <= state_while_3_end;
		    end if;

      when state_while_3_end =>
        state_next <= state_while_4_end;
				DATA_EN <= '1';
  			mx_sel_1 <= '0';--mem[pc]

			 when state_while_4_end =>
        state_next <= state_while_5_end;
				if (DATA_RDATA = X"5D") then---if ]
             cnt_inc <= '1';
				elsif (DATA_RDATA = X"5B") then---if [
				     cnt_dec <= '1';
				end if;

			 when state_while_5_end =>
        state_next <= state_fetch;
				if cnt_reg = "00000000" then --cnt = 0
					pc_inc <= '1'; --pc + 1
				else
					state_next <= state_while_3_end;--loop back
					pc_dec <= '1';--pc - 1
				end if;

        --------------
        ---| null |---
        --------------
       when state_end =>---konec, null
          state_next <= state_end;

       when others =>
          pc_inc <= '1';
			    state_next <= state_fetch;
  end case;

 end process;

end behavioral;
