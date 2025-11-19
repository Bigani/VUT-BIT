/**
  * generator.c
  * Project IFJ19
  *
  * Březina Jindřich (xbrezi21@stud.fit.vutbr.cz)
  * Gumančík Pavol (xguman01@stud.fit.vutbr.cz)
  * Kotáb Dominik (xkotab01@stud.fit.vutbr.cz)
  * Moravčík Tomáš (xmorav41@stud.fit.vutbr.cz)
  *
  * Brief: Generator of code
  *
  */


///////Personal notes
//pushframe spravi z TF LF
//popframe prehlasi LF za TF
//za funckiou (return) MOVE GF@VYSLEDOK TF@RETVAL
//nemozno predefinovavat stejnu premennu
//whitespaces v stringoch su nahradene escape seqs, ' ' = \032 '\n' = \010
///////


#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "symtable.h"
#include "scanner.h"
#include "expressions.h"
#include "list.h"
#include "generator.h"

int actNumberOfLF = 0;
symtableItem *aktualniArgumenty = NULL;
int counterZanoreni = 0;
/*int pointerCounter = 0;
void *theUltimatePointer;*/

void degenerate(tDLList *list){
    fprintf(stdout, ".IFJcode19\n");
    fprintf(stdout, "JUMP $$MAIN\n");
    generateBuiltIn();
    fprintf(stdout, "LABEL $ZEROERROR\n");
    fprintf(stdout, "EXIT int@%d\n", DIVISION_BY_ZERO);
    fprintf(stdout, "LABEL $SEM_ERROR\n");
    fprintf(stdout, "EXIT int@4\n");////wrong string
    fprintf(stdout, "LABEL $SEMERR\n");
    fprintf(stdout, "EXIT int@3\n");////sem err
    fprintf(stdout, "LABEL $$MAIN\n");
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "PUSHFRAME\n");
    doDefVars(list);
    generateInstructionREE(list);
}

//||======================================================||//
//||=============== Věstavené funkce start ===============||//

