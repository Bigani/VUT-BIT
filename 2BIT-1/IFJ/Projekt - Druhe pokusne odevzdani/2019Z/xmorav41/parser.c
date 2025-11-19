/**
  * parser.c
  * Project IFJ19
  *
  * Březina Jindřich (xbrezi21@stud.fit.vutbr.cz)
  * Gumančík Pavol (xguman01@stud.fit.vutbr.cz)
  * Kotáb Dominik (xkotab01@stud.fit.vutbr.cz)
  * Moravčík Tomáš (xmorav41@stud.fit.vutbr.cz)
  *
  * Brief: implementace parseru.. 2. cast celeho compileru
  *        vstupem jsou tokeny
  *
  */


#include "parser.h"


symbolTable *tableG;// globalni promenna uchovavajici tabulku symbolu
tDLList *list; // globalni promenna uchovavajici seznam instrukci
token tokenAct;          // globalni promenna, ve ktere bude ulozen aktualni token
int error = OK;
tStack *stack;
int doIndent = 0;

// vlozi novou instrukci do seznamu instrukci
tInstr *generateInstruction(int instType, void *addr1, void *addr2, void *addr3) {
    tInstr I;
    I.instType = instType;
    I.addr1 = addr1; //dest
    I.addr2 = addr2;
    I.addr3 = addr3;

    return DLInsertLast(list, I);
}

// ==================================================================
// jednotlive funkce odpovidajici jednotlivym nonterminalum gramatiky
// ==================================================================

