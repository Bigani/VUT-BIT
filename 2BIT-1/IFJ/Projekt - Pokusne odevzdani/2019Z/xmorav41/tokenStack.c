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
  s->Top = NULL;
}

void tokenStackDestruct(tokenStack* s) {
  if(s != NULL){
    TSItem* current = s->Top;
    TSItem* rm = NULL;
    while (current != NULL) {
      rm = current;
      current = current->Next;
      free(rm->Data);
      free(rm);
    }
    free(s);
    s = NULL;
  }
}

int tokenStackEmpty(const tokenStack* s) {
  return (s->Top == NULL);
}

SData* tokenStackTop(const tokenStack* s) {
  if(!tokenStackEmpty(s)) {
    return s->Top->Data;
  } else {
    return NULL;
  }
}

void tokenStackPop(tokenStack* s) {
  TSItem* rm = s->Top;
  s->Top = rm->Next;
  free(rm->Data);
  free(rm);
}

void tokenStackPush(tokenStack* s, SData* data) {
  TSItem* new_elem = (TSItem*) malloc(sizeof(struct SItem));
  if(new_elem == NULL){
    //maloc ree
  }
  new_elem->Data = data;
  new_elem->Next = s->Top;
  s->Top = new_elem;
}

SData* getTerminalData(tokenStack* s) {
  TSItem* current = s->Top;
  while (current != NULL) {
    if(current->Data->Type == type_token) {
      return current->Data;
    }
    current = current->Next;
  }
  return NULL;
}

void addHandler(tokenStack *s, SData * sData) {
  TSItem* current = s->Top;
  TSItem* prev = NULL;
  if (sData == NULL) {
    TSItem * newItem = malloc(sizeof(TSItem));
        if (newItem == NULL) {
          //malloc ree
        }
        newItem->Data = malloc(sizeof(SData));
        if(newItem->Data == NULL){
          //malloc ree
        }
        newItem->Data->Type = type_handler;
        newItem->Next = NULL;
        newItem->Data->Atr.Token = sData->Atr.Token;
        if (tokenStackEmpty(s)) {
          s->Top = newItem;
        } else {
          while (current->Next != NULL) {
            current = current->Next;
          }
          current->Next = newItem;
        }
      } else {
        while (current != NULL) {
          if (current->Data == sData) {
            TSItem * newItem = malloc(sizeof(TSItem));
            if (newItem == NULL) {
              //malloc ree
            }
            if (prev != NULL) {
              prev->Next = newItem;
            } else {
              s->Top = newItem;
            }
            newItem->Next = current;
            newItem->Data = malloc(sizeof(SData));
            if (newItem->Data == NULL) {
              //malloc ree
            }
            newItem->Data->Type = type_handler;
            newItem->Data->Atr.Token = sData->Atr.Token;
            return;
          }
          prev = current;
          current = current->Next;
        }
      }
}
