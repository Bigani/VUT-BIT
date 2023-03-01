/* c401.c: **********************************************************}
{* Téma: Rekurzivní implementace operací nad BVS
**                                         Vytvořil: Petr Přikryl, listopad 1994
**                                         Úpravy: Andrea Němcová, prosinec 1995
**                                                      Petr Přikryl, duben 1996
**                                                   Petr Přikryl, listopad 1997
**                                  Převod do jazyka C: Martin Tuček, říjen 2005
**                                         Úpravy: Bohuslav Křena, listopad 2009
**                                                 Karel Masařík, říjen 2013
**                                                 Radek Hranický 2014-2018
**
** Implementujte rekurzivním způsobem operace nad binárním vyhledávacím
** stromem (BVS; v angličtině BST - Binary Search Tree).
**
** Klíčem uzlu stromu je jeden znak (obecně jím může být cokoliv, podle
** čeho se vyhledává). Užitečným (vyhledávaným) obsahem je zde integer.
** Uzly s menším klíčem leží vlevo, uzly s větším klíčem leží ve stromu
** vpravo. Využijte dynamického přidělování paměti.
** Rekurzivním způsobem implementujte následující funkce:
**
**   BSTInit ...... inicializace vyhledávacího stromu
**   BSTSearch .... vyhledávání hodnoty uzlu zadaného klíčem
**   BSTInsert .... vkládání nové hodnoty
**   BSTDelete .... zrušení uzlu se zadaným klíčem
**   BSTDispose ... zrušení celého stromu
**
** ADT BVS je reprezentován kořenovým ukazatelem stromu (typ tBSTNodePtr).
** Uzel stromu (struktura typu tBSTNode) obsahuje klíč (typu char), podle
** kterého se ve stromu vyhledává, vlastní obsah uzlu (pro jednoduchost
** typu int) a ukazatel na levý a pravý podstrom (LPtr a RPtr). Přesnou definici typů
** naleznete v souboru c401.h.
**
** Pozor! Je třeba správně rozlišovat, kdy použít dereferenční operátor *
** (typicky při modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem
** (např. při vyhledávání). V tomto příkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, použijeme dereferenci.
**/

#include "c401.h"
int solved;

void BSTInit (tBSTNodePtr *RootPtr) {
/*   -------
** Funkce provede počáteční inicializaci stromu před jeho prvním použitím.
**
** Ověřit, zda byl již strom předaný přes RootPtr inicializován, nelze,
** protože před první inicializací má ukazatel nedefinovanou (tedy libovolnou)
** hodnotu. Programátor využívající ADT BVS tedy musí zajistit, aby inicializace
** byla volána pouze jednou, a to před vlastní prací s BVS. Provedení
** inicializace nad neprázdným stromem by totiž mohlo vést ke ztrátě přístupu
** k dynamicky alokované paměti (tzv. "memory leak").
**
** Všimněte si, že se v hlavičce objevuje typ ukazatel na ukazatel.
** Proto je třeba při přiřazení přes RootPtr použít dereferenční operátor *.
** Ten bude použit i ve funkcích BSTDelete, BSTInsert a BSTDispose.
**/


	*RootPtr = NULL; //Priradenie NULL na hodnotu ukazovatela RootPtr
}

int BSTSearch (tBSTNodePtr RootPtr, char K, int *Content)	{
/*  ---------
** Funkce vyhledá uzel v BVS s klíčem K.
**
** Pokud je takový nalezen, vrací funkce hodnotu TRUE a v proměnné Content se
** vrací obsah příslušného uzlu.´Pokud příslušný uzel není nalezen, vrací funkce
** hodnotu FALSE a obsah proměnné Content není definován (nic do ní proto
** nepřiřazujte).
**
** Při vyhledávání v binárním stromu bychom typicky použili cyklus ukončený
** testem dosažení listu nebo nalezení uzlu s klíčem K. V tomto případě ale
** problém řešte rekurzivním volání této funkce, přičemž nedeklarujte žádnou
** pomocnou funkci.
**/

    if (RootPtr == NULL) return FALSE; //Nenašli sme

    if (RootPtr->Key == K) {           //Našli sme
        *Content = RootPtr->BSTNodeCont;
        return TRUE;
    }
    else
        if (RootPtr->Key > K)          //Ak je K menšie ako momentálny Key tak pozri do ľavého podstromu inak do pravého
            return (BSTSearch(RootPtr->LPtr,K,Content));
        else
            return (BSTSearch(RootPtr->RPtr,K,Content));


}


