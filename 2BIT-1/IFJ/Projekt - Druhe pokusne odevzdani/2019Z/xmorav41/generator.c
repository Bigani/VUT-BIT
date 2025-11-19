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

//TODO
//urobit print -> cez pointre od tsundri nacitat globals do stacku pred zavolanim funkcie, ktory budem v print loope popovat

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "symtable.h"
#include "scanner.h"
#include "expressions.h"
#include "list.h"
#include "generator.h"

int functionCounter = 0;


void degenerate(tDLList *list){
    fprintf(stdout, ".IFJcode19\n");
    fprintf(stdout, "JUMP $$MAIN\n");
    generateBuiltIn();
    fprintf(stdout, "LABEL $ZEROERROR\n");
    fprintf(stdout, "EXIT int@%d\n", DIVISION_BY_ZERO);
    fprintf(stdout, "LABEL $$MAIN\n");
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "PUSHFRAME\n");
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
    fprintf(stdout, "POPFRAME\n");
}

void generateLen(){

    fprintf(stdout, "LABEL $LENGTH\n");
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@$RETVAL\n");
    fprintf(stdout, "DEFVAR LF@$1\n");
    fprintf(stdout, "POPS LF@$1\n");

    fprintf(stdout, "STRLEN LF@$RETVAL LF@$1\n"); ///premenne parametrov su MEME (zapis cislami)
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

    //fprintf(stdout, "MOVE LF@$RETVAL string@\n");/////this shoudda be var into string init, by example_while.src
    //ULOŽENIE PARAMETROV

    fprintf(stdout, "DEFVAR LF@$TMPLEN\n");
    fprintf(stdout, "STRLEN LF@$TMPLEN LF@$STRING\n");//delka s
    //KONTROLA POZICIE


    //fprintf(stdout, "WRITE LF@$DELKA\n");

    fprintf(stdout, "DEFVAR LF@$BOOLCHECK\n");

    //
    fprintf(stdout, "ADD LF@$TMPLEN LF@$TMPLEN int@-1\n");
    //fprintf(stdout, "ADD LF@$DELKA LF@$DELKA int@-1\n");
    //
    fprintf(stdout, "GT LF@$BOOLCHECK LF@$POSIT LF@$TMPLEN\n");
    fprintf(stdout, "JUMPIFEQ $ERRORSUB LF@$BOOLCHECK bool@true\n");

    fprintf(stdout, "LT LF@$BOOLCHECK LF@$POSIT int@0\n");

    fprintf(stdout, "JUMPIFEQ $ERRORSUB LF@$BOOLCHECK bool@true\n");
    fprintf(stdout, "GT LF@$BOOLCHECK int@0 LF@$DELKA\n");
    fprintf(stdout, "JUMPIFEQ $ERRORSUB LF@$BOOLCHECK bool@true\n");
    fprintf(stdout, "EQ LF@$BOOLCHECK int@0 LF@$DELKA\n");
    fprintf(stdout, "JUMPIFEQ $ERRORSUB LF@$BOOLCHECK bool@true\n");
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
    fprintf(stdout, "LABEL $ERRORSUB\n");
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

    fprintf(stdout, "INT2CHAR LF@$RETVAL LF@$ASC\n");
    fprintf(stdout, "PUSHS LF@$RETVAL\n");

    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
}

//||=============== Věstavené funkce konec ===============||//
//||======================================================||//

void generateBuiltIn(){
  generateOrd();
  generateInputi();
  generateSubstr();
  generateInputs();
  generateInputf();
  generateLen();
  generateChr();
    /*
    //generatePrint();*/
}




