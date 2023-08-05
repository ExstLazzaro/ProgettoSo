#include "Pr.h"
#include <stdlib.h>
#include <stdio.h>
void fetching(int listacpu[],int listaio[]) {


int k = 0;
int y = 0;
FILE* f=fopen("Dati.txt", "r");
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


  }
  if (buffer)
    free(buffer);
  fclose(f);
}

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

void order(int lista[]) {
qsort(lista, 25, sizeof(int),cmpfunc);
}


int main() {
int listacpu[30];
int listaio[30];
fetching(listacpu,listaio);
for(int i = 0;i< 25;i++) {
printf("CPU %d\n",listacpu[i]);
printf("IO %d\n",listaio[i]);}
order(listacpu);
order(listaio);
printf("\n\n------------------\n\n");
for(int i = 0;i< 25;i++) {
printf("CPU %d\n",listacpu[i]);
printf("IO %d\n",listaio[i]);}
return 1;
}