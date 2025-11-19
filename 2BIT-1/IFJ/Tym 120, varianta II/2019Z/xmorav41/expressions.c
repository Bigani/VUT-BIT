/**
  * expressions.c
  * Project IFJ19
  *
  * Březina Jindřich (xbrezi21@stud.fit.vutbr.cz)
  * Gumančík Pavol (xguman01@stud.fit.vutbr.cz)
  * Kotáb Dominik (xkotab01@stud.fit.vutbr.cz)
  * Moravčík Tomáš (xmorav41@stud.fit.vutbr.cz)
  *
  * Brief: implementation of operations that parse expressions
  *
  */

#include "expressions.h"

// KDYZ PRIJDE PRVNE TO CO JE NAPRAVO A PAK AZ TO CO PRIJDE NAHORE, PAK
// priklad prvni radek, druha cell ... +* ... prvne se provede *
// A=0  '<' = prvne se provede to co je nahore v komentu
// B=1  '=' = jen u '(' a ')' zároveň
// C=2  '>' = prvne se provede to co je napravo v komentu
// D=3      = done xd
// E=4      = chybný sytax i guess

int precedenceTable[TABLESIZE][TABLESIZE] =
{//  +-  *///    (     )     i     r     $ <- vstup, dolu stack
  {  C  ,  A  ,  A  ,  C  ,  A  ,  C  ,  C  }, // +-
  {  C  ,  C  ,  A  ,  C  ,  A  ,  C  ,  C  }, // *///
  {  A  ,  A  ,  A  ,  B  ,  A  ,  A  ,  E  }, // (
  {  C  ,  C  ,  E  ,  C  ,  E  ,  C  ,  C  }, // )
  {  C  ,  C  ,  E  ,  C  ,  E  ,  C  ,  C  }, // i
  {  A  ,  A  ,  A  ,  C  ,  A  ,  E  ,  C  }, // r (relationship operators)
  {  A  ,  A  ,  A  ,  E  ,  A  ,  A  ,  D  }  // $
};

int getPrecedenceIndex(token* tokenAct) { //vrací index z precedence tablu
  switch (tokenAct->type) {
    case PLUS: // +
    case MINUS: // -
      return 0;
    case TIMES: // *
    case DIVFLT: // /
    case DIVINT: // //
      return 1;
    case LEFTBRACKET: // (
      return 2;
    case RIGHTBRACKET: // )
      return 3;
    case INT: // i
    case FLOAT: // i
    case STR: // i
    case LITERAL: // i
    case DOCCOM: // stejný co literal
    case UNDEFINED: //uhhh
    case KEYWORD: //NONE
      if (tokenAct->type == KEYWORD && tokenAct->attribute.keyword != NONE) {
        return -1;
      }
    return 4;
  case LESSEQ: // <=
  case GREATEREQ: // >=
  case EQ: // ==
  case NOTEQ: // !=
  case LESS: // <
  case GREATER: // >
    return 5;
  case COLON: //$
  case EOL:
  case EOFTOKEN:
    return 6;
  default:
    return -1;
  }
}

int getPrecedenceOperatorValue(token* stackToken, token* vstupniToken) {
  int index1, index2 = 0;
  index1 = getPrecedenceIndex(stackToken);
  index2 = getPrecedenceIndex(vstupniToken);
  if (index1 == -1 || index2 == -1) {
    return E;
  }
  return precedenceTable[index1][index2];
}

