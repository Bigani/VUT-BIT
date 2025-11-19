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

  #include <stdio.h>
  #include <stdbool.h>
  #include <ctype.h>

  #include "symtable.h"
  #include "scanner.h"
  #include "expressions.h"

  void degenerate(tDLList *list){

    while(list->nextToken != NULL){
      fprintf(stdout, ".IFJcode19\n");
    }
  };

  //||======================================================||//
  //||=============== Věstavené funkce start ===============||//

  void generateInputs(){
    printf("JUMP _OMEGAJUMP%d\n", 5); //tmp
    printf("LABEL _OMEGALABEL%p\n", list->First->Instruction->addr1);
    printf("PUSHFRAME\n");

    //vytvor navratovou promennou
    printf("DEFVAR LF@_RTVL%p\n",list->First->Instruction->addr1);
    printf("MOVE LF@_RTVL%p nil@nil\n",list->First->Instruction->addr1);
    printf("PUSHS LF@_RTVL%p\n",list->First->Instruction->addr1);

    printf("READ LF@_RTVL%p string\n", (void*)list->First->Instruction->addr1);
    printf("PUSHS LF@_RTVL%p\n", (void*)list->First->Instruction->addr1);

    printf("POPS LF@_RTVL%p\n",list->First->Instruction->addr1);
  	printf("POPFRAME\n");

  	printf("RETURN\n");
  	printf("LABEL _OMEGAJUMP%d\n", 5); //tmp

  }

  void generateInputi(){

  }

  void generateInputf(){

  }

  void generatePrint(){

  }

  void generateLen(){

  }

  void generateSubstr(){

  }

  void generateOrd(){

  }

  void generateChr(){

  }

  //||=============== Věstavené funkce konec ===============||//
  //||======================================================||//