//-----------------------------------------STATEMENT--------------------------------------
int statement(char *funName){
  int result;
  tInstr *jmp1;

  if(tokenAct.type == KEYWORD){
    if(tokenAct.attribute.keyword == IF){ // IF----------------------------------
        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        if(tokenAct.type != INT && tokenAct.type != FLOAT && tokenAct.type != STR && tokenAct.type != LITERAL && tokenAct.type != DOCCOM && tokenAct.type != LEFTBRACKET && !(tokenAct.type == KEYWORD && tokenAct.attribute.keyword == NONE)){
          fprintf(stderr, "Ocekaval se vyraz, ale prisel necekany token\n");
          return PARSING_ERR;
        }
        symtableItem *tmpItem = NULL;
        symbolTable *table2 = NULL;
        if(strcmp(funName, "globalTable") != 0) {
          tmpItem = searchSymbolTableWithString(tableG, funName);
          table2 = tmpItem->elementType.function->sT;
        }
        jmp1 = generateInstruction(I_IF, NULL, NULL, NULL);
        jmp1->addr1 = jmp1;

        precendentExpression* exp = doPrecedenceOperation(tokenAct, NULL, tableG, table2);
        if(exp->error != OK) return exp->error;

        tokenAct = exp->returnToken;

        generateInstruction(I_EOE, NULL, NULL, NULL);

        /*tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru*/
        if(tokenAct.type != COLON) return PARSING_ERR;

        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        if(tokenAct.type != EOL) return PARSING_ERR;

        doIndent = 1;
        tokenAct = nextToken(&error, stack, doIndent);
        doIndent = 0;
        if(error != OK) return error; // zkoumani lexikalniho erroru
        if(tokenAct.type != INDENT) return PARSING_ERR;

        //musi nasledovat statement/y
        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru

        do{
            result = statement(funName);
            if(result != OK) return result;//kouknout jestli statement probehl bez erroru
            if(tokenAct.type == EOFTOKEN) return PARSING_ERR;
        } while(tokenAct.type != DEDENT); //&& tokenAct.type != EOFTOKEN
        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru

        generateInstruction(I_IF_E, NULL, NULL, NULL);
        //jmp1->addr1 = jmp1;

        if(tokenAct.type != KEYWORD) return PARSING_ERR;
        if(tokenAct.attribute.keyword != ELSE) return PARSING_ERR;

        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        if(tokenAct.type != COLON) return PARSING_ERR;

        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        if(tokenAct.type != EOL) return PARSING_ERR;

        doIndent = 1;
        tokenAct = nextToken(&error, stack, doIndent);
        doIndent = 0;
        if(error != OK) return error; // zkoumani lexikalniho erroru
        if(tokenAct.type != INDENT) return PARSING_ERR;

        //jmp2 = generateInstruction(I_JUMP, NULL, NULL, NULL); //???????????????????????????????????????????????????????????????
        //generateInstruction(I_LABEL, NULL, NULL, NULL); //???????????????????????????????????????????????????????????????

        //jmp1->addr1 = list->Last;

        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        do{
            result = statement(funName);
            if(result != OK) return result;//kouknout jestli statement probehl bez erroru
        } while(tokenAct.type != DEDENT && tokenAct.type != EOFTOKEN);

        generateInstruction(I_ELSE_E, NULL, NULL, NULL);

        doIndent = 1;
        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        //nesmi tady byt indent
        if(tokenAct.type == INDENT) return PARSING_ERR;
        doIndent = 0;
        //pokud je dedent, posleme ten token dal;
        if(tokenAct.type == DEDENT) return OK;
        //pokud neni ani indent ani dedent, tak vygenerujeme novy token ktery posleme dal
        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        return OK;

        //generateInstruction(I_LABEL, NULL, NULL, NULL);
        //jmp2->addr1 = list->Last;

        return OK;
    }
    else if(tokenAct.attribute.keyword == WHILE){ // WHILE ---------------------------------
        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru

        jmp1 = generateInstruction(I_WHILE_B, NULL, NULL, NULL);
        jmp1->addr1 = jmp1;

        if(tokenAct.type != INT && tokenAct.type != FLOAT && tokenAct.type != STR && tokenAct.type != LITERAL && tokenAct.type != DOCCOM && tokenAct.type != LEFTBRACKET && !(tokenAct.type == KEYWORD && tokenAct.attribute.keyword == NONE)){
          fprintf(stderr, "Ocekaval se vyraz, ale prisel necekany token\n");
          return PARSING_ERR;
        }
        symtableItem *tmpItem = NULL;
        symbolTable *table2 = NULL;
        if(strcmp(funName, "globalTable") != 0) {
          tmpItem = searchSymbolTableWithString(tableG, funName);
          table2 = tmpItem->elementType.function->sT;
        }
        precendentExpression* exp = doPrecedenceOperation(tokenAct, NULL, tableG, table2);
        if(exp->error != OK) return exp->error;

        generateInstruction(I_EOE, NULL, NULL, NULL); //konec expression
        tokenAct = exp->returnToken;

        if(tokenAct.type != COLON) return PARSING_ERR;

        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        if(tokenAct.type != EOL) return PARSING_ERR;
        doIndent = 1;
        tokenAct = nextToken(&error, stack, doIndent);
        doIndent = 0;
        if(error != OK) return error; // zkoumani lexikalniho erroru
        if(tokenAct.type != INDENT) return PARSING_ERR;

        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        do{
            result = statement(funName);
            if(result != OK) return result;//kouknout jestli statement probehl bez erroru
        } while(tokenAct.type != DEDENT && tokenAct.type != EOFTOKEN);

        generateInstruction(I_WHILE_E, NULL, NULL, NULL);

        doIndent = 1;

        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        //nesmi tady byt indent
        if(tokenAct.type == INDENT) return PARSING_ERR;
        doIndent = 0;
        //pokud je dedent, posleme ten token dal;
        if(tokenAct.type == DEDENT) return OK;
        //pokud neni ani indent ani dedent, tak vygenerujeme novy token ktery posleme dal
        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        return OK;


        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        if(tokenAct.type != DEDENT){
            return PARSING_ERR;
        }

        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        return OK;
    }
    else if(tokenAct.attribute.keyword == PASS){ // PASS --------------------------------------
        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        if(tokenAct.type != EOL && tokenAct.type != EOFTOKEN) return PARSING_ERR;
        if(tokenAct.type == EOFTOKEN) return OK;

        doIndent = 1;
        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        //nesmi tady byt indent
        if(tokenAct.type == INDENT) return PARSING_ERR;
        doIndent = 0;
        //pokud je dedent, posleme ten token dal;
        if(tokenAct.type == DEDENT) return OK;
        //pokud neni ani indent ani dedent, tak vygenerujeme novy token ktery posleme dal
        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        return OK;
    }
    else if(tokenAct.attribute.keyword == RETURN){ // RETURN -------------------------------------
        if(strcmp(funName, "globalTable") == 0){
          fprintf(stderr, "nelze se returnovat v hlavnim tele programu\n");
          return PARSING_ERR;
        }
        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        if(tokenAct.type != INT && tokenAct.type != FLOAT && tokenAct.type != STR && tokenAct.type != LITERAL && tokenAct.type != DOCCOM  && tokenAct.type != LEFTBRACKET){
          fprintf(stderr, "Ocekaval se vyraz, ale prisel necekany token\n");
          return PARSING_ERR;
        }
        symtableItem *tmpItem = NULL;
        symbolTable *table2 = NULL;
        if(strcmp(funName, "globalTable") != 0) {
          tmpItem = searchSymbolTableWithString(tableG, funName);
          table2 = tmpItem->elementType.function->sT;
        }
        precendentExpression* exp = doPrecedenceOperation(tokenAct, NULL, tableG, table2);
        if(exp->error != OK) return exp->error;

        tokenAct = exp->returnToken;

        generateInstruction(I_PUSHS, NULL, NULL, NULL); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1. NULL musi byt vysledek expression
        generateInstruction(I_RETURN, NULL, NULL, NULL);

        /*tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru*/
        if(tokenAct.type != EOL && tokenAct.type != EOFTOKEN) return PARSING_ERR;
        if(tokenAct.type == EOFTOKEN) return OK; // pokud je to konec filu, nezkoumame dalsi token

        doIndent = 1;
        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        //nesmi tady byt indent
        if(tokenAct.type == INDENT) return PARSING_ERR;
        doIndent = 0;
        //pokud je dedent, posleme ten token dal;
        if(tokenAct.type == DEDENT) return OK;
        //pokud neni ani indent ani dedent, tak vygenerujeme novy token ktery posleme dal
        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        return OK;
    }
    else if(tokenAct.attribute.keyword == NONE){
      symtableItem *tmpItem = NULL;
      symbolTable *table2 = NULL;
      if(strcmp(funName, "globalTable") != 0) {
        tmpItem = searchSymbolTableWithString(tableG, funName);
        table2 = tmpItem->elementType.function->sT;
      }
      precendentExpression* exp = doPrecedenceOperation(tokenAct, NULL, tableG, table2);
      if(exp->error != OK) return exp->error;

      tokenAct = exp->returnToken;

      if(tokenAct.type != EOL && tokenAct.type != EOFTOKEN) return PARSING_ERR;
      if(tokenAct.type == EOFTOKEN) return OK; // pokud je to konec filu, nezkoumame dalsi token

      doIndent = 1;
      tokenAct = nextToken(&error, stack, doIndent);
      if(error != OK) return error; // zkoumani lexikalniho erroru
      //nesmi tady byt indent
      if(tokenAct.type == INDENT) return PARSING_ERR;
      doIndent = 0;
      //pokud je dedent, posleme ten token dal;
      if(tokenAct.type == DEDENT) return OK;
      //pokud neni ani indent ani dedent, tak vygenerujeme novy token ktery posleme dal
      tokenAct = nextToken(&error, stack, doIndent);
      if(error != OK) return error; // zkoumani lexikalniho erroru
      return OK;
    }
    else{
      fprintf(stderr, "Ocekaval se prikaz, ale bohuzel prisel jiny token \n");
      return PARSING_ERR;
    }
  }
  else if(tokenAct.type == STR){
    token tmpToken = tokenAct;
    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru
    if(tokenAct.type == ASSIGN){
      tokenAct = nextToken(&error, stack, doIndent);
      if(error != OK) return error; // zkoumani lexikalniho erroru

      symtableItem *tmpItem = searchSymbolTable(tableG, tmpToken); //funkce v tabulce
      if(tmpItem != NULL && tmpItem->type == FUNCTION){
        fprintf(stderr, "snazite se definovat promennou, ktera ma stejny nazev jako nejaka funkce\n");
        return SEM_DEF_ERR;
      }

      if(strcmp(funName, "globalTable") == 0){
        if(tmpItem == NULL){
          insertSymbolTable(tableG, tmpToken, VARIABLE); //vlozeni variable do tabulky
          tmpItem = searchSymbolTable(tableG, tmpToken);
          tmpItem->defined = false;
          generateInstruction(I_DEFVAR, tmpItem, NULL, NULL);
        }
        else{
          tmpItem = searchSymbolTable(tableG, tmpToken);
        }
      }
      else {
        int succMePls = 0;
        symtableItem *tmp = searchSymbolTableWithString(tableG, funName);
        //checkujeme jestli nedefinujeme neco z argumentu funkce
        for(int i = 0; i < tmp->elementType.function->argCount; i++){
          //porovnat tmpToken s argumentama
          if(strcmp(tmp->elementType.function->arguments[i].key, tmpToken.attribute.string->string) == 0){
            tmpItem = &(tmp->elementType.function->arguments[i]);
            succMePls = 1;
            if(tmpItem->defined != true){
              tmpItem->defined = false;
            }
            break;
          }
        }
        if(succMePls == 0){
          tmpItem = searchSymbolTable(tmp->elementType.function->sT, tmpToken);
          if(tmpItem == NULL){
            insertSymbolTable(tmp->elementType.function->sT, tmpToken, VARIABLE);
            tmpItem = searchSymbolTable(tmp->elementType.function->sT, tmpToken);
            tmpItem->defined = false;
          }
          else{
            tmpItem = searchSymbolTable(tmp->elementType.function->sT, tmpToken);
          }
        }
      }

      token *someTokenThatIUsedToKnow;
      someTokenThatIUsedToKnow = NULL;
      if(tokenAct.type == STR){
        tmpToken = tokenAct;
        symtableItem *tmpItem1 = searchSymbolTable(tableG, tmpToken); //funkce v tabulce
        if(tmpItem1 != NULL && tmpItem1->type == FUNCTION){
          tokenAct = nextToken(&error, stack, doIndent);
          if(error != OK) return error; // zkoumani lexikalniho erroru

          if(tokenAct.type != LEFTBRACKET) return PARSING_ERR;

          result = callParams(tmpToken.attribute.string->string);

          if(strcmp(tmpToken.attribute.string->string, "len") == 0){
            if (tmpItem1->elementType.function->arguments->elementType.variable->type == DATA_STRING) {
              tmpItem->elementType.variable->type = DATA_INT;
              generateInstruction(I_LEN, tmpItem1->elementType.function->arguments->elementType.variable->value.string, NULL, NULL);
            } else {
              fprintf(stderr, "hYIWTE TASDZ REEEEEEE\n");
              return SEM_DEF_ERR;
            }
          }
          else if(strcmp(tmpToken.attribute.string->string, "substr") == 0){
            if (tmpItem1->elementType.function->arguments[0].elementType.variable->type == DATA_STRING && tmpItem1->elementType.function->arguments[1].elementType.variable->type == DATA_INT && tmpItem1->elementType.function->arguments[2].elementType.variable->type == DATA_INT) {
              tmpItem->elementType.variable->type = DATA_STRING;
              int* value = (int*) malloc(sizeof(int));
              int* value1 = (int*) malloc(sizeof(int));
              *value = tmpItem1->elementType.function->arguments[1].elementType.variable->value.INT;
              *value1 = tmpItem1->elementType.function->arguments[2].elementType.variable->value.INT;
              generateInstruction(I_SUBSTR, tmpItem1->elementType.function->arguments[0].elementType.variable->value.string, value, value1);
            } else {
              fprintf(stderr, "hYIWTE TASDZ REEEEEEE\n");
              return SEM_DEF_ERR;
            }
          }
          else if(strcmp(tmpToken.attribute.string->string, "chr") == 0){
            if (tmpItem1->elementType.function->arguments->elementType.variable->type == DATA_INT) {
              tmpItem->elementType.variable->type = DATA_STRING;
              generateInstruction(I_CHR, &(tmpItem1->elementType.function->arguments->elementType.variable->value.INT), NULL, NULL);
            } else {
              fprintf(stderr, "hYIWTE TASDZ REEEEEEE\n");
              return SEM_DEF_ERR;
            }
          }
          else if(strcmp(tmpToken.attribute.string->string, "ord") == 0){
            if (tmpItem1->elementType.function->arguments[0].elementType.variable->type == DATA_STRING && tmpItem1->elementType.function->arguments[1].elementType.variable->type == DATA_INT) {
              tmpItem->elementType.variable->type = DATA_STRING;
              int* value = (int*) malloc(sizeof(int));
              *value = tmpItem1->elementType.function->arguments[1].elementType.variable->value.INT;
              generateInstruction(I_ORD, tmpItem1->elementType.function->arguments[0].elementType.variable->value.string, value, NULL);
            } else {
              fprintf(stderr, "hYIWTE TASDZ REEEEEEE\n");
              return SEM_DEF_ERR;
            }
          }
          else if(strcmp(tmpToken.attribute.string->string, "print") == 0){
            symtableItem * reeItem =(symtableItem*) malloc(tmpItem->elementType.function->argCount * sizeof(symtableItem));
            reeItem = tmpItem->elementType.function->arguments;
            generateInstruction(I_PRINT, NULL, &(tmpItem->elementType.function->argCount), reeItem);
          }
          else if(strcmp(tmpToken.attribute.string->string, "inputs") == 0){
            tmpItem->elementType.variable->type = DATA_STRING;
            generateInstruction(I_INPUTS, NULL, NULL, NULL);
          }
          else if(strcmp(tmpToken.attribute.string->string, "inputi") == 0){
            tmpItem->elementType.variable->type = DATA_INT;
            generateInstruction(I_INPUTI, NULL, NULL, NULL);
          }
          else if(strcmp(tmpToken.attribute.string->string, "inputf") == 0){
            tmpItem->elementType.variable->type = DATA_FLOAT;
            generateInstruction(I_INPUTF, NULL, NULL, NULL);
          }
          else{
            generateInstruction(I_CALL, tmpItem->elementType.function, NULL, NULL); //???
          }

          generateInstruction(I_POPS, tmpItem, NULL, NULL);

          if (result != OK) return result;
          tokenAct = nextToken(&error, stack, doIndent);
          if(error != OK) return error; // zkoumani lexikalniho erroru
          if(tokenAct.type != EOL && tokenAct.type != EOFTOKEN) return PARSING_ERR;
          if(tokenAct.type == EOFTOKEN) return OK; // pokud je to konec filu, nezkoumame dalsi token

          doIndent = 1;
          tokenAct = nextToken(&error, stack, doIndent);
          if(error != OK) return error; // zkoumani lexikalniho erroru
          //nesmi tady byt indent
          if(tokenAct.type == INDENT) return PARSING_ERR;
          doIndent = 0;
          //pokud je dedent, posleme ten token dal;
          if(tokenAct.type == DEDENT) return OK;
          //pokud neni ani indent ani dedent, tak vygenerujeme novy token ktery posleme dal
          tokenAct = nextToken(&error, stack, doIndent);
          if(error != OK) return error; // zkoumani lexikalniho erroru
          return OK;
        }

        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        if(tokenAct.type == LEFTBRACKET){
          fprintf(stderr, "Snazite se priradit do promenne funkci, ktera neni definovana funkce\n");
          return SEM_DEF_ERR;
        }
        else{
          someTokenThatIUsedToKnow = &tmpToken;
        }
      }


      if(someTokenThatIUsedToKnow == NULL){
        if(tokenAct.type != INT && tokenAct.type != FLOAT && tokenAct.type != LITERAL && tokenAct.type != LEFTBRACKET && tokenAct.type != DOCCOM && tokenAct.type != STR && !(tokenAct.type == KEYWORD && tokenAct.attribute.keyword == NONE)){
        fprintf(stderr, "NELEGALNI EXPRESSION\n");
        return PARSING_ERR;
      }
      }

      symbolTable *table2 = NULL;
      if(strcmp(funName, "globalTable") != 0) {
        table2 = tmpItem->elementType.function->sT;
      }
      precendentExpression* exp = doPrecedenceOperation(tokenAct, someTokenThatIUsedToKnow, tableG, table2);
      if(exp->error != OK) return exp->error;

      tokenAct = exp->returnToken;
      generateInstruction(I_POPS, tmpItem, NULL, NULL);

      tmpItem->defined = true;
      tmpItem->elementType.variable->type = exp->returnType;
      tmpItem->elementType.variable->value = exp->returnValue;

      if(tokenAct.type != EOL && tokenAct.type != EOFTOKEN) return PARSING_ERR;
      if(tokenAct.type == EOFTOKEN) return OK; // pokud je to konec filu, nezkoumame dalsi token

      doIndent = 1;
      tokenAct = nextToken(&error, stack, doIndent);
      if(error != OK) return error; // zkoumani lexikalniho erroru
      //nesmi tady byt indent
      if(tokenAct.type == INDENT) return PARSING_ERR;
      doIndent = 0;
      //pokud je dedent, posleme ten token dal;
      if(tokenAct.type == DEDENT) return OK;
      //pokud neni ani indent ani dedent, tak vygenerujeme novy token ktery posleme dal
      tokenAct = nextToken(&error, stack, doIndent);
      if(error != OK) return error; // zkoumani lexikalniho erroru
      return OK;
    }
    else if(tokenAct.type == LEFTBRACKET){
      symtableItem *tmpItem = searchSymbolTable(tableG, tmpToken); //funkce v tabulce
      if(tmpItem == NULL || tmpItem->type != FUNCTION){
        fprintf(stderr, "snazite se volat nejakou funkci, ktera funkce vubec neni\n");
        return SEM_DEF_ERR;
      }

      result = callParams(tmpToken.attribute.string->string);

      if(strcmp(tmpToken.attribute.string->string, "len") == 0){
        generateInstruction(I_LEN, tmpItem->elementType.function->arguments->elementType.variable->value.string, NULL, NULL);
      }
      else if(strcmp(tmpToken.attribute.string->string, "substr") == 0){
        //generateInstruction(I_SUBSTR, tmpToken, NULL, NULL);
      }
      else if(strcmp(tmpToken.attribute.string->string, "chr") == 0){
        //generateInstruction(I_CHR, tmpToken, NULL, NULL);
      }
      else if(strcmp(tmpToken.attribute.string->string, "ord") == 0){
        //generateInstruction(I_ORD, tmpToken, NULL, NULL);
      }
      else if(strcmp(tmpToken.attribute.string->string, "print") == 0){
        symtableItem * reeItem =(symtableItem*) malloc(tmpItem->elementType.function->argCount * sizeof(symtableItem));
        reeItem = tmpItem->elementType.function->arguments;
        generateInstruction(I_PRINT, NULL, &(tmpItem->elementType.function->argCount), reeItem);
      }
      else if(strcmp(tmpToken.attribute.string->string, "inputs") == 0){
        generateInstruction(I_INPUTS, NULL, NULL, NULL);
      }
      else if(strcmp(tmpToken.attribute.string->string, "inputi") == 0){
        generateInstruction(I_INPUTI, NULL, NULL, NULL);
      }
      else if(strcmp(tmpToken.attribute.string->string, "inputf") == 0){
        generateInstruction(I_INPUTF, NULL, NULL, NULL);
      }
      else{
        generateInstruction(I_CALL, tmpItem->elementType.function, NULL, NULL); //???
      }

      if (result != OK) return result;
      tokenAct = nextToken(&error, stack, doIndent);
      if(error != OK) return error; // zkoumani lexikalniho erroru
      if(tokenAct.type != EOL && tokenAct.type != EOFTOKEN) return PARSING_ERR;
      if(tokenAct.type == EOFTOKEN) return OK; // pokud je to konec filu, nezkoumame dalsi token

      //generateInstruction(I_POP, , NULL, NULL);//DESTINATION

      doIndent = 1;
      tokenAct = nextToken(&error, stack, doIndent);
      if(error != OK) return error; // zkoumani lexikalniho erroru
      //nesmi tady byt indent
      if(tokenAct.type == INDENT) return PARSING_ERR;
      doIndent = 0;
      //pokud je dedent, posleme ten token dal;
      if(tokenAct.type == DEDENT) return OK;
      //pokud neni ani indent ani dedent, tak vygenerujeme novy token ktery posleme dal
      tokenAct = nextToken(&error, stack, doIndent);
      if(error != OK) return error; // zkoumani lexikalniho erroru
      return OK;
    }
    else{
      symtableItem *tmpItem = NULL;
      symbolTable *table2 = NULL;
      if(strcmp(funName, "globalTable") != 0) {
        tmpItem = searchSymbolTableWithString(tableG, funName);
        table2 = tmpItem->elementType.function->sT;
      }
      precendentExpression* exp = doPrecedenceOperation(tokenAct, &tmpToken, tableG, table2);
      if(exp->error != OK) return exp->error;

      tokenAct = exp->returnToken;

      if(tokenAct.type != EOL && tokenAct.type != EOFTOKEN) return PARSING_ERR;
      if(tokenAct.type == EOFTOKEN) return OK; // pokud je to konec filu, nezkoumame dalsi token

      doIndent = 1;
      tokenAct = nextToken(&error, stack, doIndent);
      if(error != OK) return error; // zkoumani lexikalniho erroru
      //nesmi tady byt indent
      if(tokenAct.type == INDENT) return PARSING_ERR;
      doIndent = 0;
      //pokud je dedent, posleme ten token dal;
      if(tokenAct.type == DEDENT) return OK;
      //pokud neni ani indent ani dedent, tak vygenerujeme novy token ktery posleme dal
      tokenAct = nextToken(&error, stack, doIndent);
      if(error != OK) return error; // zkoumani lexikalniho erroru
      return OK;
    }
  }
  else if(tokenAct.type == INT || tokenAct.type == FLOAT || tokenAct.type == LITERAL || tokenAct.type == LEFTBRACKET || tokenAct.type == DOCCOM){{
    symtableItem *tmpItem = NULL;
    symbolTable *table2 = NULL;
    if(strcmp(funName, "globalTable") != 0) {
      tmpItem = searchSymbolTableWithString(tableG, funName);
      table2 = tmpItem->elementType.function->sT;
    }
    precendentExpression* exp = doPrecedenceOperation(tokenAct, NULL, tableG, table2);
    if(exp->error != OK) return exp->error;

    tokenAct = exp->returnToken;

    if(tokenAct.type != EOL && tokenAct.type != EOFTOKEN) return PARSING_ERR;
    if(tokenAct.type == EOFTOKEN) return OK; // pokud je to konec filu, nezkoumame dalsi token

    doIndent = 1;
    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru
    //nesmi tady byt indent
    if(tokenAct.type == INDENT) return PARSING_ERR;
    doIndent = 0;
    //pokud je dedent, posleme ten token dal;
    if(tokenAct.type == DEDENT) return OK;
    //pokud neni ani indent ani dedent, tak vygenerujeme novy token ktery posleme dal
    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru
    return OK;
  }}
  else if(tokenAct.type == EOL){
    doIndent = 1;
    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru
    //nesmi tady byt indent
    if(tokenAct.type == INDENT) return PARSING_ERR;
    doIndent = 0;
    //pokud je dedent, posleme ten token dal;
    if(tokenAct.type == DEDENT) return OK;
    //pokud neni ani indent ani dedent, tak vygenerujeme novy token ktery posleme dal
    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru
    return OK;
  }
  else{
    fprintf(stderr, "Ocekaval se prikaz, ale bohuzel prisel jiny token\n");
    return PARSING_ERR;
  }
}