int generateInstructionREE(tDLList*list){
    int *parCounter;
    for (;list->First != NULL;list->First = list->First->rptr){
        switch(list->First->Instruction.instType){
            case(I_INPUTS):
                /*generateInputs();
                fprintf(stdout, "MOVE LF@$VAR%p TF@RETVAL\n",list->First->Instruction.addr1);*/
                fprintf(stdout, "CALL $ALFA\n");
                break;
            case(I_INPUTI):
                //generateInputi();
                //fprintf(stdout, "MOVE LF@$VAR%p TF@RETVAL\n",list->First->Instruction.addr1);
                fprintf(stdout, "CALL $BETA\n");
                break;
            case(I_INPUTF):
                /*generateInputf();
                fprintf(stdout, "MOVE LF@$VAR%p TF@RETVAL\n",list->First->Instruction.addr1);*/
                fprintf(stdout, "CALL $GAMA\n");
                break;
            case(I_LEN):
                //generateLen();
                if (0) {}
                char* st = list->First->Instruction.addr1;
                //fprintf(stdout, "PUSHS string@%s\n", st);
                fprintf(stdout, "CALL $LENGTH\n");
                break;
            case(I_SUBSTR):
                //generateSubstr();
                //fprintf(stdout, "MOVE LF@$VAR%p TF@RETVAL\n",list->First->Instruction.addr1);
                if (0) {}
                char* retezec = list->First->Instruction.addr1;
                int* pozicec = list->First->Instruction.addr2;
                int* pozicexd = list->First->Instruction.addr3;
                /*fprintf(stdout, "PUSHS string@%s\n", retezec);
                fprintf(stdout, "PUSHS int@%d\n", *pozicec);
                fprintf(stdout, "PUSHS int@%d\n", *pozicexd);*/
                fprintf(stdout, "CALL $SUBSTR\n");
                break;
            case(I_ORD):
                //generateOrd();
                //fprintf(stdout, "MOVE LF@$VAR%p TF@RETVAL\n",list->First->Instruction.addr1);
                if (0) {}
                char* retez = list->First->Instruction.addr1;
                int* pozice = list->First->Instruction.addr2;
                //fprintf(stdout, "PUSHS string@%s\n", retez);
                //fprintf(stdout, "PUSHS int@%d\n", *pozice);
                fprintf(stdout, "CALL $ORD\n");

                break;
            case(I_CHR):
                //generateChr();
                //fprintf(stdout, "MOVE LF@$VAR%p TF@RETVAL\n",list->First->Instruction.addr1);
                if (0) {}
                int* chr = list->First->Instruction.addr1;
                //fprintf(stdout, "PUSHS int@%d\n", *chr);
                fprintf(stdout, "CALL $CHAR\n");
                break;
            case(I_PRINT):
                parCounter = list->First->Instruction.addr2;  ////////////////////////dohodni sa s jindrom
                if(parCounter == 0) break;
                symtableItem *tmpItem = list->First->Instruction.addr3;
                /*for (int i = 0; i < *parCounter - 1; i++){
                  tmpItem++;
                }*/
                /*for (int i = 0; i < *parCounter; i++){
                    if(tmpItem->key != NULL){
                      fprintf(stdout, "PUSHS GF@$VAR%s\n", tmpItem->key);
                    }
                    else if (tmpItem->elementType.variable->type == DATA_INT) {
                      fprintf(stdout, "PUSHS int@%d\n", tmpItem->elementType.variable->value.INT);
                    }
                    else if(tmpItem->elementType.variable->type == DATA_STRING){
                      fprintf(stdout, "PUSHS string@%s\n", tmpItem->elementType.variable->value.string);
                    }
                    else if(tmpItem->elementType.variable->type == DATA_FLOAT){
                      fprintf(stdout, "PUSHS float@%a\n", tmpItem->elementType.variable->value.FLOAT);
                    }
                    tmpItem--;
                    //tmpItem++;
                }
                */
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
                else if(*tmp == LITERAL){
                  char* tmp8 = list->First->Instruction.addr1;
                  fprintf(stdout, "PUSHS string@%s\n", tmp8);
                }
                else if(*tmp == FLOAT){
                  double* tmp8 = list->First->Instruction.addr1;
                  fprintf(stdout, "PUSHS float@%a\n", *tmp8);
                }
                break;
            case(I_ADDS):
                /*fprintf(stdout, "CREATEFRAME\n");
                fprintf(stdout, "PUSHFRAME\n");
                fprintf(stdout, "DEFVAR LF@$1\n");
                fprintf(stdout, "DEFVAR LF@$2\n");
                fprintf(stdout, "DEFVAR LF@$VAL1\n");
                fprintf(stdout, "DEFVAR LF@$VAL2\n");
                fprintf(stdout, "DEFVAR LF@$REESULT\n");
                fprintf(stdout, "POPS LF@$VAL2\n");//delitel
                fprintf(stdout, "POPS LF@$VAL1\n");//delenec
                //checkInt2Float();
                fprintf(stdout, "MOVE LF@$1 LF@$VAL1\n");
                fprintf(stdout, "MOVE LF@$2 LF@$VAL2\n");
                checkInt2Float();
                fprintf(stdout, "ADD LF@$REESULT LF@$1 LF@$2\n");
                fprintf(stdout, "PUSHS LF@$REESULT\n");
                fprintf(stdout, "POPFRAME\n");*/
                fprintf(stdout, "ADDS\n"); //tmp
                break;
            case(I_SUBS):
                fprintf(stdout, "SUBS\n");
                break;
            case(I_MULS):
                fprintf(stdout, "MULS\n");
                break;
            case(I_DIVS):
              /*  fprintf(stdout, "CREATEFRAME\n");
                fprintf(stdout, "PUSHFRAME\n");
                fprintf(stdout, "DEFVAR LF@$1\n");
                fprintf(stdout, "DEFVAR LF@$2\n");
                fprintf(stdout, "DEFVAR LF@$VAL1\n");
                fprintf(stdout, "DEFVAR LF@$VAL2\n");
                fprintf(stdout, "DEFVAR LF@$REESULT\n");
                fprintf(stdout, "POPS LF@$VAL2\n");//delitel
                fprintf(stdout, "POPS LF@$VAL1\n");//delenec
                //checkInt2Float();
                fprintf(stdout, "MOVE LF@$1 LF@$VAL1\n");
                fprintf(stdout, "MOVE LF@$2 LF@$VAL2\n");
                checkInt2Float();

                fprintf(stdout, "JUMPIFEQ $ZEROERROR float@%a LF@$2\n", 0.0);
                fprintf(stdout, "DIV LF@$REESULT LF@$1 LF@$2\n");

                fprintf(stdout, "PUSHS LF@$REESULT\n");
                fprintf(stdout, "POPFRAME\n");*/
                break;
            case(I_IDIVS):
                fprintf(stdout, "CREATEFRAME\n");
                fprintf(stdout, "PUSHFRAME\n");
                fprintf(stdout, "DEFVAR LF@$1\n");
                fprintf(stdout, "DEFVAR LF@$2\n");
                fprintf(stdout, "DEFVAR LF@$VAL1\n");
                fprintf(stdout, "DEFVAR LF@$VAL2\n");
                fprintf(stdout, "DEFVAR LF@$REESULT\n");
                fprintf(stdout, "POPS LF@$VAL2\n");//delitel
                fprintf(stdout, "POPS LF@$VAL1\n");//delenec
                //checkInt2Float();
                fprintf(stdout, "MOVE LF@$1 LF@$VAL1\n");
                fprintf(stdout, "MOVE LF@$2 LF@$VAL2\n");
                //checkFloat2Int(); tmp

                fprintf(stdout, "JUMPIFEQ $ZEROERROR int@%d LF@$2\n", 0);
//                fprintf(stdout, "DIV LF@$REESULT LF@$1 LF@$2\n");
                fprintf(stdout, "IDIV LF@$REESULT LF@$VAL1 LF@$VAL2\n");

                fprintf(stdout, "PUSHS LF@$REESULT\n");
                fprintf(stdout, "POPFRAME\n");
                break;
            case(I_DEFVAR):
                if(0){}
                symtableItem *ree = list->First->Instruction.addr1;
                fprintf(stdout, "DEFVAR GF@$VAR%s\n", ree->key);
                break;
            case(I_POPS):
                if(0){}
                symtableItem *reee = list->First->Instruction.addr1;
                fprintf(stdout, "POPS GF@$VAR%s\n", reee->key);
                break;
            case(I_GTS): //greater
                fprintf(stdout, "GTS \n");
                break;
            case(I_LTS): //less
                fprintf(stdout, "LTS \n");
                break;
            case(I_EQS):
                fprintf(stdout, "EQS \n");
                break;
            case(I_NQS):
                fprintf(stdout, "EQS\n");
                fprintf(stdout, "NOTS\n");
                break;
            case(I_ELSE_E):
                return 1488;
            case(I_GT)://greater or equal
                fprintf(stdout, "CREATEFRAME\n");
                fprintf(stdout, "PUSHFRAME\n");
                fprintf(stdout, "DEFVAR LF@$RETVAL\n");
                fprintf(stdout, "DEFVAR LF@$1\n");
                fprintf(stdout, "POPS LF@$1\n");
                fprintf(stdout, "DEFVAR LF@$2\n");
                fprintf(stdout, "POPS LF@$2\n");
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

                fprintf(stdout, "CONCAT LF@$RETVAL LF@$STR2 LF@$STR1\n");
                fprintf(stdout, "PUSHS LF@$RETVAL\n");

                fprintf(stdout, "POPFRAME\n");
                break;
        }
    }
}

