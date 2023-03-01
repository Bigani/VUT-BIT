# ISA 2020: Odpovědní arch pro cvičení č. 2

## Vzdálený terminál - SSH, Secure Shell

### (2.) Bezpečné připojení na vzdálený počítač bez autentizačních klíčů

*Verze OpenSSH*: OpenSSH_7.4p1

*Vybraná podporovaná šifra*: chacha20-poly1305@openssh.com

*Co lze zjistit o obsahu komunikace?*: ssh verzia, podporované šifry, mechanizmus HMAC, mechanizmus výmeny kľúčov

### (3.) Vytvoření veřejného a privátního klíče

*Jak se liší práva mezi souborem s privátním a veřejným klíčem?*: S verejným kľúčom má read-only práva a s privátnym možnosť i modifikovať súbory

### (4.) Distribuce klíčů

*Jaká hesla bylo nutné zadat při kopírovaní klíčů?*: user4lab a root4lab

*Jaká hesla bylo nutné zadat při opětovném přihlášení?*: passphrase "fitvutisa"

### (6.) Pohodlné opakované použití klíče

*Museli jste znovu zadávat heslo?*: nie

## Zabezpečení transportní vrstvy - TLS, Transport Layer Security

### (1.) Nezabezpečený přenos dat

*Je možné přečíst obsah komunikace?*: áno

### (2.) Přenos dat zabezpečený TLS

*Je možné přečíst obsah komunikace?*: nie