//---------------------------------------FUNCTION-------------------------------------------
int function(){
    int result;

    if(tokenAct.type != STR) return PARSING_ERR; //za def musi nasledovat identifikator
    smartString *s = tokenAct.attribute.string;
    //musi nasledovat '('
    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru
    if(tokenAct.type != LEFTBRACKET){
      fprintf(stderr, "Za DEF ID musi nasledovat ( \n");
      return PARSING_ERR;
    }

    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru
    while(tokenAct.type != RIGHTBRACKET){
      tokenAct = nextToken(&error, stack, doIndent);
      if(error != OK) return error; // zkoumani lexikalniho erroru
    }


    generateInstruction(I_CLEARS, NULL, NULL, NULL);

    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru
    if(tokenAct.type != COLON){
      fprintf(stderr, "Za DEF ID ( pripadne argumenty ) musi nasledovat : \n");
      return PARSING_ERR;
    }
    //musi nasledovat EOL -> INDENT -> statement
    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru
    if(tokenAct.type != EOL){
      fprintf(stderr, "Za DEF ID ( pripadne argumenty ): musi nasledovat konec radku \n");
      return PARSING_ERR;
    }

    doIndent = 1;
    tokenAct = nextToken(&error, stack, doIndent);
    doIndent = 0;
    if(error != OK) return error; // zkoumani lexikalniho erroru
    if(tokenAct.type != INDENT){
      fprintf(stderr, "Za DEF ID ( pripadne argumenty ) : EOL musi nasledovat indent \n");
      return PARSING_ERR;
    }
    //musi nasledovat statement/y
    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru


    while(tokenAct.type != DEDENT && tokenAct.type != EOFTOKEN){
        result = statement(s->string);
        if(result != OK) return result;//kouknout jestli statement probehl bez erroru
    }


    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru
    return OK;
}

