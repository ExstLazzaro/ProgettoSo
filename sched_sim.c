#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "fake_os.h"

FakeOS os;

typedef struct {
  int quantum;
  int cpus;
} SchedRRArgs; //STRUTTURA SCHEDULER

void schedRR(FakeOS* os, void* args_){
  SchedRRArgs* args=(SchedRRArgs*)args_; //HO OS E ARGOMENTI SCHEDULER (CIOE QUANTUM)

  // look for the first process in ready
  // if none, return
  if (! os->ready.first) {
    return; }
  int k = 0;
  
  k = os->running.size;
  
  while( ( os->ready.first ) && ( k < (args->cpus ) ) ) {

  FakePCB* pcb=(FakePCB*) List_popFront(&os->ready); //FA UN DETACH DEL PRIMO PROCESSO IN LISTA E LO RITORNA
  List_pushBack(&os->running,(ListItem*) pcb);
  //os->running=pcb;
  
  assert(pcb->events.first);
  ProcessEvent* e = (ProcessEvent*)pcb->events.first;
  assert(e->type==CPU);

  // look at the first event
  // if duration>quantum
  // push front in the list of event a CPU event of duration quantum
  // alter the duration of the old event subtracting quantum
  if (e->duration > args->quantum) {
    ProcessEvent* qe=(ProcessEvent*)malloc(sizeof(ProcessEvent));
    qe->list.prev=qe->list.next=0;
    qe->type=CPU;
    qe->duration=args->quantum;
    e->duration-=args->quantum;
    List_pushFront(&pcb->events, (ListItem*)qe);
  }
  k++;
  
   }
};

int main(int argc, char** argv) { //RICORDA ./PROGRAMMA ARG1 ARG2 ... (ARG[0] È SEMPRE NOME PROGRAMMA)
  FakeOS_init(&os);
  SchedRRArgs srr_args;
  srr_args.quantum=5;
  srr_args.cpus= atoi(argv[1]);
  os.schedule_args=&srr_args;
  os.schedule_fn=schedRR;
  
  for (int i=1; i<argc -1; ++i){
    FakeProcess new_process;
    int num_events=FakeProcess_load(&new_process, argv[i+1]);
    printf("loading [%s], pid: %d, events:%d",
           argv[i+1], new_process.pid, num_events);
    if (num_events) {
      FakeProcess* new_process_ptr=(FakeProcess*)malloc(sizeof(FakeProcess));
      *new_process_ptr=new_process;
      List_pushBack(&os.processes, (ListItem*)new_process_ptr);
    }
  }
  printf("\nnum processes in queue %d\n", os.processes.size);
  printf("\nNumero CPU : %d \n",atoi(argv[1]));
  while(os.running.first
        || os.ready.first
        || os.waiting.first
        || os.processes.first){
    FakeOS_simStep(&os); //FINCHE LISTE HANNO ELEMENTI CONTNUA CON QUESTA FUNZIONE
  }
}
