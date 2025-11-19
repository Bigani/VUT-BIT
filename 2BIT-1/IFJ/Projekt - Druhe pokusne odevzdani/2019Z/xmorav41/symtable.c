/**
  * symtable.c
  * Project IFJ19
  *
  * Březina Jindřich (xbrezi21@stud.fit.vutbr.cz)
  * Gumančík Pavol (xguman01@stud.fit.vutbr.cz)
  * Kotáb Dominik (xkotab01@stud.fit.vutbr.cz)
  * Moravčík Tomáš (xmorav41@stud.fit.vutbr.cz)
  *
  * Brief: implementation of operations above/on symbol table
  *
  */

#include "symtable.h"

/*
 * vrací hash hodnotu do symbol table
 */
unsigned long hash(char *str) { //https://stackoverflow.com/a/7666577
  unsigned long hash = MAX_SYMTABLE_SIZE;
  int c;

  while ( (c = *str++) ) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }

  return hash % MAX_SYMTABLE_SIZE; //vrací hash v rozmezí 0 .. (MAX_SYMTABLE_SIZE - 1)
 }

/*
 * inicializace symbol table
 */
symbolTable *initSymbolTable(unsigned int size){
  symbolTable *sT = (symbolTable *) malloc(sizeof(*sT) + size * sizeof(sT->symtabList[0]));
  if (!sT) {
    printf("%s\n", "yikes");
  }
  sT->size = size;
  for (unsigned int i = 0; i < sT->size; i++) {
		sT->symtabList[i] = NULL;
	}
  return sT;
}

/*
 * uvolnění symbol table
 */
void freeSymbolTable(symbolTable *sT) { //asi nefreeuje všechno
  symtableItem *tmpItem;

  for (unsigned int i = 0; i < sT->size; i++) {
    tmpItem = sT->symtabList[i];
    while (tmpItem) {
      symtableItem *thisItem = tmpItem;
      tmpItem = tmpItem->next; //nejspis bude NULL btw
      if (thisItem->type == VARIABLE) {
        free(thisItem->elementType.variable);
      }
      if (thisItem->type == FUNCTION) { //vypadá to legit
        for(int i = 0; i < thisItem->elementType.function->argCount; i++) {
          free(thisItem->elementType.function->arguments[i].elementType.variable);
        }
        free(thisItem->elementType.function->arguments);
        freeSymbolTable(thisItem->elementType.function->sT);
        free(thisItem->elementType.function);
      }
      free(thisItem->key);
      free(thisItem);
    }
  }
  free(sT);
}

/*
 * vkládání tokenu do symbol table
 */
void insertSymbolTable(symbolTable *sT, token token, elementType type) {
  char* key = token.attribute.string->string; //získáme jmeno variable/fce
  unsigned long hashVal = hash(key); //získáme hash

  symtableItem *newItem = (symtableItem *) malloc(sizeof(symtableItem));
  if (newItem == NULL) {
    fprintf(stderr, "%s\n", "yyee");
  }

  if (type == VARIABLE) {
    newItem->elementType.variable = (variableData *) malloc(sizeof(variableData));
    if (newItem->elementType.variable == NULL) {
      fprintf(stderr, "%s\n", "yexd");
    }
    newItem->type = VARIABLE;
// token s variablem nemůže mít zároveň nějakou hodnotu, takže toto se nemůže dělat tady
/*    if (token.type == STR) {
      printf("%s\n", "je to string");
      newItem->elementType.variable->type = DATA_STRING;
      newItem->elementType.variable->value = token.attribute.string->string
    } else if (token.type == INT) {
      printf("%s\n", "je to integer");
      newItem->elementType.variable->value.INT =
    } else if (token.type == FLOAT) {
      printf("%s\n", "je to float");
    } */
  } else if (type == FUNCTION) {
    newItem->elementType.function = (functionData *) malloc(sizeof(functionData));
    if (newItem->elementType.function == NULL) {
      fprintf(stderr, "%s\n", "yexxd");
    }
    newItem->elementType.function->sT = initSymbolTable(MAX_SYMTABLE_SIZE); //nová symtable pro funkci
    newItem->type = FUNCTION;
    symbolTable *tableVeFunkci = initSymbolTable(MAX_SYMTABLE_SIZE); //vytvoreni symbol table pro variables ve funkci
    newItem->elementType.function->sT = tableVeFunkci;
    newItem->elementType.function->argCount = 0;
  }

  newItem->defined = false;
  newItem->key = key;

  if (sT->symtabList[hashVal] == NULL) { //ještě není nic na tomto indexu
    newItem->next = NULL;
  } else { //už je něco na tomto indexu
    newItem->next = sT->symtabList[hashVal];
  }

  sT->symtabList[hashVal] = newItem;

}

void deleteItemFromSymbolTable(symbolTable *sT, token token) {
  char* key = token.attribute.string->string; //získáme jmeno variable/fce
  unsigned long hashVal = hash(key); //získáme hash
  symtableItem *tmp = sT->symtabList[hashVal];
  symtableItem *tmpPrev = NULL, *tmpNext = NULL;

  while (tmp) {
    if (strcmp(key, tmp->key) == 0) {
      tmpNext = tmp->next;
      free(tmp);
      if (tmpPrev) {
        tmpPrev->next = tmpNext;
      } else {
        sT->symtabList[hashVal] = tmpNext;
      }
    }
    tmpPrev = tmp;
    tmp = tmp->next;
  }

  free(key);
}