//---------------------------------------------PROGRAM-------------------------------------
int program(){
    int result = OK; ////Inicializacia v prípade že máš hned EOF (Ne/10:28)
    doIndent = 1;
    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru
    //nesmi tady byt indent
    if(tokenAct.type == INDENT) return PARSING_ERR;
    doIndent = 0;
    //prvni token
    tokenAct = nextToken(&error, stack, doIndent);
    token tmptoksafgionasg = tokenAct;
    if(error != OK) return error; // zkoumani lexikalniho erroru

    while(tokenAct.type != EOFTOKEN){ // prochazi se cely program
        // muze se jednat bud o funkci, statement
        if(tokenAct.type == KEYWORD && tokenAct.attribute.keyword == DEF){
            tokenAct = nextToken(&error, stack, doIndent);
            if(error != OK) return error; // zkoumani lexikalniho erroru
            result = function();
            if(result != OK) return result;
        }
        else{
            if(tokenAct.type == KEYWORD && tokenAct.attribute.keyword == RETURN){
              fprintf(stderr, "V tele programu se nesmi nachazet return\n");
              return PARSING_ERR;
            }
            result = statement("globalTable");
            if(result != OK) return result;
        }
    }
    return result;
}

//---------------------------------------------DEFPARAMS----------------------------------
int defParams(char* funName){
    int argc = 0;
    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru
    if(tokenAct.type == RIGHTBRACKET) return OK;
    else if(tokenAct.type == STR){
        //chybi pridat symbol do funkce ?
        functionData *item;
        item = (functionData *) malloc(sizeof(functionData));
        item->arguments = (symtableItem *) malloc(sizeof(symtableItem));
        item->arguments[argc].type = VARIABLE;
        (item->arguments[argc]).elementType.variable = (variableData *) malloc(sizeof(variableData));
        (item->arguments[argc]).elementType.variable->type = VARIABLE;
        (item->arguments[argc]).key = tokenAct.attribute.string->string;
        item->argCount = argc+1;
        symtableItem *tmpIT = searchSymbolTableWithString(tableG, funName);
        item->sT = tmpIT->elementType.function->sT;

        tmpIT->next = tableG->symtabList[hash(funName)]; //pokud je něco na stejným indexu =)
        tmpIT->elementType.function = item;
        tableG->symtabList[hash(funName)] = tmpIT;

        symtableItem *tmp = searchSymbolTable(tableG, tokenAct);
        if (tmp && tmp->type == FUNCTION){
          fprintf(stderr, "Snazite se definovat variable(jako argument funkce), ktery je stejny jako nejaky nazev funkce\n");
          return SEM_DEF_ERR;
        }
        generateInstruction(I_POPS, NULL, NULL, NULL); ////////////////////////////OPRAVIT///////////////////////////////////////////
        return defParamsN(funName, ++argc);
    }
    else {
      fprintf(stderr, "za DEF ID (  musi nasledovat bud ) nebo nejaky identifikator\n");
      return PARSING_ERR;
    }
}

