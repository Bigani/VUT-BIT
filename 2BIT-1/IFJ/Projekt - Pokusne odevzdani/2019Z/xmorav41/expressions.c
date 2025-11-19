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

int error = OK;
tStack *stack;
int doIndent = 0;

// KDYZ PRIJDE PRVNE TO CO JE NAPRAVO A PAK AZ TO CO PRIJDE NAHORE, PAK
// priklad prvni radek, druha cell ... +* ... prvne se provede *
// A=0  '<' = prvne se provede to co je nahore v komentu
// B=1  '=' = jen u '(' a ')' zároveň
// C=2  '>' = prvne se provede to co je napravo v komentu
// E=3      = chybný sytax i guess

int precedenceTable[TABLESIZE][TABLESIZE] =
{
  //   +-  *///    (     )     i     r     $ <- vstup, dolu stack
    {  C  ,  A  ,  A  ,  C  ,  A  ,  C  ,  C  }, // +-
    {  C  ,  C  ,  A  ,  C  ,  A  ,  C  ,  C  }, // *///
    {  A  ,  A  ,  A  ,  B  ,  A  ,  A  ,  E  }, // (
    {  C  ,  C  ,  E  ,  C  ,  E  ,  C  ,  C  }, // )
    {  C  ,  C  ,  E  ,  C  ,  E  ,  C  ,  C  }, // i
    {  A  ,  A  ,  A  ,  C  ,  A  ,  E  ,  C  }, // r (relationship operators)
    {  A  ,  A  ,  A  ,  E  ,  A  ,  A  ,  E  }  // $
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
    printf("error\n");
  }
  //printf("%d a %d pricemz tokeny meli\n", stackToken->type, vstupniToken->type);
  return precedenceTable[index1][index2];
}

int findRule(tokenStack *s) {
  token* token, tokenPrvni, tokenDruhy;
  int state = 0;
  int estimate_precedence = 0;
  int rule = 0;
  int cisilko = 0;
  int operacevtokenu = -1;
  ATData aData;
  while (rule == 0) {
    SData * data = tokenStackTop(s);
    dataType type1, type2;
    switch (state) {
      case 0:
        if (data->Type == type_nonterm) {
          state = 1;
          type1 = data->DataType;
          cisilko = data->Atr.Token->attribute.INT;
          tokenPrvni = *(data->Atr.Token);
          printf("%d je cicislko\n", cisilko);
        } else if (data->Type == type_token) {
          if (data->Atr.Token->type == RIGHTBRACKET) {
            state = 2;
            estimate_precedence = 1;
          } else if (data->Atr.Token->type == INT) {
            aData.type = at_token;
            token = data->Atr.Token;
            printf("%d\n", data->Atr.Token->attribute.INT);
            estimate_precedence = 13; //????????????
            state = 3;
            type1 = DATA_INT;
          } else if (data->Atr.Token->type == FLOAT) {
            //pracuju se floaty
            type1 = DATA_FLOAT;
          } else if (data->Atr.Token->type == LITERAL) {
            //pracuju se stringy
            type1 = DATA_STRING;
          } else if (data->Atr.Token->type == STR) {
            //prohledávám symtable xd
            //type1 =
          } else {
            return -1;
          }
        } else {
          return -1;
        }
        break;
      case 1:
        if (data->Atr.Token->type == PLUS || data->Atr.Token->type == MINUS || data->Atr.Token->type == TIMES || data->Atr.Token->type == DIVFLT || data->Atr.Token->type == DIVINT) {
          state = 2;
        } else if (data->Atr.Token->type == LESS || data->Atr.Token->type == GREATER || data->Atr.Token->type == LESSEQ || data->Atr.Token->type == GREATEREQ || data->Atr.Token->type == EQ || data->Atr.Token->type == NOTEQ) {
          state = 2;
          //potom neco dodelat na logicky
        } else {
          printf("syntaxerr\n");
          return -1;
        }
        estimate_precedence = 3;
        operacevtokenu = data->Atr.Token->type;
        break;
      case 2:
        if (data->Type == type_nonterm) {
          if (estimate_precedence == 1) { //tady řeším závorky
            state = 4;
          } else {
            state = 3;
            tokenDruhy = *(data->Atr.Token);
          }
        }
        break;
      case 3:
        if (state == 3) {
          if(data->Type == type_handler) {
            SData * newData = malloc(sizeof(SData));
            if (newData == NULL) {
              //yeet
            }
            newData->Type = type_nonterm;
            if (estimate_precedence == 13) { //??
              if(aData.type != at_tsitem) {
                aData.Atr.token = token;
              }
              //newData->Atr.Leaf = make_leaf(aData);
              //newData->DataType = dataType;
            } else if (estimate_precedence == 1) {
              printf("???????\n");
            } else {
              if (operacevtokenu == PLUS || operacevtokenu == MINUS || operacevtokenu == TIMES || operacevtokenu == DIVFLT || operacevtokenu == DIVINT) {
                printf("%d operace %d\n", tokenPrvni.attribute.INT, tokenDruhy.attribute.INT);
                if (operacevtokenu == PLUS) {
                } else {
                //  printf("násobím\n");
                }
              }
            }

            if(data != NULL) {
              tokenStackPop(s);
            }
            newData->DataType = type1;
            newData->Atr.Token->attribute.INT = (token->attribute.INT);
            tokenStackPush(s,newData);
            printf("pushuju nonterm s hodnotou %d\n", token->attribute.INT);
            rule = estimate_precedence;
            continue;
          }
        }
        break;
      case 4:
        if (data->Atr.Token->type == LEFTBRACKET) {
          state = 3;
        } else {
          return -1;
        }
        break;
      }
    if (data->Atr.Token->type != COLON) {
      tokenStackPop(s);
    } else {
      return rule;
    }
  }
  return rule;
}

