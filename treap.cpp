#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <math.h>
#include <time.h>
#include <assert.h>
//#include <vector>
//using namespace std;

typedef struct node_t{
    void* key, *value;
    int priority;
    node_t *left, *right, *parent;
}node_t;

int* INT(int v){
    int* i = (int*) malloc(sizeof(int));
    *i = v;
    return i;
}
int n = 45;
int random_priority(){
    return rand();
}
void rotate_right(node_t* root);
void rotate_left (node_t* root);
void fix_priority(node_t* p);

node_t* new_node(node_t* par){
    node_t* p = (node_t*) malloc(sizeof(node_t));
    p->key = NULL;
    p->value = NULL;
    p->priority = random_priority();
    p->left = NULL;
    p->right = NULL;
    p->parent = par;
    return p;
}

void check(node_t* p){
    if(p->parent != NULL){
        assert(p->parent->right == p || p->parent->left == p);
    }
}

void *search(const void *key, 
                    void **rootp, 
                    int (*compar)(const void*, const void*),
                    void* parent){
    node_t *p = *((node_t**) rootp);

    //insert
    if(p == NULL){
        //printf("insert %d\n", *((int*)key) );
        node_t* node = new_node((node_t*) parent);
        node->key = (void*) key;
        *rootp = (void*) node;
        //check(node);

        fix_priority(node);
        return node;
    }

    //find
    int comp = (*compar)(key, p->key);
    if(comp == 0)
        return p;
    else if(comp < 1)
        return search(key, (void**) &(p->left), compar, p);
    else
        return search(key, (void**) &(p->right), compar, p);
}

void *tfind(const void *key, 
                    void *rootp, 
                    int (*compar)(const void*, const void*)){
    node_t *p = (node_t*) rootp;

    if(p == NULL)
        return NULL;

    int comp = (*compar)(key, p->key);
    if(comp == 0)
        return p;
    else if(comp < 1)
        return tfind(key, p->left, compar);
    else
        return tfind(key, p->right, compar);
}

void *tsearch(const void *key, 
                    void **rootp, 
                    int (*compar)(const void*, const void*) ){
    
    void* node = search(key, rootp, compar, NULL);
    assert(node != NULL);
    node_t* p = (node_t*) node;
    if(p->parent == NULL) // p is root
        *rootp = (void*) p;
    return node;
}

void walk(const void* root, void (*action)(const void *nodep, 
                                            const VISIT which,
                                            const int depth),
                            int depth){
    node_t *p = (node_t*) root;
    if(p == NULL)
        return;

    if(p->left == NULL && p->right == NULL)
        (*action)(p, leaf, depth);
    else{
        (*action)(p, preorder, depth);
        walk(p->left, action, depth+1);
        (*action)(p, postorder, depth);
        walk(p->right, action, depth+1);
        (*action)(p, endorder, depth);
    }

}

void twalk(const void* root, void (*action)(const void *nodep, 
                                            const VISIT which,
                                            const int depth)){
    walk(root, action, 0);
}

int is_right_child(const node_t* p){
    return p->parent->right == p;
}

void fix_priority(node_t* p){
    if(p->parent == NULL || p->priority > p->parent->priority)
        return;
    
    if(is_right_child(p))
        rotate_left(p->parent);
    else
        rotate_right(p->parent);

    fix_priority(p);
}

void point_parent(node_t* root, node_t* dest){ // points parent of a node to a new element
    if(root == NULL || root->parent == NULL)
        return;
    if(root->parent->left == root){
        root->parent->left = dest;
    }else{
        root->parent->right = dest;
    }
}

void place_child(node_t* root){
    if(root == NULL || root->parent == NULL)
        return;

    if(root->parent->left == root)
        root->parent->left = root->right;
    else
        root->parent->right = root->left;

}

void rotate_right(node_t* root){
    node_t* left = root->left;

    point_parent(root, left);
    left -> parent = root -> parent;

    root -> left = left -> right;
    if(left -> right != NULL)
        left -> right -> parent = root;
        
    left -> right = root;
    root -> parent = left;
}

