/**
  * strings.h
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

  #ifndef _STRING_H_
  #define _STRING_H_

typedef struct {
  char *string;
  int length;
  int allocatedSize;
} smartString;

void stringInit(smartString *str);
void stringFree(smartString *str);
void stringClear(smartString *str);
int stringAddChar(smartString *str, char c);
int stringAddString(smartString *str, char* c);
int stringIsKeyword(smartString *str);

#endif
