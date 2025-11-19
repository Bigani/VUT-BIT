/**
  * strings.c
  * Project IFJ19
  *
  * Březina Jindřich (xbrezi21@stud.fit.vutbr.cz)
  * Gumančík Pavol (xguman01@stud.fit.vutbr.cz)
  * Kotáb Dominik (xkotab01@stud.fit.vutbr.cz)
  * Moravčík Tomáš (xmorav41@stud.fit.vutbr.cz)
  *
  * Brief: operations for strings
  *
  */

#define ALLOC_ADD 10

#include "error.h"
#include "strings.h"
#include <stddef.h>
#include <malloc.h>
#include <string.h>

void stringInit(smartString *str) {
  str->string = NULL;
  str->length = str->allocatedSize = 0;
}

void stringFree(smartString *str) {
  free(str->string);
  str->length = str->allocatedSize = 0;
}

void stringClear(smartString *str) {
  for(int i = 0; i < str->length; i++) {
    str->string[i] = '\0';
  }
  str->length = 0;
}

int stringAddChar(smartString *str, char c) {
  if (str->length + 1 >= str->allocatedSize) {
    if ((str->string = (char *) realloc(str->string, str->allocatedSize + ALLOC_ADD)) == NULL) {
      return INTERN_ERR;
    }
    str->allocatedSize += ALLOC_ADD;
  }
  str->string[str->length] = c;
  if (c != '\0') {
    str->length++;
  }

  str->string[str->length] = '\0';

  return OK;
}

int stringAddString(smartString *str, char* c) {
  if (str->length == 0) {
    stringInit(str);
  }

  for (unsigned int i = 0; i < strlen(c); i++) {
    stringAddChar(str, c[i]);
  }
  //OwO

  return OK;
}

int stringIsKeyword(smartString *str) {
  char *keywords[] = {"def\0", "else\0", "if\0", "None\0", "pass\0", "return\0", "while\0"};
  for (unsigned int i = 0; i < (sizeof(keywords) / sizeof(keywords[0])); i++) {
    if (strcmp(keywords[i], str->string) == 0)
      return i; //ano je
  }
  return -1; //ne neni
}