//---------------------------------------------DEFPARAMSN----------------------------------
int defParamsN(char* funName, int argc){
    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru
    if(tokenAct.type == RIGHTBRACKET) return OK;
    if(tokenAct.type != COMMA){
      fprintf(stderr, "Za DEF ID ( ID  musi nasledovat bud ) nebo , \n");
      return PARSING_ERR;
    }
    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru
    if(tokenAct.type != STR){
      fprintf(stderr, "Za DEF ID ( ID ,  musi nasledovat ID   ( ID , se mohlo pred timto opakovat nekolikrat)\n");
      return PARSING_ERR;
    }
    argc++;

    //chybi pridat symbol do tabulky funkce
    //checknout jestli se 2 parametry nejmenuji stejne
    symtableItem *tmpAAAAAA = searchSymbolTableWithString(tableG, funName); //hledáme nejen na indexu, ale i jestli někam neukazuje yk
    functionData *item = tmpAAAAAA->elementType.function;
    item->argCount = argc;
    item->arguments = realloc(item->arguments, argc*sizeof(symtableItem));
    item->arguments[argc-1].type = VARIABLE;
    (item->arguments[argc-1]).key = tokenAct.attribute.string->string;
    (item->arguments[argc-1]).elementType.variable = (variableData *) malloc(sizeof(variableData));
    (item->arguments[argc-1]).elementType.variable->type = VARIABLE;

    //item->arguments[argc-1].elementType.variable->value.INT = 5;

    for (int i = 0; i < (argc-1); i++) {
      if(!strcmp((item->arguments[i]).key, (item->arguments[argc-1]).key)) return SEM_DEF_ERR;
    }

    symtableItem *tmp = searchSymbolTable(tableG, tokenAct);
    if (tmp && tmp->type == FUNCTION){
      fprintf(stderr, "Snazite se definovat variable(jako argument funkce), ktery je stejny jako nejaky nazev funkce\n");
      return SEM_DEF_ERR;
    }
    generateInstruction(I_POPS, NULL, NULL, NULL); ////////////////////////////OPRAVIT///////////////////////////////////////////
    return defParamsN(funName, argc);
}