precendentExpression* doPrecedenceOperation(token tokenAct) {
  precendentExpression* exp = malloc(sizeof(precendentExpression));
  if (!exp) {
    //malloc pp
  }
  exp->Tree = NULL;
  exp->ReturnToken = NULL;
  int readNextToken = 1;
  tokenStack *s = (tokenStack*) malloc(sizeof(tokenStack));
  if(s == NULL){
    //malloc pp
  }
  tokenStackInit(s);

  SData *dataPred = malloc(sizeof(SData)); //dávám dolar do stacku idk jestli mám
  if(dataPred == NULL){
    //yeet
  }
  dataPred->Type = type_token;
  token dolar;
  dolar.type = COLON;
  dataPred->Atr.Token = &dolar;
  tokenStackPush(s,dataPred);

  token* current = &tokenAct;
  token tmptkn;

  while(readNextToken || getTerminalData(s) != NULL){
    SData* termData = getTerminalData(s);
    token* token;
    token = termData->Atr.Token;

    if (current != NULL) {
      if (current->type == EOL || current->type == EOFTOKEN || current->type == COLON) {
        readNextToken = 0;
      }
    }

    if (getPrecedenceIndex(current) == 6 && getPrecedenceIndex(token) == 6) { //stack je $$
      printf("ukonci uspesne\n");
      break;
    }

    int operation = getPrecedenceOperatorValue(token, current); //token = ze stacku, current = ze scanneru

    if (operation == C) { //0  if <y je na vrcholu zásobníku and r: A→y∈P then zaměň <y za A & vypiš r na výstup else chyba
      int a = findRule(s); //THE REST OF THE FUCKING OWL
      if (a == -1) {
        printf("syntax error\n");
        return NULL;
      }
      SData *data = malloc(sizeof(SData));
      if(data == NULL){
          //yeet
      }
      data->Type = type_token;
      data->Atr.Token = current;
      continue; //nenačítat další token ze vstupu
    } else if (operation == B) { //1 () //push(b) & přečti další symbol b ze vstupu
      SData *data = malloc(sizeof(SData));
      if(data == NULL){
        //yeet
      }
      data->Type = type_token;
      data->Atr.Token = current;
      tokenStackPush(s,data);
    } else if (operation == A) { //2 +- atd //zaměň a za a< na zásobníku & push(b) & přečti další symbol b ze vstupu
      //printf("dávám zarážku na vrchol stacku\n");
      SData *data = malloc(sizeof(SData));
      if(data == NULL){
        //yeet
      }
      data->Type = type_token;
      data->Atr.Token = current;
      addHandler(s,termData); //dá zarážku na vrchol zásobníku
      tokenStackPush(s,data);
    } else if (operation == E) { //error
      printf("error, neplatná operace mezi dvěmi věcmi idk\n");
      return NULL;
    }

    if (readNextToken > 0) {
      current = malloc(sizeof(token));
      tmptkn = nextToken(&error, stack, doIndent);
      current->attribute = tmptkn.attribute;
      current->type = tmptkn.type;
    }
  }
}
/*
void main() {
  setFile("txt.txt");
  token token1, token2;
  token1 = nextToken(&error, stack, doIndent);

  doPrecedenceOperation(token1);
}
*/
