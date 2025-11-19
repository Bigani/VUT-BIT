/**
  * scanner.c
  * Project IFJ19
  *
  * Březina Jindřich (xbrezi21@stud.fit.vutbr.cz)
  * Gumančík Pavol (xguman01@stud.fit.vutbr.cz)
  * Kotáb Dominik (xkotab01@stud.fit.vutbr.cz)
  * Moravčík Tomáš (xmorav41@stud.fit.vutbr.cz)
  *
  * Brief: scanner and Lexical analyzer
  *
  */

#include "scanner.h"

#define STATE_START 100

#define STATE_F2 300      //Robíme číslo
#define STATE_F3 301      //Číslo bude float
#define STATE_F4 302      //Koniec čísla
#define STATE_P1 303      //Prechod pri '.' znaku floatu
#define STATE_P2 304      //Bolo použité e/E
#define STATE_P3 305      //Precho pri '+' a '-' znaku pri exp

#define STATE_F22 400     //Koncový stav literálu
#define STATE_P10 401     //Zápis charov
#define STATE_P11 402     //Možná escape sekvencia alebo hex hodnota
#define STATE_P12 403     //Potvrdenie hex hodnoty
#define STATE_P13 404     //Hex hodnota

#define TRUE 1            //
#define FALSE 0           //



int stackReset = FALSE;
//static int TotalTokenCount = 0;///////////////treba to este?


void resetToken(){
    rewind(stdin);
    stackReset = TRUE;
    //TotalTokenCount = 0;
}
/*
token ungetToken(int *error, tStack *stack, int doIndent){

    rewind(stdin);
    token Token;
    int tmp = TotalTokenCount - 1;
    TotalTokenCount = 0;

    for (int i = 0; i < tmp; i++){
        Token = nextToken(error, stack, doIndent);
    }

    return Token;
}*/


