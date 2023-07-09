#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
int main(int argc, char** argv) { 
int nprocess = atoi(argv[1]);//quanti processi genero
int nburst = atoi(argv[2]);//quanti burst avranno
//binomiale val atteso np, prob k successi e varianza np(1-p)
int widthcpuburst,widthioburst;
printf("Inserire lunghezza media cpu burst: ");
scanf("%d",&widthcpuburst);
printf("\n");
float p= 0.5;
int y = widthcpuburst / p; //ntentativicpuburst

printf("Inserire lunghezza media io burst: ");
scanf("%d",&widthioburst);
printf("\n");
int q = widthioburst / p; //ntentativiioburst

printf("Mi aspetto %d cpu burst con varianza %f \n",widthcpuburst,(y*p*(1-p)));

printf("Mi aspetto %d io burst con varianza %f \n",widthioburst,(q*p*(1-p)));

printf("\n---------------------------\n");

for(int i = 1; i <= nprocess;i++){
int arrivaltime= rand()%6;
char str0[20] = "../";
char str1[20] = "p";
char str2[20];
char str3[20] = ".txt";
sprintf(str2, "%d", i);
char result[100];
strcpy(result, str0);
strcat(result, str1);  // Copy str1 into result
strcat(result, str2);   // Add a space to result
strcat(result, str3);
FILE* f=fopen(result,"w");
 if (! f) {
    return -1;}

bool cpu = true;//inizio sempre con evento cpu
fprintf(f, "PROCESS %d %d\n", i,arrivaltime);


for(int j=0;j<nburst;j++){

 int contatore = 0;

if (cpu) {
      for(int l=0;l<y;l++){
      int success = rand()%2;
      if(success == 1) {
        contatore++;
      }
      else {
      
      }
      }
      fprintf(f, "CPU_BURST %d\n", contatore);
      cpu=false;
      }
else {
     for(int l=0;l<q;l++){
        int success = rand()%2;
      if(success == 1) {
        contatore++;
      }
      else {
      
      }
      }
      fprintf(f, "IO_BURST %d\n", contatore);
      cpu=true;
      }

    }

  
  


  fclose(f);
  printf("Processo numero : %d completato!\n",i);
}





}