void BSTInsert (tBSTNodePtr* RootPtr, char K, int Content)	{
/*   ---------
** Vloží do stromu RootPtr hodnotu Content s klíčem K.
**
** Pokud již uzel se zadaným klíčem ve stromu existuje, bude obsah uzlu
** s klíčem K nahrazen novou hodnotou. Pokud bude do stromu vložen nový
** uzel, bude vložen vždy jako list stromu.
**
** Funkci implementujte rekurzivně. Nedeklarujte žádnou pomocnou funkci.
**
** Rekurzivní implementace je méně efektivní, protože se při každém
** rekurzivním zanoření ukládá na zásobník obsah uzlu (zde integer).
** Nerekurzivní varianta by v tomto případě byla efektivnější jak z hlediska
** rychlosti, tak z hlediska paměťových nároků. Zde jde ale o školní
** příklad, na kterém si chceme ukázat eleganci rekurzivního zápisu.
**/

	if (*RootPtr == NULL){
			(*RootPtr) = (tBSTNodePtr) malloc(sizeof(struct tBSTNode)); //Alokacia noveho uzlu
			if (*RootPtr == NULL) return;

			//Vytovrenie noveho uzlu
			(*RootPtr)->Key = K;
			(*RootPtr)->BSTNodeCont = Content;
			(*RootPtr)->LPtr = NULL;
			(*RootPtr)->RPtr = NULL;
	}
	else
	if ((*RootPtr)->Key > K)       //K je mensie teda vkladame vlavo
			BSTInsert(&(*RootPtr)->LPtr, K, Content);
	else if ((*RootPtr)->Key < K)  //K je vacsie teda vkladame vpravo
			BSTInsert(&(*RootPtr)->RPtr, K, Content);
	else if ((*RootPtr)->Key == K) //K je rovne teda prepisujeme
			(*RootPtr)->BSTNodeCont = Content;
}

void ReplaceByRightmost (tBSTNodePtr PtrReplaced, tBSTNodePtr *RootPtr) {
/*   ------------------
** Pomocná funkce pro vyhledání, přesun a uvolnění nejpravějšího uzlu.
**
** Ukazatel PtrReplaced ukazuje na uzel, do kterého bude přesunuta hodnota
** nejpravějšího uzlu v podstromu, který je určen ukazatelem RootPtr.
** Předpokládá se, že hodnota ukazatele RootPtr nebude NULL (zajistěte to
** testováním před volání této funkce). Tuto funkci implementujte rekurzivně.
**
** Tato pomocná funkce bude použita dále. Než ji začnete implementovat,
** přečtěte si komentář k funkci BSTDelete().
**/

if (*RootPtr == NULL) return; //Ukazuje nikam

tBSTNodePtr toMove; //Pomocný uzol ktorý uvoľníme

if ((*RootPtr)->RPtr == NULL) { //Nemá pravý podstrom
		toMove = (*RootPtr);

		PtrReplaced->BSTNodeCont = (*RootPtr)->BSTNodeCont; //Nahradi obsah
		PtrReplaced->Key = (*RootPtr)->Key; 					 			//Nahradí key

		if ((*RootPtr)->LPtr != NULL) PtrReplaced->LPtr = (*RootPtr)->LPtr; //Kontrola či existuje ľavý podstrom, v takom prípade ho správne presunúť ako ľavý podstrom PtrReplaced
		else PtrReplaced->LPtr = NULL;

		free((toMove)); //Uvolni najpravejsi
		return;
}

if ((*RootPtr)->RPtr->RPtr == NULL){ //Pravý podstrom nemá pravý podstrom
		toMove = (*RootPtr)->RPtr;

		PtrReplaced->BSTNodeCont = (*RootPtr)->RPtr->BSTNodeCont; //Nahradí obsah
		PtrReplaced->Key = (*RootPtr)->RPtr->Key;									//Nahradí key

		if ((*RootPtr)->RPtr->LPtr != NULL) (*RootPtr)->RPtr = (*RootPtr)->RPtr->LPtr; //Kontrola či existuje ľavý podstrom, v takom prípade ho správne presunúť ako pravý podstrom vyššieho uzlu
		else (*RootPtr)->RPtr = NULL;

		free(toMove); //Uvolni najpravejsi
		return;

}

ReplaceByRightmost(PtrReplaced,&((*RootPtr)->RPtr));

}

