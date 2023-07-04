#include "fake_process.h"
#include "linked_list.h"
#pragma once


typedef struct {
  ListItem list;
  int pid;
  ListHead events;
} FakePCB; //COME REALIZZO PCB

struct FakeOS;
typedef void (*ScheduleFn)(struct FakeOS* os, void* args);

typedef struct FakeOS{
  ListHead running; //***FakePCB* running
  ListHead ready;
  ListHead waiting;
  int timer;
  ScheduleFn schedule_fn;
  void* schedule_args;

  ListHead processes;
} FakeOS; //STRUTTURA FINTO OS : HO PUNTATORE A PCB DEL RUNNING, LISTA READY E WAITING, UN TIMER, UNO SCHEDULER CON PUNTATORE AGLI ARGOMENTI. 
//LISTA PROCESSI ANCHE.


void FakeOS_init(FakeOS* os);
void FakeOS_simStep(FakeOS* os);
void FakeOS_destroy(FakeOS* os);
