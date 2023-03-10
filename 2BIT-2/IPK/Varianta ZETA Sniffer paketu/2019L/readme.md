Zadanie:

Společná část popisu:
Vytvořte komunikující aplikaci podle konkrétní vybrané specifikace obvykle za použití libpcap a/nebo síťové knihovny BSD sockets (pokud není ve variantě zadání uvedeno jinak). Projekt bude vypracován v jazyce C/C++/C#. Individuální zadání specifikuje vlastní referenční systém (spustitelný např. pomocí aplikace VMWare Player nebo VirtualBox), pod kterým musí být projekt přeložitelný a spustitelný. Pokud jste ještě nikdy nevirtualizovali, třeba vám pomůže následující článek http://www.brianlinkletter.com/how-to-use-virtualbox-to-emulate-a-network/. Program by však měl být přenositelný.

Vypracovaný projekt uložený v archívu .tar a se jménem xlogin00.tar odevzdejte elektronicky přes IS.

Termín odevzdání je 3.5.2020 (hard deadline). Odevzdání e-mailem po uplynutí termínu, dodatečné opravy či doplnění kódu není možné.
Odevzdaný projekt musí obsahovat:
soubor se zdrojovým kódem (dodržujte jména souborů uvedená v konkrétním zadání),
funkční Makefile či jiné pomocné provozy pro úspěšný překlad či interpretaci zdrojového souboru,
dokumentaci (soubor manual.pdf), která bude obsahovat uvedení do problematiky, návrhu aplikace, popis implementace, tesování.
soubor README obsahující krátký textový popis programu s případnými rozšířeními/omezeními, příklad spuštění a seznam odevzdaných souborů,
další požadované soubory podle konkrétního typu zadání. 
Pokud v projektu nestihnete implementovat všechny požadované vlastnosti, je nutné veškerá omezení jasně uvést v dokumentaci a v souboru README.
Co není v zadání jednoznačně uvedeno, můžete implementovat podle svého vlastního výběru. Závažnější designová rozhodnutí popište v dokumentaci a README.
Při řešení projektu respektujte zvyklosti zavedené v OS unixového typu (jako je například formát textového souboru).
Vytvořené programy by měly být použitelné a smysluplné, řádně komentované a formátované a členěné do funkcí a modulů. Program by měl obsahovat nápovědu informující uživatele o činnosti programu a jeho parametrech. Případné chyby budou intuitivně popisovány uživateli.
Aplikace nesmí v žádném případě skončit s chybou SEGMENTATION FAULT ani jiným násilným systémovým ukončením (např. dělení nulou).
Pokud přejímáte krátké pasáže zdrojových kódů z různých tutoriálů či příkladů z Internetu (ne mezi sebou), tak je nutné vyznačit tyto sekce a jejich autory dle licenčních podmínek, kterými se distribuce daných zdrojových kódů řídí. V případě nedodržení bude na projekt nahlíženo jako na plagiát.
Před odevzdáním zkontrolujte, zda jste dodrželi všechna jména souborů požadovaná ve společné části zadání i v zadání pro konkrétní projekt. Zkontrolujte, zda je projekt přeložitelný.
Hodnocení projektu:
Maximální počet bodů za projekt je 20 bodů.
Maximálně 13 bodů za plně funkční aplikaci.
Maximálně 7 bodů za dokumentaci. Dokumentace se hodnotí pouze v případě funkčního kódu. Pokud kód není odevzdán nebo nefunguje podle zadání, dokumentace se nehodnotí.
Příklad kriterií pro hodnocení projektů:
nepřehledný, nekomentovaný zdrojový text: až -7 bodů
nefunkční či chybějící Makefile: až -4 body
nekvalitní či chybějící dokumentace: až -7 bodů
odevzdaný soubor nelze přeložit, spustit a odzkoušet: 0 bodů
odevzdáno po termínu: 0 bodů
nedodržení zadání: 0 bodů
nefunkční kód: 0 bodů
opsáno: 0 bodů (pro všechny, kdo mají stejný kód), návrh na zahájení disciplinárního řízení.
Popis varianty:
ZADÁNÍ:
1) Navrhněte a implementujte síťový analyzátor v C/C++/C#, který bude schopný na určitém síťovém rozhraním zachytávat a filtrovat pakety (13 b) 
2) Vytvořte relevantní manuál/dokumentaci k projektu (7b)
 
