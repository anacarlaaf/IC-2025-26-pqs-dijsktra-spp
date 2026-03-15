#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct _2lv_bucket_queue_DK{
    bkt *top_bucket, *bottom_bucket;
    pool_list pool;
    int *qBucket;
    double delta;

    int sz=0, n=0;
    ll at = 0, ab = 0; // at = top bucket ativo, ab = bottom bucket ativo
    ll b_size = 0;     // tamanho dos buckets

    // inicia os buckets
    
    _2lv_bucket_queue_DK(keyType c, int n_, double delta_){                  // c = maior peso
        n = n_;
        b_size = sqrt(c + 1) + 1;
        ll aux = 1;
        while(aux < b_size) aux <<= 1;
        b_size = aux;
        top_bucket = new bkt[b_size];
        bottom_bucket = new bkt[b_size];
        qBucket = new int[n];
        pool = pool_list(n);
        delta = delta_;
        for(int i=0;i<b_size;i++){
            top_bucket[i].sz = bottom_bucket[i].sz = 0;
            top_bucket[i].tail = bottom_bucket[i].tail = -1;
        }
    };

    ~_2lv_bucket_queue_DK() {
        delete[] top_bucket;
        delete[] bottom_bucket;
        delete[] qBucket;
        delete[] pool.pool;
        delete[] pool.free_list;
        delete[] pool.idxs;
    }
 
    void insert(int v, keyType dist, keyType w){
        ll key = floor(dist/delta);
        ll i =  (key/ b_size) & (b_size-1);  // se i = top bucket ativo, insere no bottom
        ll j = key & (b_size-1);            // se não, insere no top
    
        if (i == at && j >= ab) {
            bottom_bucket[j].tail = pool.insert({key,{dist,v}}, bottom_bucket[j].tail);
            bottom_bucket[j].sz++;
            qBucket[v] = b_size+j; // pseudo índice do bottom bucket; 
        }
        else{
            top_bucket[i].tail = pool.insert({key,{dist,v}}, top_bucket[i].tail);
            top_bucket[i].sz++;
            qBucket[v] = i;
        }
        sz++;
    }
 
    void update(){
        // procura um bottom bucket não vazio
        while(ab < b_size && bottom_bucket[ab].sz==0) ab++;
        if(ab < b_size) return;

         
        // expand: se nao encontrar, distribui elementos de outro top bucket
        ll start = at;
        do {
            if(top_bucket[at].sz != 0) break;
            at++;
            if(at == b_size) at = 0;
        }  while(at != start);
        if(!top_bucket[at].sz) return;
 
        // distribui no bottom_bucket apenas os atuais, ignorando as novas inserções 
        int aux = top_bucket[at].sz;
        ab = b_size;
        int mod_b_size = b_size-1;
        for(int i=0;i<aux;i++){
            par a = pool.pool[top_bucket[at].tail].data; 
            top_bucket[at].tail = pool.pop(top_bucket[at].tail);
            top_bucket[at].sz--;

            ll nova_ab = a.first & (mod_b_size); // a.first % b_size;
            ab = min(ab, nova_ab);
            bottom_bucket[nova_ab].tail = pool.insert(a, bottom_bucket[nova_ab].tail);
            qBucket[a.second.second] = b_size+nova_ab;
            bottom_bucket[nova_ab].sz++;
        }
    }
 
    par extract_min(){
        update();
        par min_elem = pool.pool[bottom_bucket[ab].tail].data;
        bottom_bucket[ab].tail = pool.pop(bottom_bucket[ab].tail);
        bottom_bucket[ab].sz--;
        sz--;
        return min_elem;
    }
 
    bool empty() {
        return sz == 0;
    }

    void clear(){
        for(int i = 0; i < b_size; i++){
            top_bucket[i].tail = bottom_bucket[i].tail = -1;
            top_bucket[i].sz = bottom_bucket[i].sz = 0;
        }
        pool.clear(n);
        at = ab = sz = 0;
    }

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        if(pool.idxs[u] != -1) {
            int k = qBucket[u];
            int idx = pool.idxs[u];

            // Se u é o tail do bucket, atualiza o tail para o prev
            if(k < b_size) {
                if(top_bucket[k].tail == idx)
                    top_bucket[k].tail = pool.pool[idx].prev;
                top_bucket[k].sz--;
            } else {
                int kb = k & (b_size - 1);
                if(bottom_bucket[kb].tail == idx)
                    bottom_bucket[kb].tail = pool.pool[idx].prev;
                bottom_bucket[kb].sz--;
            }

            pool.remove(u);
            sz--;
        }
        insert(u, new_du, w);
    }
};