//converts int to float if needed
void checkInt2Float(){

  fprintf(stdout, "DEFVAR LF@$T1\n");
  fprintf(stdout, "DEFVAR LF@$T2\n");

  fprintf(stdout, "TYPE LF@$T1 LF@$1\n");
  fprintf(stdout, "TYPE LF@$T2 LF@$2\n");

  fprintf(stdout, "DEFVAR LF@$BOOLCHECK0\n");
  fprintf(stdout, "DEFVAR LF@$BOOLCHECK1\n");
  fprintf(stdout, "DEFVAR LF@$BOOLCHECK2\n");

  fprintf(stdout, "EQ LF@$BOOLCHECK0 LF@$T1 string@int\n");
  fprintf(stdout, "EQ LF@$BOOLCHECK1 LF@$T1 string@int\n");
  fprintf(stdout, "AND LF@$BOOLCHECK2 LF@$BOOLCHECK0 LF@$BOOLCHECK1\n");
  fprintf(stdout, "JUMPIFEQ $LAB2 LF@$BOOLCHEK2 string@true\n");

  fprintf(stdout, "LABEL $LAB0\n");
  fprintf(stdout, "JUMPIFEQ $LAB1 LF@$T1 string@float\n");
  fprintf(stdout, "INT2FLOAT LF@$1 LF@$1\n");
  fprintf(stdout, "LABEL $LAB1\n");
  fprintf(stdout, "JUMPIFEQ $LAB2 LF@$T2 string@float\n");
  fprintf(stdout, "INT2FLOAT LF@$2 LF@$2\n");
  fprintf(stdout, "LABEL $LAB2\n");

}

