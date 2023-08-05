typedef struct ElementoBurst {
  struct ElementoBurst* prev;
  struct ElementoBurst* next;
} ElementoBurst;

typedef struct ListaBurst {
  ElementoBurst* first;
  ElementoBurst* last;
  int size;
} ListaBurst;

void fetching(int listacpu[],int listaio[]);
void order(int lista[]);
int cmpfunc (const void * a, const void * b);