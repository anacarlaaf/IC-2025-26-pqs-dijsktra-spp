#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct _2lv_bucket_queue{
    cbuffer* bucket[2];   // 2 arrays de buckets alocados em um mesmo array
    bitmask bit[2];

    int sz=0;
    ll at = 0, ab = 0; // at = top bucket ativo, ab = bottom bucket ativo
    ll b_size = 0;     // tamanho dos buckets

    // inicia os buckets
    
    _2lv_bucket_queue(keyType c, int n){                  // c = maior peso
        b_size = sqrt(c + 1) + 1;
        ll aux = 1;
        while(aux < b_size) aux <<= 1;
        b_size = aux;
        bucket[0] = new cbuffer[b_size];
        bucket[1] = new cbuffer[b_size];
        for(int i=0;i<b_size;i++) {
            bucket[0][i] = cbuffer(n/b_size);
            bucket[1][i] = cbuffer(n/b_size);
        }
        bit[0].init(b_size);
        bit[1].init(b_size);
    };
 
    void insert(int v, keyType dist, keyType w){
        ll i =  (dist / b_size) & (b_size-1); // se i = top bucket ativo, insere no bottom
        ll j = dist & (b_size-1);            // se não, insere no top
    
        if (i == at && j >= ab) {
            bucket[1][j].push({dist,v});
            bit[1].set(j); // set do bit para indicar que o bucket tem elemento
        }
        else{
            bucket[0][i].push({dist,v});
            bit[0].set(i);
        }
        sz++;
    }
 
    void update(){
        // procura um bottom bucket não vazio
        if (bit[1].is_set(ab)) return;
        ll next = bit[1].lsb(ab);
        if(next != -1){
            ab = next;
            return;
        }
         
        // expand: se nao encontrar, distribui elementos do top bucket
        next = bit[0].lsb(at);
        if(next == -1) next = bit[0].lsb(0); // circular
        if(next == -1) return; // não achou
        at = next;


        // distribui no bottom_bucket 
        int aux = bucket[0][at].size();
        ab = b_size;
        for(int i=0;i<aux;i++){
            auto a = bucket[0][at].front(); bucket[0][at].pop();
            ll nova_ab = a.first & (b_size-1); // a.first % b_size;
            ab = min(ab, nova_ab);
            bucket[1][nova_ab].push(a);  // insere no bottom bucket
            bit[1].set(nova_ab);
        }
        bit[0].unset(at);
    }
 
    par extract_min(){
        update();
        par min_elem = bucket[1][ab].front();
        bucket[1][ab].pop();
        sz--;
        if(bucket[1][ab].empty()) bit[1].unset(ab); // zera bit se o bucket ficou vazio
        return min_elem;
    }
 
    bool empty() {
        return sz == 0;
    }

    void clear(){
        for(int i=0;i<2*b_size;i++) {
            bucket[0][i].clear();
            bucket[1][i].clear();
        }
        bit[0].clear();
        bit[1].clear();
        at = 0; ab = 0; sz = 0;
    }
};