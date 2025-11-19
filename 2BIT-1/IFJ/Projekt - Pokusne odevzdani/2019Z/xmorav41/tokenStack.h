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

#include "absTree.h"
#include "symtable.h"

#ifndef STACK_H
#define STACK_H

typedef enum { //polozka ve stacku
  type_token,
  type_nonterm, //E
  type_handler //zarážka
} SType;

typedef union { //ulozeni tokenu nebo ukazatele na ASS
  token* Token;
} SAtribute;

typedef struct { //data položky ve stacku
  dataType DataType;
  SType Type;
  SAtribute Atr;
} SData;

typedef struct SItem { //jednotlivé položky ze stacku
  SData* Data;
  struct SItem* Next;
} TSItem;

typedef struct Stack {
  TSItem* Top; //ukazatel na vrchol zásobníku
} tokenStack;

void tokenStackInit(tokenStack* s); //inicializace stacku pro tokeny
void tokenStackDestruct(tokenStack* s); //zrušení stacku pro tokeny
int tokenStackEmpty(const tokenStack* s); //vrátí 1, pokud je token stack prázdný
SData* tokenStackTop(const tokenStack* s); //vrátí nám data z toho, co je na vrcholu stacku
void tokenStackPop(tokenStack* s); //popne stack
void tokenStackPush(tokenStack* s, SData* data); //pushne stack
SData* getTerminalData(tokenStack* s); //najde první terminál a dá mi jeho data
void addHandler(tokenStack* s,SData * sData); //vloží do zásobníku zarážku

#endif