//converts float to int if needed
void checkFloat2Int(){

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

}


/*void generateEquality(tDLList *list){
  fprintf(stdout, "PUSHFRAME\n");
  fprintf(stdout, "DEFVAR LF@$RETVAL\n",list->First->Instruction.addr1);
  fprintf(stdout, "DEFVAR LF@$EQ1%p\n",list->First->Instruction.addr1);
  fprintf(stdout, "DEFVAR LF@$EQ2%p\n",list->First->Instruction.addr2);
  fprintf(stdout, "DEFVAR LF@$EQ3%p\n",list->First->Instruction.addr3);
  fprintf(stdout, "MOVE LF@$EQ1%p ???\n",list->First->Instruction.addr1);
  fprintf(stdout, "MOVE LF@$EQ2%p ???\n",list->First->Instruction.addr2);
  fprintf(stdout, "MOVE LF@$EQ3%p string@%s\n",list->First->Instruction.addr3);

  fprintf(stdout, "JUMPIFEQ $EQ_LESSER%p LF@$EQ3%p string@<\n",list->First->Instruction.addr3, list->First->Instruction.addr3);
  fprintf(stdout, "JUMPIFEQ $EQ_GREATER%p LF@$EQ3%p string@>\n",list->First->Instruction.addr3, list->First->Instruction.addr3);
  fprintf(stdout, "JUMPIFEQ $EQ_LSREQ%p LF@$EQ3%p string@<=\n",list->First->Instruction.addr3, list->First->Instruction.addr3);
  fprintf(stdout, "JUMPIFEQ $EQ_GTREQ%p LF@$EQ3%p string@>=\n",list->First->Instruction.addr3, list->First->Instruction.addr3);
  fprintf(stdout, "JUMPIFEQ $EQ_EQ%p LF@$EQ3%p string@==\n",list->First->Instruction.addr3, list->First->Instruction.addr3);
  fprintf(stdout, "JUMPIFEQ $EQ_NOTEQ%p LF@$EQ3%p string@!=\n",list->First->Instruction.addr3, list->First->Instruction.addr3);

  fprintf(stdout, "LABEL $EQ_LESSER%p\n",list->First->Instruction.addr3);
  fprintf(stdout, "LABEL $EQ_GREATER%p\n",list->First->Instruction.addr3);
  fprintf(stdout, "LABEL $EQ_LSREQ%p\n",list->First->Instruction.addr3);
  fprintf(stdout, "LABEL $EQ_GTREQ%p\n",list->First->Instruction.addr3);
  fprintf(stdout, "LABEL $EQ_EQ%p\n",list->First->Instruction.addr3);
  fprintf(stdout, "LABEL $EQ_NOTEQ%p\n",list->First->Instruction.addr3);

  fprintf(stdout, "DEFVAR LF@$EQ1%p\n",list->First->Instruction.addr1);
  fprintf(stdout, "DEFVAR LF@$EQ2%p\n",list->First->Instruction.addr2);

}*/

