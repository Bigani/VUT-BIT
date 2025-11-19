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

void tokenStackDestruct(tokenStack* s) {
  if (s != NULL) {
    tsItem* current = s->top;
    tsItem* rm = NULL;
    while (current != NULL) {
      rm = current;
      current = current->next;
      free(rm->data);
      free(rm);
    }
    free(s);
    s = NULL;
  }
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
  tsItem* new_elem = (tsItem*) malloc(sizeof(struct sItem));
  if (new_elem == NULL) {
    //maloc ree
  }
  new_elem->data = data;
  new_elem->next = s->top;
  s->top = new_elem;
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
            newItem->data->token = sData->token;
            return;
          }
          prev = current;
          current = current->next;
        }
      }
}
