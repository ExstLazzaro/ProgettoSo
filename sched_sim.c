#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "fake_os.h"

FakeOS os;

FakePCB * findmaxmin(int m,ListHead* head) {
  
ListItem* aux=head->first;
ListItem* auxx= head->first;
int k;
if(m == 1) {
 k = -9999;

}
else {
  k= 9999;
}
  while(aux){
    
    FakePCB* pcb=(FakePCB*) aux;
    ProcessEvent* e = (ProcessEvent*)pcb->events.first;
    //printf("di %d con %f quanto\n",((FakePCB*)aux)->pid,e->quantum);
    if(m == 0) {
      if ((e->quantum) < k) {
        k = e->quantum;
        auxx= aux;
      }
    }
    else {
      if ((e->quantum) > k) {
        k = e->quantum;
        auxx= aux;
      }
    }
    aux=aux->next;
  }

  return (FakePCB*)(List_find(head,auxx));
}


//SCHEDULER
void schedRR(FakeOS* os, void* args_){

  SchedRRArgs* args=(SchedRRArgs*)args_; //HO OS E ARGOMENTI SCHEDULER (CIOE QUANTUM)
  
  int k = 0;
  k = os->running.size;
  
  for(int i=0; i < k; i++) { //CONTROLLO SE QUANTO ESAUIRITO

    FakePCB* pcb=(FakePCB*) List_popFront(&os->running);
    ProcessEvent* e = (ProcessEvent*)pcb->events.first;
    List_pushBack(&os->running,(ListItem*) pcb);
   
  if (e->quantum <= 0) {
    printf("Old quantum %f\n",e->fixatedquantum);
    e->quantum = ((args->a) * e->eventimer) + (((1) - args->a) * e->fixatedquantum);
    printf("New quantum %f\n",e->quantum);
    e->fixatedquantum += e->quantum;
    printf("Total of the prediction %f\n",e->fixatedquantum);
    } }
   

   if (! os->ready.first) {

    return; }
  

  //se non tutte occupate
  while(k < args->cpus && os->ready.first) {
   if(os->ready.size != 1) {
   FakePCB* pcb = findmaxmin(0,&os->ready);
   List_detach(&os->ready, (ListItem*) pcb);
   List_pushBack(&os->running,(ListItem*) pcb);
   k++;
   }
   else {
   FakePCB* pcb = (FakePCB*) List_popFront(&os->ready);
   List_pushFront(&os->running,(ListItem*) pcb);
   k++;
   }
  }

  //se tutte cpu occupate
  if(k == args->cpus && os->ready.first) {
  for(int i=0;i<os->ready.size;i++) {
  FakePCB* pcb1 = findmaxmin(0,&os->ready);
  ProcessEvent* e1 = (ProcessEvent*)pcb1->events.first;
  FakePCB* pcb2 = findmaxmin(1,&os->running);
  ProcessEvent* e2 = (ProcessEvent*)pcb2->events.first;
  if(e1->quantum < e2->quantum) {
  printf("Process PID:%d of ready swapped with Process PID:%d of running \n",pcb1->pid,pcb2->pid);
  List_detach(&os->ready, (ListItem*) pcb1);
  List_detach(&os->running, (ListItem*) pcb2);
  List_pushBack(&os->running,(ListItem*) pcb1);
  List_pushBack(&os->ready,(ListItem*) pcb2);
  }
  else {
    //printf("Nothing else to swap \n");
    break;

  } 
  } }
 
   
};

int main(int argc, char** argv) { //PRIMO ARGOMENTO CORES,SECONDO VARIABILE a
  FakeOS_init(&os);
  SchedRRArgs srr_args;
  srr_args.a=atof(argv[2]);
  srr_args.cpus= atoi(argv[1]);
  os.schedule_args=&srr_args;
  os.schedule_fn=schedRR;
  
  for (int i=1; i<argc -2; ++i){
    FakeProcess new_process;
    int num_events=FakeProcess_load(&new_process, argv[i+2]);
    printf("loading [%s], pid: %d, events:%d",
           argv[i+2], new_process.pid, num_events);
    if (num_events) {
      FakeProcess* new_process_ptr=(FakeProcess*)malloc(sizeof(FakeProcess));
      *new_process_ptr=new_process;
      List_pushBack(&os.processes, (ListItem*)new_process_ptr);
    }
  }
  printf("----------\nnum processes in queue %d\n----------", os.processes.size);
  printf("----------\nNumero CPU : %d \n------------",atoi(argv[1]));
  printf("----------\nVariabile alfa : %f \n-----------",atof(argv[2]));
  while(os.running.first
        || os.ready.first
        || os.waiting.first
        || os.processes.first){
    FakeOS_simStep(&os); //FINCHE LISTE HANNO ELEMENTI CONTNUA CON QUESTA FUNZIONE
  }
}