//---------------------------------------------CALLPARAMS----------------------------------
int callParams(char* funName){
  symtableItem *tmpItem0 = searchSymbolTableWithString(tableG, funName);

  tokenAct = nextToken(&error, stack, doIndent);
  if(error != OK) return error; // zkoumani lexikalniho erroru
  if(tokenAct.type == RIGHTBRACKET){
    if (tmpItem0->elementType.function->argCount == 0 || strcmp(funName, "print") == 0) {
      return OK;
    }
    else{
      fprintf(stderr, "volate funkci, ktera ocekava nejake parametry, ale vy do ni zadne nedavate\n");
      return SEM_PAR_ERR;
    }
  }

  if(strcmp(funName, "print") == 0){
    functionData *item;
    item = (functionData *) malloc(sizeof(functionData));
    item->arguments = (symtableItem *) malloc(sizeof(symtableItem));
    item->arguments[0].type = VARIABLE;
    (item->arguments[0]).elementType.variable = (variableData *) malloc(sizeof(variableData));
    (item->arguments[0]).elementType.variable->type = VARIABLE;
    item->argCount = 1;

    symtableItem *tmpIT = searchSymbolTableWithString(tableG, funName);
    free(tmpIT->elementType.function->arguments);
    item->sT = tmpIT->elementType.function->sT;
    tmpIT->next = tableG->symtabList[hash(funName)]; //pokud je něco na stejným indexu =)
    tmpIT->elementType.function = item;
    tableG->symtabList[hash(funName)] = tmpIT;
  }

  if(tmpItem0->elementType.function->argCount <= 0 && strcmp(funName, "print") != 0){
    fprintf(stderr, "snazite se do volani funkce vlozit vic parametru, nez funkce vyzaduje\n");
    return SEM_PAR_ERR;
  }
  else{
    if(tokenAct.type == STR){
      symtableItem *tmpItem = searchSymbolTable(tableG, tokenAct);

      if(tmpItem != NULL && tmpItem->type == FUNCTION){
        fprintf(stderr, "do argumentu funkce nemuzete davat funkce(nazvy funkci)\n");
        return SEM_MISC_ERR;
      }
      else if(tmpItem != NULL && tmpItem->type == VARIABLE){
        (tmpItem0->elementType.function->arguments[0]).key = tmpItem->key;
        //(tmpItem0->elementType.function->arguments[0]).elementType.variable->value = tmpItem->elementType.variable->value;
        (tmpItem0->elementType.function->arguments[0]).elementType.variable->type = tmpItem->elementType.variable->type;
        generateInstruction(I_PUSHS, tmpItem->key,NULL,tmpItem->key);
      }
      else{
        fprintf(stderr, "do funkce davate jako parametr nedefinovanou promennou\n");
        return SEM_DEF_ERR;
      }
    }
    else if(tokenAct.type == INT || tokenAct.type == FLOAT || tokenAct.type == LITERAL || tokenAct.type == DOCCOM){
      if(tokenAct.type == INT){
        (tmpItem0->elementType.function->arguments[0]).elementType.variable->value.INT = tokenAct.attribute.INT;
        (tmpItem0->elementType.function->arguments[0]).elementType.variable->type = DATA_INT;
        int* hodnota = (int*) malloc(sizeof(int));
        int* typTokenu = (int*) malloc(sizeof(int));
        *hodnota = tokenAct.attribute.INT;
        *typTokenu = tokenAct.type;
        generateInstruction(I_PUSHS, hodnota, typTokenu, NULL);
      }
      else if(tokenAct.type == FLOAT){
        (tmpItem0->elementType.function->arguments[0]).elementType.variable->value.FLOAT = tokenAct.attribute.FLOAT;
        (tmpItem0->elementType.function->arguments[0]).elementType.variable->type = DATA_FLOAT;
        double* hodnota = (double*) malloc(sizeof(double));
        int* typTokenu = (int*) malloc(sizeof(int));
        *hodnota = tokenAct.attribute.FLOAT;
        *typTokenu = tokenAct.type;
        generateInstruction(I_PUSHS, hodnota, typTokenu, NULL);
      }
      else if(tokenAct.type == LITERAL || tokenAct.type == DOCCOM){
        (tmpItem0->elementType.function->arguments[0]).elementType.variable->value.string = tokenAct.attribute.string->string;
        (tmpItem0->elementType.function->arguments[0]).elementType.variable->type = DATA_STRING;
        char* hodnota = (char*) malloc(sizeof(char));
        int* typTokenu = (int*) malloc(sizeof(int));
        hodnota = tokenAct.attribute.string->string;
        *typTokenu = tokenAct.type;
        generateInstruction(I_PUSHS, hodnota, typTokenu, NULL);
      }
    }
    else if (tokenAct.type == KEYWORD && tokenAct.attribute.keyword == NONE){
      (tmpItem0->elementType.function->arguments[0]).elementType.variable->type = DATA_UNDEFINED;
    }
    else{
      fprintf(stderr, "Nespravny typ argumentu pri volani funkce\n");
      return PARSING_ERR;
    }

    //generateInstruction(I_PUSH, , NULL, NULL);
    return callParamsN(funName, 1);
  }
}

//---------------------------------------------CALLPARAMSN----------------------------------
int callParamsN(char* funName, int argc){
  symtableItem *tmpItem0 = searchSymbolTableWithString(tableG, funName);

  tokenAct = nextToken(&error, stack, doIndent);
  if(error != OK) return error; // zkoumani lexikalniho erroru


  if(tokenAct.type == RIGHTBRACKET){
    if (tmpItem0->elementType.function->argCount == argc || tmpItem0->elementType.function->argCount == -1) {
      return OK;
    }
    else{
      fprintf(stderr, "volate funkci, ale davate do ni malo parametru\n");
      return SEM_PAR_ERR;
    }
  }
  else if(tokenAct.type == COMMA){
    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru
  }
  else{
    fprintf(stderr, "za argumentem ve volani funkce musi nasledovat  ,  nebo  )  \n");
    return PARSING_ERR;
  }

  if(strcmp(funName, "print") == 0){
    symtableItem *tmpAAAAAA = searchSymbolTableWithString(tableG, funName); //hledáme nejen na indexu, ale i jestli někam neukazuje yk
    functionData *item = tmpAAAAAA->elementType.function;
    item->arguments = (symtableItem *) realloc(item->arguments, (argc + 1) * sizeof(symtableItem));
    item->arguments[argc].type = VARIABLE;
    (item->arguments[argc]).elementType.variable = (variableData *) malloc(sizeof(variableData));
    (item->arguments[argc]).elementType.variable->type = VARIABLE;
    item->argCount = argc + 1;

    symtableItem *tmpIT = searchSymbolTableWithString(tableG, funName);
    item->sT = tmpIT->elementType.function->sT;
    tmpIT->next = tableG->symtabList[hash(funName)]; //pokud je něco na stejným indexu =)
    tmpIT->elementType.function = item;
    tableG->symtabList[hash(funName)] = tmpIT;
  }

  if(tmpItem0->elementType.function->argCount <= argc && tmpItem0->elementType.function->argCount != -1){
    fprintf(stderr, "snazite se do volani funkce vlozit vic parametru, nez funkce vyzaduje\n");
    return SEM_MISC_ERR;
  }
  else{
    if(tokenAct.type == STR){
      symtableItem *tmpItem = searchSymbolTable(tableG, tokenAct);
      if(tmpItem != NULL && tmpItem->type == FUNCTION){
        fprintf(stderr, "do argumentu funkce nemuzete davat funkce(nazvy funkci)\n");
        return SEM_MISC_ERR;
      }
      else if(tmpItem != NULL && tmpItem->type == VARIABLE){
        (tmpItem0->elementType.function->arguments[argc]).key = tmpItem->key;
        //(tmpItem0->elementType.function->arguments[argc]).elementType.variable->value = tmpItem->elementType.variable->value;
        (tmpItem0->elementType.function->arguments[argc]).elementType.variable->type = tmpItem->elementType.variable->type;
        generateInstruction(I_PUSHS, tmpItem->key,NULL,tmpItem->key);
      }
      else{
        fprintf(stderr, "do funkce davate jako parametr nedefinovanou promennou\n");
        return SEM_DEF_ERR;
      }
    }
    else if(tokenAct.type == INT || tokenAct.type == FLOAT || tokenAct.type == LITERAL || tokenAct.type == DOCCOM){
      if(tokenAct.type == INT){
        (tmpItem0->elementType.function->arguments[argc]).elementType.variable->value.INT = tokenAct.attribute.INT;
        (tmpItem0->elementType.function->arguments[argc]).elementType.variable->type = DATA_INT;
        int* hodnota = (int*) malloc(sizeof(int));
        int* typTokenu = (int*) malloc(sizeof(int));
        *hodnota = tokenAct.attribute.INT;
        *typTokenu = tokenAct.type;
        generateInstruction(I_PUSHS, hodnota, typTokenu, NULL);
      }
      else if(tokenAct.type == FLOAT){
        (tmpItem0->elementType.function->arguments[argc]).elementType.variable->value.FLOAT = tokenAct.attribute.FLOAT;
        (tmpItem0->elementType.function->arguments[argc]).elementType.variable->type = DATA_FLOAT;
        double* hodnota = (double*) malloc(sizeof(double));
        int* typTokenu = (int*) malloc(sizeof(int));
        *hodnota = tokenAct.attribute.FLOAT;
        *typTokenu = tokenAct.type;
        generateInstruction(I_PUSHS, hodnota, typTokenu, NULL);
      }
      else if(tokenAct.type == LITERAL || tokenAct.type == DOCCOM){
        (tmpItem0->elementType.function->arguments[argc]).elementType.variable->value.string = tokenAct.attribute.string->string;
        (tmpItem0->elementType.function->arguments[argc]).elementType.variable->type = DATA_STRING;
        char* hodnota = (char*) malloc(sizeof(char));
        int* typTokenu = (int*) malloc(sizeof(int));
        hodnota = tokenAct.attribute.string->string;
        *typTokenu = tokenAct.type;
        generateInstruction(I_PUSHS, hodnota, typTokenu, NULL);
      }
    }
    else if (tokenAct.type == KEYWORD && tokenAct.attribute.keyword == NONE){
      (tmpItem0->elementType.function->arguments[argc]).elementType.variable->type = DATA_UNDEFINED;
    }
    else{
      fprintf(stderr, "Nespravny typ argumentu pri volani funkce\n");
      return PARSING_ERR;
    }
    argc++;
    //generateInstruction(I_PUSH,, NULL, NULL);
    return callParamsN(funName, argc);
  }
}

