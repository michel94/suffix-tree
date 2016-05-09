#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 90
#define false 0
#define true 1

char* Ti;
int Tsize;
int Tid;

typedef struct node_t node_t;

struct node_t{
  int Ti;
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

int updateLink = 0;
node_t* n_slink = NULL;

node_t* root;
node_t* fake;

node_t* new_node(){
  node_t* node = (node_t*) malloc(sizeof(node_t));
  node->children = malloc(sizeof(node_t*) * SIZE);
  memset(node->children, 0, sizeof(node->children));
  node->head = 0;
  return node;
}

void point(point_t* p, node_t* a, node_t* b, int sdep){
  p->a = a;
  p->b = b;
  p->depth = sdep;
}

int descendQ(point_t* p, char c){
  printf("descendQ with %c, on %s\n", c, p->a == root ? "root" : "not root");
  c -= 'A';

  if(p->b == p->a){
    printf("b=a\n");
    p->b = p->a->children[c];
  }

  if(p->b == NULL){
    return false; // there is no node to descend to
  }
  
  if(p->depth == 0 && p->a->children[c] != NULL){
    printf("Can Descend\n");
    return true;
  }else if(p->depth > 0 && p->a->children[c] != NULL){
    int pos = p->b->head + p->depth;
    if(c == Ti[pos] - 'A'){
      printf("Can Descend\n");
      return true;
    }
    else
      return false; // the current edge is missing the current character
    
  }

}

void descend(point_t* p, char c){
  printf("Descending with %c; head: %d, depth: %d, sdep: %d\n", c, p->b->head, p->depth, p->b->sdep);
  p->depth++;
  if(p->depth >= p->b->sdep){
    printf("Descend to next node\n");
    p->a = p->b;
    p->depth = 0;
  }
}

void add_leaf(point_t* p, int j){
  if(p->depth == 0){
    printf("depth = 0\n");
    node_t* leaf = new_node();
    leaf->head = j;
    leaf->sdep = Tsize - j + 1;
    p->a->children[Ti[j]-'A'] = leaf;
    p->b = leaf;
  }else if(p->b->head + p->depth < p->b->sdep){
    node_t* old_leaf = p->b;
    node_t* new_leaf = new_node();
    node_t* internal = new_node();

    int head=p->b->head, depth=p->depth, sdep = p->b->sdep;
    
    internal->head = head;
    internal->sdep = depth;

    old_leaf->head = head + depth;
    old_leaf->sdep = sdep - depth;
    
    new_leaf->head = j;
    new_leaf->sdep = Tsize - j + 1;

    internal->children[Ti[old_leaf->head]-'A'] = old_leaf;
    internal->children[Ti[new_leaf->head]-'A'] = new_leaf;
    p->a->children[Ti[head]-'A'] = internal;
    p->b = internal;
    
    internal->slink = root;
    
    if(updateLink){
      updateLink = 0;
      printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> POINTING SLINK OF HEAD %d TO HEAD %d\n", n_slink->head, internal->head );
      n_slink->slink = internal;
    }

    n_slink = internal;
    updateLink = 1;
  }

}

int what_char(point_t* p){
  for(int i=0; i<SIZE; i++)
    if(p->a->children[i] == p->b)
      return i;
  return -1;
}

void print_tree(node_t* root);

void suffix_jump(point_t* p, int j){
  char c = what_char(p);
  if(p->b != p->a){
    printf("Suffix jump\n");
    int head = j-p->depth;
    int depth = p->depth;
    
    /*
    printf("a head %d\n", p->a->head);
    printf("b head %d\n", p->b->head);
    printf("slink %x\n", p->a->slink);
    printf("slink head %x\n", p->a->slink->head);
    printf("slink tree\n");*/

    p->a = p->a->slink;
    p->b = p->a->children[c];

    p->depth = 0;
    
    while(depth > 0){
      if(depth < p->b->sdep){
        printf("SJ: Descend along edge\n");
        p->depth = depth;
        depth = 0;
      }else{
        depth -= p->b->sdep;
        printf("SJ: Jump node, depth = %d\n", depth);
        head += p->b->sdep;
        printf("SJ: head is %d\n", head);
        p->a = p->b;
        p->b = p->a->children[Ti[head] - 'A'];
      }
    }

    if(p->b == NULL)
      p->b = p->a;
  }else{
    p->a = p->a->slink;
    p->b = p->a;
  }

  printf("After suffix jump: head: %d, sdep: %d\n", p->b->head, p->b->sdep);
}

int TAB = 0;

void print_tree(node_t* root){
  for(int i=0; i<SIZE; i++)
    if(root->children[i] != NULL){
      //printf("first: %d\n", i);
      for (int s = 0; s < TAB; ++s)
        printf(" ");

      for(int j=root->children[i]->head; j<root->children[i]->head+root->children[i]->sdep; j++)
        printf("%c", Ti[j]);
      printf("\n");
      TAB++;
      print_tree(root->children[i]);
      TAB--;
    }
}

void ukkonen(node_t* root, char* T, int s_id){
  int size = strlen(T)-1;
  int j = 0;
  point_t *p = malloc(sizeof(point_t));
  point(p, root, root, 0);
  
  Ti = T;
  Tsize = size;

  while(j <= size){
    printf("------- Inserting element %d: %c -------\n", j, Ti[j]);
    while(!descendQ(p, Ti[j]) ){
      printf("Add leaf\n");
      add_leaf(p, j);
      print_tree(root);
      suffix_jump(p, j);
    }
    updateLink = 0;
    descend(p, Ti[j]);
    print_tree(root);
    j++;
  }
}

void init_tree(){
  root = new_node();
  root->slink = new_node();
  fake = root->slink;
  root->slink->head = -1;
  root->slink->sdep = 0;
  root->sdep = 1;
  for(int i=0; i<SIZE; i++)
    root->slink->children[i] = root;
}

int main(){
  //char T0[] = "ABCABD$";
  //char T0[] = "MISSISSIPID";
  char T0[] = "MISISIMISIB";
  //T0[strlen(T0)-1] = 91;
  init_tree();
  ukkonen(root, T0, 0);
  //ukkonen(root, T1, 0);

  return 0;
}
