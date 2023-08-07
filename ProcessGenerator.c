#include "Pr.h"
#include <stdlib.h>
#include <stdio.h>
#define MAX_BURST 100

void List_inizializzazione(ListaBurst* list) {
list->first = 0;
list->last = 0;
list->size = 0;

}

ElementoBurst* List_insert(ListaBurst* head, ElementoBurst* prev, ElementoBurst* item) {
  if (item->next || item->prev)
    return 0;


  ElementoBurst* next= prev ? prev->next : head->first;
  if (prev) {
    item->prev=prev;
    prev->next=item;
  }
  if (next) {
    item->next=next;
    next->prev=item;
  }
  if (!prev)
    head->first=item;
  if(!next)
    head->last=item;
  ++head->size;
  return item;
}


ElementoBurst* List_pushBack(ListaBurst* head, ElementoBurst* item) {
  return List_insert(head, head->last, item);
};

ElementoBurst* List_find(ListaBurst* head, int dato) {
  // linear scanning of list
  ElementoBurst* aux=head->first;
  while(aux){
    if (aux->durata==dato)
      return aux;
    aux=aux->next;
  }
  return NULL;
}

void List_print(ListaBurst* head) {
  // linear scanning of list
  ElementoBurst* aux=head->first;
  while(aux){
    printf("Burst che ha Durata %d e Occorrenza %d\n",aux->durata,aux->occorrenza);
    aux=aux->next;
  }
  printf("\n---------------------------------\n");
  
}



void fetching(int listacpu[],int listaio[],int * ncpus,int * nio) {


int k = 0;
int y = 0;
FILE* f=fopen("Dati.txt", "r");
  if (! f)
    return ;
  // read the PID
  char *buffer=NULL;
  size_t line_length=0;

  while (getline(&buffer, &line_length, f) >0){ //PRENDO LA RIGA OGNI VOLTA
  //printf("\n%d %d \n",k,y);
    int num_tokens=0;
    int duration = -1;

    num_tokens =sscanf(buffer, "CPU %d", &duration);
    if (num_tokens==1){
      // CREO UN EVENTO DI TIPO CPU BURST
      listacpu[k] = duration;
      k++;
    }//LEGGO CPU BURST


    num_tokens=sscanf(buffer, "IO %d", &duration);
    if (num_tokens==1){
        listaio[y] = duration;
        y++;
    }//LEGGO IO BURST

     num_tokens=sscanf(buffer, "NCPU %d", &duration);
    if (num_tokens==1){
        *ncpus = duration;
    }//LEGGO IO BURST

     num_tokens=sscanf(buffer, "NIO %d", &duration);
    if (num_tokens==1){
        *nio =duration;
    }//LEGGO IO BURST



  }
  if (buffer)
    free(buffer);
  fclose(f);
}

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

void order(int lista[],int n) {
qsort(lista, n, sizeof(int),cmpfunc);
}


int main() {
printf("----------------------\n");
int * ncpus = (int *) malloc(sizeof(int));
int * nio = (int *) malloc(sizeof(int));

int listacpu[MAX_BURST];
int listaio[MAX_BURST];

fetching(listacpu,listaio,ncpus,nio);
/*printf("\nCOSA HO PRESO----------------------\n");
for(int i = 0;i< *ncpus;i++) {
printf("CPU %d\n",listacpu[i]);
printf("IO %d\n",listaio[i]);} */
order(listacpu,*ncpus);
order(listaio,*nio);
/*
printf("\n\nECCO ORDINATO------------------\n\n");
for(int i = 0;i< *ncpus;i++) {
printf("CPU %d\n",listacpu[i]);
printf("IO %d\n",listaio[i]);
} */

ListaBurst * cpubursts = (ListaBurst *) malloc(sizeof(ListaBurst));
ListaBurst * iobursts = (ListaBurst *) malloc(sizeof(ListaBurst));
List_inizializzazione(cpubursts);
List_inizializzazione(iobursts);





for(int i = 0;i < *ncpus;i++ ) {
ElementoBurst * aux = List_find(cpubursts,listacpu[i]);
if(aux) {
aux->occorrenza++;
}
else {
ElementoBurst * new = (ElementoBurst *) malloc(sizeof(ElementoBurst));
new->next = 0;
new->prev = 0;
new->durata = listacpu[i];
new->occorrenza = 1;
List_pushBack(cpubursts,new);
}
}


for(int i = 0;i < *nio;i++ ) {
ElementoBurst * aux = List_find(iobursts,listaio[i]);
if(aux) {
aux->occorrenza++;
}
else {
ElementoBurst * new = (ElementoBurst *) malloc(sizeof(ElementoBurst));
new->next = 0;
new->prev = 0;
new->durata = listaio[i];
new->occorrenza = 1;
List_pushBack(iobursts,new);
}
}




printf("quanti cpu bursts %d \n",cpubursts->size);
printf("quanti io bursts %d \n",iobursts->size);
List_print(cpubursts);
List_print(iobursts);





return 1;
}