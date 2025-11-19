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
#include "symtable.h"
#include "list.h"
#include "string.h"
#include "error.h"
#include "generator.h"
#include "expressions.h"

#define NEJSEMVIFUELSU 0
#define JSEMVIFU 1
#define JSEMVELSU 2

int parse(symbolTable *ST, tDLList *instrList);
int program();
int expression();
int function();
int statement(char *funName, int ifNeboNe);
tInstr *generateInstruction(int instType, void *addr1, void *addr2, void *addr3);
int initFunctions();
int defParams(char* funName);
int defParamsN(char* funName, int argc);
int callParams(char* funName, char* aktualniFunkce);
int callParamsN(char* funName, int argc, char* aktualniFunkce);
void addBuildInFunc();

#endif
