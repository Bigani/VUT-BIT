### Implementační dokumentace k 2. úloze do IPP 2019/2020
Jméno a příjmení: Tomáš Moravčík
Login: xmorav41

## Časť prvá: interpret.py
### Úloha
___
Vytvorenie interpretu pre jazyk **IPPcode20**, ktorého vstupom je **XML** reprezentácia tohto jazyka a výstupom je vygenerovaný **IPPcode20** výstup. Skript je napísaný v jazyku Python 3. 

### Priebeh 
***
Skript sa pomocou knižnice **xml.etree.ElementTree** pokúsi spracovať **XML** vstupný súbor. V úspešnom prípade sparsuje vstupné argumenty a pokračuje ich úpravou. 
V triede **MAIN** zoradí inštrukcie podľa atribúty **order**, vytvorí list s návestiami a až tak začne cyklicky spracovávať inštrukcie. Toto zabezpečí trieda **INST**, ktorá identifikuje inštrukciu podľa atributy **opcode** a vykoná danú inštrukciu. Drvivá väčšina využíva triedu **FRAMES**, v ktorej je najviac metód. Tu sa odohráva narábanie s dátami **IPPcode20**, ako napríklad ich uloženie, prepísanie, zistenie typu, nastavenie framu a iných. Okrem metód obsahuje asociatívne polia pomenované globálny, lokálny a dočasný rámec, ktoré obsahujú názov premennej ako **key** a hodnotu premennej ako **value**.
V prípade akom som spracovával dáta, neukladal som informáciu o ich type ale zisťoval ju priamo z hodnoty. V určitých prípadoch pri **IPPcode20** som musel robiť špeciálne ošetrenia, najmä pre prípady **nil@nil**. Na to som vytvoril  **customNil**, ktorý ma hodnotu špecifickú hodnotu stringu so špeciálnymi charaktermi, ktorý využívam pri porovnávaní a ošetrovaní pre prípady s nilom.  Príklad zistenia typu nil z hodnoty premennej: ``elif tmpVal == customNil: return 'nil'``
Po úspešnom spracovaní všetkých inštrukcií sa vypíše ich výstup na **STDOUT** a program sa ukončí.
___
___
___

## Časť druhá: test.php
### Úloha
___
Vytvorenie skriptu v PHP7.8, ktorý spustí testovanie skriptov parse.php a interpret.py. Výstupom je prehľadná html tabulka, ktorá popisuje úspešnosť v rámci skupiny testov.  
### Priebeh
___
Generovanie html jazyka je dynamické za behu programu, počas každého testu sa vytvorí informácia o jeho vyhodnotení a napojí sa na predchádzajúcu. Hlavné telo sa skladá z troch častí, určené pre typ testovania; **'parse-only', 'int-only'** a **'both'**.
Skript po sparsovaní argumentov naplní listy súbormi z adresára, argument **'--recursive'** rozhodne, či hľadať testy vo všetkých zložkách adresára. Ďalej prebehne kontrola prítomnosti dodatočných súborov  **.rc** **.in** a **.out**, pri nenájdení sa vytvoria s implicitnými hodnotami. 
Následne sa listy **srcFiles, rcFiles, inFiles a outFiles** zoradia pomocou **Bubble Sortu** pre zaistenie uniformného indexovania medzi jednotlivými súbormi testu.
Inicializujú sa listy pre výsledné hodnoty testov a rozhodne sa nad typom testovania. Každý typ má svoju špecifickú **html** tabulku, ktorú napĺňa. Priebeh jednotlivých typov je podobný, najväčším rozdielom je skript(y), ktorý sa testuje. Zavolá sa pomocou príkazu **exec()**, ktorý sa môže pohodlne naplniť meniacimi sa hodnotami ako napríklad cesta k súboru. 
V prípade **'parse-only'** sa používa na vyhodnotenie testov **A7Soft JExamXML**, inak unixový príkaz **diff**.
Na základe vyhodnotenia testu sa pripojí príslušný **html** kód predstavujúci jeden riadok k cieľovému **html** reťazcu a iteruje sa, dokým sa nespustia všetky testy v liste **srcFiles**.
