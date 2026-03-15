#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct _1lv_bucket_queue{
    vector<par> *b;
 
    int b_size = 0;
    int a = 0, sz=0;

    _1lv_bucket_queue(keyType c_, int n){
        b_size = c_;
        ll aux = 1;
        while(aux < b_size) aux <<=1;
        b_size = aux;
        b = new vector<par>[b_size];
    };

    ~_1lv_bucket_queue() {
        delete[] b;
    }
 
    void clear() {
        for(int i=0;i<b_size;i++) while(!b[i].empty()) b[i].pop_back();
        a = 0, sz=0;
    }
    
    void insert(int u, keyType du, keyType w) {
        int id = (a+w) & (b_size-1);
        b[id].push_back({du,u});
        sz++;
    }

    void update() {
        if (b[a].size()) return;

        int bg = a;
        int aux = b_size-1;
        do {
            a = (a + 1) & (aux);
            if (b[a].size()) return;
        } while (a != bg);

        assert(false);
    }
 
    par extract_min() {
        update();
        par u = b[a].back();
        b[a].pop_back();
        sz--;
        return u;
    }
 
    bool empty() {
        return sz==0;
    }
};


struct _1lv_bucket_queue_DK{
    bkt *bucket;
    pool_list pool;
    int sz=0, n=0;
    ll a=0; // at = top bucket ativo, ab = bottom bucket ativo
    ll b_size = 0;     // tamanho dos buckets
    int *qBucket;

    // inicia os buckets
    
    _1lv_bucket_queue_DK(keyType c, int n_){                  // c = maior peso
         n = n_;
        b_size = sqrt(c + 1) + 1;
        ll aux = 1;
        while(aux < b_size) aux <<= 1;
        b_size = aux;
        bucket = new bkt[b_size];
        qBucket = new int[n];
        pool = pool_list(n);
        for(int i=0;i<b_size;i++){
            bucket[i].sz = 0;
            bucket[i].tail = -1;
        }
    };

    ~_1lv_bucket_queue_DK() {
        delete[] bucket;
        delete[] qBucket;
        delete[] pool.pool;
        delete[] pool.free_list;
        delete[] pool.idxs;
    }

    void insert(int v, keyType dist, keyType w){
        ll id = (a+w) & (b_size-1);
        bucket[id].tail = pool.insert({dist,v}, bucket[id].tail);
        bucket[id].sz++;
        qBucket[v] = id;
        sz++;
    }

    void update(){

        if(bucket[a].sz) return;

        ll aux = a;
        int mod = b_size-1;
        do{
            a = (a+1) & mod;
            if (bucket[a].sz) return;
        } while(a != aux);

        assert(false);
    }

    par extract_min(){
        update();
        par min_elem = pool.pool[bucket[a].tail].data;
        bucket[a].tail = pool.pop(bucket[a].tail);
        bucket[a].sz--;
        sz--;
        return min_elem;
    }
 
    bool empty() {
        return sz == 0;
    }

    void clear(){
        for(int i = 0; i < b_size; i++){
            bucket[i].tail = -1;
            bucket[i].sz = 0;
        }

        for(int i = 0; i < n; i++)
            qBucket[i] = -1;

        pool.clear(n);
        a = sz = 0;
    }

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        if(pool.idxs[u] != -1) {
            int k = qBucket[u];
            int idx = pool.idxs[u];

            // Se u é o tail do bucket, atualiza o tail para o prev
            if (bucket[k].tail == idx) bucket[k].tail = pool.pool[idx].prev;
            bucket[k].sz--;
            pool.remove(u);
            sz--;
        }
        insert(u, new_du, w);
    }
};