/*projede cely soubor a najde tam definice funkci a tyto funkce vlozi do tabulky funkci*/
int initFunctions(){
    int result = OK;
    tokenAct = nextToken(&error, stack, doIndent);
    token rrrrrrrrrrrrrr = tokenAct;
    if(error != OK) return error;

    while(tokenAct.type != EOFTOKEN){
        if(tokenAct.type == EOL){
            doIndent = 1;
            tokenAct = nextToken(&error, stack, doIndent);
            doIndent = 0;
        }
        if(tokenAct.type == KEYWORD && tokenAct.attribute.keyword == DEF){

            tokenAct = nextToken(&error, stack, doIndent);
            if(error != OK) return error; // zkoumani lexikalniho erroru

            if(tokenAct.type != STR){
              fprintf(stderr, "Za keywordem DEF musi nasledovat identifikator\n");
              return PARSING_ERR;
            }

            if(searchSymbolTable(tableG, tokenAct) != NULL){
              fprintf(stderr, "Chyba, kvuli snaze o redefinici funkce\n");
              return SEM_DEF_ERR;
            }
            insertSymbolTable(tableG, tokenAct, FUNCTION); //vlozeni funkce do tabulky funkci

            token tmpToken = tokenAct;

            tokenAct = nextToken(&error, stack, doIndent);
            if(error != OK) return error; // zkoumani lexikalniho erroru

            if(tokenAct.type != LEFTBRACKET){
              fprintf(stderr, "za DEF ID ma nasledovat  (\n");
              return PARSING_ERR;
            }

            result = defParams(tmpToken.attribute.string->string);
            if(result != OK) return result;
        }

        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error;
    }

    resetToken();
    return OK;
}

