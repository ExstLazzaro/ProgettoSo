typedef struct ElementoBurst {
  struct ElementoBurst* prev;
  struct ElementoBurst* next;
} ElementoBurst;

typedef struct ListaBurst {
  ElementoBurst* first;
  ElementoBurst* last;
  int size;
} ListaBurst;

void fetching(int listacpu[],int listaio[],int * ncpus,int * nio);
void order(int lista[],int n);
int cmpfunc (const void * a, const void * b);
