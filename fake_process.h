#pragma once
#include "linked_list.h"

typedef enum {CPU=0, IO=1} ResourceType;

// event of a process, is in a list
typedef struct {
  ListItem list; //OGNI EVENTO È UN LISTITEM.
  ResourceType type;
  int duration;
  float quantum; //QUANTUM
  float fixatedquantum;
  int eventimer;
} ProcessEvent; //OGNI EVENTO HA DURATA E UNA RISORSA.

// fake process
typedef struct {
  ListItem list; //INDICO EREDITARIETÀ.UN PROCESSO È UN LISTITEM.
  int pid; // assigned by us
  int arrival_time;
  ListHead events;
} FakeProcess; //PER PROCESSO. HA PID, TEMPO ARRIVO,LISTA EVENTI (GUARDA PROCESSEVENT).

int FakeProcess_load(FakeProcess* p, const char* filename);

int FakeProcess_save(const FakeProcess* p, const char* filename);
