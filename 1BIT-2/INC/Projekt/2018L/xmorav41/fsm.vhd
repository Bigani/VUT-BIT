-- fsm.vhd: Finite State Machine
-- Author(s): Tomáš Moravčík <xmorav41>
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (TEST1,TEST_2A,TEST_2B,TEST_3A,TEST_3B,TEST_4A,TEST_4B,TEST_5A,TEST_5B,TEST_6A,TEST_6B,TEST_7A,TEST_7B,TEST_8A,TEST_8B,TEST_9A,TEST_9B,TEST_10A,TEST_10B, RESULT, ACCESS_DENIED, ACCESS_PERMITTED, FAIL, FINISH);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= TEST1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST1 => -- Kod A / B => 1 / 3
      next_state <= TEST1;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(1) = '1') then
         next_state <= TEST_2A;
      elsif (KEY(3) = '1') then
         next_state <= TEST_2B;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_2A => -- 10
      next_state <= TEST_2A;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(0) = '1') then
         next_state <= TEST_3A;
       elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_3A => -- 101
      next_state <= TEST_3A;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(1) = '1') then
         next_state <= TEST_4A;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_4A => --1011
      next_state <= TEST_4A;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(1) = '1') then
         next_state <= TEST_5A;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_5A => --10111
      next_state <= TEST_5A;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(1) = '1') then
         next_state <= TEST_6A;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_6A => --101116
      next_state <= TEST_6A;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(6) = '1') then
         next_state <= TEST_7A;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_7A => --1011167
      next_state <= TEST_7A;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(7) = '1') then
         next_state <= TEST_8A;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_8A => --10111670
      next_state <= TEST_8A;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(0) = '1') then
         next_state <= TEST_9A;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_9A => --101116704
      next_state <= TEST_9A;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(4) = '1') then
         next_state <= TEST_10A;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_10A => --1011167045
      next_state <= TEST_10A;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(5) = '1') then
         next_state <= RESULT;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_2B => --35
      next_state <= TEST_2B;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(5) = '1') then
         next_state <= TEST_3B;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_3B => --353
      next_state <= TEST_3B;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(3) = '1') then
         next_state <= TEST_4B;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_4B => --3539
      next_state <= TEST_4B;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(9) = '1') then
         next_state <= TEST_5B;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_5B => --35390
      next_state <= TEST_5B;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(0) = '1') then
         next_state <= TEST_6B;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_6B => --353908
      next_state <= TEST_6B;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(8) = '1') then
         next_state <= TEST_7B;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_7B => --3539084
      next_state <= TEST_7B;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(4) = '1') then
         next_state <= TEST_8B;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_8B => --35390846
      next_state <= TEST_8B;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(6) = '1') then
         next_state <= TEST_9B;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_9B => --353908466
      next_state <= TEST_9B;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(6) = '1') then
         next_state <= TEST_10B;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST_10B => --3539084660
      next_state <= TEST_10B;
      if (KEY(15) = '1') then
         next_state <= ACCESS_DENIED;
      elsif (KEY(0) = '1') then
         next_state <= RESULT;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when RESULT =>
      next_state <= RESULT;
      if (KEY(15) = '1') then
        next_state <= ACCESS_PERMITTED;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FAIL =>
      next_state <= FAIL;
   if (KEY(15) = '1') then
      next_state <= ACCESS_DENIED;
   end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ACCESS_PERMITTED =>
      next_state <= ACCESS_PERMITTED;
   if (CNT_OF = '1') then
      next_state <= FINISH;
   end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ACCESS_DENIED =>
      next_state <= ACCESS_DENIED;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= TEST1;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= TEST1;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ACCESS_DENIED =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
      FSM_MX_MEM     <= '0';--nay
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ACCESS_PERMITTED =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_LCD_WR     <= '1';
      FSM_MX_MEM     <= '1';--yay
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
     if (KEY(14 downto 0) /= "000000000000000") then
        FSM_LCD_WR     <= '1';
     end if;
     if (KEY(15) = '1') then
        FSM_LCD_CLR    <= '1';
     end if;

   end case;
end process output_logic;

end architecture behavioral;
