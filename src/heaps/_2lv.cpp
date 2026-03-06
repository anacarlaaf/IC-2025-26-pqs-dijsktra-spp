#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct _2lv_bucket_queue{
    cbuffer *bucket; // 2 arrays de buckets alocados em um mesmo array
    //queue<par> *top_bucket, *bottom_bucket;

    int sz=0;
    ll at = 0, ab = 0; // at = top bucket ativo, ab = bottom bucket ativo
    ll b_size = 0;     // tamanho dos buckets

    // inicia os buckets
    
    _2lv_bucket_queue(keyType c, int n){                  // c = maior peso
        b_size = sqrt(c + 1) + 1;
        ll aux = 1;
        while(aux < b_size) aux <<= 1;
        b_size = aux;
        bucket = new cbuffer[2*b_size];
        aux = ceil(n/b_size);
        for(int i=0;i<2*b_size;i++) bucket[i] = cbuffer(n/b_size);
    };
 
    void insert(int v, keyType dist, keyType w){
        ll i =  (dist / b_size) & (b_size-1); // se i = top bucket ativo, insere no bottom
        ll j = dist & (b_size-1);            // se não, insere no top
    
        if (i == at && j >= ab) {
            bucket[b_size+j].push({dist,v});
        }
        else{
            bucket[i].push({dist,v});
        }
        sz++;
    }
 
    void update(){
        // procura um bottom bucket não vazio
        while(ab < b_size && bucket[b_size+ab].empty()) ab++;
        if(ab < b_size) return;

         
        // expand: se nao encontrar, distribui elementos de outro top bucket
        ll start = at;
        do {
            if(bucket[at].size()) break;
            at++;
            if(at == b_size) at = 0;
        }  while(at != start);
        if(!bucket[at].size()) return;
 
        // distribui no bottom_bucket apenas os atuais, ignorando as novas inserções 
        int aux = bucket[at].size();
        ab = b_size;
        for(int i=0;i<aux;i++){
            auto a = bucket[at].front(); bucket[at].pop();
            ll nova_ab = a.first & (b_size-1); // a.first % b_size;
            ab = min(ab, nova_ab);
            bucket[b_size+nova_ab].push(a);  // insere no bottom bucket
        }
    }
 
    par extract_min(){
        update();
        par min_elem = bucket[b_size+ab].front();
        bucket[b_size+ab].pop();
        sz--;
        return min_elem;
    }
 
    bool empty() {
        return sz == 0;
    }

    void clear(){
        for(int i=0;i<b_size;i++){
            while(!bucket[i].empty()) bucket[i].pop();
            while(!bucket[b_size+i].empty()) bucket[b_size+i].pop();
        }

        at = 0; ab = 0; sz=0;
    }
};