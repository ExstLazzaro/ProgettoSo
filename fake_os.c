#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "fake_os.h"

void FakeOS_init(FakeOS* os) {
  List_init(&os->ready); //***os->running=0
  List_init(&os->ready);
  List_init(&os->waiting);
  List_init(&os->processes);
  os->timer=0;
  os->schedule_fn=0;
  os->file=fopen("Dati.txt","w");
  if (! os->file) {
    return -1;}
  os->ncpuburst = 0;
  os->nioburst = 0;
} //ALL'INIZIO TUTTO VUOTO

void FakeOS_createProcess(FakeOS* os, FakeProcess* p) { //CREO PROCESSO P CON IL SUO PUNTATORE.
  // sanity check
  assert(p->arrival_time==os->timer && "time mismatch in creation");
  // we check that in the list of PCBs there is no
  // pcb having the same pid
  /* assert( (!os->running || os->running->pid!=p->pid) && "pid taken"); */

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
  
  float a = 0;
  SchedRRArgs* args=(SchedRRArgs*) os->schedule_args;
  a = args->a;
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
    printf("\twaiting pid: %d\n", pcb->pid); //--------------------CHI È IN WAIT
    assert(e->type==IO);
    e->eventimer++; 
    printf("\t\tremaining time:%d\n",(e->duration - e->eventimer)); //----------------QUANTO RIMANE DI EVENTO SPECIFICO (IN QUESTO CASO IO)
    if (e->duration==e->eventimer){//SE HO FINITO, POPPO L'EVENTO E LO LIBERO
       os->nioburst++;
      fprintf(os->file, "IO %d\n", e->eventimer);
      float quanto=e->fixatedquantum;
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
        e->quantum = quanto;
        e->fixatedquantum = quanto;
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
  aux=os->running.first;
  if(aux == NULL ) {
    printf("\trunning pid: -1\n");
  }
  while (aux) {           //-----------QUA CE IF E NON WHILE.. PERCHE DI RUNNING SOLO UNO PUO ESSERCI
    FakePCB* pcb=(FakePCB*)aux;
    aux=aux->next;
    printf("\trunning pid: %d\n", pcb->pid);
    ProcessEvent* e=(ProcessEvent*) pcb->events.first;
    assert(e->type==CPU);
    e->eventimer++;
    e->quantum--; //DECREMENTO DURATA BURST CPU DI UNO
    if((e->duration != e->eventimer) && (e->quantum <= 0)) {
    printf("\t\tBurst should be finished, but it is not,---> recalcolate quantum\n");
    }
    else if ((e->duration == e->eventimer) && (e->quantum <= 0)) {
    printf("\t\tBurst finished when predicted\n");

    }
    else {
    printf("\t\texpected remaining time:%f\n",e->quantum);} //----------------QUANTO RIMANE DI EVENTO SPECIFICO (IN QUESTO CASO CPU)
    if (e->duration==e->eventimer){
      os->ncpuburst++;
      fprintf(os->file, "CPU %d\n", e->eventimer);
      printf("\t\tend burst\n");
      if(e->quantum >= 1) {
      printf("\t\tBurst finished sooner than predicted\n");}
      float vecchioquanto=e->fixatedquantum;
      int vecchiadurata=e->duration; 
      List_popFront(&pcb->events);
      free(e);
      List_detach(&os->running, (ListItem*)pcb);
      if (! pcb->events.first) {
        printf("\t\tend process\n");
        free(pcb); // SE ULTIMO EVENTO KILLO
      } else {
        e=(ProcessEvent*) pcb->events.first;
        //printf("durata che è stata %d, quanto vecchio %f \n",vecchiadurata,e->quantum);
        e->quantum = (a * vecchiadurata) + ((1-a) * (vecchioquanto));
        //printf("processo numero %d ",pcb->pid);
        //printf("quanto vecchio %f, quanto nuovo %f \n",vecchioquanto,e->quantum);
        e->fixatedquantum = e->quantum;
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



  if (os->schedule_fn){
    (*os->schedule_fn)(os, os->schedule_args); 
  } 
  
  ++os->timer;

}

void FakeOS_destroy(FakeOS* os) {
}
