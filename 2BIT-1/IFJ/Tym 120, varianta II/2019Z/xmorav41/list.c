/**
  * list.c
  * Project IFJ19
  *
  * Březina Jindřich (xbrezi21@stud.fit.vutbr.cz)
  * Gumančík Pavol (xguman01@stud.fit.vutbr.cz)
  * Kotáb Dominik (xkotab01@stud.fit.vutbr.cz)
  * Moravčík Tomáš (xmorav41@stud.fit.vutbr.cz)
  *
  * Brief: ial c206.c
  *
  */

/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
#include "list.h"
#include "scanner.h"

int errflg;

/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/
void DLError() {
    //printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = INTERN_ERR;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
  L->Act = NULL; //inicializace ukazatelů
  L->First = NULL;
  L->Last = NULL;
}

/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free.
**/
void DLDisposeList (tDLList *L) {
  tDLElemPtr temp;
  while (L->First != NULL) {
    L->Act = NULL;
    temp = L->First;
    L->First = L->First->rptr;
    free(temp);
  }

  DLInitList(L);
}

/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
void DLInsertFirst (tDLList *L, tInstr val) {
	tDLElemPtr temp = (tDLElemPtr) malloc(sizeof(struct tDLElem));
  if (temp == NULL) { //vypořádání se s errorem
    DLError();
    return;
  }

  temp->Instruction = val;
  temp->lptr = NULL;
  temp->rptr = L->First; //L->First může být NULL

  if (L->First) { //L není prázdné
    L->First->lptr = temp;
  } else { //L je prázdné
    L->Last = temp;
  }

  L->First = temp;
}

/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
tInstr *DLInsertLast(tDLList *L, tInstr val) {
  tDLElemPtr temp = (tDLElemPtr) malloc(sizeof(struct tDLElem));
  if (temp == NULL) { //vypořádání se s errorem
    DLError();
    return NULL;
  }

  temp->Instruction = val;
  temp->rptr = NULL;
  temp->lptr = L->Last;

  if(L->Last) { //L není prázdné
    L->Last->rptr = temp;
  } else {//L je prázdné
    L->First = temp;
  }

  L->Last = temp;
  return &(temp->Instruction);
}

/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
void DLDeleteFirst (tDLList *L) {
  tDLElemPtr temp = L->First;
  if (temp) {
    if (temp == L->Last) { //pokud je první zároveň poslední, poslední se taktéž ruší
      L->Last = NULL;
    }

    if (temp == L->Act) { //pokud je aktivní, aktivita se ztrácí
      L->Act = NULL;
    }

    if (L->First->rptr) { //pokud existuje druhej prvek, nebude ukazovat na první
      temp->rptr->lptr = NULL;
    }

    L->First = temp->rptr; //nastavujeme jako první prvek ten, co byl druhý
    free(temp);
  }
}

/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
void DLDeleteLast (tDLList *L) {
  tDLElemPtr temp = L->Last;
  if (temp) {
    if(temp == L->Act) { //pokud je aktivní, aktivita se ztrácí (aktivitu musíme rušit jako první)
      L->Act = NULL;
    }

    if (temp == L->First) { //pokud je poslední zároveň první, první se taktéž ruší
      L->First = NULL;
    }

    if (L->Last->lptr) { //pokud existuje předposlední prvek, nebude ukazovat na poslední
      temp->lptr->rptr = NULL;
    }

    L->Last = temp->lptr; //nastavujeme jako poslední ten, co byl předposlední
    free(temp);
  }
}

/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
void DLPostInsert (tDLList *L, tInstr val) {
  if(L->Act) { //musí být aktivní
    tDLElemPtr temp = (tDLElemPtr) malloc(sizeof(struct tDLElem));
    if (temp == NULL) { //vypořádání se s errorem
      DLError();
      return;
    }

    temp->Instruction = val; //přiřazení dat a předchozích/následujících prvků
    temp->lptr = L->Act;
    temp->rptr = L->Act->rptr;

    if(L->Act == L->Last) {
      L->Last = temp; //pokud je aktivní prvek poslední, je insertovaný prvek poslední
    } else {
      L->Act->rptr->lptr = temp; //pokud není poslední, tak nastavujeme aby prvek, který byl do této doby za aktivním prvkem, ukazoval na insertovaný prvek
    }
    L->Act->rptr = temp; //přiřazujeme prvek na správné místo
  }
}

/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
void DLSucc (tDLList *L) {
	if (L->Act) {
    L->Act = L->Act->rptr; //aktivita se přesouvá na prvek za aktivním
  }
}

tInstr *listGetData(tDLList *L)
// vrati aktivni instrukci
{
  if (L->Act == NULL)
  {
    errflg = INTERN_ERR;
    return NULL;
  }
  else return &(L->Act->Instruction);
}
