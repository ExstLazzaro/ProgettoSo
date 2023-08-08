typedef struct ElementoBurst {
  struct ElementoBurst* prev;
  struct ElementoBurst* next;
  int durata;
  int occorrenza;
  float px; //PROBABILITA x<Durata
  float lowerbound;
  float upperbound;

} ElementoBurst;

typedef struct ListaBurst {
  ElementoBurst* first;
  ElementoBurst* last;
  int size;
} ListaBurst;

void fetching(int listacpu[],int listaio[],int * ncpus,int * nio);
void order(int lista[],int n);
int cmpfunc (const void * a, const void * b);
void List_inizializzazione(ListaBurst* list);
ElementoBurst* List_insert(ListaBurst* head, ElementoBurst* prev, ElementoBurst* item);
ElementoBurst* List_pushBack(ListaBurst* head, ElementoBurst* item);
ElementoBurst* List_find(ListaBurst* head, int dato);
void List_print(ListaBurst* head);
void Setting_up(ListaBurst* head,int n);
ElementoBurst* RandomChosen(ListaBurst* head, int dato);
void PlotFile(ListaBurst* head,int mode);
int minrange(ListaBurst* head,ElementoBurst* target);
void PlotData(ListaBurst* head,int mode);