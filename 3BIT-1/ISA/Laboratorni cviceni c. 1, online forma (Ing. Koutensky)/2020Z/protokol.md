 # ISA 2020: Odpovědní arch pro cvičení č. 1

## Zjišťování konfigurace

### (1.) Rozhraní enp0s3

*MAC adresa*: 08:00:27:21:84:5d

*IPv4 adresa*: 10.0.2.15

*Délka prefixu*: 24

*Adresa síťe*: 10.0.2.0

*Broadcastová adresa*: 10.0.2.255

### (2.) Výchozí brána

*MAC adresa*: 52:54:00:12:35:02

*IPv4 adresa*: 10.0.2.2

### (4.) DNS servery

*Soubor*: /etc/resolv.conf

*DNS servery*: search kn.vutbr.cz
nameserver 147.229.191.143
nameserver 147.229.190.143


### (5.) Ping na výchozí bránu

*Soubor*: /etc/hosts

*Úprava*: "\\n 10.0.2.2 gw"

### (6.) TCP spojení

*Záznam + popis*:

| State | Recv-Q | Send-Q | Local Address:Port | Peer Address:Port   |
|-------|--------|--------|--------------------|---------------------|
| ESTAB | 0      | 0      | 10.0.2.15:42500    | 195.113.232.80:http |
| ESTAB | 0      | 0      | 10.0.2.15:43968    | 99.86.243.93:https  |



State: stav socketu<br/>
Recv-Q: počet prijatých paketov<br/>
Send-Q: počet odoslaných paketov<br/>
Local Address:Port: Lokálna adresa a číslo portu<br/>
Peer Address:Port: Vzdialená adresa a číslo portu<br/>


### (8.) NetworkManager události

*Příkaz*: journalctl -u NetworkManager

### (9.) Chybová hláška sudo

*Příkaz*: sudo wireshark

*Chybová hláška*: <br/>Oct 03 14:07:20 localhost.localdomain sudo[21800]:  user : command not allowed ; TTY=pts/0 ; PWD=/ ; USER=root ; COMMAND=/bin/wireshark

## Wireshark

### (1.) Capture filter

*Capture filter*: tcp port 80

### (2.) Zachycená HTTP komunikace

Komu patří nalezené IPv4 adresy a MAC adresy?<br/>
Patria odosielateľovi a prijímateľovi danej komunikácie <br/>
Vypisovali jste již některé z nich?<br/>
Áno, v prvej úlohe<br/>
Proč tomu tak je?<br/>
Aby odosielteľ dokázal poslať paket správnemu prijímateľovi a prijímateľ aby mohol odoslať odpoveď správenu odosielateľovi<br/>

#### Požadavek HTTP

Cílová MAC adresa

  - *Adresa*: 52:54:00:12:35:02
  - *Role zařízení*: prijímateľ (gateway)

Cílová IP adresa

  - *Adresa*: 147.229.177.179
  - *Role zařízení*: prijímateľ (server)

Zdrojová MAC adresa

  - *Adresa*: 08:00:27:21:84:5d
  - *Role zařízení*: odosielateľ (klient)

Zdrojová IP adresa

  - *Adresa*: 10.0.2.15
  - *Role zařízení*: odosielateľ (klient)


#### Odpověď HTTP

Cílová MAC adresa

  - *Adresa*: 08:00:27:21:84:5d
  - *Role zařízení*: odosielateľ (klient)

Cílová IP adresa

  - *Adresa*: 10.0.2.15
  - *Role zařízení*: odosielateľ (klient)

Zdrojová MAC adresa

  - *Adresa*: 52:54:00:12:35:02
  - *Role zařízení*: prijímateľ (gateway)

Zdrojová IP adresa

  - *Adresa*: 147.229.177.179
  - *Role zařízení*: prijímateľ (server)

### (3.) Zachycená ARP komunikace

*Display filter*: icmp or arp

### (6.) Follow TCP stream

*Follow TCP stream*: Trafika od klienta ku serveru je vyznačená červenou a od serveru ku klientu modrou.<br/>
*Follow TCP stream*: Jeho význam je ukázať užívateľovi protokol z pohľadu aplikačnej vrstvy. Používa sa pre porozumenie dátového toku.<br/>
