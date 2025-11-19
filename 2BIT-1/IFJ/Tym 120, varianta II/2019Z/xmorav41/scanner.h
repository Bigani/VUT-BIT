/**
  * scanner.h
  * Project IFJ19
  *
  * Březina Jindřich (xbrezi21@stud.fit.vutbr.cz)
  * Gumančík Pavol (xguman01@stud.fit.vutbr.cz)
  * Kotáb Dominik (xkotab01@stud.fit.vutbr.cz)
  * Moravčík Tomáš (xmorav41@stud.fit.vutbr.cz)
  *
  * Brief: header for scanner and Lexical analyzer
  *
  */

#ifndef _SCANNER_H_
#define _SCANNER_H_

#include "strings.h"
#include "stack.h"
#include "error.h"
#include <ctype.h>
#include <stdbool.h>
#include <malloc.h>


typedef enum {
    DEF,
    ELSE,
    IF,
    NONE,
    PASS,
    RETURN,
    WHILE,
    INPUTS,
    INPUTI,
    INPUTF,
    PRINT,
    LEN,
    SUBSTR,
    ORD,
    CHR
} keyword;

typedef enum {
    PLUS, //+
    MINUS, //-
    TIMES, //*
    INT, //integer
    FLOAT, //float
    LITERAL, //LITERALLY SHAKING RIGHT NOW == String   print('foidaiofd')  'foidaiofd' je LITERAL(string)
    STR, //string == ID print('foidaiofd')     print je STR(ID)
    DIVFLT, //dělení floatov
    DIVINT, //dělení integerov
    LESS, //<
    GREATER, //>
    INDENT,
    DEDENT,
    DOCCOM, //dokumentační komentář
    KEYWORD,
    LESSEQ, //<=
    GREATEREQ, //>=
    BROKEN,
    EQ, //==
    NOTEQ, //!=
    EOL, //konec radku
    EOFTOKEN, //konec souboru
    LEFTBRACKET, // (
    RIGHTBRACKET, // )
    COMMA, // ,
    COLON, // :
    BOOL, //asi meme
    ASSIGN, // =
    UNDEFINED // pro parametry funkcí
} tokenType;

typedef union {
    keyword keyword;
    smartString *string; //string value
    int INT;
    double FLOAT;
    bool BOOL;
} tokenAttribute;

typedef struct {
    tokenType type; //type
    tokenAttribute attribute; //value
} token;

////*    Here may be other functions such as opening source file etc    *////


// Proly na ziskavanie tokenov
token ungetToken(int *error, tStack *stack, int doIndent);
token nextToken(int *error, tStack *stack, int doIndent);

void resetToken();

void setFile(char* sourceFile);
//  Exemplar token insertion  //
/*
  * token Token;
  * smartString *s = malloc(sizeof(smartString));
  * stringInit(s);
  * stringAddChar(s,'+');
  * Token.attribute.string = s;
  * Token.type = PLUS;
*/



#endif
