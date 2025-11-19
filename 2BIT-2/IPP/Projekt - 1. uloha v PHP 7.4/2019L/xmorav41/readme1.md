### Implementační dokumentace k 1. úloze do IPP 2019/2020
Jméno a příjmení: Tomáš Moravčík
Login: xmorav41

### Úloha
___
Zadanie pozostávalo z vytvorenia parsovacieho skriptu pre neštrukturovaný imperatívny jayzk *IPPcode20*, ktorý zanalyzuje vstup z lexikálneho a syntaktického hľadiska a za predpokladu správneho zápisu vygeneruje na výstup XML reprezentácia programu.
Skript *parse.php* je napísaný v jazyku PHP 7.4
### Štruktúra skriptu
___
Skript sa skladá zo 4 vlastných funkcií a 1 triedy. Hlavné telo pozostáva z prvotných inicializácií a prípravy na slučku, v ktorej sa bude vyhodnocovať vstup na základe jednotlivých riadkov.
### Priebeh skriptu
___
Činnosť skriptu sa začína kontrolou a spracovaním argumetov z terminálu pomocou vlastnej funckie a triedy, ktorá bude v sebe tieto údaju uchovávať.
Ďalej program postúpi na kontrolu hlavičky vstupu a jej prípadné ošetrenie (odstránenie komentárov, chybové hlásenie). Po tomto bode sa vytvorí hlavné telo XML výstupu, do ktorého sa budú vkladať dané inštruckie. Dostávame sa do slučky, v ktorej sa získa a pripravý reťazec s inštrukciou na jeho kontrolu, ktorá pozostáva zo switchu s prípadmi jednotlivých inštrukcií. 
V prípade nálezu inštrukcie sa spraví kontrola jej formátu prostredníctvom *regex* výrazov, a buď sa vyhodnotí na chybovú hlášku alebo sa vytvorí daná inštrukcia. Na jej vytvorenie sa zavolá jediná funkcia, ktorá dokáže spracovať všetky typy inštrukcií, a ktorá ďalej spolu s pomocnou funckiou vytvorí XML reprezentáciu inštrukcie s jej argumentmi. Slučka sa ukončí v prípade konca vstupu alebo pri zachytení chyby na vstupe.
