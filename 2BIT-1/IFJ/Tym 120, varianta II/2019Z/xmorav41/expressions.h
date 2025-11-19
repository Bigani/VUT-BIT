/**
  * expressions.h
  * Project IFJ19
  *
  * Březina Jindřich (xbrezi21@stud.fit.vutbr.cz)
  * Gumančík Pavol (xguman01@stud.fit.vutbr.cz)
  * Kotáb Dominik (xkotab01@stud.fit.vutbr.cz)
  * Moravčík Tomáš (xmorav41@stud.fit.vutbr.cz)
  *
  * Brief: header file of expressions
  *
  */

#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include "parser.h"
#include "tokenStack.h"
#include "scanner.h"
#include "list.h"

#define TABLESIZE 7 //velikost precedenční tabulky

typedef struct{
    int returnType;
    token returnToken;
    int error;
} precendentExpression;

typedef enum {
  A, //0 <
  B, //1 =
  C, //2 >
	D, //3 DONE
  E  //4 ERROR
} precedenceSign;

int getPrecedenceIndex(token* tokenAct); //vrací index věci z precedenční tabulky
int getPrecedenceOperatorValue(token* stackToken, token* vstupniToken); //vrací hodnotu z precedenční tabulky
int findRule(tokenStack *s, int *type, symbolTable* tableG, symbolTable* tableGG, char* jmenoFunkce); //vygeneruje příslušnou instrukci pro generátor
precendentExpression* doPrecedenceOperation(token tokenAct, token* tokenAct2, symbolTable* tableG, symbolTable* tableGG, char* jmenoFunkce); //rozhodne podle precedence, co udělá s výrazem dál

#endif