token nextToken(int *error, tStack *stack, int doIndent) {

    token Token;
    Token.type = 0;

    int tmpNum = 0;
    int counter = 0;
    char hexvalue[3] = {0,};
    static int CurrentIndentCount;

    if (stackReset == TRUE){
        stackInit(stack);
        stackPush(stack,0);
        stackReset = FALSE;
    }

    char c;



    //=========================================================================//
    //============================INDENTS/DEDENTS==============================//
    //=========================================================================//

    if (doIndent == TRUE){

        c = getchar();
        //Počíta počet medzier
        while (c == ' ' || c == '\n' || c == EOF || c == '#') {
            counter++;
            if (c == '\n') { //If the line is empty start again
                counter = 0;
            }
            if (c == EOF) {
                Token.type = EOFTOKEN;
                return Token;
            }
            if (c == '#'){
                c = getchar();
                while ((c != '\n') && (c != EOF)){
                    c = getchar();
                }
                ungetc(c, stdin);
            }
            c = getchar();
        }
        ungetc(c, stdin); //Vrati znak buduceho tokenu

        stackTop(stack, &tmpNum);
        if (counter > tmpNum) { //Creates new Indent if counter is higher
            stackPush(stack, counter);
            Token.attribute.INT = counter;
            Token.type = INDENT;
            return Token;
        } else if (counter == tmpNum){  //Indents are same, returns broken token because has to return smth
            Token.type = BROKEN;
            return Token;
        } else {                //Robim dedenty
            CurrentIndentCount = counter;
            stackPop(stack);
            stackTop(stack, &tmpNum);
            if (tmpNum < CurrentIndentCount){
                *error = LEXICAL_ERR;
                return Token;
            }
            Token.attribute.INT = counter;
            Token.type = DEDENT;
        }
        for (int i = 0; i < counter; i++) {
            ungetc(' ',stdin);
        }
        return Token;

    }

    //=========================================================================//
    //========================END OF INDENTS/DEDENTS===========================//
    //=========================================================================//


    //TotalTokenCount++;

    smartString *s = malloc(sizeof(smartString));
    if (s == NULL) {
        *error = INTERN_ERR;
        return Token;
    }
    stringInit(s);

    int state = STATE_START;
    while (c = getchar(), c != EOF) {
/////////////check prepisovanie c s podmienkou while


        switch (c) {
            // SINGULAR //

            case ' ':
                continue;

            case '+':
                stringAddChar(s, '+');
                Token.attribute.string = s;
                Token.type = PLUS;
                return Token;

            case '-':
                stringAddChar(s, '-');
                Token.attribute.string = s;
                Token.type = MINUS;
                return Token;

            case '*':
                stringAddChar(s, '*');
                Token.attribute.string = s;
                Token.type = TIMES;
                return Token;

            case '(':
                stringAddChar(s, '(');
                Token.attribute.string = s;
                Token.type = LEFTBRACKET;
                return Token;

            case ')':
                stringAddChar(s, ')');
                Token.attribute.string = s;
                Token.type = RIGHTBRACKET;
                return Token;

            case ':':
                stringAddChar(s, ':');
                Token.attribute.string = s;
                Token.type = COLON;
                return Token;

            case ',':
                stringAddChar(s, ',');
                Token.attribute.string = s;
                Token.type = COMMA;
                return Token;
                // MULTIPLE //
            case '/':
                if (c = getchar(), c == '/') {
                    stringAddChar(s, '/');
                    stringAddChar(s, '/');
                    Token.attribute.string = s;
                    Token.type = DIVINT;
                    return Token;
                } else {
                    stringAddChar(s, '/');
                    Token.attribute.string = s;
                    Token.type = DIVFLT;
                    ungetc(c, stdin);
                    return Token;
                }

            case '=':
                if (c = getchar(), c == '=') {
                    stringAddChar(s, '=');
                    stringAddChar(s, '=');
                    Token.attribute.string = s;
                    Token.type = EQ;
                    return Token;
                } else {
                    stringAddChar(s, '=');
                    Token.attribute.string = s;
                    Token.type = ASSIGN;
                    ungetc(c, stdin);
                    return Token;
                }

            case '>':
                if (c = getchar(), c == '=') {
                    stringAddChar(s, '>');
                    stringAddChar(s, '=');
                    Token.attribute.string = s;
                    Token.type = GREATEREQ;
                    return Token;
                } else {
                    stringAddChar(s, '>');
                    Token.attribute.string = s;
                    Token.type = GREATER;
                    ungetc(c, stdin);
                    return Token;
                }

            case '<':
                if (c = getchar(), c == '=') {
                    stringAddChar(s, '<');
                    stringAddChar(s, '=');
                    Token.attribute.string = s;
                    Token.type = LESSEQ;
                    return Token;
                } else {
                    stringAddChar(s, '<');
                    Token.attribute.string = s;

                    Token.type = LESS;
                    ungetc(c, stdin);
                    return Token;
                }

            case '!':
                if (c = getchar(), c == '=') {
                    stringAddChar(s, '!');
                    stringAddChar(s, '=');
                    Token.attribute.string = s;
                    Token.type = NOTEQ;
                    return Token;
                } else {
                    *error = LEXICAL_ERR;
                    return Token;
                }

            case '"':
                if (c = getchar(), c != '"') {
                    *error = LEXICAL_ERR;
                    return Token;
                }

                if (c = getchar(), c != '"') {
                    *error = LEXICAL_ERR;
                    return Token;
                }
                Token.type = DOCCOM;

                while ( (c = getchar()) ) {
                    if (c == '"') {
                        if (c = getchar(), c == '"') {
                            if (c = getchar(), c == '"') {
                                break;
                            } else {
                                stringAddChar(s, '"');
                                stringAddChar(s, '"');
                            }
                        } else {
                            stringAddChar(s, '"');
                        }
                    }
                    if (c == 32) { //space
                      stringAddChar(s, '\\');stringAddChar(s, '0');stringAddChar(s, '3');stringAddChar(s, '2');
                      continue;
                    }
                    if (c == 13) { //pepe windows
                      continue;
                    }
                    if (c == EOF) {
                      *error = 1;
                      return Token;
                    }
                    else if (c == '\n') { // Spraví EOL
                      stringAddChar(s, '\\');stringAddChar(s, '0');stringAddChar(s, '1');stringAddChar(s, '0');
                      continue;
                  }
                    stringAddChar(s, c);
                }
                Token.attribute.string = s;
                return Token;

            case '#':///koment
                c = getchar();
                free(s);
                while ((c != '\n') && (c != EOF)){
                    c = getchar();
                }
                if (c == '\n'){///yeetne eol token, ten sa basically ocakava
                    c = getchar();
                    Token.type = EOL;
                    ungetc(c, stdin);
                    return Token;
                }
                else if (c == EOF){
                    Token.type = EOFTOKEN;
                    return Token;
                }
                break;

            case '\n':
                free(s);
                Token.type = EOL;
                return Token;

            case '\r': //lebo pepega robi projekty na windowse
                continue;

            default:
                if (isalpha(c) || c == '_') { //Robíme basic string
                    Token.type = STR;

                    while (1) {
                        if ( (isalnum(c) == 0) && (c != '_') ) {
                            break;
                        }
                        stringAddChar(s, c);
                        c = getchar();
                    }

                    ungetc(c, stdin);
                    int i = 0;
                    if ((i = stringIsKeyword(s)) != -1) {
                        Token.attribute.keyword = i;
                        Token.type = KEYWORD;
                    } else
                        Token.attribute.string = s;

                    return Token;
                }

                else if (isdigit(c)) { //Robíme číslo
                    stringAddChar(s, c);
                    int tmpNum = c - '0'; //lebo ascii hodnota
                    c = getchar();
                    if (tmpNum == 0 && (c != '.' && c != 'e' && c != 'E' && isdigit(c))) { //Spravnost zaciatku cisla Nulou
                        *error = LEXICAL_ERR;
                        return Token;
                    }



                    state = STATE_F2;
                    int isINTorFLT = 0; //0 is for INT, 1 is for FLOAT
                    char *ptr; //pri prevode
                    while (1) {
                        switch (state) {
                            case (STATE_F2): //Začiatočný state čísla
                                if (c == '.') { //Bude float
                                    stringAddChar(s, c);
                                    c = getchar();
                                    state = STATE_P1;
                                } else if (c == 'E' || c == 'e') { //Bude Exp
                                    stringAddChar(s, c);
                                    c = getchar();
                                    state = STATE_P2;
                                } else if (isdigit(c)) { //Bude cislo
                                    stringAddChar(s, c);
                                    c = getchar();
                                    state = STATE_F2;
                                } else {//if (isspace(c) || c == '\n' || c == EOF || c == ':') {
                                    Token.attribute.INT = strtod(s->string, &ptr);
                                    Token.type = INT;
                                    state = STATE_START;
                                    ungetc(c, stdin);
                                    free(s);
                                    return Token;
                                }
                                break;

                            case (STATE_P1): //Je float
                                if (isdigit(c)) { //Musí nasledovať číslo
                                    stringAddChar(s, c);
                                    c = getchar();
                                    state = STATE_F3;
                                } else {
                                    *error = LEXICAL_ERR;
                                    free(s);
                                    return Token;
                                }
                                break;

                            case (STATE_F3): //Je float v tvare (example) 132.1_
                                isINTorFLT = 1; ///Kontrola že je TYP FLOATEXP
                                if (c == 'E' || c == 'e') { //Bude 132.1e
                                    stringAddChar(s, c);
                                    c = getchar();
                                    state = STATE_P2;
                                } else if (isdigit(c)) { //Bude 132.12
                                    stringAddChar(s, c);
                                    c = getchar();
                                    state = STATE_F3; //Vráti sa tu
                                } else { //if (isspace(c) || c == '\n' || c == EOF || c == ':') {
                                    Token.attribute.FLOAT = strtod(s->string, &ptr);
                                    Token.type = FLOAT;
                                    ungetc(c, stdin);
                                    state = STATE_START;
                                    free(s);
                                    return Token; //Koniec float čísla
                                }
                                break;

                            case (STATE_P2):
                                if (isalnum(c)) {
                                    stringAddChar(s, c);
                                    c = getchar();
                                    state = STATE_F4;
                                } else if (c == '+' || c == '-') {
                                    stringAddChar(s, c);
                                    c = getchar();
                                    state = STATE_P3;
                                } else {
                                    *error = LEXICAL_ERR;
                                    free(s);
                                    return Token;
                                }
                                break;

                            case (STATE_P3):
                                if (isalnum(c)) {
                                    stringAddChar(s, c);
                                    c = getchar();
                                    state = STATE_F4;
                                } else {
                                    *error = LEXICAL_ERR;
                                    free(s);
                                    return Token;
                                }
                                break;

                            case (STATE_F4):
                                if (isalnum(c)) {
                                    stringAddChar(s, c);
                                    c = getchar();
                                    state = STATE_F4;
                                } else {
                                    if (isINTorFLT == 1) {
                                        Token.attribute.FLOAT = strtod(s->string, &ptr);
                                        Token.type = FLOAT;
                                        ungetc(c, stdin);
                                        isINTorFLT = 0;
                                        free(s);
                                        return Token; //Koniec float
                                    } else {
                                        Token.attribute.INT = strtod(s->string, &ptr);
                                        Token.type = INT;
                                        ungetc(c, stdin);
                                        free(s);
                                        return Token; //Koniec integer
                                    }
                                    state = STATE_START;
                                }
                                break;
                        }
                    }
                }

                else if (c == '\'') { // Robíme literál
                    c = getchar();
                    state = STATE_P10;

                    if (c == '\''){
                        stringAddChar(s, '\0');
                        Token.attribute.string = s;
                        Token.type = LITERAL;
                        state = STATE_START;
                        return Token;
                    }

                    while (1) {
                        if (c == EOF) {
                            *error = LEXICAL_ERR;
                            free(s);
                            return Token;
                        }
                        switch (state) {
                            case (STATE_P10):
                                if (c == 32) { //space
                                    stringAddChar(s, '\\');stringAddChar(s, '0');stringAddChar(s, '3');stringAddChar(s, '2');
                                    c = getchar();
                                    state = STATE_P10;
                                    continue;
                                }
                                else if (c == 35) { //#
                                    stringAddChar(s, '\\');stringAddChar(s, '0');stringAddChar(s, '3');stringAddChar(s, '5');
                                    c = getchar();
                                    state = STATE_P10;
                                    continue;
                                }
                                else if (c > 32 && c != 92 && c != 35 && c != 39) { // '\'' && '\\' && '\,' //normalny znak
                                    stringAddChar(s, c);
                                    c = getchar();
                                    state = STATE_P10;
                                    continue;
                                } else if (c == 39) { //Apostrof teda koniec stringu
                                    //stringAddChar(s, c);
                                    //c = getchar();
                                    state = STATE_F22;
                                    continue;
                                } else if (c == 92) { // Backslash teda escape seq
                                    c = getchar();
                                    state = STATE_P11;
                                    continue;
                                } else {
                                    *error = LEXICAL_ERR;
                                    free(s);
                                    return Token;
                                }
                                continue;

                            case (STATE_P11): //Riešime či escape alebo nah
                                if (c > 32 && c != 35 && c != 92 && c != 39 && c != 44 && c != 'n' && c != 't' && c != 'x') {//nebola
                                    stringAddChar(s, c);
                                    c = getchar();
                                    state = STATE_P10;
                                }
                                else if (c == 35) { //#
                                    stringAddChar(s, '\\');stringAddChar(s, '0');stringAddChar(s, '3');stringAddChar(s, '5');
                                    c = getchar();
                                    state = STATE_P10;
                                }
                                else if (c == 32) { //space
                                    stringAddChar(s, '\\');stringAddChar(s, '0');stringAddChar(s, '3');stringAddChar(s, '2');
                                    c = getchar();
                                    state = STATE_P10;
                                }
                                else if (c == 92) {//  _/_
                                    stringAddChar(s, '\\');stringAddChar(s, '0');stringAddChar(s, '9');stringAddChar(s, '2');
                                    c = getchar();
                                    state = STATE_P10;
                                }
                                else if (c == 39 ) {// _'_
                                    stringAddChar(s, '\\');stringAddChar(s, '0');stringAddChar(s, '3');stringAddChar(s, '9');
                                    c = getchar();
                                    state = STATE_P10;
                                  }
                                else if (c == 34) {// _"_
                                    stringAddChar(s, '\\');stringAddChar(s, '0');stringAddChar(s, '3');stringAddChar(s, '4');
                                    c = getchar();
                                    state = STATE_P10;
                                } else if (c == 'n') { // Spraví EOL
                                    stringAddChar(s, '\\');stringAddChar(s, '0');stringAddChar(s, '1');stringAddChar(s, '0');
                                    c = getchar();
                                    state = STATE_P10;
                                } else if (c == 't') { // Spraví TAB
                                    stringAddChar(s, '\\');stringAddChar(s, '0');stringAddChar(s, '1');stringAddChar(s, '1');
                                    c = getchar();
                                    state = STATE_P10;
                                } else if (c == 'x') { //Bude robiť HEX
                                    c = getchar();
                                    state = STATE_P12;
                                } else {
                                    *error = LEXICAL_ERR;
                                    free(s);
                                    return Token;
                                }
                                continue;

                            case (STATE_P12): //Riešime prvú hex val
                                if ((c >= 48 && c <= 57) || (c >= 65 && c <= 70) ||
                                    (c >= 97 && c <= 102)) { // 0..9 || A..F || a..f
                                    hexvalue[0] = c;
                                    c = getchar();
                                    state = STATE_P13;
                                } else {
                                    *error = LEXICAL_ERR;
                                    free(s);
                                    return Token;
                                }
                                continue;

                            case (STATE_P13): //Riešime druhú hex val
                                if ((c >= 48 && c <= 57) || (c >= 65 && c <= 70) ||
                                    (c >= 97 && c <= 102)) { // 0..9 || A..F || a..f
                                    hexvalue[1] = c;
                                    int decvalue = (int) strtol(hexvalue, NULL, 16); //convertne hexvalue na int v decimáloch
                                    char tmp[2];
                                    sprintf(tmp, "%c", decvalue); //convertnem decvalue na ASCII znak
                                    c = *tmp;
                                    stringAddChar(s, c);          //realne proud ako to mam vymyslene
                                    c = getchar();
                                    state = STATE_P10;
                                } else {
                                    *error = LEXICAL_ERR;
                                    free(s);
                                    return Token;
                                }
                                continue;

                            case (STATE_F22): //Koniec
                                Token.attribute.string = s;
                                Token.type = LITERAL;
                                state = STATE_START;
                                return Token;
                                break;
                        }
                        break;
                    }
                }

                else {
                  *error = LEXICAL_ERR;
                  Token.type = BROKEN;
                  return Token;
                }
        }
    }
    Token.type = EOFTOKEN;
    free(s);
    return Token;
}
