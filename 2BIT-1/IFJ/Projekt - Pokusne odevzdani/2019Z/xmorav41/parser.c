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
//smartString attr;        // globalni promenna, ve ktere bude ulozen atribut tokenu
int error = OK;

int counterVar = 1;
tStack *stack;
int doIndent = 0;

// generuje jedinecne nazvy identifikatoru
// nazev se sklada ze znaku $ nasledovanym cislem
// postupne se tu generuji prirozena cisla a do nazvu promenne se ukladaji
// v reverzovanem poradi - na funkcnost to nema vliv, ale je jednodussi implementace
void generateVariable(smartString *var) {
    stringClear(var);
    stringAddChar(var, '$');
    int i = counterVar; //counterVar je globalní proměnná, kdyžtak změnit na argument i guess
    while (i != 0) {
        stringAddChar(var, (char)(i % 10 + '0'));
        i = i / 10;
    }
    counterVar ++;
}

// vlozi novou instrukci do seznamu instrukci
tInstr *generateInstruction(int instType, void *addr1, void *addr2, void *addr3) {
    tInstr I;
    I.instType = instType;
    I.addr1 = addr1;
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
  tInstr *jmp1, *jmp2;


  if(tokenAct.type == KEYWORD){
    if(tokenAct.attribute.keyword == IF){ // IF----------------------------------
        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        if(tokenAct.type != INT && tokenAct.type != FLOAT && tokenAct.type != STR && tokenAct.type != LITERAL && tokenAct.type != DOCCOM){
          fprintf(stderr, "Ocekaval se vyraz, ale prisel necekany token\n");
          return PARSING_ERR;
        }
        result = expression();
        if(result != OK) return result;
        jmp1 = generateInstruction(I_JUMP, NULL, NULL, NULL); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!2. NULL musi byt vysledek expression

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

        jmp2 = generateInstruction(I_JUMP, NULL, NULL, NULL); //???????????????????????????????????????????????????????????????
        generateInstruction(I_LABEL, NULL, NULL, NULL); //???????????????????????????????????????????????????????????????

        jmp1->addr1 = list->Last;

        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        do{
            result = statement(funName);
            if(result != OK) return result;//kouknout jestli statement probehl bez erroru
        } while(tokenAct.type != DEDENT && tokenAct.type != EOFTOKEN);

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

        generateInstruction(I_LABEL, NULL, NULL, NULL);
        jmp2->addr1 = list->Last;

        return OK;

        /*
        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        if(tokenAct.type != EOL){
            return PARSING_ERR;
        }
        doIndent = 1;
        tokenAct = nextToken(&error, stack, doIndent);
        doIndent = 0;
        if(error != OK) return error; // zkoumani lexikalniho erroru
        if(tokenAct.type != DEDENT){
            return PARSING_ERR;
        }
        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        return OK;*/
    }
    else if(tokenAct.attribute.keyword == WHILE){ // WHILE ---------------------------------
        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru

        jmp2 = generateInstruction(I_JUMP, NULL, NULL, NULL);
        generateInstruction(I_LABEL, NULL, NULL, NULL);
        jmp2->addr1 = list->Last;

        if(tokenAct.type != INT && tokenAct.type != FLOAT && tokenAct.type != STR && tokenAct.type != LITERAL && tokenAct.type != DOCCOM){
          fprintf(stderr, "Ocekaval se vyraz, ale prisel necekany token\n");
          return PARSING_ERR;
        }
        result = expression();
        if(result != OK) return result;
        jmp1 = generateInstruction(I_JUMP, NULL, NULL, NULL); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!2. NULL musi byt vysledek expression

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

        tokenAct = nextToken(&error, stack, doIndent);
        if(error != OK) return error; // zkoumani lexikalniho erroru
        do{
            result = statement(funName);
            if(result != OK) return result;//kouknout jestli statement probehl bez erroru
        } while(tokenAct.type != DEDENT && tokenAct.type != EOFTOKEN);

        generateInstruction(jmp2->instType, jmp2->addr1, jmp2->addr2, jmp2->addr3);
        generateInstruction(I_LABEL, NULL, NULL, NULL);
        jmp1->addr1 = list->Last;

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
        if(tokenAct.type != INT && tokenAct.type != FLOAT && tokenAct.type != STR && tokenAct.type != LITERAL && tokenAct.type != DOCCOM){
          fprintf(stderr, "Ocekaval se vyraz, ale prisel necekany token\n");
          return PARSING_ERR;
        }
        result = expression();
        if(result != OK) return result;

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
    else if(tokenAct.attribute.keyword == INPUTS ||
            tokenAct.attribute.keyword == INPUTF ||
            tokenAct.attribute.keyword == INPUTI ||
            tokenAct.attribute.keyword == PRINT  ||
            tokenAct.attribute.keyword == LEN    ||
            tokenAct.attribute.keyword == SUBSTR ){
              tokenAct = nextToken(&error, stack, doIndent);
              if(error != OK) return error; // zkoumani lexikalniho erroru
              if(tokenAct.type == LEFTBRACKET){
                fprintf(stderr, "za nazvem vestavene funkce musi nasledovat ( \n");
                return PARSING_ERR;
              }
              //callParams atd atd
    }
    else if(tokenAct.attribute.keyword == NONE){
      result = expression();
      if(result != OK) return result;

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
        }
        else{
          tmpItem = searchSymbolTable(tableG, tmpToken);
        }
      } else {
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
        //printf("%s\n", tableG->symtabList[hash(funName)]->elementType.function->sT->symtabList[hash(tmpToken.attribute.string->string)]->key);
      }


      if(tokenAct.type == STR){
        tmpToken = tokenAct;
        symtableItem *tmpItem1 = searchSymbolTable(tableG, tmpToken); //funkce v tabulce
        if(tmpItem1 != NULL && tmpItem1->type == FUNCTION){
          tokenAct = nextToken(&error, stack, doIndent);
          if(error != OK) return error; // zkoumani lexikalniho erroru

          if(tokenAct.type != LEFTBRACKET) return PARSING_ERR;

          result = callParams(tmpToken.attribute.string->string);
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
      }

      result = expression();
      if(result != OK) return result;

      tmpItem->defined = true;
      //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!TADY BY SE DO PROMENNE MELA ZAPSAT VALUE A TYP
      /*tmpItem->elementType.variable->value = ;
      tmpItem->elementType.variable->type = ;
      */
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
    else{
      tokenAct = ungetToken(&error, stack, doIndent);
      tmpToken = tokenAct;
      result = expression();
      if(result != OK) return result;

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
  else if(tokenAct.type == INT || tokenAct.type == FLOAT || tokenAct.type == LITERAL || tokenAct.type == LEFTBRACKET || DOCCOM){
    result = expression();
    if(result != OK) return result;

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
    fprintf(stderr, "Ocekaval se prikaz, ale bohuzel prisel jiny token\n");
    return PARSING_ERR;
  }

  return OK;
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

    /*
    //musi nasledovat identifikator
    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru

    if(tokenAct.type != STR && tokenAct.type != RIGHTBRACKET) return PARSING_ERR;

    if (tokenAct.type == STR) {
        tokenAct = nextToken(&error, stack, doIndent);
        if (error != OK) return error; // zkoumani lexikalniho erroru
        while (tokenAct.type != RIGHTBRACKET) {

            //dalsi token musi byt ','
            if (tokenAct.type != COMMA) return PARSING_ERR;

            //dalsi token musi byt identifikator
            tokenAct = nextToken(&error, stack, doIndent);
            if (error != OK) return error; // zkoumani lexikalniho erroru

            if (tokenAct.type != STR) return PARSING_ERR;

            tokenAct = nextToken(&error, stack, doIndent);
            if (error != OK) return error; // zkoumani lexikalniho erroru
        }
    }
*/
    /*result = defParams(s->string);
    if(result != OK) return result;
    */
    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru
    while(tokenAct.type != RIGHTBRACKET){
      tokenAct = nextToken(&error, stack, doIndent);
      if(error != OK) return error; // zkoumani lexikalniho erroru
    }


    generateInstruction(I_CLEARS, NULL, NULL, NULL);

    tokenAct = nextToken(&error, stack, doIndent);
    if(error != OK) return error; // zkoumani lexikalniho erroru
    token tmpToken = tokenAct;
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
    //prvni token
    tokenAct = nextToken(&error, stack, doIndent);
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
    if (tmpItem0->elementType.function->argCount == 0 || tmpItem0->elementType.function->argCount == -1) {
      return OK;
    }
    else{
      fprintf(stderr, "volate funkci, ktera ocekava nejake parametry, ale vy do ni zadne nedavate\n");
      return SEM_PAR_ERR;
    }
  }

  if(tmpItem0->elementType.function->argCount <= 0 && tmpItem0->elementType.function->argCount != -1){
    fprintf(stderr, "snazite se do volani funkce vlozit vic parametru, nez funkce vyzaduje\n");
    return SEM_MISC_ERR;
  }
  else{
    if(tokenAct.type == STR){
      symtableItem *tmpItem = searchSymbolTable(tableG, tokenAct);
    /*  symtableItem *tmp = searchSymbolTableWithString(tableG, funName);
      for(int i = 0; i < tmpItem->elementType.function->argCount; i++){
        //porovnat tmpToken s argumentama
        if(strcmp(tmp->elementType.function->arguments[i].key, tokenAct.attribute.string->string) == 0){
          tmpItem = &(tmp->elementType.function->arguments[i]);
          //succMePls = 1;
          if(tmpItem->defined != true){
            tmpItem->defined = false;
          }
          break;
        }
      }*/

      if(tmpItem != NULL && tmpItem->type == FUNCTION){
        fprintf(stderr, "do argumentu funkce nemuzete davat funkce(nazvy funkci)\n");
        return SEM_MISC_ERR;
      }
      else if(tmpItem != NULL && tmpItem->type == VARIABLE){
        (tmpItem0->elementType.function->arguments[0]).elementType.variable->value = tmpItem->elementType.variable->value;
        (tmpItem0->elementType.function->arguments[0]).elementType.variable->type = tmpItem->elementType.variable->type;
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
      }
      else if(tokenAct.type == FLOAT){
        (tmpItem0->elementType.function->arguments[0]).elementType.variable->value.FLOAT = tokenAct.attribute.FLOAT;
        (tmpItem0->elementType.function->arguments[0]).elementType.variable->type = DATA_FLOAT;
      }
      else if(tokenAct.type == LITERAL || tokenAct.type == DOCCOM){
        (tmpItem0->elementType.function->arguments[0]).elementType.variable->value.string = tokenAct.attribute.string->string;
        (tmpItem0->elementType.function->arguments[0]).elementType.variable->type = DATA_STRING;
      }
    }
    else if (tokenAct.type == KEYWORD && tokenAct.attribute.keyword == NONE){
      (tmpItem0->elementType.function->arguments[0]).elementType.variable->type = DATA_UNDEFINED;
    }
    else{
      fprintf(stderr, "Nespravny typ argumentu pri volani funkce\n");
      return PARSING_ERR;
    }

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
        (tmpItem0->elementType.function->arguments[argc]).elementType.variable->value = tmpItem->elementType.variable->value;
        (tmpItem0->elementType.function->arguments[argc]).elementType.variable->type = tmpItem->elementType.variable->type;
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
      }
      else if(tokenAct.type == FLOAT){
        (tmpItem0->elementType.function->arguments[argc]).elementType.variable->value.FLOAT = tokenAct.attribute.FLOAT;
        (tmpItem0->elementType.function->arguments[argc]).elementType.variable->type = DATA_FLOAT;
      }
      else if(tokenAct.type == LITERAL || tokenAct.type == DOCCOM){
        (tmpItem0->elementType.function->arguments[argc]).elementType.variable->value.string = tokenAct.attribute.string->string;
        (tmpItem0->elementType.function->arguments[argc]).elementType.variable->type = DATA_STRING;
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
    return callParamsN(funName, argc);
  }
}

/*projede cely soubor a najde tam definice funkci a tyto funkce vlozi do tabulky funkci*/
int initFunctions(){
    int result = OK;
    tokenAct = nextToken(&error, stack, doIndent);
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

  symtableItem *tmpIT3 = searchSymbolTable(tableG, tmpToken);
  tmpIT3->elementType.function->argCount = 1;
  /*item1->arguments = (symtableItem *) malloc(sizeof(symtableItem));
  item1->arguments[0].type = VARIABLE;
  (item1->arguments[0]).elementType.variable = (variableData *) malloc(sizeof(variableData));
  (item1->arguments[0]).elementType.variable->type = VARIABLE;
  (item1->arguments[0]).key = "s";*/  //????????????????????????????????????????????????????????? maybe? kdyztak dodelat i u substr
  //item1->argCount = 1;
  //tableG->symtabList[hash("len")]->elementType.function = item1;

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

  symtableItem *tmpIT4 = searchSymbolTable(tableG, tmpToken);
  tmpIT4->elementType.function->argCount = 3;
  //tableG->symtabList[hash("substr")]->elementType.function = item2;

  //-------------------ORD
  smartString *tmpString5 = malloc(sizeof(smartString));
  stringInit(tmpString5);
  stringAddChar(tmpString5,'o');
  stringAddChar(tmpString5,'r');
  stringAddChar(tmpString5,'d');

  tmpToken.attribute.string = tmpString5;
  tmpToken.type = KEYWORD;
  insertSymbolTable(tableG, tmpToken, FUNCTION); //vlozeni funkce do tabulky funkci

  symtableItem *tmpIT5 = searchSymbolTable(tableG, tmpToken);
  tmpIT5->elementType.function->argCount = 2;

  //-------------------CHR
  smartString *tmpString6 = malloc(sizeof(smartString));
  stringInit(tmpString6);
  stringAddChar(tmpString6,'c');
  stringAddChar(tmpString6,'h');
  stringAddChar(tmpString6,'r');

  tmpToken.attribute.string = tmpString6;
  tmpToken.type = KEYWORD;
  insertSymbolTable(tableG, tmpToken, FUNCTION); //vlozeni funkce do tabulky funkci

  symtableItem *tmpIT6 = searchSymbolTable(tableG, tmpToken);
  tmpIT6->elementType.function->argCount = 1;
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


//for the MEMES
int expression(){
  tokenAct = nextToken(&error, stack, doIndent);
  if(error != OK) return error; // zkoumani lexikalniho erroru
  return OK;
}










// DEBUGGING
//||
//|| Terminal now= [ ./parser <txt.txt ]
//||
int main(){
    stack = malloc(sizeof(tStack));
    stackInit(stack);
    stackPush(stack, 0);

    tDLList *instrList = malloc(sizeof(tDLList));
    DLInitList(instrList);
    symbolTable *tableGG = initSymbolTable(MAX_SYMTABLE_SIZE);

    //setFile("txt.txt");

    int result = parse(tableGG, instrList);

    //printf("%d\n", result);
    return result;
}


// expression ... predpoklada se, ze do expression prijde rovnou novy token ktery se ma rozparsovat
//                a ze z expression jeden token vyjde

//malloc argumentu vestavenych funkci ?
