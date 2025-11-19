## Sniffer paketov [ZETA] v C 
## 2. projekt do IPK 2019/2020
Meno a priezvisko: Tomáš Moravčík
Login: xmorav41

### Popis programu:
___
Tento program využíva knižnicu pcap.h určenú na monitorovanie sieťového prenosu. Sniffer zachytáva pakety TCP/UDP na určitom rozhraní s možnosťou filtrovania.  
### Funkcionalita:
***
Identifikácia TCP/UDP paketov
Rozlíšenie IPV4/IPV6
Filtrovanie TCP alebo UDP (len IPV4)
Počúvanie na vybranom porte/rozhraní
Výber počtu zobrazených paketov (implicitne 1)

### Príklad spustenia
***
```./ipk-sniffer -i wlp2s0 -p 22 --tcp --udp```

Názorný output:
```
18:16:27.000350 Kringe-Aspire : 58363 > merlin.fit.vutbr.cz : 22
0x0000: 10 fe ed a5 01 56 3c 95  09 96 0a f7 08 00 45 00    .....V<.......E.
0x0010: 00 3c 79 4f 40 00 40 06  bc 61 c0 a8 00 6a 93 e5    .<yO@.@..a...j..
0x0020: b0 13 e3 fb 00 16 1e 46  36 7f 00 00 00 00 a0 02    .......F6.......
0x0030: fa f0 05 3a 00 00 02 04  05 b4 04 02 08 0a 41 bf    ...:..........A.
0x0040: f8 5e 00 00 00 00 01 03  03 07                      .^........
```

### Zoznam súborov
***
ipk-sniffer.c
Makefile
manual.pdf
README.md
