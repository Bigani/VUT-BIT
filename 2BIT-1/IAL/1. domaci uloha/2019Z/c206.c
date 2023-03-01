
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2019
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu,
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem,
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu,
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"


int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/

    //Ukazatele inicializuje na NULL
    L->First = NULL;
    L->Act = NULL;
    L->Last = NULL;


}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/

    L->Act = L->First;              //Pohybujeme sa zoznamom cez Act ukazovateľ
    tDLElemPtr tmpptr = L->Act;     //Dočasný ukazovateľ
    while (L->Act != L->Last)       //Dokým neprejde všetkými prvkami
    {
        L->Act = L->Act->rptr;      //Aktuálny sa stane nasledujúcim
        free(tmpptr);               //Uvoľnenie prvku z pamäti
        tmpptr = L->Act;            //Posun dočasného ukazovateľa
    }
	free(L->Last);		    //Uvolni posledný prvok

    //Ukazatele inicializuje na NULL
    L->First = NULL;
    L->Act = NULL;
    L->Last = NULL;

}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

    tDLElemPtr tmpptr = (tDLElemPtr) malloc(sizeof(struct tDLElem));    //Nový prvok
    if (!tmpptr)                                                //Kontrola
    {
        DLError();
        return;
    }

    //Nastavenie nového prvku
    tmpptr->data = val;
    tmpptr->rptr = L->First;
    tmpptr->lptr = NULL;

    if(L->First != NULL)                                        //Zoznam nie je prázdny
    {
        L->First->lptr = tmpptr;                                //Aktualizácia ľavej strany starého prvého prvku
    }
    else                                                        //Zoznam je prázdny
    {
        L->Last = tmpptr;
    }
    L->First = tmpptr;

}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

    tDLElemPtr tmpptr = (tDLElemPtr) malloc(sizeof(struct tDLElem));    //Nový prvok
    if (!tmpptr)                                                //Kontrola
    {
        DLError();
        return;
    }

    //Nastavenie nového prvku
    tmpptr->data = val;
    tmpptr->lptr = L->Last;
    tmpptr->rptr = NULL;

    if(L->Last != NULL)                                         //Zoznam nie je prázdny
    {
        L->Last->rptr = tmpptr;                                //Aktualizácia pravej strany starého posledného prvku
    }
    else                                                        //Zoznam je prázdny
    {
        L->First = tmpptr;
    }
    L->Last = tmpptr;

}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/

    L->Act = L->First;
