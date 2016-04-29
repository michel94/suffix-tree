#include <stdio.h>
#include <string.h>

#define SIZE 30

char* Ti;
char Tsize;

struct node_t{
  int Ti;
  int head;
  int sdep;
  node_t** children;
  node_t* slink;
};

struct point_t{
  node_t *a, *b;
  int depth;
}

/*
struct tree_t{
  node_t* root;
}

tree_t* new_stree(){
  tree_t* t = (void*) malloc(sizeof(tree_t));
  t->root = (void*) malloc(sizeof(node_t));
  t->root->sdep = 0;
  t->root->head = 0;
  t->root->children = (void**) malloc(sizeof(node_t*) * SIZE);
  memset(t->root->children, NULL, SIZE * sizeof(node_t*));
  return t;
}
*/

node_t* new_node(){
  node_t* node = malloc(sizeof(node_t));
  return node;
}

void point(point_t* p, node_t* a, node_t* b, int sdep){
  p->a = a;
  p->b = b;
  p->depth = sdep;
}

int descendQ(point_t* p, char c){
  int pos = p->a->head + p->depth;
  int Ti = p->a->Ti;
  if(c == Ti[pos])
    return true;
  else
    return false;
}

void descend(point_t* p, char c){
  p->depth++;
}

void add_leaf(point_t* p, int j){
  if(depth == 0){
    node_t* leaf = new_node();
    p->a->children[Ti[j]] = leaf;
  }else if(p->a->head + p->depth < p->b->head){
    node* internal = new_node();
    internal->head = j;
    internal->sdep = p->depth;
    node* leaf = new_node();
    leaf->head = Tsize;
    leaf->sdep = Tsize - p->depth;

    p->a->children[Ti[j]] = internal;
    internal->children[Ti[j]] = leaf;
  }

}

void suffix_jump(point_t* p){
  char c = Ti[p->a->head];
  p->a = p->a->slink;
  p->b = p->a->children[c];
  p->depth--;
}

void ukkonen(tree_t* tree, char T[], int s_id){
  int size = strlen(t);
  int j = 0;
  point_t *p;
  point(p, tree->root, tree->root, sdep);
  Ti = T;
  Tsize = size;

  while(j <= size){
    while(!descendQ(p, T[j]) ){
      add_leaf(p, j);
      suffix_jump(p);
    }
    descend(p, T[j]);
    j++;
  }
}

int main(){
  tree_t s_tree = new_stree();
  ukkonen(tree, "abc", 0);

  return 0;
}
