/**
  * error.h
  * Project IFJ19
  *
  * Březina Jindřich (xbrezi21@stud.fit.vutbr.cz)
  * Gumančík Pavol (xguman01@stud.fit.vutbr.cz)
  * Kotáb Dominik (xkotab01@stud.fit.vutbr.cz)
  * Moravčík Tomáš (xmorav41@stud.fit.vutbr.cz)
  *
  * Brief: header for errors
  *
  */

  #ifndef _ERROR_H_
  #define _ERROR_H_

  #define OK 0
  #define LEXICAL_ERR 1 //chyba v programu v rámci lexikální analýzy
  #define PARSING_ERR 2 //chyba v programu v rámci syntaktické analýzy
  #define SEM_DEF_ERR 3 //sémantická chyba v programu – nedefinovaná funkce/proměnná, pokus o redefinici funkce/proměnné, atp.
  #define SEM_RUN_ERR 4 //sémantická/běhová chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech
  #define SEM_PAR_ERR 5 //sémantická chyba v programu – špatný počet parametrů u volání funkce.
  #define SEM_MISC_ERR 6 //ostatní sémantické chyby
  #define DIVISION_BY_ZERO 9 //běhová chyba dělení nulou.
  #define INTERN_ERR 99 //interní chyba překladače tj. neovlivněná vstupním programem (např. chyba alokace paměti, atd.).

#endif