UPŘESNĚNÍ ZADÁNÍ:
Ad 1)
Volání programu:

./ipk-sniffer -i rozhraní [-p ­­port] [--tcp|-t] [--udp|-u] [-n num]

kde
-i eth0 (rozhraní, na kterém se bude poslouchat. Nebude-li tento parametr uveden, vypíše se seznam aktivních rozhraní)
-p 23 (bude filtrování paketů na daném rozhraní podle portu; nebude-li tento parametr uveden, uvažují se všechny porty)
-t nebo --tcp (bude zobrazovat pouze tcp pakety)
-u nebo --udp (bude zobrazovat pouze udp pakety)
Pokud nebude -tcp ani -udp specifikováno, uvažují se TCP a UDP pakety zároveň
-n 10 (určuje počet paketů, které se mají zobrazit; pokud není uvedeno, uvažujte zobrazení pouze 1 paket)

Formát výstupu:
čas IP|FQDN : port > IP|FQDN : port

počet_vypsaných_bajtů:  výpis_bajtů_hexa výpis_bajtů_ASCII

(takto vypíšete úplně celý paket)

Příklady volání:

./ipk-sniffer -i eth0 -p 23 --tcp -n 2
./ipk-sniffer -i eth0 --udp
./ipk-sniffer -i eth0 -n 10      
./ipk-sniffer -i eth0 -p 22 --tcp --udp   .... stejné jako:
./ipk-sniffer -i eth0 -p 22
./ipk-sniffer -i eth0

Příklady výstupu:

11:52:49.079012 pcvesely.fit.vutbr.cz : 4093 > 10.10.10.56 : 80

0x0000:  00 19 d1 f7 be e5 00 04  96 1d 34 20 08 00 45 00  ........ ..4 ..
0x0010:  05 a0 52 5b 40 00 36 06  5b db d9 43 16 8c 93 e5  ..R[@.6. [..C....
0x0020:  0d 6d 00 50 0d fb 3d cd  0a ed 41 d1 a4 ff 50 18  .m.P..=. ..A...P.
0x0030:  19 20 c7 cd 00 00 99 17  f1 60 7a bc 1f 97 2e b7  . ...... .`z.....
0x0040:  a1 18 f4 0b 5a ff 5f ac 07 71 a8 ac 54 67 3b 39  ....Z._. .q..Tg;9
0x0050:  4e 31 c5 5c 5f b5 37 ed  bd 66 ee ea b1 2b 0c 26  N1.\_.7. .f...+.&
0x0060:  98 9d b8 c8 00 80 0c 57  61 87 b0 cd 08 80 00 a1  .......W a.......


Netisknutelné znaky budou nahrazeny tečkou. Kde nepůjde zjistit doménové jméno, ponechte IP adresu.

Ad 2)
V dobré dokumentaci se OČEKÁVÁ následující: titulní strana, obsah, logické strukturování textu, výcuc relevantních informací z nastudované literatury, popis zajímavějších pasáží implementace, sekce o testování ( ve které kromě vlastního programu otestujete nějaký obecně známý open-source nástroj), bibliografie, popisy k řešení bonusových zadání.

DOPORUČENÍ:
Při implementaci použijte knihoven pcap / libnet
Pcap: http://www.tcpdump.org/pcap3_man.html
Libnet: http://www.packetfactory.net/projects/libnet/
U syntaxe vstupních voleb jednotlivým programům složené závorky {} znamenají, že volba je nepovinná (pokud není přítomna, tak se použíje implicitní hodnota), oproti tomu [] znamená povinnou volbu. Přičemž pořadí jednotlivých voleb a jejich parametrů může být libovolné. Pro jejich snadné parsování se doporučuje použít funkci getopt().
Výsledky vaší implementace by měly být co možná nejvíce multiplatformní mezi OS založenými na unixu, ovšem samotné přeložení projektu a funkčnost vaší aplikace budou testovány na referenčním Linux image pro síťové předměty (přihlašovací údaje student / student).
ODEVZDÁNÍ:
Součástí projektu budou zdrojové soubory přeložitelné na referenčním operačním systému, funkční Makefile (či pomocné provozy C#), soubor manual.pdf a README (viz obecné pokyny). Projekt odevzdejte jako jeden soubor xlogin00.tar, který vytvoříte programem tar.
