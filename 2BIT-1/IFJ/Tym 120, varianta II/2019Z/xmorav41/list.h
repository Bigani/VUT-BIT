/**
  * list.h
  * Project IFJ19
  *
  * Březina Jindřich (xbrezi21@stud.fit.vutbr.cz)
  * Gumančík Pavol (xguman01@stud.fit.vutbr.cz)
  * Kotáb Dominik (xkotab01@stud.fit.vutbr.cz)
  * Moravčík Tomáš (xmorav41@stud.fit.vutbr.cz)
  *
  * Brief: ial c206.h
  *
  */

#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"

#ifndef _LIST_H_
#define _LIST_H_


// aka kdyz zapomenete ze existuje enum ((:
//typy instrukci
#define I_MOVE 0
#define I_CREATEFRAME 1
#define I_PUSHFRAME 2
#define I_POPFRAME 3
#define I_DEFVAR 4
#define I_CALL 5
#define I_RETURN 6
#define I_PUSHS 7
#define I_POPS 8
#define I_CLEARS 9
#define I_ADD 10
#define I_SUB 11
#define I_MUL 12
#define I_DIV 13
#define I_IDIV 14
#define I_ADDS 15
#define I_SUBS 16
#define I_MULS 17
#define I_DIVS 18
#define I_IDIVS 19
#define I_LT 20
#define I_GT 21
#define I_EQ 22
#define I_LTS 23
#define I_GTS 24
#define I_EQS 25
#define I_AND 26
#define I_OR 27
#define I_NOT 28
#define I_ANDS 29
#define I_ORS 30
#define I_NOTS 31
#define I_INT2FLOAT 32
#define I_FLOAT2INT 33
#define I_INT2CHAR 34
#define I_STRI2INT 35
#define I_INT2FLOATS 36
#define I_FLOAT2INTS 37
#define I_INT2CHARS 38
#define I_STRI2INTS 39
#define I_READ 40
#define I_WRITE 41
#define I_CONCAT 42
#define I_STRLEN 43
#define I_GETCHAR 44
#define I_SETCHAR 45
#define I_TYPE 46
#define I_LABEL 47
#define I_JUMP 48
#define I_JUMPIFEQ 49
#define I_JUMPIFNEQ 50
#define I_JUMPIFEQS 51
#define I_JUMPIFNEQS 52
#define I_EXIT 53
#define I_BREAK 54
#define I_DPRINT 55

#define I_INPUTS 56
#define I_INPUTI 57
#define I_INPUTF 58
#define I_LEN 59
#define I_SUBSTR 60
#define I_ORD 61
#define I_CHR 62
#define I_PRINT 63

#define I_IF 64
#define I_WHILE_B 65
#define I_WHILE_E 66
#define I_IF_E 67
#define I_ELSE_E 68
#define I_NQS 69
#define I_EOE 70
#define I_STARTOFFUNC 71
#define I_ENDOFFUNC 72
#define I_DEFVARLF 73
#define I_PUSHSLF 74
#define I_DEFVARLOCAL 75
#define I_POPSLOCAL 76

typedef struct
{
  int instType;  // typ instrukce
  void *addr1; // adresa 1
  void *addr2; // adresa 2
  void *addr3; // adresa 3
} tInstr;

typedef struct tDLElem {                 /* prvek dvousměrně vázaného seznamu */
        tInstr Instruction;            /* ukazatel na token */
        struct tDLElem *lptr;          /* ukazatel na předchozí prvek seznamu */
        struct tDLElem *rptr;        /* ukazatel na následující prvek seznamu */
} *tDLElemPtr;

typedef struct {                                  /* dvousměrně vázaný seznam */
    tDLElemPtr First;                      /* ukazatel na první prvek seznamu */
    tDLElemPtr Act;                     /* ukazatel na aktuální prvek seznamu */
    tDLElemPtr Last;                    /* ukazatel na posledni prvek seznamu */
} tDLList;

void DLInitList (tDLList *);
void DLDisposeList (tDLList *);
void DLInsertFirst (tDLList *, tInstr);
tInstr *DLInsertLast(tDLList *, tInstr);
void DLDeleteFirst (tDLList *);
void DLDeleteLast (tDLList *);
void DLPostInsert (tDLList *, tInstr);
void DLSucc (tDLList *);

#endif
