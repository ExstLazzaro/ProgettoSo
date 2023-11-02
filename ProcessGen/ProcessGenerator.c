#include "Pr.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
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



ElementoBurst* RandomChosen(ListaBurst* head, int dato) {
  // linear scanning of list
  float datof = (float) dato;
  ElementoBurst* aux=head->first;
  while(aux){
    if ( (datof >= aux->lowerbound) && (datof <= aux->upperbound) ) {
      return aux; }
    aux=aux->next;
  }
  return NULL;
}

void List_print(ListaBurst* head) {
  // linear scanning of list
  ElementoBurst* aux=head->first;
  while(aux){
    printf("> é presente un burst di durata %d e con occorrenza %d\n",aux->durata,aux->occorrenza);
    aux=aux->next;
  }
  printf("\n--------------------------------------------------\n");
  
}

void Setting_up(ListaBurst* head,int n) {
  //Preparo la struttura dati della lista dei burst, impostando le varie probabilità
  head->first->lowerbound = 0;
  head->last->upperbound = 100;
  ElementoBurst* aux=head->first;
  float prevb = 0;
  float preprob = 0;
  float q = (float)n;
  while(aux){
    aux->px = preprob + ((aux->occorrenza)/q );

    aux->lowerbound = prevb;
    if(aux->upperbound != 100) {
    aux->upperbound =  (aux->px * 100);

    }
    printf("La durata %d ha prob <X  %f e lower bound %f e upper bound %f\n",aux->durata,aux->px,aux->lowerbound,aux->upperbound);
    preprob = aux->px;
    prevb = aux->upperbound;
    aux = aux->next;
    
  }
  printf("\n--------------------------------------------------\n");
  
}



void fetching(int listacpu[],int listaio[],int * ncpus,int * nio) {


int k = 0;
int y = 0;
FILE* f=fopen("../Dati.txt", "r");
  if (! f)
    return ;
  // read the PID
  char *buffer=NULL;
  size_t line_length=0;

  while (getline(&buffer, &line_length, f) >0){ //PRENDO LA RIGA OGNI VOLTA
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
    }//Leggo il numero totale dei cpu burst alla fine

     num_tokens=sscanf(buffer, "NIO %d", &duration);
    if (num_tokens==1){
        *nio =duration;
    }//Leggo il numero totale dei io burst alla fine



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

void PlotFile(ListaBurst* head,int mode) {
  FILE *fp=NULL;
  if(mode == 1) {
  fp=fopen("graphcpu.txt","w"); }
  else {
  fp=fopen("graphio.txt","w");
  }
  fprintf(fp,"%d\t%f\n",0,(float)0);
  int i = 1;
 ElementoBurst* aux=head->first;
  while(aux){
    if(i <  aux->durata) {
      fprintf(fp,"%d\t%f\n",i,aux->px);

    }
    else if (i == aux->durata) {
      fprintf(fp,"%d\t%f\n",i,aux->px);
      aux = aux->next;


    }
    i++;


  }
  fprintf(fp,"%d\t%f\n",i,(float)1);
  fclose(fp);

}




void PlotData(ListaBurst* head,int mode) {
  FILE *fp=NULL;
  if(mode == 1) {
  fp=fopen("datacpu.txt","w"); }
  else {
  fp=fopen("dataio.txt","w");
  }
  int i = 0;
 ElementoBurst* aux=head->first;
  while(aux){
 if (i == aux->durata) {
      fprintf(fp,"%d\t%d\n",i,aux->occorrenza);
      aux = aux->next;
      }
  else {
    fprintf(fp,"%d\t%d\n",i,0); }
    i++;


  }
  fclose(fp);

}


int minrange(ListaBurst* head,ElementoBurst* target) {

ElementoBurst* aux=head->first;
if((head->first->durata) == target->durata) {
return 1;
}

  while(aux){
    if((aux->next->durata) == (target->durata)) {
      return aux->durata;
     }
    aux=aux->next;
  }
  return 1;

}

int main(int argc, char** argv) {
int nprocess = atoi(argv[1]);//quanti processi genero
int nburst = atoi(argv[2]);//quanti burst avranno
printf("--------------------------------------------------\n");
int * ncpus = (int *) malloc(sizeof(int)); //TOTALE OCCORRENZE CPU
int * nio = (int *) malloc(sizeof(int)); //TOTALE OCCORRENZE IO

int listacpu[MAX_BURST]; //Posso avere al massimo 100 burst cpu/io
int listaio[MAX_BURST];

fetching(listacpu,listaio,ncpus,nio); //Raccolgo i dati sui vari burst, da una precedente esecuzione di sched_sim
order(listacpu,*ncpus); //ordino con quicksort le liste
order(listaio,*nio);


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




printf("Sono stati generati %d valori di CPU Burst\n\n",cpubursts->size);
List_print(cpubursts); //Faccio una semplice inizializzazione per le liste
printf("Sono stati generati %d valori di IO Burst\n\n",iobursts->size);
List_print(iobursts);

printf("\n----------------Probabilità dei vari CPU Burst-----------------\n");
Setting_up(cpubursts,*ncpus);
printf("\n----------------Probabilità dei vari IO Burst-----------------\n");
Setting_up(iobursts,*nio);
//INIZIALIZZO PROB E LOWE/UPPER BOUND



PlotData(cpubursts,1);
PlotData(iobursts,0);
PlotFile(cpubursts,1);
PlotFile(iobursts,0);



srand(time(NULL)); //Genera un seed diverso ogni volta


printf("----------------------------------------------------------------------------------------------------------\n");
for(int i = 1; i <= nprocess;i++){
int arrivaltime= rand()%6;

char str1[20] = "../p";
char str2[20];
char str3[20] = ".txt";
sprintf(str2, "%d", i);
char result[100];
strcpy(result, str1);  // Copy str1 into result
strcat(result, str2);   // Add a space to result
strcat(result, str3);
FILE* f=fopen(result,"w");
 if (! f) {
    return -1;}

bool cpu = true;//inizio sempre con evento cpu
fprintf(f, "PROCESS %d %d\n", i,arrivaltime);


for(int j=0;j<nburst;j++){
 int contatore = 0;
 int random = rand() % 100;

if (cpu) {
ElementoBurst * val = RandomChosen(cpubursts,random); //Trovo il valore del burst che è nel range trovato dal numero random
printf("Random number chosen : %d  --->    ",random);
int randv = rand() % (val->durata - minrange(cpubursts,val) + 1) + ( minrange(cpubursts,val) );
printf("Così abbiamo scelto come durata del cpu burst : %d      \n",randv);
fprintf(f, "CPU_BURST %d\n", randv);
cpu = false;
}

else {
ElementoBurst * val = RandomChosen(iobursts,random);
printf("Random number chosen : %d  --->    ",random);
int randv = rand() % (val->durata - minrange(iobursts,val) +1 ) + ( minrange(iobursts,val) );  
printf("Così abbiamo scelto come durata del io burst : %d      \n",randv);
fprintf(f, "IO_BURST %d\n", randv);
cpu = true;

    }

  
}


  fclose(f);
  printf("Processo numero : %d completato!\n",i);
  printf("----------------------------------------------------------------------------------------------------------\n");
}







































return 1;
}