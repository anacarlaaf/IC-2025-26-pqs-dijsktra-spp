#include <bits/stdc++.h>
using namespace std;

typedef unsigned long long ll ;
typedef ll keyInt;
typedef ll keyType;


typedef pair<keyInt,int> parInt;
typedef pair<keyType,int> par;

typedef vector<vector<parInt>> graphInt;
typedef vector<vector<par>> graph;


#ifndef NPTR_DEFINED
#define NPTR_DEFINED
typedef struct nptr {
    int lvl;
    int idx;
    list<par>::iterator it;
} nptr;


enum PQS {
    BINHCPP,
    RBT,
    FIBH,
    BINH,
    RH,
    PAIRING,
    HOT,
    SEQUENCE,
    _1LVBQ,
    _2LVBQ,
    _KLVBQ,
    _1LVBQDK,
    _2LVBQDK,
    _KLVBQDK,
};

map<string, PQS> toPq = {
    {"RBT", RBT},
    {"FIBH", FIBH},
    {"BINH", BINH},
    {"RH", RH},
    {"PAIRING", PAIRING},
    {"SEQUENCE", SEQUENCE},
    {"HOT", HOT},
    {"1LVBQ", _1LVBQ},
    {"2LVBQ", _2LVBQ},
    {"KLVBQ", _KLVBQ},
    {"1LVBQDK", _1LVBQDK},
    {"2LVBQDK", _2LVBQDK},
    {"KLVBQDK", _KLVBQDK},
};

struct bkt{
    int tail, sz;
};

struct element{
    par data;
    int prev, prox;
};

struct ops{
    // ops de pq
    int ins=0;
    int exmin=0;
    int dk=0;
    int bkemp=0;

    // ops aux
    int upd=0;
};

struct pool_list{

    element *pool;
    int *free_list;
    int *idxs;
    int free_top;

    pool_list() : free_list(nullptr), idxs(nullptr), free_top(0) {}
    pool_list(int c) {
        pool = new element[c];
        free_list = new int[c];
        idxs = new int[c];
        for(int i=0;i<c;i++) {
            free_list[i] = i; // todos livre;
            idxs[i] = -1;
        }
        free_top = c-1;
    }

    int alocar(par novo){
        int idx = free_list[free_top--];
        pool[idx].data = novo;
        pool[idx].prev = pool[idx].prox = -1;;
        return idx;
    }
    
    void insert(par novo, bkt *cauda){ 
        int idx =  alocar(novo);
        idxs[novo.second] = idx;
        int t = cauda->tail;
        if (t!=-1) {
            pool[t].prox = idx;
            pool[idx].prev = t;
        }
        cauda->tail = idx;
        cauda->sz++;
    }

    void pop(bkt *bucket){
        int t = bucket->tail;
        int ant = pool[t].prev;

        if (ant != -1) {
            pool[ant].prox = -1;
        }

        free_top++;
        free_list[free_top] = t;
        idxs[pool[t].data.second] = -1;

        bucket->tail = ant;
        bucket->sz--;
    }

    void remove(int u, bkt *bucket){
        int idx = idxs[u];
        int ant = pool[idx].prev;
        int next = pool[idx].prox;

        if(bucket->tail == idx) bucket->tail = pool[idx].prev;
        bucket->sz--;

        if (ant != -1) pool[ant].prox = next;
        if (next != -1) pool[next].prev = ant;

        free_top++;
        free_list[free_top] = idx;
        idxs[u] = -1;
    }

    void clear(int n) {
        for(int i = 0; i < n; i++) { free_list[i] = i; idxs[i] = -1; }
        free_top = n - 1;
    }

    void del(){
        delete[] pool;
        delete[] free_list;
        delete[] idxs;
    }
};

struct bitmask{ // usar para evitar percorrer buckets vazios
    static const int W = 128;
    unsigned __int128 *mask;
    int n_masks;

    void init(int b_size){
        n_masks = (b_size + W - 1) / W; // teto
        mask = new unsigned __int128[n_masks]();
    }

    inline void set(ll i){ // bucket tem elementos
        mask[i/W] |= ((unsigned __int128)1 << (i%W));
    }

    inline void unset(ll i){ // bucket vazio
        mask[i/W] &= ~((unsigned __int128)1 << (i%W));
    }

    inline bool is_set(ll i) { // verifica se bucket tem elementos
        return (mask[i / W] >> (i % W)) & 1;
    }

    ll lsb(ll from=0){ // "from" equivale ao bucket ativo do nível
        ll w = from/W; // qual trecho da máscara
        ll bit = from % W; // qual bit da máscara

        unsigned __int128 m = mask[w] >> bit;
        if(m) return w * W + bit + ctz128(m);
        // trecho * 128 + bkt ativo + próximo bkt não-vazio

        // se não achar, busca nos próximos trechos da máscara
        for(ll i=w+1; i<n_masks;i++){
            if(mask[i]) return i * W + ctz128(mask[i]);
        }

        return -1;
    }

    static int ctz128(unsigned __int128 x) { // acha bit mais significativo
        uint64_t lo = (uint64_t)x;           // pega os 64 bits inferiores
        if (lo) return __builtin_ctzll(lo);  // se tem bit setado embaixo, responde direto
        return 64 + __builtin_ctzll((uint64_t)(x >> 64));  // senão, olha os 64 superiores
    }

    void clear() {
        memset(mask, 0, n_masks * sizeof(__int128));
    }

};

#endif