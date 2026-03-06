#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct cbuffer { // usar no lugar de queue<>
    par *data;
    int head = 0, tail = 0, cap;

    cbuffer() : data(nullptr), cap(0) {}
    cbuffer(int c) {
        cap = 1;
        while(cap < c) cap <<= 1;
        data = new par[cap];
    }

    void resize() {
        int new_cap = cap << 1;  // dobra
        par *new_data = new par[new_cap];
        for(int i = head; i < tail; i++)
            new_data[i & (new_cap-1)] = data[i & (cap-1)];
        delete[] data;
        data = new_data;
        cap = new_cap;
    }

    void push(par x) {
        if(size() == cap) resize();  // só redimensiona se necessário
        data[tail++ & (cap-1)] = x;
    }

    par  front()  { return data[head & (cap-1)]; }
    void pop()    { head++; }
    bool empty()  { return head == tail; }
    int  size()   { return tail - head; }
    void clear()  { head = tail = 0; }
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