void rotate_left(node_t* root){
    node_t* right = root->right;
    
    point_parent(root, right);
    right -> parent = root -> parent;

    root -> right = right -> left;
    if(right -> left != NULL)
        right -> left -> parent = root;
        
    right -> left = root;
    root -> parent = right;

}

node_t* rotate_down(node_t* root){
    if(root->left == NULL && root->right == NULL)
        return NULL;

    if(root->left != NULL && root->right == NULL)
        rotate_right(root);
    else if(root->right != NULL && root->left == NULL)
        rotate_left(root);
    else{
        if(root->left->priority < root->right->priority){ // left should become the new root
            rotate_right(root);
        }else{
            rotate_left(root);
        }
        node_t* new_root = root->parent;
        rotate_down(root);
        return new_root;

    }
    return rotate_down(root);
}

void *tdelete(const void *key, void **rootp, int (*compar)(const void *, const void *)){
    node_t* node = (node_t *) tfind(key, *rootp, compar); // TODO: should use a tfind
    node_t* new_root = NULL;

    if(node->left == NULL && node->right == NULL){
        //printf("LEAF\n");
        point_parent(node, NULL);

        new_root = node->parent;
    }else if(node->left != NULL && node->right == NULL){
        //printf("HAS LEFT CHILD\n");
        point_parent(node, node->left);
        node->left->parent = node->parent;

        new_root = node->left;
    }else if(node->right != NULL && node->left == NULL){
        //printf("HAS RIGHT CHILD\n");
        point_parent(node, node->right);
        node->right->parent = node->parent;

        new_root = node->right;
    }else{
        //printf("HAS TWO CHILDREN\n");
        new_root = rotate_down(node);
        point_parent(node, NULL);
    }

    if(new_root != NULL && new_root->parent == NULL) // the new root that replaced 'node' has the root of its subtree is in fact the root of the whole tree
        *rootp = (void*) new_root;

    return node;
}

void tclear(void **rootp){
    node_t *p = *((node_t **) rootp);
    if(p->left != NULL)
        tclear((void**) &(p->left));

    if(p->right != NULL)
        tclear((void**) &(p->right));

    free(p);
    *rootp = NULL;
}

int count = 0;
void print_node(const void *nodep, 
                const VISIT which,
                const int depth){
    node_t *p = (node_t*) nodep;

    if(which == preorder || which == leaf){
        count++;
        for(int i=0; i<depth; i++)
            printf("  ");
        printf("%d: %d\n", *(int*)p->key, p->priority);
    }
}

void print_inorder(const void *nodep, 
                const VISIT which,
                const int depth){
    node_t *p = (node_t*) nodep;

    if(which == postorder || which == leaf){
        printf("%d (%d), ", *(int*)p->key, p->priority);
    }
}

/*vector<int> stack;

void check_parents(const void *nodep, 
                const VISIT which,
                const int depth){
    node_t *p = (node_t*) nodep;

    if(which == leaf || which == preorder){
        if(p->parent != NULL){
            int pk = *((int*)(p->parent->key));
            assert( stack.front() == pk );
            stack.pop_back();
        }
        if(p->left != NULL)
            stack.push_back( *((int*)(p->key)) );
        if(p->right != NULL)
            stack.push_back( *((int*)(p->key)) );
    }
}*/

int compare(const void* it1, const void* it2){
    int k1 = *(int*) it1,
        k2 = *(int*) it2;
    if(k1 < k2)
        return -1;
    else if(k1 == k2)
        return 0;
    else
        return 1;
}

int main(){
    srand(time(NULL));
    node_t* root = NULL;

    for(int i=0; i<3000; i++){
        tsearch(INT(i), (void**) &root, &compare);
        if(i % 2 == 1){
            void* node = tdelete(INT(i), (void**) &root, &compare);
            free(node);
        }
        //printf("delete %d\n", i/2);
    }
    twalk(root, print_node);
    printf("%d\n", count);
    //twalk(root, print_node);
    //twalk(root, print_inorder);

    printf("Done.\n");
    
    tclear((void **)&root);
}