void generateInputs(){
    fprintf(stdout, "LABEL $ALFA\n");
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@$RETVAL\n");
    fprintf(stdout, "READ LF@$RETVAL string\n");
    fprintf(stdout, "PUSHS LF@$RETVAL\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
}

void generateInputi(){
    fprintf(stdout, "LABEL $BETA\n");
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@$RETVAL\n");
    fprintf(stdout, "READ LF@$RETVAL int\n");
    fprintf(stdout, "DEFVAR LF@$TMPVAL\n");
    fprintf(stdout, "TYPE LF@$TMPVAL LF@$RETVAL\n");
    fprintf(stdout, "JUMPIFEQ $INTOK LF@$TMPVAL string@int\n");
    fprintf(stdout, "MOVE LF@$RETVAL string@None\n");
    fprintf(stdout, "LABEL $INTOK\n");
    fprintf(stdout, "PUSHS LF@$RETVAL\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
}

void generateInputf(){
    fprintf(stdout, "LABEL $GAMA\n");
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@$RETVAL\n");
    fprintf(stdout, "READ LF@$RETVAL float\n");

    fprintf(stdout, "DEFVAR LF@$TMPVAL\n");
    fprintf(stdout, "TYPE LF@$TMPVAL LF@$RETVAL\n");
    fprintf(stdout, "JUMPIFEQ $FLOATOK LF@$TMPVAL string@float\n");
    fprintf(stdout, "MOVE LF@$RETVAL string@None\n");
    fprintf(stdout, "LABEL $FLOATOK\n");
    fprintf(stdout, "PUSHS LF@$RETVAL\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
}

void generatePrint(int *parCounter){
    //fprintf(stdout, "LABEL $DELTA%p\n", parCounter);
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@$RETVAL%p\n", parCounter);
    //fprintf(stdout, "DEFVAR LF@$TMPVAL%p\n", parCounter);
    for (int i = 0; i < *parCounter; i++)
    {
      fprintf(stdout, "DEFVAR LF@$PEPEGA%d\n", i);
      fprintf(stdout, "POPS LF@$PEPEGA%d\n", i);
    }
    for(int j = *parCounter-1; j >= 0; j--){
      fprintf(stdout, "WRITE LF@$PEPEGA%d\n", j);
      if(j != 0) fprintf(stdout, "WRITE string@\\032\n");
    }
    fprintf(stdout, "WRITE string@\\010\n");
    fprintf(stdout, "MOVE LF@$RETVAL%p  string@None\n", parCounter);
    fprintf(stdout, "PUSHS LF@$RETVAL%p\n", parCounter);
    fprintf(stdout, "POPFRAME\n");
}

void generateLen(){
    fprintf(stdout, "LABEL $LENGTH\n");
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@$RETVAL\n");
    fprintf(stdout, "DEFVAR LF@$1\n");
    fprintf(stdout, "POPS LF@$1\n");
    fprintf(stdout, "STRLEN LF@$RETVAL LF@$1\n");
    fprintf(stdout, "PUSHS LF@$RETVAL\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
}
//substr(s,i,n)– Vrátí podřetězec s zadaného řetězce. Druhým parametrem i je dán začátek požadovaného podřetězce (počítáno od nuly)
//a třetí parametr n určuje délku podřetězce. Je-li index i mimo meze 0 až len(s)nebo n < 0, vrací funkce None.
//Je-li n > len(s)−𝑖, jsou jako řetězec vráceny od i-tého znaku všechny zbývající znaky řetězce.
void generateSubstr(){
    //INIT
    fprintf(stdout, "LABEL $SUBSTR\n");
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@$RETVAL\n");
    fprintf(stdout, "DEFVAR LF@$DELKA\n");//n
    fprintf(stdout, "POPS LF@$DELKA\n");
    fprintf(stdout, "DEFVAR LF@$POSIT\n");//i
    fprintf(stdout, "POPS LF@$POSIT\n");
    fprintf(stdout, "DEFVAR LF@$STRING\n");//s
    fprintf(stdout, "POPS LF@$STRING\n");
    //ULOŽENIE PARAMETROV
    fprintf(stdout, "DEFVAR LF@$TMPLEN\n");
    fprintf(stdout, "STRLEN LF@$TMPLEN LF@$STRING\n");//delka s
    //KONTROLA POZICIE
    fprintf(stdout, "DEFVAR LF@$BOOLCHECK\n");
    fprintf(stdout, "ADD LF@$TMPLEN LF@$TMPLEN int@-1\n");
    fprintf(stdout, "GT LF@$BOOLCHECK LF@$POSIT LF@$TMPLEN\n");
    fprintf(stdout, "JUMPIFEQ $ERRORSUBSTR LF@$BOOLCHECK bool@true\n");
    fprintf(stdout, "LT LF@$BOOLCHECK LF@$POSIT int@0\n");
    fprintf(stdout, "JUMPIFEQ $ERRORSUBSTR LF@$BOOLCHECK bool@true\n");
    fprintf(stdout, "GT LF@$BOOLCHECK int@0 LF@$DELKA\n");
    fprintf(stdout, "JUMPIFEQ $ERRORSUBSTR LF@$BOOLCHECK bool@true\n");
    fprintf(stdout, "EQ LF@$BOOLCHECK int@0 LF@$DELKA\n");
    fprintf(stdout, "JUMPIFEQ $ERRORSUBSTR LF@$BOOLCHECK bool@true\n");
    //FUNKCIA
    //fprintf(stdout, "ADD LF@$DELKA LF@$DELKA int@1\n");
    fprintf(stdout, "ADD LF@$TMPLEN LF@$TMPLEN int@1\n");
    //
    fprintf(stdout, "GETCHAR LF@$RETVAL LF@$STRING LF@$POSIT\n");//prvy znak return stringu
    fprintf(stdout, "ADD LF@$POSIT LF@$POSIT int@1\n");//inkrementácia pozicie z ktorej sa bude brat
    fprintf(stdout, "DEFVAR LF@$TMPSTR\n");//char ktory sa bude pridavat
    fprintf(stdout, "ADD LF@$DELKA LF@$DELKA LF@$POSIT\n");
    fprintf(stdout, "SUB LF@$DELKA LF@$DELKA int@1\n");//podmienka na while loop hmmmmmm?
    //SMYČKA
    fprintf(stdout, "LABEL $WHILE_LOOP_B\n");
    fprintf(stdout, "JUMPIFEQ $WHILE_LOOP_E LF@$POSIT LF@$DELKA\n");//skoč ak si dokončil
    fprintf(stdout, "JUMPIFEQ $WHILE_LOOP_E LF@$POSIT LF@$TMPLEN\n");//skoč ak je koniec stringu
    fprintf(stdout, "GETCHAR LF@$TMPSTR LF@$STRING LF@$POSIT\n");//getchar
    fprintf(stdout, "CONCAT LF@$RETVAL LF@$RETVAL LF@$TMPSTR\n");//concat stringy
    fprintf(stdout, "ADD LF@$POSIT LF@$POSIT int@1\n");//inkrementácia
    fprintf(stdout, "JUMP $WHILE_LOOP_B\n");
    //KONIEC SMYČKY
    fprintf(stdout, "LABEL $WHILE_LOOP_E\n");
    fprintf(stdout, "PUSHS LF@$RETVAL\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
    //ERROR
    fprintf(stdout, "LABEL $ERRORSUBSTR\n");
    fprintf(stdout, "MOVE LF@$RETVAL string@None\n");
    fprintf(stdout, "PUSHS LF@$RETVAL\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
}
//ord(s,i)– Vrátí ordinální hodnotu (ASCII) znaku s na pozici i v řetězci. Je-lipozice mimo meze řetězce (0 ažlen(s)- 1), vrací None.
void generateOrd(){
    //INIT
    fprintf(stdout, "LABEL $ORD\n");
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@$RETVAL\n");
    //ULOŽENIE PARAMETROV
    fprintf(stdout, "DEFVAR LF@$POSIT\n");
    fprintf(stdout, "POPS LF@$POSIT\n");
    fprintf(stdout, "DEFVAR LF@$STRING\n");
    fprintf(stdout, "POPS LF@$STRING\n");

    fprintf(stdout, "DEFVAR LF@$CHR2INT\n");
    fprintf(stdout, "DEFVAR LF@$TMPLEN\n");
    fprintf(stdout, "STRLEN LF@$TMPLEN LF@$STRING\n");
    //KONTROLA POZICIE
    fprintf(stdout, "ADD LF@$TMPLEN LF@$TMPLEN int@-1\n");
    //
    fprintf(stdout, "DEFVAR LF@$BOOLCHECK\n");
    fprintf(stdout, "GT LF@$BOOLCHECK LF@$POSIT LF@$TMPLEN\n");
    fprintf(stdout, "JUMPIFEQ $ERRORORD LF@$BOOLCHECK bool@true\n");
    fprintf(stdout, "LT LF@$BOOLCHECK LF@$POSIT int@0\n");
    fprintf(stdout, "JUMPIFEQ $ERRORORD LF@$BOOLCHECK bool@true\n");
    //
    fprintf(stdout, "ADD LF@$TMPLEN LF@$TMPLEN int@1\n");
    //VÝPOČET A NÁVRAT
    fprintf(stdout, "STRI2INT LF@$RETVAL LF@$STRING LF@$POSIT\n");
    fprintf(stdout, "PUSHS LF@$RETVAL\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
    //ERROR

    fprintf(stdout, "LABEL $ERRORORD\n");
    fprintf(stdout, "MOVE LF@$RETVAL string@None\n");
    fprintf(stdout, "PUSHS LF@$RETVAL\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");

}
//chr(i)– Vrátí jednoznakový řetězec se znakem, jehož ASCII kód je zadán para-metrem𝑖. Případ, kdy je𝑖mimo interval[0; 255], vede na běhovou chybu při prácis řetězcem.
void generateChr(){
    fprintf(stdout, "LABEL $CHAR\n");
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@$RETVAL\n");
    fprintf(stdout, "DEFVAR LF@$ASC\n");
    fprintf(stdout, "POPS LF@$ASC\n");

    fprintf(stdout, "DEFVAR LF@$BOOLCHECK\n");
    fprintf(stdout, "GT LF@$BOOLCHECK LF@$ASC int@255\n");
    fprintf(stdout, "JUMPIFEQ $SEM_ERROR LF@$BOOLCHECK bool@true\n");
    fprintf(stdout, "LT LF@$BOOLCHECK LF@$ASC int@0\n");
    fprintf(stdout, "JUMPIFEQ $SEM_ERROR LF@$BOOLCHECK bool@true\n");

    fprintf(stdout, "INT2CHAR LF@$RETVAL LF@$ASC\n");
    fprintf(stdout, "PUSHS LF@$RETVAL\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");

}

//||=============== Věstavené funkce konec ===============||//
//||======================================================||//

void generateBuiltIn(){
  generateSubstr();
  checkInt2Float();
  generateInputf();
  generateInputi();
  generateInputs();
  checkString();
  checkFloat2Int();
  checkInt2FloatDiv();
  generateLen();
  generateOrd();
  generateChr();
  /*
  //generatePrint();*/
}

int generateInstructionREE(tDLList*list){
    int *parCounter;
    int *labcount = 0;
    for (;list->First != NULL;list->First = list->First->rptr){
        switch(list->First->Instruction.instType){
            case(I_INPUTS):
                fprintf(stdout, "CALL $ALFA\n");
                break;
            case(I_INPUTI):
                fprintf(stdout, "CALL $BETA\n");
                break;
            case(I_INPUTF):
                fprintf(stdout, "CALL $GAMA\n");
                break;
            case(I_LEN):
                if (0) {}
                fprintf(stdout, "CALL $LENGTH\n");
                break;
            case(I_SUBSTR):
                if (0) {}
                fprintf(stdout, "CALL $SUBSTR\n");
                break;
            case(I_ORD):
                if (0) {}
                fprintf(stdout, "CALL $ORD\n");
                break;
            case(I_CHR):
                if (0) {}
                fprintf(stdout, "CALL $CHAR\n");
                break;
            case(I_PRINT):
                parCounter = list->First->Instruction.addr2;  ////////////////////////dohodni sa s jindrom
                //symtableItem *tmpItem = list->First->Instruction.addr3;
                generatePrint(parCounter);
                break;
            case(I_IF):
                generateIf(list,list->First->Instruction.addr1);
                if (!list->First) {
                  return 1;
                }
                break;
            case(I_IF_E):
                return 88;
            case(I_WHILE_B):
                generateWhile(list, list->First->Instruction.addr1);
                break;
            case(I_WHILE_E):
                return 69;
                break;
            case(I_PUSHS):
                if(0){}
                char* tmp1 = list->First->Instruction.addr1;
                if (list->First->Instruction.addr3 != NULL) {
                  fprintf(stdout, "PUSHS GF@$VAR%s\n", tmp1);
                  break;
                }
                int* tmp = list->First->Instruction.addr2;
                if (*tmp == INT) {
                  int* tmp8 = list->First->Instruction.addr1;
                  fprintf(stdout, "PUSHS int@%d\n", *tmp8);
                }
                else if(*tmp == LITERAL || *tmp == DOCCOM){
                  char* tmp8 = list->First->Instruction.addr1;
                  fprintf(stdout, "PUSHS string@%s\n", tmp8);
                }
                else if(*tmp == FLOAT){
                  double* tmp8 = list->First->Instruction.addr1;
                  fprintf(stdout, "PUSHS float@%a\n", *tmp8);
                }
                else if(*tmp == KEYWORD){
                  fprintf(stdout, "PUSHS string@None\n");
                }
                break;
            case(I_PUSHSLF):
                if(0){}
                char* tmp11 = list->First->Instruction.addr1;
                if (list->First->Instruction.addr3 != NULL) {
                  fprintf(stdout, "PUSHS LF@$VAR%s\n", tmp11);
                  break;
                }
                int* tmp22 = list->First->Instruction.addr2;
                if (*tmp22 == INT) {
                  int* tmp8 = list->First->Instruction.addr1;
                  fprintf(stdout, "PUSHS int@%d\n", *tmp8);
                }
                else if(*tmp22 == LITERAL|| *tmp == DOCCOM){
                  char* tmp8 = list->First->Instruction.addr1;
                  fprintf(stdout, "PUSHS string@%s\n", tmp8);
                }
                else if(*tmp22 == FLOAT){
                  double* tmp8 = list->First->Instruction.addr1;
                  fprintf(stdout, "PUSHS float@%a\n", *tmp8);
                }
                break;
            case(I_ADDS):
                  fprintf(stdout, "CREATEFRAME\n");
                  fprintf(stdout, "PUSHFRAME\n");
                  fprintf(stdout, "DEFVAR LF@$1\n");
                  fprintf(stdout, "DEFVAR LF@$2\n");
                  fprintf(stdout, "DEFVAR LF@$VAL1\n");
                  fprintf(stdout, "DEFVAR LF@$VAL2\n");
                  fprintf(stdout, "DEFVAR LF@$REESULT\n");
                  fprintf(stdout, "POPS LF@$VAL2\n");//delitel
                  fprintf(stdout, "POPS LF@$VAL1\n");//delenec
                  fprintf(stdout, "MOVE LF@$1 LF@$VAL1\n");
                  fprintf(stdout, "MOVE LF@$2 LF@$VAL2\n");
                  fprintf(stdout, "CALL $checkSTRING\n");////shoudlla be ok neotestovane
                  fprintf(stdout, "JUMPIFEQ $CONCAT%p LF@$BOOLCHECK_00 bool@true\n", &list->First->Instruction.instType);
                  fprintf(stdout, "CALL $checkINT2FLT\n");
                  fprintf(stdout, "ADD LF@$REESULT LF@$1 LF@$2\n");
                  fprintf(stdout, "JUMP $RESULT_END%p\n",&list->First->Instruction.instType);
                  fprintf(stdout, "LABEL $CONCAT%p\n",&list->First->Instruction.instType);
                  fprintf(stdout, "CONCAT LF@$REESULT LF@$1 LF@$2\n");
                  fprintf(stdout, "LABEL $RESULT_END%p\n",&list->First->Instruction.instType);
                  fprintf(stdout, "PUSHS LF@$REESULT\n");
                  fprintf(stdout, "POPFRAME\n");
                  break;
            case(I_SUBS):
                fprintf(stdout, "CREATEFRAME\n");
                fprintf(stdout, "PUSHFRAME\n");
                fprintf(stdout, "DEFVAR LF@$1\n");
                fprintf(stdout, "DEFVAR LF@$2\n");
                fprintf(stdout, "DEFVAR LF@$VAL1\n");
                fprintf(stdout, "DEFVAR LF@$VAL2\n");
                fprintf(stdout, "DEFVAR LF@$REESULT\n");
                fprintf(stdout, "DEFVAR LF@$CHECKNONE\n");
                fprintf(stdout, "POPS LF@$VAL2\n");
                fprintf(stdout, "POPS LF@$VAL1\n");
                fprintf(stdout, "MOVE LF@$1 LF@$VAL1\n");
                fprintf(stdout, "MOVE LF@$2 LF@$VAL2\n");
                fprintf(stdout, "TYPE LF@$CHECKNONE LF@$VAL1\n");
                fprintf(stdout, "JUMPIFEQ $SEM_ERROR LF@$CHECKNONE string@string\n");
                fprintf(stdout, "TYPE LF@$CHECKNONE LF@$VAL2\n");
                fprintf(stdout, "JUMPIFEQ $SEM_ERROR LF@$CHECKNONE string@string\n");
                fprintf(stdout, "DEFVAR LF@$BOOLCHECK_00\n");
                fprintf(stdout, "MOVE LF@$BOOLCHECK_00 bool@false\n");///meme podmienka kvoli stringom
                fprintf(stdout, "CALL $checkINT2FLT\n");
                fprintf(stdout, "SUB LF@$REESULT LF@$1 LF@$2\n");
                fprintf(stdout, "PUSHS LF@$REESULT\n");
                fprintf(stdout, "POPFRAME\n");
                break;
            case(I_MULS):
                fprintf(stdout, "CREATEFRAME\n");
                fprintf(stdout, "PUSHFRAME\n");
                fprintf(stdout, "DEFVAR LF@$1\n");
                fprintf(stdout, "DEFVAR LF@$2\n");
                fprintf(stdout, "DEFVAR LF@$VAL1\n");
                fprintf(stdout, "DEFVAR LF@$VAL2\n");
                fprintf(stdout, "DEFVAR LF@$REESULT\n");
                fprintf(stdout, "DEFVAR LF@$CHECKNONE\n");
                fprintf(stdout, "POPS LF@$VAL2\n");
                fprintf(stdout, "POPS LF@$VAL1\n");
                fprintf(stdout, "MOVE LF@$1 LF@$VAL1\n");
                fprintf(stdout, "MOVE LF@$2 LF@$VAL2\n");
                fprintf(stdout, "TYPE LF@$CHECKNONE LF@$VAL1\n");
                fprintf(stdout, "JUMPIFEQ $SEM_ERROR LF@$CHECKNONE string@string\n");
                fprintf(stdout, "TYPE LF@$CHECKNONE LF@$VAL2\n");
                fprintf(stdout, "JUMPIFEQ $SEM_ERROR LF@$CHECKNONE string@string\n");
                fprintf(stdout, "DEFVAR LF@$BOOLCHECK_00\n");
                fprintf(stdout, "MOVE LF@$BOOLCHECK_00 bool@false\n");///meme podmienka kvoli stringom
                fprintf(stdout, "CALL $checkINT2FLT\n");
                fprintf(stdout, "MUL LF@$REESULT LF@$1 LF@$2\n");
                fprintf(stdout, "PUSHS LF@$REESULT\n");
                fprintf(stdout, "POPFRAME\n");
                break;
            case(I_DIVS):
                fprintf(stdout, "CREATEFRAME\n");
                fprintf(stdout, "PUSHFRAME\n");
                fprintf(stdout, "DEFVAR LF@$1\n");
                fprintf(stdout, "DEFVAR LF@$2\n");
                fprintf(stdout, "DEFVAR LF@$VAL1\n");
                fprintf(stdout, "DEFVAR LF@$VAL2\n");
                fprintf(stdout, "DEFVAR LF@$REESULT\n");
                fprintf(stdout, "DEFVAR LF@$CHECKNONE\n");
                fprintf(stdout, "POPS LF@$VAL2\n");//delitel
                fprintf(stdout, "POPS LF@$VAL1\n");//delenec
                fprintf(stdout, "MOVE LF@$1 LF@$VAL1\n");
                fprintf(stdout, "MOVE LF@$2 LF@$VAL2\n");
                fprintf(stdout, "TYPE LF@$CHECKNONE LF@$VAL1\n");
                fprintf(stdout, "JUMPIFEQ $SEM_ERROR LF@$CHECKNONE string@string\n");
                fprintf(stdout, "TYPE LF@$CHECKNONE LF@$VAL2\n");
                fprintf(stdout, "JUMPIFEQ $SEM_ERROR LF@$CHECKNONE string@string\n");
                fprintf(stdout, "DEFVAR LF@$BOOLCHECK_00\n");
                fprintf(stdout, "MOVE LF@$BOOLCHECK_00 bool@false\n");///meme podmienka kvoli stringom
                fprintf(stdout, "CALL $checkINT2FLTDIV\n");
                fprintf(stdout, "JUMPIFEQ $ZEROERROR float@%a LF@$2\n", 0.0);
                fprintf(stdout, "DIV LF@$REESULT LF@$1 LF@$2\n");
                fprintf(stdout, "PUSHS LF@$REESULT\n");
                fprintf(stdout, "POPFRAME\n");
                break;
            case(I_IDIVS):
                fprintf(stdout, "CREATEFRAME\n");
                fprintf(stdout, "PUSHFRAME\n");
                //DEFINICE PROMENNYCH
                fprintf(stdout, "DEFVAR LF@$1\n");
                fprintf(stdout, "DEFVAR LF@$2\n");
                fprintf(stdout, "DEFVAR LF@$VAL1\n");
                fprintf(stdout, "DEFVAR LF@$VAL2\n");
                fprintf(stdout, "DEFVAR LF@$REESULT\n");
                fprintf(stdout, "DEFVAR LF@$CHECKNONE\n");
                //INICIALIZACE
                fprintf(stdout, "POPS LF@$VAL2\n");//delitel
                fprintf(stdout, "POPS LF@$VAL1\n");//delenec
                fprintf(stdout, "MOVE LF@$1 LF@$VAL1\n");
                fprintf(stdout, "MOVE LF@$2 LF@$VAL2\n");
                //CHECKOVANI FLOATU ATD
                fprintf(stdout, "TYPE LF@$CHECKNONE LF@$VAL1\n");
                fprintf(stdout, "JUMPIFEQ $SEM_ERROR LF@$CHECKNONE string@string\n");
                fprintf(stdout, "TYPE LF@$CHECKNONE LF@$VAL2\n");
                fprintf(stdout, "JUMPIFEQ $SEM_ERROR LF@$CHECKNONE string@string\n");
                fprintf(stdout, "CALL $checkFLT2INT\n");
                fprintf(stdout, "JUMPIFEQ $ZEROERROR int@0 LF@$VAL2\n");
                fprintf(stdout, "IDIV LF@$REESULT LF@$1 LF@$2\n");
                fprintf(stdout, "PUSHS LF@$REESULT\n");
                fprintf(stdout, "POPFRAME\n");

                break;
            case(I_DEFVAR):
                if(0){}
                break;
            case(I_DEFVARLOCAL):
                break;
            case(I_DEFVARLF):
                break;
            case(I_POPS):
                if(0){}
                symtableItem *reee = list->First->Instruction.addr1;
                fprintf(stdout, "POPS GF@$VAR%s\n", reee->key);
                break;
            case(I_POPSLOCAL):
                if(0){}
                symtableItem *localPop = list->First->Instruction.addr1;
                fprintf(stdout, "POPS LF@$VAR%s\n", localPop->key);
                break;
            case(I_GTS): //greater
                fprintf(stdout, "CREATEFRAME\n");
                fprintf(stdout, "PUSHFRAME\n");
                fprintf(stdout, "DEFVAR LF@$1\n");
                fprintf(stdout, "DEFVAR LF@$2\n");
                fprintf(stdout, "DEFVAR LF@$VAL1\n");
                fprintf(stdout, "DEFVAR LF@$VAL2\n");
                fprintf(stdout, "DEFVAR LF@$REESULT\n");
                fprintf(stdout, "POPS LF@$VAL2\n");
                fprintf(stdout, "POPS LF@$VAL1\n");
                fprintf(stdout, "MOVE LF@$1 LF@$VAL1\n");
                fprintf(stdout, "MOVE LF@$2 LF@$VAL2\n");

                //fprintf(stdout, "DEFVAR LF@$BOOLCHECK_00\n");
                //fprintf(stdout, "MOVE LF@$BOOLCHECK_00 bool@false\n");///meme podmienka kvoli stringom

                fprintf(stdout, "CALL $checkSTRING\n");
                fprintf(stdout, "CALL $checkINT2FLT\n");
                fprintf(stdout, "GT LF@$REESULT LF@$1 LF@$2\n");
                fprintf(stdout, "PUSHS LF@$REESULT\n");
                fprintf(stdout, "POPFRAME\n");
                break;
            case(I_LTS): //less
                fprintf(stdout, "CREATEFRAME\n");
                fprintf(stdout, "PUSHFRAME\n");
                fprintf(stdout, "DEFVAR LF@$1\n");
                fprintf(stdout, "DEFVAR LF@$2\n");
                fprintf(stdout, "DEFVAR LF@$VAL1\n");
                fprintf(stdout, "DEFVAR LF@$VAL2\n");
                fprintf(stdout, "DEFVAR LF@$REESULT\n");
                fprintf(stdout, "POPS LF@$VAL2\n");//delitel
                fprintf(stdout, "POPS LF@$VAL1\n");//delenec
                fprintf(stdout, "MOVE LF@$1 LF@$VAL1\n");
                fprintf(stdout, "MOVE LF@$2 LF@$VAL2\n");
                //fprintf(stdout, "DEFVAR LF@$BOOLCHECK_00\n");
                //fprintf(stdout, "MOVE LF@$BOOLCHECK_00 bool@false\n");///meme podmienka kvoli stringom ked nie je check string
                fprintf(stdout, "CALL $checkSTRING\n");
                fprintf(stdout, "CALL $checkINT2FLT\n");
                fprintf(stdout, "LT LF@$REESULT LF@$1 LF@$2\n");
                fprintf(stdout, "PUSHS LF@$REESULT\n");
                fprintf(stdout, "POPFRAME\n");
                break;
            case(I_EQS):
                //fprintf(stdout, "EQS \n");
                fprintf(stdout, "CREATEFRAME\n");
                fprintf(stdout, "PUSHFRAME\n");
                fprintf(stdout, "DEFVAR LF@$1\n");
                fprintf(stdout, "DEFVAR LF@$2\n");
                fprintf(stdout, "DEFVAR LF@$VAL1\n");
                fprintf(stdout, "DEFVAR LF@$VAL2\n");
                fprintf(stdout, "DEFVAR LF@$REESULT\n");
                fprintf(stdout, "POPS LF@$VAL2\n");
                fprintf(stdout, "POPS LF@$VAL1\n");
                fprintf(stdout, "MOVE LF@$1 LF@$VAL1\n");
                fprintf(stdout, "MOVE LF@$2 LF@$VAL2\n");
                fprintf(stdout, "CALL $checkSTRING\n");
                fprintf(stdout, "CALL $checkINT2FLT\n");
                fprintf(stdout, "EQ LF@$REESULT LF@$1 LF@$2\n");
                fprintf(stdout, "PUSHS LF@$REESULT\n");
                fprintf(stdout, "POPFRAME\n");
                break;
            case(I_NQS):

                fprintf(stdout, "CREATEFRAME\n");
                fprintf(stdout, "PUSHFRAME\n");
                fprintf(stdout, "DEFVAR LF@$1\n");
                fprintf(stdout, "DEFVAR LF@$2\n");
                fprintf(stdout, "DEFVAR LF@$VAL1\n");
                fprintf(stdout, "DEFVAR LF@$VAL2\n");
                fprintf(stdout, "DEFVAR LF@$REESULT\n");
                fprintf(stdout, "POPS LF@$VAL2\n");
                fprintf(stdout, "POPS LF@$VAL1\n");
                fprintf(stdout, "MOVE LF@$1 LF@$VAL1\n");
                fprintf(stdout, "MOVE LF@$2 LF@$VAL2\n");
                fprintf(stdout, "CALL $checkSTRING\n");
                fprintf(stdout, "CALL $checkINT2FLT\n");
                fprintf(stdout, "EQ LF@$REESULT LF@$1 LF@$2\n");
                fprintf(stdout, "NOT LF@$REESULT LF@$REESULT\n");
                fprintf(stdout, "PUSHS LF@$REESULT\n");
                fprintf(stdout, "POPFRAME\n");
                break;
            case(I_ELSE_E):
                return 177013;
            case(I_GT)://greater or equal
                fprintf(stdout, "CREATEFRAME\n");
                fprintf(stdout, "PUSHFRAME\n");
                fprintf(stdout, "DEFVAR LF@$RETVAL\n");
                fprintf(stdout, "DEFVAR LF@$1\n");
                fprintf(stdout, "POPS LF@$1\n");
                fprintf(stdout, "DEFVAR LF@$2\n");
                fprintf(stdout, "POPS LF@$2\n");
                //
                fprintf(stdout, "DEFVAR LF@$BOOLCHECK_00\n");
                fprintf(stdout, "MOVE LF@$BOOLCHECK_00 bool@false\n");///meme podmienka kvoli stringom
                fprintf(stdout, "CALL $checkINT2FLT\n");
                //
                fprintf(stdout, "LT LF@$RETVAL LF@$2 LF@$1\n");
                fprintf(stdout, "NOT LF@$RETVAL LF@$RETVAL\n");
                fprintf(stdout, "PUSHS LF@$RETVAL\n");
                fprintf(stdout, "POPFRAME\n");
                break;
            case(I_LT):
                fprintf(stdout, "CREATEFRAME\n");
                fprintf(stdout, "PUSHFRAME\n");
                fprintf(stdout, "DEFVAR LF@$RETVAL\n");
                fprintf(stdout, "DEFVAR LF@$1\n");
                fprintf(stdout, "POPS LF@$1\n");
                fprintf(stdout, "DEFVAR LF@$2\n");
                fprintf(stdout, "POPS LF@$2\n");
                //
                //fprintf(stdout, "DEFVAR LF@$BOOLCHECK_00\n");
                //fprintf(stdout, "MOVE LF@$BOOLCHECK_00 bool@false\n");///meme podmienka kvoli stringom
                fprintf(stdout, "CALL $checkSTRING\n");
                fprintf(stdout, "CALL $checkINT2FLT\n");
                //
                fprintf(stdout, "GT LF@$RETVAL LF@$2 LF@$1\n");
                fprintf(stdout, "NOT LF@$RETVAL LF@$RETVAL\n");
                fprintf(stdout, "PUSHS LF@$RETVAL\n");
                fprintf(stdout, "POPFRAME\n");
                break;
            case(I_EOE):
                return 69;
            case(I_CONCAT):
                fprintf(stdout, "CREATEFRAME\n");
                fprintf(stdout, "PUSHFRAME\n");
                fprintf(stdout, "DEFVAR LF@$RETVAL\n");
                fprintf(stdout, "DEFVAR LF@$STR1\n");
                fprintf(stdout, "POPS LF@$STR1\n");
                fprintf(stdout, "DEFVAR LF@$STR2\n");
                fprintf(stdout, "POPS LF@$STR2\n");
                /////uh ošetrit ci su oba stringy?
                fprintf(stdout, "CONCAT LF@$RETVAL LF@$STR2 LF@$STR1\n");
                fprintf(stdout, "PUSHS LF@$RETVAL\n");
                fprintf(stdout, "POPFRAME\n");
                break;
            case(I_STARTOFFUNC):
                if(0){}
                char *idk = list->First->Instruction.addr1;
                symtableItem *itemNaArgumenty = list->First->Instruction.addr2;
                aktualniArgumenty = malloc(actNumberOfLF * sizeof(symtableItem*));
                actNumberOfLF = itemNaArgumenty->elementType.function->argCount;
                aktualniArgumenty = (itemNaArgumenty->elementType.function->arguments);
                fprintf(stdout, "JUMP $FUNCTIONEND%s\n", idk);
                fprintf(stdout, "LABEL $FUNCTION%s\n", idk);
                fprintf(stdout, "CREATEFRAME\n");
                fprintf(stdout, "PUSHFRAME\n");
                list->First = list->First->rptr;
                doDefVarsLocal(list);
                generateInstructionREE(list);
                break;
            case(I_ENDOFFUNC):
                if(0){}
                char *idk2 = list->First->Instruction.addr1;
                fprintf(stdout, "POPFRAME\n");
                fprintf(stdout, "RETURN\n");
                fprintf(stdout, "LABEL $FUNCTIONEND%s\n", idk2);
                counterZanoreni = 0;
                actNumberOfLF = 0;
                free(aktualniArgumenty);
                return 666;
                break;
            case(I_CALL):
                if(0){}
                functionData *funkce = list->First->Instruction.addr1;
                char *nazevFunkce = list->First->Instruction.addr2;
                defenestrace(funkce->argCount);
                fprintf(stdout, "CALL $FUNCTION%s\n", nazevFunkce);
                break;
            case(I_RETURN):
                for(int i = 0; i < counterZanoreni; i++) {
                  fprintf(stdout, "POPFRAME\n");
                }
                fprintf(stdout, "POPFRAME\n");
                fprintf(stdout, "RETURN\n");
                break;
        }
    }
}

//converts int to float if needed
void checkInt2Float(){
  fprintf(stdout, "LABEL $checkINT2FLT\n");
  fprintf(stdout, "JUMPIFEQ $LAB22 LF@$BOOLCHECK_00 bool@true\n");
  fprintf(stdout, "DEFVAR LF@$T1\n");
  fprintf(stdout, "DEFVAR LF@$T2\n");
  fprintf(stdout, "TYPE LF@$T1 LF@$1\n");
  fprintf(stdout, "TYPE LF@$T2 LF@$2\n");
  fprintf(stdout, "DEFVAR LF@$BOOLCHECK0\n");
  fprintf(stdout, "DEFVAR LF@$BOOLCHECK1\n");
  fprintf(stdout, "DEFVAR LF@$BOOLCHECK2\n");
  fprintf(stdout, "EQ LF@$BOOLCHECK0 LF@$T1 string@int\n");
  fprintf(stdout, "EQ LF@$BOOLCHECK1 LF@$T2 string@int\n");
  fprintf(stdout, "AND LF@$BOOLCHECK2 LF@$BOOLCHECK0 LF@$BOOLCHECK1\n");
  fprintf(stdout, "JUMPIFEQ $LAB22 LF@$BOOLCHECK2 bool@true\n");
  fprintf(stdout, "LABEL $LAB00\n");
  fprintf(stdout, "JUMPIFEQ $LAB11 LF@$T1 string@float\n");
  fprintf(stdout, "INT2FLOAT LF@$1 LF@$1\n");
  fprintf(stdout, "LABEL $LAB11\n");
  fprintf(stdout, "JUMPIFEQ $LAB22 LF@$T2 string@float\n");
  fprintf(stdout, "INT2FLOAT LF@$2 LF@$2\n");
  fprintf(stdout, "LABEL $LAB22\n");
  fprintf(stdout, "RETURN\n");
}

void checkString(){
    fprintf(stdout, "LABEL $checkSTRING\n");
    fprintf(stdout, "DEFVAR LF@$ST1\n");
    fprintf(stdout, "DEFVAR LF@$ST2\n");
    fprintf(stdout, "TYPE LF@$ST1 LF@$1\n");
    fprintf(stdout, "TYPE LF@$ST2 LF@$2\n");

    fprintf(stdout, "DEFVAR LF@$BOOLCHECK_A\n");
    fprintf(stdout, "DEFVAR LF@$BOOLCHECK_B\n");
    fprintf(stdout, "DEFVAR LF@$BOOLCHECK_00\n");
    fprintf(stdout, "DEFVAR LF@$BOOLCHECK_0\n");
    fprintf(stdout, "DEFVAR LF@$BOOLCHECK_1\n");
    fprintf(stdout, "DEFVAR LF@$BOOLCHECK_2\n");

    fprintf(stdout, "EQ LF@$BOOLCHECK_A LF@$ST1 string@string\n");
    fprintf(stdout, "EQ LF@$BOOLCHECK_B LF@$ST2 string@string\n");

    fprintf(stdout, "AND LF@$BOOLCHECK_00 LF@$BOOLCHECK_A LF@$BOOLCHECK_B\n");

    fprintf(stdout, "OR LF@$BOOLCHECK_0 LF@$BOOLCHECK_A LF@$BOOLCHECK_B\n");

    fprintf(stdout, "NOT LF@$BOOLCHECK_A LF@$BOOLCHECK_A\n");
    fprintf(stdout, "NOT LF@$BOOLCHECK_B LF@$BOOLCHECK_B\n");

    fprintf(stdout, "OR LF@$BOOLCHECK_1 LF@$BOOLCHECK_A LF@$BOOLCHECK_B\n");

    fprintf(stdout, "AND LF@$BOOLCHECK_2 LF@$BOOLCHECK_0 LF@$BOOLCHECK_1\n");

    fprintf(stdout, "JUMPIFEQ $SEM_ERROR LF@$BOOLCHECK_2 bool@true\n");
    fprintf(stdout, "RETURN\n");
}

void checkInt2FloatDiv(){
  fprintf(stdout, "LABEL $checkINT2FLTDIV\n");
  fprintf(stdout, "DEFVAR LF@$T1\n");
  fprintf(stdout, "DEFVAR LF@$T2\n");
  fprintf(stdout, "TYPE LF@$T1 LF@$1\n");
  fprintf(stdout, "TYPE LF@$T2 LF@$2\n");
  fprintf(stdout, "JUMPIFEQ $LAB111 LF@$T1 string@float\n");
  fprintf(stdout, "INT2FLOAT LF@$1 LF@$1\n");
  fprintf(stdout, "LABEL $LAB111\n");
  fprintf(stdout, "JUMPIFEQ $LAB222 LF@$T2 string@float\n");
  fprintf(stdout, "INT2FLOAT LF@$2 LF@$2\n");
  fprintf(stdout, "LABEL $LAB222\n");
  fprintf(stdout, "RETURN\n");
}

//converts float to int if needed
void checkFloat2Int(){
  fprintf(stdout, "LABEL $checkFLT2INT\n");
  fprintf(stdout, "DEFVAR LF@$T1\n");
  fprintf(stdout, "DEFVAR LF@$T2\n");
  fprintf(stdout, "TYPE LF@$T1 LF@$1\n");
  fprintf(stdout, "TYPE LF@$T2 LF@$2\n");
  fprintf(stdout, "JUMPIFEQ $LAB1 LF@$T1 string@int\n");
  fprintf(stdout, "FLOAT2INT LF@$1 LF@$1\n");
  fprintf(stdout, "LABEL $LAB1\n");
  fprintf(stdout, "JUMPIFEQ $LAB2 LF@$T2 string@int\n");
  fprintf(stdout, "FLOAT2INT LF@$2 LF@$2\n");
  fprintf(stdout, "LABEL $LAB2\n");
  fprintf(stdout, "RETURN\n");
}

void defenestrace(int antiHussites){
  fprintf(stdout, "CREATEFRAME\n");
  fprintf(stdout, "PUSHFRAME\n");
  for (int i = 0; i < antiHussites; i++){
    fprintf(stdout, "DEFVAR LF@$WINDOW%d\n", i);
    fprintf(stdout, "POPS LF@$WINDOW%d\n", i);
  }
  for (int i = 0; i < antiHussites; i++){
    fprintf(stdout, "PUSHS LF@$WINDOW%d\n", i);
  }
  fprintf(stdout, "POPFRAME\n");
}


///na IF potrebujem nejako vediet indent že dokedy vykonavat funckie vo vnutri, alebo
///potrebujem vediet prechody, teda Zaciatok IF , ELSE , Koniec IF a to niekde pocitat v pripade vnutornych IFov
///nasledne mozem volat generateInstruction rekurzivne...? Asi ano..
void generateIf(tDLList*list, void *origi){
  counterZanoreni++;

  int pocetLokalnichPred = actNumberOfLF;
  fprintf(stdout, "CREATEFRAME\n");
  for(int i = 0; i < actNumberOfLF; i++){
        fprintf(stdout,"DEFVAR TF@$VAR%s\n", (aktualniArgumenty[i]).key);
        fprintf(stdout,"MOVE TF@$VAR%s LF@$VAR%s\n", (aktualniArgumenty[i]).key, (aktualniArgumenty[i]).key);
  }
  fprintf(stdout, "PUSHFRAME\n");
  //zistit condition
  list->First = list->First->rptr;
  generateInstructionREE(list);
  fprintf(stdout, "DEFVAR LF@$COND%p\n", origi);
  fprintf(stdout, "POPS LF@$COND%p\n", origi);

  fprintf(stdout, "DEFVAR LF@$$TYPE%p\n", origi);
  fprintf(stdout, "TYPE LF@$$TYPE%p LF@$COND%p\n",origi, origi);  //'' None 0

  fprintf(stdout, "JUMPIFEQ $COND_TYPE_INT%p LF@$$TYPE%p string@int\n", origi, origi);  // 0
  fprintf(stdout, "JUMPIFEQ $COND_TYPE_FLOAT%p LF@$$TYPE%p string@float\n", origi, origi);  //0.0
  fprintf(stdout, "JUMPIFEQ $COND_TYPE_STRING%p LF@$$TYPE%p string@string\n", origi, origi);  // None
  fprintf(stdout, "JUMPIFEQ $SKIPPUSHTRUE%p LF@$$TYPE%p string@bool\n", origi, origi);  // None
  fprintf(stdout, "JUMPIFEQ $PUSHFALSE%p LF@$COND%p string@\n", origi, origi);  // None

  fprintf(stdout, "JUMP $SKIPPUSHFALSE%p\n",origi);

  fprintf(stdout, "LABEL $COND_TYPE_INT%p\n",origi);
  fprintf(stdout, "JUMPIFEQ $PUSHFALSE%p LF@$COND%p int@0\n",origi, origi);
  fprintf(stdout, "JUMP $SKIPPUSHFALSE%p\n",origi);


  fprintf(stdout, "LABEL $COND_TYPE_FLOAT%p\n",origi);
  fprintf(stdout, "JUMPIFEQ $PUSHFALSE%p LF@$COND%p float@%a\n",origi, origi,0.0);
  fprintf(stdout, "JUMP $SKIPPUSHFALSE%p\n",origi);


  fprintf(stdout, "LABEL $COND_TYPE_STRING%p\n",origi);
  //fprintf(stdout, "JUMPIFEQ $PUSHFALSE%p LF@$COND%p nil@nil\n",origi, origi);
  fprintf(stdout, "JUMPIFEQ $PUSHFALSE%p LF@$COND%p string@None\n",origi, origi);
  //fprintf(stdout, "JUMPIFEQ $PUSHFALSE%p LF@$COND%p string@\\010\n",origi, origi);

  fprintf(stdout, "JUMP $SKIPPUSHFALSE%p\n",origi);

  fprintf(stdout, "LABEL $PUSHFALSE%p\n",origi);
  fprintf(stdout, "MOVE LF@$COND%p bool@false\n",origi);
  fprintf(stdout, "JUMP $SKIPPUSHTRUE%p\n",origi);

  fprintf(stdout, "LABEL $SKIPPUSHFALSE%p\n",origi);
  fprintf(stdout, "MOVE LF@$COND%p bool@true\n",origi);
  fprintf(stdout, "LABEL $SKIPPUSHTRUE%p\n",origi);

  //fprintf(stdout, "JUMPIFNEQ WHILE$END$%p bool@true LF@$COND%p\n", origi, origi);
  fprintf(stdout, "JUMPIFNEQ $ELSE%p bool@true LF@$COND%p\n",origi, origi);
  list->First = list->First->rptr;
  generateInstructionREE(list); //dokym nenajdem rovnaky indent
  fprintf(stdout, "JUMP END%p\n",origi);
  fprintf(stdout, "LABEL $ELSE%p\n",origi);
  list->First = list->First->rptr;
  generateInstructionREE(list); //dokym nenajdem rovnaky indent
  fprintf(stdout, "LABEL END%p\n",origi);
  for(int i = actNumberOfLF-1; i >= 0; i--){
        fprintf(stdout,"PUSHS LF@$VAR%s\n", (aktualniArgumenty[i]).key);
  }
  fprintf(stdout, "POPFRAME\n");
  for(int i = actNumberOfLF-1; i>=pocetLokalnichPred; i--){
    fprintf(stdout,"DEFVAR LF@$VAR%s\n", (aktualniArgumenty[i]).key);
  }
  for(int i = 0; i < actNumberOfLF; i++){
        fprintf(stdout,"POPS LF@$VAR%s\n", (aktualniArgumenty[i]).key);
  }
  counterZanoreni--;
}

void generateWhile(tDLList*list, void *origi){
  counterZanoreni++;
  //fprintf(stdout, "DEFVAR GF@WHILEDEF%p\n", origi);
  //fprintf(stdout, "MOVE GF@WHILEDEF%p int@0\n", origi);
  //int pocetLoopov = 0;
  int pocetLokalnichPred = actNumberOfLF;
  fprintf(stdout, "CREATEFRAME\n");
  //////predavanie hodnot i guess
  for(int i = 0; i < actNumberOfLF; i++){
        fprintf(stdout,"DEFVAR TF@$VAR%s\n", (aktualniArgumenty[i]).key);
        fprintf(stdout,"MOVE TF@$VAR%s LF@$VAR%s\n", (aktualniArgumenty[i]).key, (aktualniArgumenty[i]).key);
  }
  //////////////////////////
  fprintf(stdout, "PUSHFRAME\n");
  fprintf(stdout, "DEFVAR LF@$COND%p\n", origi);
  tDLElemPtr jmp2 = list->First->rptr;
  list->First = list->First->rptr;
  generateInstructionREE(list);
  fprintf(stdout, "DEFVAR LF@$TYPE%p\n", origi);
  fprintf(stdout, "LABEL WHILE$BEGIN$%p\n", origi);
  fprintf(stdout, "POPS LF@$COND%p\n", origi);
  ////
  ////
  fprintf(stdout, "TYPE LF@$TYPE%p LF@$COND%p\n",origi, origi);  //'' None 0
  ///jump na zaklade typu
  fprintf(stdout, "JUMPIFEQ $COND_TYPE_INT%p LF@$TYPE%p string@int\n", origi, origi);  // 0
  fprintf(stdout, "JUMPIFEQ $COND_TYPE_FLOAT%p LF@$TYPE%p string@float\n", origi, origi);  //0.0
  fprintf(stdout, "JUMPIFEQ $COND_TYPE_STRING%p LF@$TYPE%p string@string\n", origi, origi);  // None
  fprintf(stdout, "JUMPIFEQ $SKIPPUSHTRUE%p LF@$TYPE%p string@bool\n", origi, origi);  // None
  fprintf(stdout, "JUMPIFEQ $PUSHFALSE%p LF@$COND%p string@\\000\n", origi, origi);  // None
  ///cond je okey
  fprintf(stdout, "JUMP $SKIPPUSHFALSE%p\n",origi);
  ///cond je int tak check ci nula
  fprintf(stdout, "LABEL $COND_TYPE_INT%p\n",origi);
  fprintf(stdout, "JUMPIFEQ $PUSHFALSE%p LF@$COND%p int@0\n",origi, origi);
  fprintf(stdout, "JUMP $SKIPPUSHFALSE%p\n",origi);
  //cond je flaot tak check ci nula.nula
  fprintf(stdout, "LABEL $COND_TYPE_FLOAT%p\n",origi);
  fprintf(stdout, "JUMPIFEQ $PUSHFALSE%p LF@$COND%p float@%a\n",origi, origi,0.0);
  fprintf(stdout, "JUMP $SKIPPUSHFALSE%p\n",origi);
  //string tak ci je None
  fprintf(stdout, "LABEL $COND_TYPE_STRING%p\n",origi);
  //fprintf(stdout, "JUMPIFEQ $PUSHFALSE%p LF@$COND%p nil@nil\n",origi, origi);
  fprintf(stdout, "JUMPIFEQ $PUSHFALSE%p LF@$COND%p string@None\n",origi, origi);
  fprintf(stdout, "JUMPIFEQ $PUSHFALSE%p LF@$COND%p string@\\010\n",origi, origi);
  fprintf(stdout, "JUMPIFEQ $PUSHFALSE%p LF@$COND%p string@\\032\n",origi, origi);
  fprintf(stdout, "JUMP $SKIPPUSHFALSE%p\n",origi);

  fprintf(stdout, "LABEL $PUSHFALSE%p\n",origi);
  fprintf(stdout, "MOVE LF@$COND%p bool@false\n",origi);
  fprintf(stdout, "JUMP $SKIPPUSHTRUE%p\n",origi);

  fprintf(stdout, "LABEL $SKIPPUSHFALSE%p\n",origi);
  fprintf(stdout, "MOVE LF@$COND%p bool@true\n",origi);
  fprintf(stdout, "LABEL $SKIPPUSHTRUE%p\n",origi);
  ////
  ////
  fprintf(stdout, "JUMPIFNEQ WHILE$END$%p bool@true LF@$COND%p\n", origi, origi);
  list->First = list->First->rptr;
  generateInstructionREE(list); //dokym nenajdem dedent
  tDLElemPtr jmp1 = list->First;
  //fprintf(stdout, "ADD GF@WHILEDEF%p GF@WHILEDEF%p int@1\n", origi, origi);
  list->First = jmp2;
  generateInstructionREE(list); //dokym nenajdem konec expression(podminky)
  list->First = jmp1;
  fprintf(stdout, "JUMP WHILE$BEGIN$%p\n", origi);
  fprintf(stdout, "LABEL WHILE$END$%p\n", origi);
  for(int i = actNumberOfLF-1; i >= 0; i--){
        fprintf(stdout,"PUSHS LF@$VAR%s\n", (aktualniArgumenty[i]).key);
  }
  fprintf(stdout, "POPFRAME\n");
  for(int i = actNumberOfLF-1; i>=pocetLokalnichPred; i--){
    fprintf(stdout,"DEFVAR LF@$VAR%s\n", (aktualniArgumenty[i]).key);
  }
  for(int i = 0; i < actNumberOfLF; i++){
        fprintf(stdout,"POPS LF@$VAR%s\n", (aktualniArgumenty[i]).key);
  }
  counterZanoreni--;
}

void doDefVars(tDLList*list){
  list->Act = list->First;
  while(list->Act != NULL){
    if(list->Act->Instruction.instType == I_DEFVAR){
      symtableItem *ree = list->Act->Instruction.addr1;
      fprintf(stdout, "DEFVAR GF@$VAR%s\n", ree->key);

      if(list->Act->lptr != NULL){
        list->Act->lptr->rptr = list->Act->rptr;
      }
      else{
        list->First = list->First->rptr;
      }
    }
    list->Act = list->Act->rptr;
  }
}

void doDefVarsLocal(tDLList*list){
  list->Act = list->First;
  while(list->Act->Instruction.instType != I_ENDOFFUNC && list->Act != NULL){
    if(list->Act->Instruction.instType == I_DEFVARLF){
      char *notree = list->Act->Instruction.addr1;
      fprintf(stdout, "DEFVAR LF@$VAR%s\n", notree);
      fprintf(stdout, "POPS LF@$VAR%s\n", notree);
      if(list->Act->lptr != NULL){
        list->Act->lptr->rptr = list->Act->rptr;
      }
      else{
        list->First = list->First->rptr;
      }
    }
    else if(list->Act->Instruction.instType == I_DEFVARLOCAL){
      symtableItem *ree = list->Act->Instruction.addr1;
      fprintf(stdout, "DEFVAR LF@$VAR%s\n", ree->key);
      fprintf(stdout, "MOVE LF@$VAR%s nil@nil\n", ree->key);
      if(list->Act->Instruction.addr3 == NULL){
        actNumberOfLF++;
        aktualniArgumenty = realloc(aktualniArgumenty, actNumberOfLF * sizeof(symtableItem));
        (aktualniArgumenty[actNumberOfLF-1]).key = ree->key;
      }

      if(list->Act->lptr != NULL){
        list->Act->lptr->rptr = list->Act->rptr;
      }
      else{
        list->First = list->First->rptr;
      }
    }
    list->Act = list->Act->rptr;
  }
}

// IF už funguje pre None 0 0.0 as false others true
