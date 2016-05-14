
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define false 0
#define true 1

#define SIZE 1000
int TERM = 'Z' + 1;
int TOTAL_SIZE=0;

#define DEBUG 0

int SET_SIZE;

int* Ti;
int Tsize=0;
int Tid;
int Ti_len;

int d;

typedef struct node_t node_t;
typedef struct list_t list_t;

struct list_t{
  node_t* pointer;
  list_t* next;
  int index;
};

struct node_t{
  int head;
  int sdep;
  node_t* slink;
  list_t* list;
};


typedef struct point_t point_t;

struct point_t{
  node_t *a, *b;
  int depth;
};

typedef long long int lld;

typedef struct set_t set_t;

struct set_t{
  lld* value;
  int count;
};

int updateLink = 0;
node_t* n_slink = NULL;

node_t* root;
node_t* fake;

list_t* new_list(){
  list_t* l = malloc(sizeof(list_t));
  l->pointer = NULL;
  l->next = NULL;
  l->index = -1;
  return l;
}

node_t* new_node(){
  node_t* node = (node_t*) malloc(sizeof(node_t));
  /*node->children = (node_t**) malloc(sizeof(node_t*) * SIZE);
  memset(node->children, 0, sizeof(node_t*) * SIZE);*/
  node->head = 0;
  node->list = new_list();
  return node;
}

node_t* get(node_t* node, int c){
  list_t* el = node->list;
  while(el->next != NULL){
    if(el->next->index == c){
      return el->next->pointer;
    }
    el = el->next;
  }
  return NULL;
}

void set(node_t* node, int c, node_t* p){
  list_t* el = node->list;
  while(el->next != NULL){
    if(el->next->index == c){
      el->next->pointer = p;
      return;
    }
    el = el->next;
  }
  el->next = new_list();
  el->next->pointer = p;
  el->next->index = c;
}

int descendQ(point_t* p, int c){
  if(DEBUG) printf("Try to descend by %c\n", c);

  if(p->b == p->a){
    /*p->b = p->a->children[c];*/
    p->b = get(p->a, c);
  }

  if(p->b == NULL){
    return false; /* there is no node to descend to */
  }
  
  if(p->depth == 0){
    if(DEBUG) printf("Can Descend\n");
    return true;
  }else if(p->depth > 0){
    int pos = p->b->head + p->depth;
    if(c == Ti[pos]){
      if(DEBUG) printf("Can Descend\n");
      return true;
    }
    else
      return false; /* the current edge is missing the current character */
    
  }
  if(DEBUG) printf("Error!\n");
  return false;

}

void descend(point_t* p, int c){
  p->depth++;
  if(p->depth >= p->b->sdep){
    p->a = p->b;
    p->depth = 0;
  }
}

void add_leaf(point_t* p, int j){
  if(p->b)
    if(DEBUG) printf("head: %d depth: %d sdep: %d\n", p->b->head, p->depth, p->b->sdep);
  if(p->depth == 0){
    if(DEBUG) printf("depth = 0\n");
    node_t* leaf = new_node();
    leaf->head = j;
    leaf->sdep = Tsize - j + 1;
    /*p->a->children[Ti[j]] = leaf;*/
    set(p->a, Ti[j], leaf);
    p->b = leaf;
  }else if(p->depth <= p->b->sdep){
    node_t* old_leaf = p->b;
    node_t* new_leaf = new_node();
    node_t* internal = new_node();

    if(DEBUG) printf("new node\n");
    int head=p->b->head, depth=p->depth, sdep = p->b->sdep;
    
    internal->head = head;
    internal->sdep = depth;

    old_leaf->head = head + depth;
    old_leaf->sdep = sdep - depth;
    
    new_leaf->head = j;
    new_leaf->sdep = Tsize - j + 1;
    if(DEBUG) printf("size: %d, sdep: %d\n", Tsize, new_leaf->sdep);

    /*internal->children[Ti[old_leaf->head]] = old_leaf;*/
    set(internal, Ti[old_leaf->head], old_leaf);
    /*internal->children[Ti[new_leaf->head]] = new_leaf;*/
    set(internal, Ti[new_leaf->head], new_leaf);
    /*p->a->children[Ti[head]] = internal;*/
    set(p->a, Ti[head], internal);
    p->b = internal;
    
    internal->slink = root;
    
    if(updateLink){
      updateLink = 0;
      n_slink->slink = internal;
    }

    n_slink = internal;
    updateLink = 1;
  }else{
    ;
  }

}

