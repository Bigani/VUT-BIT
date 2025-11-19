/**
  * tokenStack.h
  * Project IFJ19
  *
  * Březina Jindřich (xbrezi21@stud.fit.vutbr.cz)
  * Gumančík Pavol (xguman01@stud.fit.vutbr.cz)
  * Kotáb Dominik (xkotab01@stud.fit.vutbr.cz)
  * Moravčík Tomáš (xmorav41@stud.fit.vutbr.cz)
  *
  * Brief: header for stack for tokens
  *
  */

#include "symtable.h"

#ifndef STACK_H
#define STACK_H

typedef enum { //polozka ve stacku
  typeToken,
  typeNonterm, //E
  typeHandler //zarážka
} sType;

typedef struct { //data položky ve stacku
  tokenType dataType;
  sType type;
  token* token;
} sData;

typedef struct sItem { //jednotlivé položky ze stacku
  sData* data;
  struct sItem* next;
} tsItem;

typedef struct Stack {
  tsItem* top; //ukazatel na vrchol zásobníku
} tokenStack;

void tokenStackInit(tokenStack* s); //inicializace stacku pro tokeny
int tokenStackEmpty(const tokenStack* s); //vrátí 1, pokud je token stack prázdný
sData* tokenStackTop(const tokenStack* s); //vrátí nám data z toho, co je na vrcholu stacku
void tokenStackPop(tokenStack* s); //popne stack
void tokenStackPush(tokenStack* s, sData* data); //pushne stack
sData* getTerminalData(tokenStack* s); //najde první terminál a dá mi jeho data
void addHandler(tokenStack* s, sData * sData); //vloží do zásobníku zarážku

#endif