/*
void generateAdd(tDLList*list){
  printf("MOVE GF@$VAR%p\n",list->First->Instruction.addr1);
}*/


///na IF potrebujem nejako vediet indent že dokedy vykonavat funckie vo vnutri, alebo
///potrebujem vediet prechody, teda Zaciatok IF , ELSE , Koniec IF a to niekde pocitat v pripade vnutornych IFov
///nasledne mozem volat generateInstruction rekurzivne...? Asi ano..
void generateIf(tDLList*list, void *origi){

  fprintf(stdout, "CREATEFRAME\n");
  fprintf(stdout, "PUSHFRAME\n");
  //zistit condition
  list->First = list->First->rptr;
  generateInstructionREE(list);
  fprintf(stdout, "DEFVAR LF@$COND%p\n", origi);
  fprintf(stdout, "POPS LF@$COND%p\n", origi);

  //fprintf(stdout, "JUMPIFNEQ WHILE$END$%p bool@true LF@$COND%p\n", origi, origi);
  fprintf(stdout, "JUMPIFNEQ ELSE%p bool@true LF@$COND%p\n",origi, origi);
  list->First = list->First->rptr;
  generateInstructionREE(list); //dokym nenajdem rovnaky indent
  fprintf(stdout, "JUMP END%p\n",origi);
  fprintf(stdout, "LABEL ELSE%p\n",origi);
  list->First = list->First->rptr;
  generateInstructionREE(list); //dokym nenajdem rovnaky indent
  fprintf(stdout, "LABEL END%p\n",origi);
  fprintf(stdout, "POPFRAME\n");
}

void generateWhile(tDLList*list, void *origi){

  fprintf(stdout, "CREATEFRAME\n");
  fprintf(stdout, "PUSHFRAME\n");
  fprintf(stdout, "DEFVAR LF@$COND%p\n", origi);

  tDLElemPtr jmp2 = list->First->rptr;

  list->First = list->First->rptr;
  generateInstructionREE(list);


  fprintf(stdout, "LABEL WHILE$BEGIN$%p\n", origi);
  fprintf(stdout, "POPS LF@$COND%p\n", origi);
  fprintf(stdout, "JUMPIFNEQ WHILE$END$%p bool@true LF@$COND%p\n", origi, origi);

  list->First = list->First->rptr;
  generateInstructionREE(list); //dokym nenajdem dedent

  tDLElemPtr jmp1 = list->First;
  list->First = jmp2;
  generateInstructionREE(list); //dokym nenajdem konec expression(podminky)
  list->First = jmp1;

  fprintf(stdout, "JUMP WHILE$BEGIN$%p\n", origi);
  fprintf(stdout, "LABEL WHILE$END$%p\n", origi);

  fprintf(stdout, "POPFRAME\n");

  //fprintf(stdout, "POPFRAME\n");
  //fprintf(stdout, "CREATEFRAME\n"); ////pre nove var memecka

  /*for (int i = 0; i < count; i++) { ///daky loop na vars
    printf("DEFVAR TF@$VAR%p\n",dake id meme);
    printf("MOVE TF@$VAR%p LF@$VAR%p\n",cez dake to id, vsetky by mali byt rovnake);
  }*/
  //fprintf(stdout, "PUSHFRAME\n");

  //tu dako zistit condition do loopu
  //XXXX


  /*
  int whileCounter = 0;
  fprintf(stdout, "DEFVAR LF@$COND%p_%d\n", origi, ++whileCounter);
  fprintf(stdout, "POPS LF@$COND%p_%d\n", origi, whileCounter);
  */


  //fprintf(stdout, "POPFRAME\n");
  //Pred jumpom vratit vars ako premenne
  /*for (int i = 0; i < count; i++) { ///daky loop na vars
    printf("MOVE LF@$VAR%p TF@$VAR%p\n",cez dake to id, vsetky by mali byt rovnake);
  }*/
  //
}


//bug .. print ma vracet None
