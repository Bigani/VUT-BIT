/**
  * generator.h
  * Project IFJ19
  *
  * Březina Jindřich (xbrezi21@stud.fit.vutbr.cz)
  * Gumančík Pavol (xguman01@stud.fit.vutbr.cz)
  * Kotáb Dominik (xkotab01@stud.fit.vutbr.cz)
  * Moravčík Tomáš (xmorav41@stud.fit.vutbr.cz)
  *
  * Brief: Header of generator
  *
  */

#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>
#include <stdbool.h>

#include "symtable.h"
#include "scanner.h"
#include "expressions.h"

void degenerate(tDLList *list);

void generateInputs();
void generateInputi();
void generateInputf();
void generateLen();
void generateOrd();
void generateChr();
void generateSubstr();

void generateBuiltIn();
void generateAssign(tDLList*list);
void generateGFVariable();
int generateInstructionREE(tDLList*list);
void generateWhile(tDLList*list,void* origi);
void generateIf(tDLList*list,void* origi);
void doDefVars(tDLList*list);
void doDefVarsLocal(tDLList*list);

void checkInt2Float();
void checkFloat2Int();
void checkInt2FloatDiv();
void checkString();
void defenestrace(int antiHussites);


#endif