/*prida vestavene funkce do symtable*/
void addBuildInFunc(){
  //--------------------INPUTS
  token tmpToken;
  smartString *tmpString = malloc(sizeof(smartString));
  stringInit(tmpString);
  stringAddChar(tmpString,'i');
  stringAddChar(tmpString,'n');
  stringAddChar(tmpString,'p');
  stringAddChar(tmpString,'u');
  stringAddChar(tmpString,'t');
  stringAddChar(tmpString,'s');

  tmpToken.attribute.string = tmpString;
  tmpToken.type = KEYWORD;
  insertSymbolTable(tableG, tmpToken, FUNCTION); //vlozeni funkce do tabulky funkci

  symtableItem *tmpIT = searchSymbolTable(tableG, tmpToken);

  tmpIT->elementType.function->argCount = 0;

  //-------------------------INPUTI
  smartString *tmpString0 = malloc(sizeof(smartString));
  stringInit(tmpString0);
  stringAddChar(tmpString0,'i');
  stringAddChar(tmpString0,'n');
  stringAddChar(tmpString0,'p');
  stringAddChar(tmpString0,'u');
  stringAddChar(tmpString0,'t');
  stringAddChar(tmpString0,'i');

  tmpToken.attribute.string = tmpString0;
  tmpToken.type = KEYWORD;
  insertSymbolTable(tableG, tmpToken, FUNCTION); //vlozeni funkce do tabulky funkci

  symtableItem *tmpIT0 = searchSymbolTable(tableG, tmpToken);

  tmpIT0->elementType.function->argCount = 0;

  //----------------------INPUTF
  smartString *tmpString1 = malloc(sizeof(smartString));
  stringInit(tmpString1);
  stringAddChar(tmpString1,'i');
  stringAddChar(tmpString1,'n');
  stringAddChar(tmpString1,'p');
  stringAddChar(tmpString1,'u');
  stringAddChar(tmpString1,'t');
  stringAddChar(tmpString1,'f');

  tmpToken.attribute.string = tmpString1;
  tmpToken.type = KEYWORD;
  insertSymbolTable(tableG, tmpToken, FUNCTION); //vlozeni funkce do tabulky funkci

  symtableItem *tmpIT1 = searchSymbolTable(tableG, tmpToken);

  tmpIT1->elementType.function->argCount = 0;

  //-------------------PRINT
  smartString *tmpString2 = malloc(sizeof(smartString));
  stringInit(tmpString2);
  stringAddChar(tmpString2,'p');
  stringAddChar(tmpString2,'r');
  stringAddChar(tmpString2,'i');
  stringAddChar(tmpString2,'n');
  stringAddChar(tmpString2,'t');

  tmpToken.attribute.string = tmpString2;
  tmpToken.type = KEYWORD;
  insertSymbolTable(tableG, tmpToken, FUNCTION); //vlozeni funkce do tabulky funkci

  symtableItem *tmpIT2 = searchSymbolTable(tableG, tmpToken);
  tmpIT2->elementType.function->argCount = -1;


  //-------------------LEN
  smartString *tmpString3 = malloc(sizeof(smartString));
  stringInit(tmpString3);
  stringAddChar(tmpString3,'l');
  stringAddChar(tmpString3,'e');
  stringAddChar(tmpString3,'n');

  tmpToken.attribute.string = tmpString3;
  tmpToken.type = KEYWORD;
  insertSymbolTable(tableG, tmpToken, FUNCTION); //vlozeni funkce do tabulky funkci

  functionData *item;
  item = (functionData *) malloc(sizeof(functionData));
  item->arguments = (symtableItem *) malloc(sizeof(symtableItem));
  item->arguments[0].type = VARIABLE;
  (item->arguments[0]).elementType.variable = (variableData *) malloc(sizeof(variableData));
  (item->arguments[0]).elementType.variable->type = VARIABLE;
  (item->arguments[0]).key = "s";
  item->argCount = 1;
  symtableItem *tmpITT = searchSymbolTableWithString(tableG, "len");
  item->sT = tmpITT->elementType.function->sT;

  tmpITT->next = tableG->symtabList[hash("len")]; //pokud je něco na stejným indexu =)
  tmpITT->elementType.function = item;
  tableG->symtabList[hash("len")] = tmpITT;

  //-------------------SUBSTR
  smartString *tmpString4 = malloc(sizeof(smartString));
  stringInit(tmpString4);
  stringAddChar(tmpString4,'s');
  stringAddChar(tmpString4,'u');
  stringAddChar(tmpString4,'b');
  stringAddChar(tmpString4,'s');
  stringAddChar(tmpString4,'t');
  stringAddChar(tmpString4,'r');

  tmpToken.attribute.string = tmpString4;
  tmpToken.type = KEYWORD;
  insertSymbolTable(tableG, tmpToken, FUNCTION); //vlozeni funkce do tabulky funkci

  functionData *item0;
  item0 = (functionData *) malloc(sizeof(functionData));
  item0->arguments = (symtableItem *) malloc( 3 * sizeof(symtableItem));
  item0->arguments[0].type = VARIABLE;
  (item0->arguments[0]).elementType.variable = (variableData *) malloc(sizeof(variableData));
  (item0->arguments[0]).elementType.variable->type = VARIABLE;
  (item0->arguments[0]).key = "s";
  item0->arguments[1].type = VARIABLE;
  (item0->arguments[1]).elementType.variable = (variableData *) malloc(sizeof(variableData));
  (item0->arguments[1]).elementType.variable->type = VARIABLE;
  (item0->arguments[1]).key = "i";
  item0->arguments[2].type = VARIABLE;
  (item0->arguments[2]).elementType.variable = (variableData *) malloc(sizeof(variableData));
  (item0->arguments[2]).elementType.variable->type = VARIABLE;
  (item0->arguments[2]).key = "n";
  item0->argCount = 3;
  symtableItem *tmpITT0 = searchSymbolTableWithString(tableG, "substr");
  item0->sT = tmpITT0->elementType.function->sT;

  tmpITT0->next = tableG->symtabList[hash("substr")]; //pokud je něco na stejným indexu =)
  tmpITT0->elementType.function = item0;
  tableG->symtabList[hash("substr")] = tmpITT0;

  //-------------------ORD
  smartString *tmpString5 = malloc(sizeof(smartString));
  stringInit(tmpString5);
  stringAddChar(tmpString5,'o');
  stringAddChar(tmpString5,'r');
  stringAddChar(tmpString5,'d');

  tmpToken.attribute.string = tmpString5;
  tmpToken.type = KEYWORD;
  insertSymbolTable(tableG, tmpToken, FUNCTION); //vlozeni funkce do tabulky funkci

  functionData *item1;
  item1 = (functionData *) malloc(sizeof(functionData));
  item1->arguments = (symtableItem *) malloc(2 * sizeof(symtableItem));
  item1->arguments[0].type = VARIABLE;
  (item1->arguments[0]).elementType.variable = (variableData *) malloc(sizeof(variableData));
  (item1->arguments[0]).elementType.variable->type = VARIABLE;
  (item1->arguments[0]).key = "s";
  item1->arguments[1].type = VARIABLE;
  (item1->arguments[1]).elementType.variable = (variableData *) malloc(sizeof(variableData));
  (item1->arguments[1]).elementType.variable->type = VARIABLE;
  (item1->arguments[1]).key = "i";
  item1->argCount = 2;
  symtableItem *tmpITT1 = searchSymbolTableWithString(tableG, "ord");
  item1->sT = tmpITT1->elementType.function->sT;

  tmpITT1->next = tableG->symtabList[hash("ord")]; //pokud je něco na stejným indexu =)
  tmpITT1->elementType.function = item1;
  tableG->symtabList[hash("ord")] = tmpITT1;

  //-------------------CHR
  smartString *tmpString6 = malloc(sizeof(smartString));
  stringInit(tmpString6);
  stringAddChar(tmpString6,'c');
  stringAddChar(tmpString6,'h');
  stringAddChar(tmpString6,'r');

  tmpToken.attribute.string = tmpString6;
  tmpToken.type = KEYWORD;
  insertSymbolTable(tableG, tmpToken, FUNCTION); //vlozeni funkce do tabulky funkci

  functionData *item2;
  item2 = (functionData *) malloc(sizeof(functionData));
  item2->arguments = (symtableItem *) malloc(sizeof(symtableItem));
  item2->arguments[0].type = VARIABLE;
  (item2->arguments[0]).elementType.variable = (variableData *) malloc(sizeof(variableData));
  (item2->arguments[0]).elementType.variable->type = VARIABLE;
  (item2->arguments[0]).key = "s";
  item2->argCount = 1;
  symtableItem *tmpITT2 = searchSymbolTableWithString(tableG, "chr");
  item2->sT = tmpITT2->elementType.function->sT;

  tmpITT2->next = tableG->symtabList[hash("chr")]; //pokud je něco na stejným indexu =)
  tmpITT2->elementType.function = item2;
  tableG->symtabList[hash("chr")] = tmpITT2;
}

// hlavni funkce ktera udela parse :]]]
int parse(symbolTable *ST,  tDLList *instrList){
    int result; //to co budeme vracet (bud error, nebo OK)
    tableG = ST; // globalni promenna pro globalni promenne
    list = instrList; //list do ktereho se budou davat instrukce

    addBuildInFunc();

    result = initFunctions();
    if(result != OK) return result;

    result = program();

    return result;
}


// DEBUGGING
int main(){
    stack = malloc(sizeof(tStack));
    stackInit(stack);
    stackPush(stack, 0);
    tokenAct.type = 0;
    tokenAct.attribute.INT = 0;

    tDLList *instrList = malloc(sizeof(tDLList));
    DLInitList(instrList);
    symbolTable *tableGG = initSymbolTable(MAX_SYMTABLE_SIZE);
    //setFile("txt.txt");
    //freopen("txt.txt","r",stdin);
    int result = parse(tableGG, instrList);
    if(result != OK) return result;
    //printf("%d\n", result);
    degenerate(instrList);

    return result;
}


// indent a za nim dokumentacni komentar
//none v callparams + v pushs(generatoru)