int findRule(tokenStack *s, int *type, symbolTable* tableG, symbolTable* tableGG, char* jmenoFunkce) {
  token* token = malloc(sizeof(token));
  token->attribute.INT = 0;
  token->type = 0;
  sData* data = NULL;
  int state = 0;
  int zpracuj = 0;
  int rule = 0;
  int operacevtokenu = -1;
  int zesym = 0;
  while (rule == 0) { //dokud to neudělá nějakou konverzi podle i -> E, E + E -> E, (E) -> E atd..
    int type1, type2;
    data = tokenStackTop(s);
    switch (state) {
      case 0: //prvně to vždy půjde sem
        if (data->type == typeNonterm) {
          state = 1;
          type1 = data->dataType;
        } else if (data->type == typeToken) {
          if (data->token->type == RIGHTBRACKET) {
            state = 2;
            zpracuj = 1;
          } else if (data->token->type == INT || data->token->type == FLOAT || data->token->type == LITERAL || data->token->type == STR || (data->token->type == KEYWORD && data->token->attribute.keyword == NONE) || data->token->type == DOCCOM || data->token->type == UNDEFINED) { //projde jen int, float, literal, str, atd.
            token = data->token;
            zpracuj = 2;
            state = 3;
            if (data->token->type == INT) {
              type1 = INT;
            } else if (data->token->type == FLOAT) {
              type1 = FLOAT;
            } else if (data->token->type == LITERAL || data->token->type == DOCCOM) {
              type1 = LITERAL;
            } else if (data->token->type == KEYWORD && data->token->attribute.keyword == NONE) {
              type1 = KEYWORD;
            } else if (data->token->type == STR) { //je to proměnná, prohledávám symtable
              symtableItem* item = NULL;
              if (tableGG) { //existuje funkce
                item = searchSymbolTableWithString(tableGG, data->token->attribute.string->string);
                if (item) { //bylo to v tabulce funkce
                  zesym = 2;
                  if (item->type == VARIABLE && item->elementType.variable->type == DATA_INT) {
                    type1 = INT;
                  } else if (item->type == VARIABLE && item->elementType.variable->type == DATA_FLOAT) {
                    type1 = FLOAT;
                  } else if (item->type == VARIABLE && item->elementType.variable->type == DATA_STRING) {
                    type1 = LITERAL;
                  } else {
                    type1 = UNDEFINED;
                  }
                } else { //nebylo to v tabulce funkce, prohledám args
                  item = searchSymbolTableWithString(tableG, jmenoFunkce);
                  for (int i = 0; i < item->elementType.function->argCount; i++) {
                    if (strcmp(item->elementType.function->arguments[i].key, data->token->attribute.string->string) == 0) { //je to arg fun
                      zesym = 2;
                      type1 = UNDEFINED;
                    }
                  }
                }
              }
              if (tableG && zesym == 0) { //nebylo to ve funkci
                item = searchSymbolTableWithString(tableG, data->token->attribute.string->string);
                if (item) {
                  zesym = 1;
                  if (item->type == VARIABLE && item->elementType.variable->type == DATA_INT) {
                    type1 = INT;
                  } else if (item->type == VARIABLE && item->elementType.variable->type == DATA_FLOAT) {
                    type1 = FLOAT;
                  } else if (item->type == VARIABLE && item->elementType.variable->type == DATA_STRING) {
                    type1 = LITERAL;
                  } else {
                    type1 = UNDEFINED;
                  }
                } else { //není v symtablu
                  return -3; //proměnná není deklarovaná
                }
              }
            } else {
              return -1; //dostanu něco jinýho než int, str, float, lit
            }
          } else {
            return -1; //dostanu něco jinýho než int, str, float, lit
          }
        } else {
          return -1; //dostanu něco jinýho než token a nonterm
        }
        break;
      case 1:
        if (data->token->type == PLUS || data->token->type == MINUS || data->token->type == TIMES || data->token->type == DIVFLT || data->token->type == DIVINT || data->token->type == LESS || data->token->type == GREATER || data->token->type == LESSEQ || data->token->type == GREATEREQ || data->token->type == EQ || data->token->type == NOTEQ) {
          state = 2;
        } else {
          return -5; //neplatný výraz mezi 2mi věcmi
        }
        zpracuj = 3;
        operacevtokenu = data->token->type;
        break;
      case 2:
        if (data->type == typeNonterm) {
          if (zpracuj == 1) { //tady řeším závorky
            state = 4;
            type1 = data->dataType;
          } else {
            state = 3;
            type2 = data->dataType;
          }
        }
        break;
      case 3:
        if (state == 3) {
          if (data->type == typeHandler) {
            sData* newData = calloc(1, sizeof(sData));
            if (newData == NULL) {
              return -2; //intern err
            }
            newData->type = typeNonterm;
            if (zpracuj == 2) { //zpracování i -> E
              int* typetokenu = (int *) malloc(sizeof(int));
              char* promena;
              if (zesym > 0) {
                 promena = (char *) malloc(sizeof(char));
              }
              if (type1 == INT && zesym == 0) { //pushuju INT do generátoru
                int* hodnota = (int*) malloc(sizeof(int));
                *hodnota = token->attribute.INT;
                *typetokenu = token->type;
                generateInstruction(I_PUSHS, hodnota, typetokenu, NULL);
              } else if (type1 == KEYWORD && zesym == 0) { //pushuju None do generátoru
                *typetokenu = token->type;
                generateInstruction(I_PUSHS, NULL, typetokenu, NULL);
              } else if (zesym == 1) { //z globálního symtablu, pushuju název proměnné
                promena = token->attribute.string->string;
                generateInstruction(I_PUSHS, promena, NULL, promena);
              } else if (type1 == LITERAL && zesym == 0) { //pushuju STR do generátoru
                char* hodnota = (char *) malloc(sizeof(char));
                hodnota = token->attribute.string->string;
                *typetokenu = token->type;
                generateInstruction(I_PUSHS, hodnota, typetokenu, NULL);
              } else if (type1 == FLOAT && zesym == 0) { //pushuju FLOAT do generátoru
                double* hodnota = (double *) malloc(sizeof(double));
                *hodnota = token->attribute.FLOAT;
                *typetokenu = token->type;
                generateInstruction(I_PUSHS, hodnota, typetokenu, NULL);
              } else if (zesym == 2) { //ze symtablu funkce nebo z jejího parametru
                promena = token->attribute.string->string;
                generateInstruction(I_PUSHSLF, promena, NULL, promena);
              }
            } else if (zpracuj == 1) { //tady zavorky
              newData->dataType = type1;
            } else if (type1 == INT && type2 == INT) { //tady generuju instrukce mezi dvěmi INT
              if (operacevtokenu == PLUS) { // +
                generateInstruction(I_ADDS, NULL, NULL, NULL);
              } else if (operacevtokenu == MINUS) { // -
                generateInstruction(I_SUBS, NULL, NULL, NULL);
              } else if (operacevtokenu == TIMES) { // *
                generateInstruction(I_MULS, NULL, NULL, NULL);
              } else if (operacevtokenu == DIVINT) { // //
                generateInstruction(I_IDIVS, NULL, NULL, NULL);
              } else if (operacevtokenu == DIVFLT) { // /
                generateInstruction(I_DIVS, NULL, NULL, NULL);
                type1 = FLOAT; //idk honestly jestli nutný, ale nechám to tu
              } else if (operacevtokenu == LESS) { // <
                generateInstruction(I_LTS, NULL, NULL, NULL);
              } else if (operacevtokenu == GREATER) { // >
                generateInstruction(I_GTS, NULL, NULL, NULL);
              } else if (operacevtokenu == LESSEQ) { // <=
                generateInstruction(I_LT, NULL, NULL, NULL);
              } else if (operacevtokenu == GREATEREQ) { // >=
                generateInstruction(I_GT, NULL, NULL, NULL);
              } else if (operacevtokenu == EQ) { // ==
                generateInstruction(I_EQS, NULL, NULL, NULL);
              } else if (operacevtokenu == NOTEQ) { // !=
                generateInstruction(I_NQS, NULL, NULL, NULL);
              } else {
                return -5; //neplatná operace mezi 2mi inty
              }
            } else if (type1 == LITERAL && type2 == LITERAL) { //platné operace mezi dvěmi STR
              if (operacevtokenu == PLUS) { // +
                generateInstruction(I_CONCAT, NULL, NULL, NULL);
              } else if (operacevtokenu == LESS) { // <
                generateInstruction(I_LTS, NULL, NULL, NULL);
              } else if (operacevtokenu == GREATER) { // >
                generateInstruction(I_GTS, NULL, NULL, NULL);
              } else if (operacevtokenu == LESSEQ) { // <=
                generateInstruction(I_LT, NULL, NULL, NULL);
              } else if (operacevtokenu == GREATEREQ) { // >=
                generateInstruction(I_GT, NULL, NULL, NULL);
              } else if (operacevtokenu == EQ) { // ==
                generateInstruction(I_EQS, NULL, NULL, NULL);
              } else if (operacevtokenu == NOTEQ) { // =!
                generateInstruction(I_NQS, NULL, NULL, NULL);
              } else {
                return -5; //neplatná operace mezi dvěma str
              }
            } else if (type1 == FLOAT && type2 == FLOAT) { //tady generuju platné operace mezi dvěma FLOAT
              if (operacevtokenu == PLUS) { // +
                generateInstruction(I_ADDS, NULL, NULL, NULL);
              } else if (operacevtokenu == MINUS) { // -
                generateInstruction(I_SUBS, NULL, NULL, NULL);
              } else if (operacevtokenu == TIMES) { // *
                generateInstruction(I_MULS, NULL, NULL, NULL);
              } else if (operacevtokenu == DIVFLT) { // /
                generateInstruction(I_DIVS, NULL, NULL, NULL);
              } else if (operacevtokenu == LESS) { // <
                generateInstruction(I_LTS, NULL, NULL, NULL);
              } else if (operacevtokenu == GREATER) { // >
                generateInstruction(I_GTS, NULL, NULL, NULL);
              } else if (operacevtokenu == LESSEQ) { // <=
                generateInstruction(I_LT, NULL, NULL, NULL);
              } else if (operacevtokenu == GREATEREQ) { // >=
                generateInstruction(I_GT, NULL, NULL, NULL);
              } else if (operacevtokenu == EQ) { // ==
                generateInstruction(I_EQS, NULL, NULL, NULL);
              } else if (operacevtokenu == NOTEQ) { // !=
                generateInstruction(I_NQS, NULL, NULL, NULL);
              } else {
                return -5; //neplatná operace mezi 2mi floaty
              }
            } else if ((type1 == INT && type2 == FLOAT) || (type1 == FLOAT && type2 == INT)) {
              if (type1 == INT && type2 == FLOAT) {
                type1 = FLOAT;
              }
              if (operacevtokenu == PLUS) { // +
                generateInstruction(I_ADDS, NULL, NULL, NULL);
              } else if (operacevtokenu == MINUS) { // -
                generateInstruction(I_SUBS, NULL, NULL, NULL);
              } else if (operacevtokenu == TIMES) { // *
                generateInstruction(I_MULS, NULL, NULL, NULL);
              } else if (operacevtokenu == DIVFLT) { // /
                generateInstruction(I_DIVS, NULL, NULL, NULL);
              } else if (operacevtokenu == LESS) { // <
                generateInstruction(I_LTS, NULL, NULL, NULL);
              } else if (operacevtokenu == GREATER) { // >
                generateInstruction(I_GTS, NULL, NULL, NULL);
              } else if (operacevtokenu == LESSEQ) { // <=
                generateInstruction(I_LT, NULL, NULL, NULL);
              } else if (operacevtokenu == GREATEREQ) { // >=
                generateInstruction(I_GT, NULL, NULL, NULL);
              } else if (operacevtokenu == EQ) { // ==
                generateInstruction(I_EQS, NULL, NULL, NULL);
              } else if (operacevtokenu == NOTEQ) { // !=
                generateInstruction(I_NQS, NULL, NULL, NULL);
              } else {
                return -5; //neplatná operace mezi int a float
              }
            } else if (type1 == KEYWORD || type2 == KEYWORD) { //operace mezi None a čímkoliv jiným
              if (operacevtokenu == LESS) { // <
                generateInstruction(I_LTS, NULL, NULL, NULL);
              } else if (operacevtokenu == GREATER) { // >
                generateInstruction(I_GTS, NULL, NULL, NULL);
              } else if (operacevtokenu == LESSEQ) { // <=
                generateInstruction(I_LT, NULL, NULL, NULL);
              } else if (operacevtokenu == GREATEREQ) { // >=
                generateInstruction(I_GT, NULL, NULL, NULL);
              } else if (operacevtokenu == EQ) { // ==
                generateInstruction(I_EQS, NULL, NULL, NULL);
              } else if (operacevtokenu == NOTEQ) { // !=
                generateInstruction(I_NQS, NULL, NULL, NULL);
              } else {
                return -5; //neplatná operace mezi NONE a čímkoliv jinýho
              }
            } else if (type1 == UNDEFINED || type2 == UNDEFINED) {
              if (operacevtokenu == PLUS) { // +
                generateInstruction(I_ADDS, NULL, NULL, NULL);
              } else if (operacevtokenu == MINUS) { // -
                generateInstruction(I_SUBS, NULL, NULL, NULL);
              } else if (operacevtokenu == TIMES) { // *
                generateInstruction(I_MULS, NULL, NULL, NULL);
              } else if (operacevtokenu == DIVINT) { // //
                generateInstruction(I_IDIVS, NULL, NULL, NULL);
              } else if (operacevtokenu == DIVFLT) { // /
                generateInstruction(I_DIVS, NULL, NULL, NULL);
              } else if (operacevtokenu == LESS) { // <
                generateInstruction(I_LTS, NULL, NULL, NULL);
              } else if (operacevtokenu == GREATER) { // >
                generateInstruction(I_GTS, NULL, NULL, NULL);
              } else if (operacevtokenu == LESSEQ) { // <=
                generateInstruction(I_LT, NULL, NULL, NULL);
              } else if (operacevtokenu == GREATEREQ) { // >=
                generateInstruction(I_GT, NULL, NULL, NULL);
              } else if (operacevtokenu == EQ) { // ==
                generateInstruction(I_EQS, NULL, NULL, NULL);
              } else if (operacevtokenu == NOTEQ) { // !=
                generateInstruction(I_NQS, NULL, NULL, NULL);
              } else { //neplatná operace mezi UNDEFINED a čímkoliv jinýho (udefined == parametr funkce)
                return -5;
              }
            } else {
              return -5; //neplatná operace mezi dvěmy typy, semantická chyba
            }

            if (data != NULL) {
              tokenStackPop(s);
            }

            *type = type1; //návratový typ
            newData->dataType = type1;
            newData->token = token;
            tokenStackPush(s,newData);
            rule = zpracuj;
            continue;
          } else {
            return -1; //toto by se asi ani nemělo stát
          }
        }
        break;
      case 4:
        if (data->token->type == LEFTBRACKET) {
          state = 3;
        } else {
          return -1; //pro každou pravou závorku musí být levá
        }
        break;
      }

      if (data->token->type != COLON) { //nakonci stacku je mnou vložený $ (colon)
        tokenStackPop(s);
      } else {
        return rule;
    }
  }
  return rule;
}