;
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/

    L->Act = L->Last;

}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

    if (L == NULL || L->First == NULL)  //Kontrola
    {
        DLError();
        return;
    }

    *val = L->First->data;

}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

    if (L == NULL || L->Last == NULL)  //Kontrola
    {
        DLError();
        return;
    }

    *val = L->Last->data;

}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/

    if (L == NULL)  //Kontrola
    {
        return;
    }

    tDLElemPtr tmpptr = L->First;   //Dočasný ukazovateľ na prvý prvok
    if (tmpptr == L->Act)           //Ak je prvý prvok aktuálny tak sa aktivita stratí
    {
        L->Act = NULL;
    }
    if (tmpptr == L->Last)           //Ak je dočasný prvok posledným tak ukazovateľ posledného bude NULL
    {
        L->Last = NULL;
    }
    else
    {
        L->First->rptr->lptr = NULL;
    }

    L->First = tmpptr->rptr;      //Prvý sa stane druhým
    free(tmpptr);                   //Uvoľnenie z pamäti
}

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/

    if (L == NULL)  //Kontrola
    {
        return;
    }

    tDLElemPtr tmpptr = L->Last;  //Dočasný ukazovateľ na posledný prvok

    if (tmpptr == L->Act)         //Ak je posledný prvok aktuálny tak sa aktivita stratí
    {
        L->Act = NULL;
    }
    if (tmpptr == L->First)       //Ak je dočasný prvok prvým tak ukazovateľ prvého bude NULL
    {
        L->First = NULL;
    }
    else
    {
        L->Last->lptr->rptr = NULL;
    }

    L->Last = tmpptr->lptr;      //Posledný sa stane predposledným
    free(tmpptr);                 //Uvoľnenie z pamäti
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/

    if (!(L->Act) || L->Act == L->Last)     //Kontrola aktivity / posledného prvku
    {
        return;
    }

    tDLElemPtr tmpptr = L->Act->rptr;       //Dočasný ukazovateľ na nasledujúci prvok od aktívneho
    L->Act->rptr = L->Act->rptr->rptr;      //Nasledujúci prvok od aktívneho bude ukazovať o 2 ďalej napravo
    if (tmpptr == L->Last)                 //Ak je dočasný zároveň posledný tak sa posledný stane aktívnym
    {
        L->Last = L->Act;
    }
    else
    {
        tmpptr->rptr->lptr = L->Act;  //Aktívnym sa stane ľavá strana prvku čo je o 2 posunutý napravo od aktívneho
    }
    free(tmpptr);                           //Uvoľnenie z pamäti

}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/

    if (!(L->Act) || L->Act == L->First)    //Kontrola aktivity / posledného prvku
    {
        return;
    }

    tDLElemPtr tmpptr = L->Act->lptr;       //Dočasný ukazovateľ na predchádzajúci prvok od aktívneho
    L->Act->lptr = tmpptr->lptr;            //Predchádzajúci prvok od aktívneho bude ukazovať o 2 ďalej naľavo
    if (tmpptr == L->First)                 //Ak je dočasný zároveň prvý tak sa prvý stane aktívnym
    {
        L->First = L->Act;
    }
    else
    {
        L->Act->lptr->lptr->rptr->rptr = L->Act;  //Aktívnym sa stane pravá strana prvku čo je o 2 posunutý naľavo od aktívneho
    }
    free(tmpptr);                           //Uvoľnenie z pamäti

}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

    if (!L->Act)    //Kontrola
    {
        return;
    }

    tDLElemPtr tmpptr = (tDLElemPtr) malloc(sizeof(struct tDLElem));   //Nový prvok
    if (!tmpptr)                                                //Kontrola
    {
        DLError();
        return;
    }

    //Nastavenie nového prvku
    tmpptr->data = val;
    tmpptr->lptr = L->Act;
    tmpptr->rptr = L->Act->rptr;

    if (L->Act == L->Last)              //Ak aktívny je posledný tak sa nový stane posledným
    {
        L->Last = tmpptr;
    }
    else
    {
        L->Act->rptr->lptr = tmpptr;    //Inak ľavá strana nasledovného prvku bude ukazovať na nový prvok
    }

    L->Act->rptr = tmpptr;              //Nasledujúci prvok od aktívneho bude nový prvok

}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    if (!L->Act)    //Kontrola
    {
        return;
    }

    tDLElemPtr tmpptr = (tDLElemPtr) malloc(sizeof(struct tDLElem));
    if (!tmpptr)    //Kontrola
    {
        DLError();
        return;
    }

    //Nastavenie nového prvku
    tmpptr->data = val;
    tmpptr->rptr = L->Act;
    tmpptr->lptr = L->Act->lptr;

    if (L->Act == L->First)              //Ak aktívny je prvým prvkom tak sa nový stane prvým
    {
        L->First = tmpptr;
    }
    else
    {
        L->Act->lptr->rptr = tmpptr;    //Inak pravá strana predchádyajúceho prvku bude ukazovať na nový prvok
    }

    L->Act->lptr = tmpptr;              //Predchádzajúci prvok od aktívneho bude nový prvok

}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/

    if (!L->Act)    //Kontrola
    {
        DLError();
        return;
    }

    *val = L->Act->data;    //Zoberie hodnotu prvku
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/

    if (!L->Act)    //Kontrola
    {
        return;
    }

    L->Act->data = val;     //Uloží hodnotu val

}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/

    if (!L->Act)
    {
        return;
    }

    L->Act = L->Act->rptr;  //Presun aktivity
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/

    if (!L->Act)
    {
        return;
    }

    L->Act = L->Act->lptr;  //Presun aktivity
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/


    return (L->Act != NULL);
}

/* Konec c206.c*/