symtableItem *searchSymbolTable(symbolTable *sT, token token) {
  char* key = token.attribute.string->string; //získáme jmeno variable/fce
  unsigned long hashVal = hash(key); //získáme hash
  symtableItem *tmp = sT->symtabList[hashVal];

  while (tmp) {
    if (!strcmp(key, tmp->key)) {
      tmp->next = NULL; //neukazovat na další, myslím, že by už ukazovat neměl když ho vytahujeme ze symtablu?
      return tmp;
    }
    tmp = tmp->next;
  }

  return NULL;
}

symtableItem *searchSymbolTableWithString(symbolTable *sT, char* string) {
  unsigned long hashVal = hash(string); //získáme hash
  symtableItem *tmp = sT->symtabList[hashVal];

  while (tmp) {
    if (!strcmp(string, tmp->key)) {
      tmp->next = NULL; //neukazovat na další, myslím, že by už ukazovat neměl když ho vytahujeme ze symtablu?
      return tmp;
    }
    tmp = tmp->next;
  }

  return NULL;
}

/* testy ---- můžete se inspirovat syntaxem i guess, cleaned up
void main(int argc, char** argv) {
  //inicializace věcí se kterýma pracuju
  symbolTable *table = initSymbolTable(MAX_SYMTABLE_SIZE);
  token TokenUwu, TokenOwo, TokenQQ;
  smartString *s = malloc(sizeof(smartString));
  if (s == NULL){
      return;
  }
  stringInit(s);
  stringAddChar(s,'u');
  stringAddChar(s,'w');
  stringAddChar(s,'u');

  TokenUwu.attribute.string = s;
  TokenUwu.type = STR;
  insertSymbolTable(table, TokenUwu, FUNCTION);

  smartString *z = malloc(sizeof(smartString));
  if (z == NULL){
      return;
  }
  stringInit(z);
  stringAddChar(z,'o');
  stringAddChar(z,'w');
  stringAddChar(z,'o');

  TokenOwo.attribute.string = z;
  TokenOwo.type = STR;
  //insertSymbolTable(table, TokenOwo, VARIABLE);

  smartString *q = malloc(sizeof(smartString));
  if (q == NULL){
      return;
  }
  stringInit(q);
  stringAddChar(q,'q');
  stringAddChar(q,'.');
  stringAddChar(q,'q');
  TokenQQ.attribute.string = q;
  //insertSymbolTable(table, TokenQQ, VARIABLE);

  //funkci "uwu" patří dva parametry, string "owo" a string "q.q"
  table->symtabList[hash("uwu")]->elementType.function->argCount = 2;
  functionData *item;
  item = (functionData *) malloc(sizeof(functionData));
  item->arguments = (symtableItem *) malloc(sizeof(symtableItem));
  item->arguments[0].type = VARIABLE;
  (item->arguments[0]).elementType.variable = (variableData *) malloc(sizeof(variableData));
  (item->arguments[0]).elementType.variable->type = VARIABLE;
	(item->arguments[0]).key = z->string;
  //(item->arguments[0]).elementType.variable->value = ; //hodnota kterou nemáme

  table->symtabList[hash("uwu")]->elementType.function = item;

  //další arg
  item->arguments = realloc(item->arguments, 2*sizeof(symtableItem)); //alokujeme už pro 2
  item->arguments[1].type = VARIABLE;
  (item->arguments[1]).elementType.variable = (variableData *) malloc(sizeof(variableData));
  (item->arguments[1]).elementType.variable->type = VARIABLE;
	(item->arguments[1]).key = q->string;

  //tisk toho, že to funguje
  printf("%s\n", table->symtabList[hash("uwu")]->elementType.function->arguments[0].key);
  printf("%s\n", table->symtabList[hash("uwu")]->elementType.function->arguments[1].key);

  //jelikož chci testovat freeování symbol table s funkcí, tak musím založit další symbol table pro table pro funkci
  symbolTable *tableVeFunkci = initSymbolTable(MAX_SYMTABLE_SIZE);
  table->symtabList[hash("uwu")]->elementType.function->sT = tableVeFunkci;
  insertSymbolTable(tableVeFunkci, TokenQQ, VARIABLE); //něco tam můžu dát idk

  //testování, že searchSymbolTable snad funguje správně
  insertSymbolTable(table, TokenOwo, VARIABLE);
  symtableItem *tmp = searchSymbolTable(table, TokenOwo); //vraci ukazatel
  printf("delete %s\n", tmp->key);
  deleteItemFromSymbolTable(table, TokenOwo);
  tmp = searchSymbolTable(table, TokenOwo); //vrati NULL
  if (tmp) {
    printf("%s\n", tmp->key);
  }

  //testování, že deleteItemFromSymbolTable funguje správně na stejném indexu
    //printf("1. %s\n", table->symtabList[0]->key);
    //printf("2. %s\n", table->symtabList[0]->next->key);
    //printf("3. %s\n", table->symtabList[0]->next->next->key);

    //Token.attribute.string = q;
    //deleteItemFromSymbolTable(table, Token); //odstraň owo
    //printf("1. %s\n", table->symtabList[0]->key);
    //printf("2. %s\n", table->symtabList[0]->next->key);


    freeSymbolTable(table);
}
*/
