#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define false 0
#define true 1

#define SIZE 100
#define MAX_LEN 1000
#define MAX_STRINGS 100
#define SET_SIZE (MAX_STRINGS+64) / 64
#define TERM 'Z' + 1

#define DEBUG 0

char* Ti;
int Tsize;
int Tid;
char *strings[MAX_STRINGS];

int d;

typedef struct node_t node_t;

struct node_t{
  int head;
  int sdep;
  node_t** children;
  node_t* slink;
};

typedef struct point_t point_t;

struct point_t{
  node_t *a, *b;
  int depth;
};

typedef long long int lld;

typedef struct set_t set_t;

struct set_t{
  lld value[SET_SIZE];
  char count;
};

int updateLink = 0;
node_t* n_slink = NULL;

node_t* root;
node_t* fake;

node_t* new_node(){
  node_t* node = (node_t*) malloc(sizeof(node_t));
  node->children = (node_t**) malloc(sizeof(node_t*) * SIZE);
  memset(node->children, 0, sizeof(node_t*) * SIZE);
  node->head = 0;
  return node;
}

void point(point_t* p, node_t* a, node_t* b, int sdep){
  p->a = a;
  p->b = b;
  p->depth = sdep;
}

int descendQ(point_t* p, int c){
  if(DEBUG) printf("descendQ with %c, on %s\n", c, p->a == root ? "root" : "not root");
  c -= 'A';

  if(p->b == p->a){
    if(DEBUG) printf("b=a\n");
    p->b = p->a->children[c];
  }

  if(p->b == NULL){
    return false; /* there is no node to descend to */
  }
  
  if(p->depth == 0){
    if(DEBUG) printf("Can Descend\n");
    return true;
  }else if(p->depth > 0){
    int pos = p->b->head + p->depth;
    if(c == Ti[pos] - 'A'){
      if(DEBUG) printf("Can Descend\n");
      return true;
    }
    else
      return false; /* the current edge is missing the current character */
    
  }
  if(DEBUG) printf("Error!\n");
  return false;

}

void descend(point_t* p, char c){
  if(DEBUG) printf("Descending with %c; head: %d, depth: %d, sdep: %d\n", c, p->b->head, p->depth, p->b->sdep);
  p->depth++;
  if(p->depth >= p->b->sdep){
    if(DEBUG) printf("Descend to next node\n");
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
    p->a->children[Ti[j]-'A'] = leaf;
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

    internal->children[Ti[old_leaf->head]-'A'] = old_leaf;
    internal->children[Ti[new_leaf->head]-'A'] = new_leaf;
    p->a->children[Ti[head]-'A'] = internal;
    p->b = internal;
    
    internal->slink = root;
    
    if(updateLink){
      updateLink = 0;
      if(DEBUG) printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> POINTING SLINK OF HEAD %d TO HEAD %d\n", n_slink->head, internal->head );
      n_slink->slink = internal;
    }

    n_slink = internal;
    updateLink = 1;
  }else{
    if(DEBUG) printf("WTF!!!\n");
  }

}

int what_char(point_t* p){
  int i;
  for(i=0; i<SIZE; i++)
    if(p->a->children[i] == p->b)
      return i;
  return -1;
}

void print_tree(node_t* root);
void print_node(node_t* root);

void suffix_jump(point_t* p, int j){
  int c = what_char(p);
  if(p->b != p->a){
    int head = j-p->depth;
    int depth = p->depth;

    p->a = p->a->slink;
    p->b = p->a->children[c];

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
        p->b = p->a->children[Ti[head] - 'A'];
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

void print_tree(node_t* root){
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
}

int best[MAX_STRINGS];

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
  memset(set->value, 0, SET_SIZE * sizeof(lld) );
  set->count = 0;

  return set;
}

void insert(set_t* set, int pos){
  int i = pos / 64;
  int j = pos % 64;
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
  int i;
  for(i=0; i<SIZE; i++){
    if(root->children[i] != NULL){

      set_t* n_set = dfs(root->children[i], sdep + root->children[i]->sdep);
      set->count = join(set, n_set);      

    }
  }
  TAB--;
  
  if(DEBUG) printf("Count: %d\n", set->count);

  updateBest(set->count, sdep);
  return set;

}

void ukkonen(node_t* root, char* T2){
  char* T = (char*) calloc(strlen(T2) + 2, 1);
  if(DEBUG) printf("Adding %s\n", T2);
  strcpy(T, T2);
  T[strlen(T2)] = TERM + Tid;
  T[strlen(T2)+1] = 0;
  if(DEBUG) printf("Adding %s\n", T);
  
  int size = strlen(T)-1;
  int j = strlen(Ti);
  point_t *p = malloc(sizeof(point_t));
  point(p, root, root, 0);

  strcat(Ti, T);
  
  Tid++;
  Tsize = strlen(Ti)-1;
  int tip = j + size;

  while(j <= tip){
    if(DEBUG) printf("------- Inserting element %d: %c -------\n", j, Ti[j]);
    while(!descendQ(p, Ti[j]) ){
      if(DEBUG) printf("Add leaf\n");
      add_leaf(p, j);
      if(DEBUG) print_tree(root);
      suffix_jump(p, j);
    }
    updateLink = 0;
    descend(p, Ti[j]);
    if(DEBUG) print_tree(root);
    j++;
  }

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
  for(i=0; i<SIZE; i++)
    root->slink->children[i] = root;

  Ti = (char*) calloc(MAX_LEN * MAX_STRINGS, 1);
  Ti[0] = 0;
}

char Ts[MAX_STRINGS][MAX_LEN];

int main(){
  
  init_tree();

  int n, p;
  scanf("%d", &n);
  if(DEBUG) printf("n: %d\n", n);
  int i;
  for(i=0; i<n; i++){
    scanf("%d %s", &p, Ts[i]);
    ukkonen(root, Ts[i]);
  }
  if(DEBUG) printf("\n");

  memset(best, 0, sizeof(best));
  set_t* set = dfs(root, 0);
  if(DEBUG) printf("count %d\n", set->count);
  /*print_tree(root);*/
  for(i=2; i<=Tid; i++)
    printf("%d ", best[i]);
  printf("\n");

  if(DEBUG) printf("%s\n", Ti);

  return 0;
}
