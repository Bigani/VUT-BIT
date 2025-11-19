/**
  * stack.c
  * Project IFJ19
  *
  * Březina Jindřich (xbrezi21@stud.fit.vutbr.cz)
  * Gumančík Pavol (xguman01@stud.fit.vutbr.cz)
  * Kotáb Dominik (xkotab01@stud.fit.vutbr.cz)
  * Moravčík Tomáš (xmorav41@stud.fit.vutbr.cz)
  *
  * Brief: implementace operaci nad stackem, ktery se bude vyuzivat predevsim ve scanneru
  *
  */

#include "stack.h"
#include "error.h"

int err_flag;

void stackError(int error_code) {
  	static const char* SERR_STRINGS[MAX_SERR+1] = {"Unknown error","Stack error: INIT","Stack error: PUSH","Stack error: TOP"};
  	if ( error_code <= 0 || error_code > MAX_SERR )
  		error_code = 0;
  	fprintf (stderr,"%s error in stack.\n", SERR_STRINGS[error_code] );
  	exit (INTERN_ERR);
  }


    void stackInit ( tStack* s ) {
  /*
  ** Provede inicializaci zásobníku - nastaví vrchol zásobníku.
  */

    		if(s == NULL){
    			stackError(SERR_INIT);
    			return;
    		}

        int *head;
        head = (int *) calloc(100, sizeof(int));
        if(head == NULL){
          stackError(SERR_INIT);
          return;
        }

        s->head = head;
        s->currentSize = 100;
    		s->top = -1;
    }

    void stackResize ( tStack* s ) {
  /*
  ** Zvetsi velikost zasobniku
  */

      int *resizedHead;
      resizedHead = (int *) calloc(s->currentSize + 100, sizeof(int));
      if(resizedHead == NULL) {
        stackError(SERR_INIT);
        return;
      }

      memcpy(resizedHead, s->head, sizeof(int) * s->top + 1);

      free(s->head);
      s->head = resizedHead;
      s->currentSize = s->currentSize + 100;
    }

    void stackDestroy ( tStack* s ) {
  /*
  ** Znici vnitrek stacku
  */

      free(s->head);

      s->head = NULL;
      s->currentSize = 0;
      s->top = -1;
    }

    void stackTop ( tStack* s, int* c ) {
  /*
  ** Vrací znak z vrcholu zásobníku prostřednictvím parametru c.
  ** Tato operace ale prvek z vrcholu zásobníku neodstraňuje.
  */

    /* checkne jestli neni stack prazdny, pokud neni, vrati vrchol*/
      if(stackEmpty(s) != 0){
        stackError(SERR_TOP);
        return;
      }
      *c = s->head[s->top];
    }

    void stackPop ( tStack* s ) {
      /*   --------
      ** Odstraní prvek z vrcholu zásobníku. Pro ověření, zda je zásobník prázdný,
      ** použijte dříve definovanou funkci stackEmpty.
      */
      if(stackEmpty(s) != 0){
        return;
      }
      (s->top)--;
    }

    void stackPush ( tStack* s, int c){
    /*   ---------
    ** Vloží znak na vrchol zásobníku. Pokus o vložení prvku do plného zásobníku
    ** je nekorektní a ošetřete ho voláním procedury stackError(SERR_PUSH).
    */
      if(stackFull(s) != 0){
        stackResize(s);
      }

      (s->top)++;
      s->head[s->top] = c;
    }

    int stackFull ( tStack* s ){
  /*  ---------
  ** Vrací nenulovou hodnotu, je-li zásobník plný, jinak vrací hodnotu 0.
  */
      return (s->top == s->currentSize - 1) ? 1 : 0;
    }

    int stackEmpty ( tStack* s ) {
    /*  ----------
    ** Vrací nenulovou hodnotu, pokud je zásobník prázdný, jinak vrací hodnotu 0.
    */
    		return (s->top < 0) ? 1 : 0;
    }
