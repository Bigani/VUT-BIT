## IPK - Počítačové komunikace a sítě
## Projekt 1 - HTTP resolver doménových mien
### Autor: Tomáš Moravčík
#### Login: xmorav41
#### Jazyk: Python3

Spustenie:
```$ make run PORT=1234```
pričom číslo portu musí byť v rozsahu 1 - 65536

Skript vytvorí server, ktorý bude komunikovať s klientom prostredníctvom **GET** alebo **POST** žiadostí
Príklad pre **GET**:

Intput:
```sh 
$ curl localhost:1234/resolve?name=www.fit.vutbr.cz\&type=A
```
Output:
```sh 
www.fit.vutbr.cz:A=147.229.9.23
```

Príklad pre **POST**:

Intput:
```sh 
$ curl --data-binary @queries.txt -X POST http://localhost:1234/dns-query
```
Kde soubor queries.txt obsahuje toto:
www.fit.vutbr.cz:A
www.google.com:A
www.seznam.cz:A
147.229.14.131:PTR
ihned.cz:A

Output:
```sh 
www.fit.vutbr.cz:A=147.229.9.23
www.google.com:A=216.58.201.68www.seznam.cz:A=77.75.74.176
147.229.14.131:PTR=dhcpz131.fit.vutbr.cz
ihned.cz:A=46.255.231.42
```