void BSTDelete (tBSTNodePtr *RootPtr, char K) 		{
/*   ---------
** Zruší uzel stromu, který obsahuje klíč K.
**
** Pokud uzel se zadaným klíčem neexistuje, nedělá funkce nic.
** Pokud má rušený uzel jen jeden podstrom, pak jej zdědí otec rušeného uzlu.
** Pokud má rušený uzel oba podstromy, pak je rušený uzel nahrazen nejpravějším
** uzlem levého podstromu. Pozor! Nejpravější uzel nemusí být listem.
**
** Tuto funkci implementujte rekurzivně s využitím dříve deklarované
** pomocné funkce ReplaceByRightmost.
**/

    if (*RootPtr == NULL) return;

    else if ((*RootPtr)->Key > K) {     //K je menšie teda hľadame vlavo

        if (((*RootPtr)->LPtr != NULL) && ((*RootPtr)->LPtr->Key == K)){

            tBSTNodePtr toDelete = (*RootPtr)->LPtr; //Pomocný uzol

            if (((*RootPtr)->LPtr->RPtr == NULL) && ((*RootPtr)->LPtr->LPtr == NULL)){      //Nemá podstrom
                (*RootPtr)->LPtr = NULL;
                free(toDelete);
                return;
            }

            else if (((*RootPtr)->LPtr->RPtr != NULL) && ((*RootPtr)->LPtr->LPtr != NULL)){ //Má oba podstromy
                ReplaceByRightmost((*RootPtr)->LPtr,&((*RootPtr)->LPtr->LPtr));  //Nahradí rušený uzol najpravejším z ľava
                return;
            }

            else if (((*RootPtr)->LPtr->RPtr != NULL) && ((*RootPtr)->LPtr->LPtr == NULL)){ //Má pravý podstrom
                (*RootPtr)->LPtr = (*RootPtr)->LPtr->RPtr;  //Prepojenie ľavej strany s novopríslušným uzlom
                free(toDelete);
                return;
            }
            else if (((*RootPtr)->LPtr->RPtr == NULL) && ((*RootPtr)->LPtr->LPtr != NULL)){ //Má ľavý podstrom
                (*RootPtr)->LPtr = (*RootPtr)->LPtr->LPtr;  //Prepojenie ľavej strany s novopríslušným uzlom
                free(toDelete);
                return;
            }
        }

        BSTDelete(&((*RootPtr)->LPtr),K);
        return;
    }


    else if ((*RootPtr)->Key < K) {  //K je väcsie teda hľadame vpravo

        if (((*RootPtr)->RPtr != NULL) && ((*RootPtr)->RPtr->Key == K)){

            tBSTNodePtr toDelete = (*RootPtr)->RPtr; //Pomocný uzol

            if (((*RootPtr)->RPtr->RPtr == NULL) && ((*RootPtr)->RPtr->LPtr == NULL)){      //Nemá podstrom
                (*RootPtr)->RPtr = NULL;
                free(toDelete);
                return;
            }

            else if (((*RootPtr)->RPtr->RPtr != NULL) && ((*RootPtr)->RPtr->LPtr != NULL)){ //Má oba podstromy
                ReplaceByRightmost((*RootPtr)->RPtr,&((*RootPtr)->RPtr->LPtr));  //Nahradí rušený uzol najpravejším z ľava
                return;
            }

            else if (((*RootPtr)->RPtr->RPtr != NULL) && ((*RootPtr)->RPtr->LPtr == NULL)){ //Má pravý podstrom
                (*RootPtr)->RPtr = (*RootPtr)->RPtr->RPtr;  //Prepojenie ľavej strany s novopríslušným uzlom
                free(toDelete);
                return;
            }
            else if (((*RootPtr)->RPtr->RPtr == NULL) && ((*RootPtr)->RPtr->LPtr != NULL)){ //Má ľavý podstrom
                (*RootPtr)->RPtr = (*RootPtr)->RPtr->LPtr;  //Prepojenie ľavej strany s novopríslušným uzlom
                free(toDelete);
                return;
            }
        }

        BSTDelete(&((*RootPtr)->RPtr),K);
        return;
    }


    else if ((*RootPtr)->Key == K) { //Key je rovný teda ho mažeme

        tBSTNodePtr toDelete = (*RootPtr); //Pomocný uzol


        if (((*RootPtr)->RPtr == NULL) && ((*RootPtr)->LPtr == NULL)){      //Nemá podstrom
            *RootPtr = NULL;
            free(toDelete);
            return;
        }

        else if (((*RootPtr)->RPtr != NULL) && ((*RootPtr)->LPtr != NULL)){ //Má oba podstromy
            ReplaceByRightmost((*RootPtr),&((*RootPtr)->LPtr));  //Nahradí rušený uzol najpravejším z ľava
            return;
        }

        else if (((*RootPtr)->RPtr != NULL) && ((*RootPtr)->LPtr == NULL)){ //Má pravý podstrom
            *RootPtr = (*RootPtr)->RPtr;  //Prepojenie ľavej strany s novopríslušným uzlom
            free(toDelete);
            return;
            }
        else if (((*RootPtr)->RPtr == NULL) && ((*RootPtr)->LPtr != NULL)){ //Má ľavý podstrom
            *RootPtr = (*RootPtr)->LPtr;  //Prepojenie ľavej strany s novopríslušným uzlom
            free(toDelete);
            return;
            }
    }
}

void BSTDispose (tBSTNodePtr *RootPtr) {
/*   ----------
** Zruší celý binární vyhledávací strom a korektně uvolní paměť.
**
** Po zrušení se bude BVS nacházet ve stejném stavu, jako se nacházel po
** inicializaci. Tuto funkci implementujte rekurzivně bez deklarování pomocné
** funkce.
**/
	if ((*RootPtr) != NULL){
			BSTDispose(&(*RootPtr)->LPtr); //Ruší ľavú stranu
			BSTDispose(&(*RootPtr)->RPtr); //Ruší pravú stranu
			free(*RootPtr);	//Uvoľníme
			*RootPtr = NULL; 
	}
	else return;

}

/* konec c401.c */