int what_char(point_t* p){
  list_t* el = p->a->list;
  while(el->next != NULL){
    if(el->next->pointer == p->b){
      return el->next->index;
    }
    el = el->next;
  }
  return -1;
/*
  int i;
  for(i=0; i<SIZE; i++)
    if(p->a->children[i] == p->b)
      return i;
  return -1;*/
}

void print_tree(node_t* root);

void suffix_jump(point_t* p, int j){
  int c = what_char(p);
  if(p->b != p->a){
    int head = j-p->depth;
    int depth = p->depth;

    p->a = p->a->slink;
    /*p->b = p->a->children[c];*/
    /*printf("slink of a is %s\n", p->a == fake ? "fake node": "other");*/
    /*printf("On suffix_jump, descending by %c\n", c);*/
    p->b = get(p->a, c);

    p->depth = 0;
    
    while(depth > 0){
      if(depth < p->b->sdep){
        if(DEBUG) printf("SJ: Descend along edge\n");
        p->depth = depth;
        depth = 0;
      }else{
        depth -= p->b->sdep;
        if(DEBUG) printf("SJ: Jump node, depth = %d\n", depth);
        head += p->b->sdep;
        if(DEBUG) printf("SJ: head is %d\n", head);
        p->a = p->b;
        /*p->b = p->a->children[Ti[head]];*/
        p->b = get(p->a, Ti[head]);
      }
    }
    if(updateLink && p->depth == 0){
      updateLink = 0;
      n_slink->slink = p->a;
    }

    if(p->b == NULL)
      p->b = p->a;
  }else{
    p->a = p->a->slink;
    p->b = p->a;
  }

  if(DEBUG) printf("After suffix jump: head: %d, sdep: %d\n", p->b->head, p->b->sdep);
}

int TAB = 0;
/*
void print_node(node_t* root){
  if(root == NULL)
    return;

  int s;
  for (s = 0; s < TAB; s++)
    printf(" ");
  int j;
  for(j=root->head; j<root->head + root->sdep; j++)
    printf("%c", Ti[j]);
  printf("\n");

  int i;
  for(i=0; i<SIZE; i++){
    TAB++;
    print_node(root->children[i]);
    TAB--;
  }
}
*/

/*void print_tree(node_t* root){
  int i;
  for(i=0; i<SIZE; i++){
    if(root->children[i] != NULL){
      int s, j;
      for (s = 0; s < TAB; s++)
        printf(" ");

      for(j=root->children[i]->head; j<root->children[i]->head+root->children[i]->sdep; j++)
        printf("%c", Ti[j]);
      printf("\n");
      TAB++;
      print_tree(root->children[i]);
      TAB--;
    }
  }
}*/

int* best;

void updateBest(int value, int sdep){
  int i;
  for(i=2; i<=value; i++){
    if(sdep > best[i]){
      best[i] = sdep;
      /*printf("%d: %d\n", i, sdep);*/
    }
  }
}

set_t* new_set(){
  set_t* set = (set_t*) malloc(sizeof(set_t));
  set->value = malloc( (SET_SIZE*2) * sizeof(int) );
  memset(set->value, 0, (SET_SIZE*2) * sizeof(int) );
  set->count = 0;

  return set;
}

