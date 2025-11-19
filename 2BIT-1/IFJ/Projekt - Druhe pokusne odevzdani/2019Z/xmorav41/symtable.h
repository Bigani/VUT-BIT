/**
  * symtable.h
  * Project IFJ19
  *
  * Březina Jindřich (xbrezi21@stud.fit.vutbr.cz)
  * Gumančík Pavol (xguman01@stud.fit.vutbr.cz)
  * Kotáb Dominik (xkotab01@stud.fit.vutbr.cz)
  * Moravčík Tomáš (xmorav41@stud.fit.vutbr.cz)
  *
  * Brief: header for symbol table
  *
  */

#include <stdbool.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "scanner.h"
#include "strings.h"

#define MAX_SYMTABLE_SIZE 17569 //nějaká meme hodnota, která je shodou okolností prvočíslo

#ifndef _SYMBOLTABLE_H_
#define _SYMBOLTABLE_H_

//init value
struct symtable;
struct symtableItm;

typedef union{
    char *string;
    int INT;
    double FLOAT;
    bool BOOL;
} value;

typedef enum { //možné datové typy - 10.3 v zadání
	DATA_UNDEFINED, //nvm jestli nutný
	DATA_INT,
	DATA_BOOL,
	DATA_FLOAT,
	DATA_STRING,
} dataType;

typedef enum {
  FUNCTION,
	VARIABLE,
} elementType;

typedef struct {
	value value;
	dataType type;
} variableData;

typedef struct {
	dataType returnType;
	int argCount;
	struct symtableItm *arguments;
	struct symtable *sT;
} functionData;

typedef struct symtableItm { //tady musí být
  bool defined; //je poprvé přiřazena hodnota
	struct symtableItm *next;
	char *key;
	union {
		functionData *function;
		variableData *variable;
	} elementType;
	elementType type;
} symtableItem;

typedef struct symtable { //tady musi
	unsigned int size;
  symtableItem *currentFunc;
  symtableItem *symtabList[];
} symbolTable;


unsigned long hash(char *str); //získání hashe
symbolTable *initSymbolTable(unsigned int size); //init celého symbol tablu
void freeSymbolTable(symbolTable *sT); //uvolnění celého symbol tablu
void insertSymbolTable(symbolTable *sT, token token, elementType type); //přidání prvku do symbol tablu
symtableItem *searchSymbolTable(symbolTable *sT, token token); //vrací ukazatel na prvek v symbol tablu pokud tam je, jinak NULL
symtableItem *searchSymbolTableWithString(symbolTable *sT, char* string); //vrací ukazatel na prvek v symbol tablu pokud tam je, jinak NULL
void deleteItemFromSymbolTable(symbolTable *sT, token token); //odstraní deklarovanou variable/fci z symtable

#endif
