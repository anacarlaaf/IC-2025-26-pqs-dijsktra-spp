#include<bits/stdc++.h>

using namespace std;

typedef struct node{
    int key;
    int degree;
    node *parent;
    node *child;
    node *sibling;
} node;

typedef struct binomialHeap{
    node *head;
} binomialHeap;

void makeBinomialHeap(binomialHeap *t){
    t->head=NULL;
}

//O(logn)
node *findMin(binomialHeap *t){
    //Passa por todas as roots de t comparando o valor minimo
    node *aux = t->head, *minNode=aux;
    int minKey = aux->key;
    aux = aux->sibling;
    while(aux){
        if(aux->key<minKey){
            minKey = aux->key;
            minNode = aux;
        }
        aux = aux->sibling;
    }
    return minNode;
}

bool isEmpty(binomialHeap *b1){
    if(b1->head!=NULL) return false;
    else return true;
}

//priv
void mergeTrees(binomialHeap *b1, binomialHeap *b2){
    //Merge das árvores em ordem não-decrescente de degree
    node *i=b1->head, *j=b2->head, *aux, *st=NULL;

    // guardar o endereço do menor degree pra ser o novo b1->head;
    //cout << i << " " << j << "\n";
    if((i && !j) || ((i && j) && i->degree<=j->degree)){
        aux=i;
        st=i;
        i=i->sibling;
    }
    else if (j) {
        aux=j;
        st=j;
        j=j->sibling;
    }
    
    //merge
    while(i && j){
        if(i->degree<=j->degree){
            aux->sibling=i;
            aux=i;
            i=i->sibling;
        }
        else{
            aux->sibling=j;
            aux=j;
            j=j->sibling;
        }
    }

    //resto
    while(i){
        aux->sibling=i;
        aux=i;
        i=i->sibling;
    }

    while(j){
        aux->sibling=j;
        aux=j;
        j=j->sibling;
    }

    b1->head=st;
    b2->head=NULL; // limpar a b2
}

void uniteBinomialHeaps(binomialHeap *b1, binomialHeap *b2){
    mergeTrees(b1, b2);
    if(!b1->head || !b1->head->sibling) return;

    node *prev=NULL, *at=b1->head, *next=at->sibling;

    while(at && next){
        //Degrees diferentes ou degree do próximo do próximo igual ao atual
        if((at->degree!=next->degree) || (next->sibling && at->degree==next->sibling->degree)){
            //cout << "caso1 ou 2 \n";
            prev=at;
            at=at->sibling;
            next=next->sibling;
        }

        // Mesmo degrees, compara as chaves
        else if(at->key<=next->key){
            //cout << "caso3 \n";
            //linkTrees(at, next);
            at->sibling=next->sibling;
            next->sibling = at->child;
            at->child=next;
            next->parent=at;
            at->degree++;

            next=at->sibling;
        }
        else{
            //cout << "caso4 \n";
            //linkTrees(next, at);
            if(!prev) b1->head = next;
            else prev->sibling=next;
            at->sibling=next->child;
            next->child=at;
            at->parent=next;
            next->degree++;

            at=next;
            next=next->sibling;
        }
    }
}

void insert(binomialHeap *b1, int val){
    node *newNode = (node*) malloc(sizeof(node));
    newNode->key = val;
    newNode->parent = NULL;
    newNode->child = NULL;
    newNode->degree = 0;
    newNode->sibling = NULL;
    
    binomialHeap b2;
    b2.head = newNode;

    uniteBinomialHeaps(b1,&b2);
}

void extractMin(binomialHeap *t){
    //encontrar o mínimo e remover da heap t
    node *ext = findMin(t), *aux;

    if(ext==t->head) {
        t->head=NULL;
        if(!ext->child && !ext->sibling) return;
    }
    else {
        aux = t->head;
        while(aux->sibling){
            if(aux->sibling==ext){
                aux->sibling=aux->sibling->sibling;
                break;
            }
            aux = aux->sibling;
        }
    }

    // tornar filhos do min em roots do novo heap
    binomialHeap t2;
    makeBinomialHeap(&t2);
    node *at, *next;
    aux = ext->child;
    if(aux && aux->sibling){
        at=aux->sibling;
        aux->parent=aux->sibling=NULL;
        while(at){
            at->parent=NULL;
            next = at->sibling;
            at->sibling=aux;
            aux=at;
            at=next;
        }
    }
    t2.head=aux;
    if(!t->head) t->head = ext->sibling;
    uniteBinomialHeaps(t, &t2);
    free(ext);
}

//O(logn)
void decreaseKey(binomialHeap *t, node *x, int n){
    x->key = n;
    node *y = x, *z = x->parent;
    int aux;
    while (z && y->key < z->key){
        aux = z->key;
        z->key = n;
        y->key = aux;
        y = z;
        z = y->parent;
    }
}

void deleteKey(binomialHeap *t, node *x){
    decreaseKey(t, x, -1);
    extractMin(t);
}