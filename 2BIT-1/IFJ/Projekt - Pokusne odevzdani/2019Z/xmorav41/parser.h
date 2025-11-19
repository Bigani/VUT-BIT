/**
  * parser.h
  * Project IFJ19
  *
  * Březina Jindřich (xbrezi21@stud.fit.vutbr.cz)
  * Gumančík Pavol (xguman01@stud.fit.vutbr.cz)
  * Kotáb Dominik (xkotab01@stud.fit.vutbr.cz)
  * Moravčík Tomáš (xmorav41@stud.fit.vutbr.cz)
  *
  * Brief: header file for parser (heart of compiler (RIP))
  *
  */

#ifndef _PARSER_H_
#define _PARSER_H_

#include "scanner.h"
#include "scanner.c"
#include "symtable.h"
#include "symtable.c"
#include "list.h"
#include "list.c"
#include "string.h"
#include "error.h"

int parse(symbolTable *ST, tDLList *instrList);
int program();
int expression();
int function();
int statement(char *funName);
tInstr* generateInstruction(int instType, void *addr1, void *addr2, void *addr3);
void generateVariable(smartString *var);
int initFunctions();
int defParams(char* funName);
int defParamsN(char* funName, int argc);
int callParams(char* funName);
int callParamsN(char* funName, int argc);
void addBuildInFunc();

#endif
