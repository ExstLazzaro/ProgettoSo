#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "fake_os.h"

void FakeOS_init(FakeOS* os) {
  os->running=0;
  List_init(&os->ready);
  List_init(&os->waiting);
  List_init(&os->processes);
  os->timer=0;
  os->schedule_fn=0;
} //ALL'INIZIO TUTTO VUOTO

void FakeOS_createProcess(FakeOS* os, FakeProcess* p) { //CREO PROCESSO P CON IL SUO PUNTATORE.
  // sanity check
  assert(p->arrival_time==os->timer && "time mismatch in creation");
  // we check that in the list of PCBs there is no
  // pcb having the same pid
  assert( (!os->running || os->running->pid!=p->pid) && "pid taken");

  ListItem* aux=os->ready.first; //PRENDO PRIMO PROCESSO READY
  while(aux){ //SCORRO FINO A CHE NON ARRIVO ALLA FINE DELLA LISTA READY
    FakePCB* pcb=(FakePCB*)aux;
    assert(pcb->pid!=p->pid && "pid taken"); //CONTROLLO CHE NON CI SIA UN PROCESSO CON STESSO PID DI P.
    aux=aux->next;
  }

  aux=os->waiting.first;
  while(aux){
    FakePCB* pcb=(FakePCB*)aux;
    assert(pcb->pid!=p->pid && "pid taken"); //CONTROLLO UGUALE PER WAITING LIST
    aux=aux->next;
  }

  // all fine, no such pcb exists.CREO NUOVO PCB
  FakePCB* new_pcb=(FakePCB*) malloc(sizeof(FakePCB));
  new_pcb->list.next=new_pcb->list.prev=0; //ALL'INIZIO NON IN LISTA
  new_pcb->pid=p->pid;
  new_pcb->events=p->events;

  assert(new_pcb->events.first && "process without events");

  // depending on the type of the first event
  // we put the process either in ready or in waiting
  ProcessEvent* e=(ProcessEvent*)new_pcb->events.first;
  switch(e->type){
  case CPU:
    List_pushBack(&os->ready, (ListItem*) new_pcb); //SE PRIMO EVENTO È CPU LO METTO IN READY. IN FONDO.
    break;
  case IO:
    List_pushBack(&os->waiting, (ListItem*) new_pcb); //SE IO LO METTO IN WAITING. IN FONDO.
    break;
  default:
    assert(0 && "illegal resource");
    ;
  }
}




void FakeOS_simStep(FakeOS* os){
  
  printf("************** TIME: %08d **************\n", os->timer);

  //scan process waiting to be started
  //and create all processes starting now
  ListItem* aux=os->processes.first;
  while (aux){
    FakeProcess* proc=(FakeProcess*)aux;
    FakeProcess* new_process=0;
    if (proc->arrival_time==os->timer){
      new_process=proc;
    }//SE BECCO UN PROCESSO CHE È NEL SUO ARRIVAL TIME LO METTO IN LISTA
    aux=aux->next;
    if (new_process) { //QUI LO METTO (CON CREATEPROCESS)
      printf("\tcreate pid:%d\n", new_process->pid);
      new_process=(FakeProcess*)List_detach(&os->processes, (ListItem*)new_process);
      FakeOS_createProcess(os, new_process);
      free(new_process);
    }
  } //FINCHE CE UN PROCESSO DA SCANNERIZZARE CONTROLLO

  // scan waiting list, and put in ready all items whose event terminates
  aux=os->waiting.first; //VADO A PRENDERE CHI DEVE FARE IO IN WAITING LIST
  while(aux) {
    FakePCB* pcb=(FakePCB*)aux; 
    aux=aux->next; //PRENDO PROCESS CONTROL BLOCK DEL MIO AUX POI PUNTO AL NEXT (QUANDO SARA NULL FINIRÀ CICLO)
    ProcessEvent* e=(ProcessEvent*) pcb->events.first;
    printf("\twaiting pid: %d\n", pcb->pid);
    assert(e->type==IO);
    e->duration--; //DECREMENTO DURATA  BURST IO DI UNO
    printf("\t\tremaining time:%d\n",e->duration);
    if (e->duration==0){//SE HO FINITO, POPPO L'EVENTO E LO LIBERO
      printf("\t\tend burst\n");
      List_popFront(&pcb->events);
      free(e);
      List_detach(&os->waiting, (ListItem*)pcb);
      if (! pcb->events.first) {
        // KILLO PROCESSO SE ERA ULTIMO IO BURST
        printf("\t\tend process\n");
        free(pcb); 
      } else { //SENNÒ
        //handle next event
        e=(ProcessEvent*) pcb->events.first;
        switch (e->type){
        case CPU:
          printf("\t\tmove to ready\n");
          List_pushBack(&os->ready, (ListItem*) pcb); //SE PROX SARÀ BURST CPU LO RIMETTO ALLA FINE DI READY
          break;
        case IO:
          printf("\t\tmove to waiting\n");
          List_pushBack(&os->waiting, (ListItem*) pcb); //SE PROX SARÀ BURST IO LO RIMETTO ALLA FINE DI WAITING
          break;
        }
      }
    }
  }

  

  // decrement the duration of running
  // if event over, destroy event
  // and reschedule process
  // if last event, destroy running
  FakePCB* running=os->running;
  printf("\trunning pid: %d\n", running?running->pid:-1);
  if (running) {
    ProcessEvent* e=(ProcessEvent*) running->events.first;
    assert(e->type==CPU);
    e->duration--; //DECREMENTO DURATA BURST CPU DI UNO
    printf("\t\tremaining time:%d\n",e->duration);
    if (e->duration==0){
      printf("\t\tend burst\n");
      List_popFront(&running->events);
      free(e);
      if (! running->events.first) {
        printf("\t\tend process\n");
        free(running); // SE ULTIMO EVENTO KILLO
      } else {
        e=(ProcessEvent*) running->events.first;
        switch (e->type){
        case CPU:
          printf("\t\tmove to ready\n");
          List_pushBack(&os->ready, (ListItem*) running); //SE PROX SARÀ BURST CPU LO RIMETTO ALLA FINE DI READY
          break;
        case IO:
          printf("\t\tmove to waiting\n");
          List_pushBack(&os->waiting, (ListItem*) running); //SE PROX SARÀ BURST IO LO RIMETTO ALLA FINE DI WAITING
          break;
        }
      }
      os->running = 0;
    }
  }


  // call schedule, if defined
  if (os->schedule_fn && ! os->running){
    (*os->schedule_fn)(os, os->schedule_args); 
  }

  // if running not defined and ready queue not empty
  // put the first in ready to run
  if (! os->running && os->ready.first) {
    os->running=(FakePCB*) List_popFront(&os->ready);
  }

  ++os->timer;

}

void FakeOS_destroy(FakeOS* os) {
}