precendentExpression* doPrecedenceOperation(token tokenAct, token* tokenAct2, symbolTable* tableG, symbolTable* tableGG, char* jmenoFunkce) {
  precendentExpression* exp = malloc(sizeof(precendentExpression));
  if (!exp) {
    return NULL;
  }

  token* current;
  if (tokenAct2) {
    current = tokenAct2;
  } else {
    current = &tokenAct;
  }

  tokenStack *s = (tokenStack*) malloc(sizeof(tokenStack));
  if (s == NULL) {
    exp->error = INTERN_ERR;
    return exp;
  }
  tokenStackInit(s);

  sData *dataPred = malloc(sizeof(sData)); //dávám dolar do stacku
  if(dataPred == NULL){
    exp->error = INTERN_ERR;
    return exp;
  }
  dataPred->type = typeToken;
  token dolar;
  dolar.type = COLON;
  dataPred->token = &dolar;
  tokenStackPush(s, dataPred);

  token tmptkn;
  int navr = 0; //návratová hodnota exp
  int errorE = OK; //error pro načítání při z nextToken

  while(1) { //dostanu se z toho daijoubně, dw
    sData* termData = getTerminalData(s);
    token* token = termData->token;
    int operation = getPrecedenceOperatorValue(token, current); //token = ze stacku, current = ze scanneru

    if (operation == A) { //0 +- atd //zaměň a za a< na zásobníku & push(b) & přečti další symbol b ze vstupu
      sData *data = malloc(sizeof(sData));
      if (data == NULL) {
        exp->error = INTERN_ERR;
        return exp;
      }
      data->type = typeToken;
      data->token = current;
      addHandler(s, termData); //dá zarážku na vrchol zásobníku
      tokenStackPush(s,data);
    } else if (operation == B) { //1 () //push(b) & přečti další symbol b ze vstupu
      sData *data = malloc(sizeof(sData));
      if (data == NULL) {
        exp->error = INTERN_ERR;
        return exp;
      }
      data->type = typeToken;
      data->token = current;
      tokenStackPush(s,data);
    } else if (operation == C) { //2  if <y je na vrcholu zásobníku and r: A→y∈P then zaměň <y za A & vypiš r na výstup else chyba
      int a = findRule(s, &navr, tableG, tableGG, jmenoFunkce); //THE REST OF THE LOVELY OWL
      if (a == -1) { //chyby, které se tu vyskytly předám parseru
        exp->error = PARSING_ERR; //2
        return exp;
      } else if (a == -2) {
        exp->error = INTERN_ERR; //99
        return exp;
      } else if (a == -3) {
        exp->error = SEM_DEF_ERR; //3
        return exp;
      } else if (a == -5) {
        exp->error = SEM_RUN_ERR; //4
        return exp;
      }
      continue; //nenačítat další token ze vstupu (přesně jako podle algoritmu)
    } else if (operation == D) { //D jako done lmao xddddddd, setování naší struktury
        exp->returnToken.type = current->type;
        exp->returnToken.attribute = current->attribute;
        if (navr == INT) {
          exp->returnType = DATA_INT;
        } else if (navr == LITERAL) {
          exp->returnType = DATA_STRING;
        } else if (navr == FLOAT) {
          exp->returnType = DATA_FLOAT;
        } else {
          exp->returnType = DATA_UNDEFINED;
        }
        exp->error = OK;
        return exp;
    } else if (operation == E) { //error
        exp->error = PARSING_ERR;
        return exp;
    }

    current = malloc(sizeof(token)); //načítáme další token ze vstupu (nebo z parseru)
    if (tokenAct2) { //pokud parser poslal 2 tokeny, tak prvně místo načítání ze vstupu zpracujeme token z parseru
      current->attribute = tokenAct.attribute;
      current->type = tokenAct.type;
      tokenAct2 = NULL;
      continue;
    }

    tmptkn = nextToken(&errorE, NULL, 0); //nepotřebuju stack, ani indenty
    if (errorE != OK) {
      exp->error = errorE;
      return exp;
    }
    current->attribute = tmptkn.attribute;
    current->type = tmptkn.type;
  }
}
