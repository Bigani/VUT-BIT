/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2014
**                              Radek Hranický, 2014-2018
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

void htInit ( tHTable* ptrht ) {

    if (ptrht == NULL) return;
    memset(*ptrht,0,sizeof(struct tHTItem*)*HTSIZE); //Nastaví ukazovatele na NULL

}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není,
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {

    if (ptrht == NULL) return NULL;

    int hC = hashCode(key);             //index daný funkciou hashCode
    tHTItem *tmp = (*ptrht)[hC];        //Ukazovatel na indexe hC
    while(tmp != NULL){
        if (strcmp(tmp->key,key) == 0){ //Ak sa kľúče rovnajú vráť ukazovateľ
            return tmp;
        }
        else
            tmp = tmp->ptrnext;         //Ďalší prvok
    }
    return NULL;                        //Prvok nie je v tabuľke
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {

    if (ptrht == NULL) return;
    tHTItem *tmp = htSearch(ptrht,key);	//Skúsi nájsť a prepísať prvok ak existuje
    if (tmp != NULL){
        tmp->data = data;
        return;
    }
    int hC = hashCode(key);
    tHTItem *new = (tHTItem *) malloc(sizeof(tHTItem));	//Nový prvok
    if (new == NULL) return;

    new->data = data;
    new->key = key;
    new->ptrnext = (*ptrht)[hC];
    (*ptrht)[hC] = new;	//Uloží na začiatok
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {

    if (ptrht == NULL) return NULL;

    tHTItem *tmp = htSearch(ptrht,key);
    if (tmp != NULL)
        return &tmp->data;	//Vráti dáta
    else
        return NULL;				//Prvok sa nenašiel

}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {

    int hC = hashCode(key);             //index daný funkciou hashCode
    tHTItem *tmp = (*ptrht)[hC];        //Ukazovatel na prechod/uvoľnenie
    tHTItem *tmp2 = NULL;        				//Ukazovatel na predchadzajuci prvok (korekcia prepojení)

    while(tmp != NULL){
        if (strcmp(tmp->key,key) == 0){ 			//Kľúče sa rovnajú
            if (tmp2 != NULL){          			//Hľadaný prvok nie je prvý preto uprav prepojenie
                tmp2->ptrnext = tmp->ptrnext;
            }
            else {
                (*ptrht)[hC] = tmp->ptrnext;	//Hľadaný prvok je prvý preto sa ďalší stane prvým
            }
            free(tmp);
            return;
        }
        else{
            tmp2 = tmp;
            tmp = tmp->ptrnext;         //Posun

        }
    }

}

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll ( tHTable* ptrht ) {

    tHTItem *tmp;        //Ukazovatel na prechod/uvoľnenie
    tHTItem *tmp2;       //Ukazovatel na predchadzajuci prvok (korekcia prepojení)

    for (int i = 0; i < HTSIZE; i++) {	//Prejdi celým zoznamom

        tmp = (*ptrht)[i];
        while(tmp != NULL){							//Prechádzaj synonymami a maž ich
            tmp2 = tmp;
            tmp = tmp->ptrnext;
            free(tmp2);
            }
        }
    htInit(ptrht);											//Uveď do počiatočného stavu
    }
