-- Autor reseni: Tomáš Moravčík xmorav41

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
port ( -- Sem doplnte popis rozhrani obvodu.
      RESET      : IN std_logic;
      SMCLK      : IN std_logic; -- SMCLK/256 = 7 372 800/256 = 28800; 28800/8 = 3600
      ROW		     : OUT std_logic_vector (0 to 7);
		  LED   	   : OUT std_logic_vector (7 downto 0)
    );
end ledc8x8;

architecture main of ledc8x8 is

    -- Sem doplnte definice vnitrnich signalu.

    signal riadok:      std_logic_vector (7 downto 0) := (others => '0');  -- 8 riadkov
    signal leds:        std_logic_vector (7 downto 0); 							       -- 8 stlpcov
    signal enable_led:  std_logic_vector (11 downto 0) := (others => '0'); -- Frekvencia pre zmenu riadku
    signal stav_tma:    std_logic_vector (22 downto 0) := (others => '0'); -- Pol sekunda 1110000100000000000000(2) Sekunda 1110000100000000000000(2) tickov 
    signal checkpoint1: boolean := true;  -- Pre prepnutie riadka
    signal checkpoint2: boolean := false; -- Pre prve zhasnutie
	  signal checkpoint3: boolean := false; -- Pre druhe zasvietenie

begin
    --Frekvencia premien riadkov
    stav1: process(SMCLK, RESET, enable_led)
    begin
        --Urobi reset
        if (RESET = '1') then
          enable_led <= (others => '0');

        --Rata upravenu frenkvenciu
        elsif rising_edge(SMCLK) then
            enable_led <= enable_led + 1;
        end if;

    end process stav1;
	  checkpoint1 <= true when enable_led = "111000010000" else false;
    ------------------

    --Tma pol sekundy
    stav2: process(SMCLK, RESET,stav_tma,checkpoint2,checkpoint3)
    begin
      --Urobi reset
      if (RESET = '1') then
        stav_tma <= (others => '0');

      --Prepocitava do 1s
			elsif (rising_edge(SMCLK) AND not checkpoint3) then
			  stav_tma <= stav_tma + 1;
	  	end if;

    end process stav2;
    checkpoint2 <= true when stav_tma >= "01110000100000000000000" AND stav_tma <= "11100001000000000000000" else false;
    checkpoint3 <= true when stav_tma = "11100001000000000000000" else false;
	 ------------------

    prechod_riadkami: process (SMCLK,RESET,checkpoint1,enable_led)
    begin
      if RESET = '1' then                                   --Ak je začiatok tak je aktívny prvý riadok
  			riadok <= "10000000";
  		elsif rising_edge(SMCLK) then
        if (checkpoint1) then
  			     riadok <= riadok(0) & riadok(7 downto 1);      --V riadku sa posunie jednotka skrze konkatenáciu
  		  end if;
      end if;
    end process prechod_riadkami;
	 ------------------

    -- Ledky s iniciálkami --
    inicialy: process (riadok,checkpoint2)
    begin
      if (checkpoint2 AND not checkpoint3) then
		  leds <= "11111111";
		else
        case riadok is
              when "10000000" => leds <= "00000111";
              when "01000000" => leds <= "11011111";
              when "00100000" => leds <= "11011111";
              when "00010000" => leds <= "11001110";
              when "00001000" => leds <= "11000100";
              when "00000100" => leds <= "11101010";
              when "00000010" => leds <= "11101110";
              when "00000001" => leds <= "11101110";
              when others =>     leds <= "11111111";
        end case;
      end if;

    end process inicialy;

    LED <= leds;
    ROW <= riadok;

end main;




-- ISID: 75579
