#include "Pr.h"
#include <stdlib.h>
#include <stdio.h>
#define MAX_BURST 100









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
  printf("\n%d %d \n",k,y);
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
printf("\nCOSA HO PRESO----------------------\n");
for(int i = 0;i< *ncpus;i++) {
printf("CPU %d\n",listacpu[i]);
printf("IO %d\n",listaio[i]);}
order(listacpu,*ncpus);
order(listaio,*nio);
printf("\n\nECCO ORDINATO------------------\n\n");
for(int i = 0;i< *ncpus;i++) {
printf("CPU %d\n",listacpu[i]);
printf("IO %d\n",listaio[i]);}
return 1;
}