void insert(set_t* set, int pos){
  int i = pos / 32;
  int j = pos % 32;
  set->value[i] |= 1 << j;
}

int join(set_t* set1, set_t* set2){
  int size=0, i;
  for(i=0; i<SET_SIZE; i++){
    set1->value[i] |= set2->value[i];
    size += __builtin_popcount(set1->value[i]);
  }
  return size;
}

set_t* dfs(node_t* root, int sdep){
  set_t* set = new_set();
  
  int s;
  for(s = 0; s < TAB; ++s)
    if(DEBUG) printf(" ");
  int j;
  for(j=root->head; j<root->head + root->sdep; j++)
    if(DEBUG) printf("%c", Ti[j]);
  if(DEBUG) printf("\n");

  if(Ti[root->head+root->sdep-1] >= TERM){
    insert(set, Ti[root->head+root->sdep-1] - TERM);
    set->count = 1;

    return set;
  }
  
  TAB++;
  list_t* l;
  for(l = root->list; l->next != NULL; l=l->next){
    if(l->next->pointer != NULL){
      set_t* n_set = dfs(l->next->pointer, sdep + l->next->pointer->sdep);
      set->count = join(set, n_set);
      free(n_set->value);
      free(n_set);
    }
  }
  TAB--;
  
  if(DEBUG) printf("Count: %d\n", set->count);

  updateBest(set->count, sdep);
  return set;

}

void copy(int *dest, char* src){
  int i;
  for(i=0; src[i] != 0; i++)
    dest[i] = src[i];
  dest[i] = 0;
}

void ukkonen(node_t* root, char* T2){
  int* T = (int*) malloc((strlen(T2) + 2) * sizeof(int));
  
  copy(T, T2);
  int len = strlen(T2);
  T[len] = TERM + Tid;
  T[len+1] = 0;
  
  int size = len+1;
  int j = Ti_len;
  point_t *p = malloc(sizeof(point_t));
  p->a = root;
  p->b = root;
  p->depth = 0;

  memcpy(&Ti[Ti_len], T, size * sizeof(int));
  Ti_len += len+1;

  Tid++;
  Tsize = Ti_len-1;
  int tip = j + size;

  while(j <= tip){
    if(DEBUG) printf("------- Inserting element %d: %c -------\n", j, Ti[j]);
    while(!descendQ(p, Ti[j]) ){
      if(DEBUG) printf("Add leaf\n");
      add_leaf(p, j);
      suffix_jump(p, j);
    }
    updateLink = 0;
    descend(p, Ti[j]);
    j++;
  }

  free(p);
  free(T);
}

void init_tree(){
  root = new_node();
  root->slink = new_node();
  fake = root->slink;
  root->slink->head = -1;
  root->slink->sdep = 0;
  root->sdep = 1;
  int i;
  for(i=0; i<SIZE; i++){
    set(root->slink, i, root);
    /*root->slink->children[i] = root;*/
  }

  Ti = (int*) malloc(TOTAL_SIZE * sizeof(int));
  Ti[0] = 0;
}

char** Ts;

int main(){
  int n, len;
  scanf("%d", &n);

  Ts = (char**) malloc(sizeof(char*) * (n+2) );
  SET_SIZE = (n+32) / 32 + 1;
  best = (int*) malloc(sizeof(int) * (n+5) );
  memset(best, 0, sizeof(int) * (n+5) );

  int i;
  for(i=0; i<n; i++){
    scanf("%d", &len);
    TOTAL_SIZE += len+2;
    Ts[i] = malloc(len+2);
    scanf("%s", Ts[i]);
  }

  init_tree();

  for(i=0; i<n; i++){
    ukkonen(root, Ts[i]);
  }
  
  dfs(root, 0);
  for(i=2; i<=Tid; i++)
    printf("%d ", best[i]);
  printf("\n");
  
  for(i=0; i<n; i++){
    free(Ts[i]);
  }
  
  
  return 0;
}
