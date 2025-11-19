/**
  * tokenStack.c
  * Project IFJ19
  *
  * Březina Jindřich (xbrezi21@stud.fit.vutbr.cz)
  * Gumančík Pavol (xguman01@stud.fit.vutbr.cz)
  * Kotáb Dominik (xkotab01@stud.fit.vutbr.cz)
  * Moravčík Tomáš (xmorav41@stud.fit.vutbr.cz)
  *
  * Brief: stack for tokens
  *
  */

#include "tokenStack.h"
#include <stdlib.h>
#include <stdio.h>

void tokenStackInit(tokenStack* s) {
  s->top = NULL;
}

int tokenStackEmpty(const tokenStack* s) {
  return (s->top == NULL);
}

sData* tokenStackTop(const tokenStack* s) {
  if (!tokenStackEmpty(s)) {
    return s->top->data;
  } else {
    return NULL;
  }
}

void tokenStackPop(tokenStack* s) {
  tsItem* rm = s->top;
  s->top = rm->next;
  free(rm->data);
  free(rm);
}

void tokenStackPush(tokenStack* s, sData* data) {
  tsItem* newElem = (tsItem*) malloc(sizeof(struct sItem));
  if (newElem == NULL) {
    //maloc ree
  }
  newElem->data = data;
  newElem->next = s->top;
  s->top = newElem;
}

sData* getTerminalData(tokenStack* s) {
  tsItem* current = s->top;
  while (current != NULL) {
    if (current->data->type == typeToken) {
      return current->data;
    }
    current = current->next;
  }
  return NULL;
}

void addHandler(tokenStack *s, sData* sData) {
  tsItem* current = s->top;
  tsItem* prev = NULL;
  if (sData == NULL) {
    tsItem * newItem = malloc(sizeof(tsItem));
      if (newItem == NULL) {
        //malloc ree
      }
      newItem->data = malloc(sizeof(sData));
      if (newItem->data == NULL) {
        //malloc ree
      }
      newItem->data->type = typeHandler;
      newItem->next = NULL;
      newItem->data->token = sData->token;
      if (tokenStackEmpty(s)) {
        s->top = newItem;
      } else {
        while (current->next != NULL) {
          current = current->next;
        }
        current->next = newItem;
      }
    } else {
      while (current != NULL) {
        if (current->data == sData) {
          tsItem * newItem = malloc(sizeof(tsItem));
          if (newItem == NULL) {
            //malloc ree
          }
          if (prev != NULL) {
            prev->next = newItem;
          } else {
            s->top = newItem;
          }
          newItem->next = current;
          newItem->data = malloc(sizeof(sData));
          if (newItem->data == NULL) {
            //malloc ree
          }
          newItem->data->type = typeHandler;
          return;
        }
        prev = current;
        current = current->next;
      }
    }
}
