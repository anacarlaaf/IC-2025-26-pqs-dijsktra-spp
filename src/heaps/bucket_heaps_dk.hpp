#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct _1lv_bucket_queue_DK{
    ops op;
    bkt *bucket;
    pool_list pool;
    int *qBucket;

    int sz=0, n=0;
    ll a=0; // at = top bucket ativo, ab = bottom bucket ativo
    ll b_size = 0;     // tamanho dos buckets

    // inicia os buckets
    
    _1lv_bucket_queue_DK(keyType c, int n_) {
        n     = n_;
        b_size = 1 << (32 - __builtin_clz((int)c-1));
        
        bucket  = new bkt[b_size]();   // zero-init: sz=0, tail=0
        qBucket = new int[n]();
        pool    = pool_list(n);

        // tail precisa ser -1, não 0
        for (int i = 0; i < b_size; i++) bucket[i].tail = -1;
        memset(qBucket, -1, sizeof(int) * n);
    }

    ~_1lv_bucket_queue_DK() {
        delete[] bucket;
        delete[] qBucket;
        pool.del();
    }

    void insert(int v, keyType dist, keyType w){
        ll id = (a + w) & (b_size - 1); 
        pool.insert({dist, v}, &bucket[id]);
        qBucket[v] = id;
        sz++;
        op.ins++;
    }

    void update(){
        if (bucket[a].sz) return;
        
        int bg = a;
        int aux = b_size-1;
        do {
            a = (a + 1) & (aux);
            if (bucket[a].sz) return;
            op.bkemp++;
        } while (a != bg);

        assert(false);
    }

    par extract_min(){
        update();
        par min_elem = pool.pool[bucket[a].tail].data;
        pool.pop(&bucket[a]);
        sz--;
        op.exmin++;
        return min_elem;
    }
 
    bool empty() {
        return sz == 0;
    }

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        if(pool.idxs[u] != -1) {
            int k = qBucket[u];
            pool.remove(u, &bucket[k]);
            sz--;
            op.dk++;
        }
        insert(u, new_du, w);
    }
};

struct _2lv_bucket_queue_DK{
    ops op;
    bkt *top_bucket, *bottom_bucket;
    pool_list pool;
    int *qBucket;

    int sz=0, n=0;
    ll at = 0, ab = 0; // at = top bucket ativo, ab = bottom bucket ativo
    ll b_size = 0;     // tamanho dos buckets

    // inicia os buckets
    
    _2lv_bucket_queue_DK(keyType c, int n_) {
        n      = n_;
        int aux = sqrt((int)c);
        b_size = 1 << (32 - __builtin_clz(aux-1));
        top_bucket    = new bkt[b_size]();
        bottom_bucket = new bkt[b_size]();
        qBucket       = new int[n]();
        pool          = pool_list(n);

        for (int i = 0; i < b_size; i++)
            top_bucket[i].tail = bottom_bucket[i].tail = -1;
        memset(qBucket, -1, sizeof(int) * n);
    }

    ~_2lv_bucket_queue_DK() {
        delete[] top_bucket;
        delete[] bottom_bucket;
        delete[] qBucket;
        pool.del();
    }
 
    void insert(int v, keyType dist, keyType w){
        op.ins++;
        ll i =  (dist / b_size) & (b_size-1);  // se i = top bucket ativo, insere no bottom
        ll j = dist & (b_size-1);            // se não, insere no top
    
        if (i == at && j >= ab) {
            pool.insert({dist,v}, &bottom_bucket[j]);
            qBucket[v] = b_size+j; // pseudo índice do bottom bucket; 
        }
        else{
            pool.insert({dist,v}, &top_bucket[i]);
            qBucket[v] = i;
        }
        sz++;
    }
 
    void update(){
        // procura um bottom bucket não vazio
        while(ab < b_size && bottom_bucket[ab].sz==0) {
            ab++;
            op.bkemp++;
        }
        if(ab < b_size) return;

        op.upd++;
         
        // expand: se nao encontrar, distribui elementos de outro top bucket
        ll start = at;
        do {
            if(top_bucket[at].sz != 0) break;
            at++;
            op.bkemp++;
            if(at == b_size) at = 0;
        }  while(at != start);
        if(!top_bucket[at].sz) return;
 
        // distribui no bottom_bucket apenas os atuais, ignorando as novas inserções 
        int aux = top_bucket[at].sz;
        ab = b_size;
        int mod_b_size = b_size-1;
        for(int i=0;i<aux;i++){
            par a = pool.pool[top_bucket[at].tail].data; 
            pool.pop(&top_bucket[at]);

            ll nova_ab = a.first & (mod_b_size); // a.first % b_size;
            ab = min(ab, nova_ab);
            pool.insert(a, &bottom_bucket[nova_ab]);
            qBucket[a.second] = b_size+nova_ab;
        }
    }
 
    par extract_min(){
        update();
        op.exmin++;
        par min_elem = pool.pool[bottom_bucket[ab].tail].data;
        pool.pop(&bottom_bucket[ab]);
        sz--;
        return min_elem;
    }
 
    bool empty() {
        return sz == 0;
    }

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        if(pool.idxs[u] != -1) {
            int k = qBucket[u];

            if(k < b_size) {
                pool.remove(u, &top_bucket[qBucket[u]]);
            } else {
                int kb = k & (b_size - 1);
                pool.remove(u, &bottom_bucket[kb]);
            }
            sz--;
            op.dk++;
        }
        insert(u, new_du, w);
    }
};

struct _klv_bucket_queue_DK{
    
    ops op;
    pool_list pool;
    pair<int,int> *qBucket;
    bkt **bucket; //
    int *actLv; //
    ll *size;   //
    int *act;    //     // bucket ativos
    int sz;
    int lv;
 
    // inicia os buckets
    _klv_bucket_queue_DK(keyType c, int n_, int k) {
        lv = k;

        c++;
        int targ= ceil(pow((int)c, 1.0 / k));
        int aux = 1 << (32 - __builtin_clz(targ - 1));

        size    = new ll[k]();
        act     = new int[k]();
        actLv   = new int[k]();
        bucket  = new bkt*[k];
        qBucket = new pair<int,int>[n_];
        pool    = pool_list(n_);

        ll acc = aux;
        for (int i = 0; i < k; i++) {
            size[i]   = acc;
            bucket[i] = new bkt[aux]();                        // sz=0, tail=0
            for (int j = 0; j < aux; j++) bucket[i][j].tail = -1;
            acc *= aux;
        }

        memset(qBucket, -1, sizeof(pair<int,int>) * n_);
        sz = 0;
    }

    ~_klv_bucket_queue_DK(){
        for(int i=0;i<lv;i++){
            delete[] bucket[i];
        }
        delete[] bucket;
        delete[] actLv;
        delete[] size;
        delete[] act;
        delete[] qBucket;
        pool.del();
    };
 
    void insert(int v, keyType dist, keyType w){
        sz++;
        op.ins++;
        
        ll lvs;
        ll mod = size[0]-1;
        for(int i=0;i<lv-2;i++){
            lvs = (dist/size[lv-i-1]) & mod;
            if(lvs != act[i]){
                pool.insert({dist,v}, &bucket[i][lvs]);
                qBucket[v].first = i; qBucket[v].second = lvs;
                actLv[i]++;
                return;
            }
        }

        lvs = (dist/size[lv-2]) & mod;
        ll last_lvs = dist & mod;
        if (lvs != act[lv-2] || last_lvs < act[lv-1]){
            pool.insert({dist,v}, &bucket[lv-2][lvs]);
            qBucket[v].first = lv-2; qBucket[v].second = lvs;
            actLv[lv-2]++;
        }
        else{
            pool.insert({dist,v}, &bucket[lv-1][last_lvs]);
            qBucket[v].first = lv-1; qBucket[v].second = last_lvs;
            actLv[lv-1]++;
        }
    }
 
    void update(){
        // procura bucket não vazio no nível mais baixo
        while(act[lv-1] < size[0] && bucket[lv-1][act[lv-1]].sz==0) {
            act[lv-1]++;
            op.bkemp++;
        }
        if(act[lv-1] < size[0]) return;

        op.upd++;
        // se não encontrar, expandir
        // procurar nível não vazio
        int aux = lv-2;
        while(aux>=0 && actLv[aux] <= 0) aux--;

        // atualiza bucket ativo
        int mod = size[0]-1;
        int b = act[aux];
         do{
            if(bucket[aux][b].sz) break;
            op.bkemp++;
            b = (b + 1) & mod;
        } while(b!=act[aux]);
        act[aux] = b;
        assert(b < size[0]);
        assert(bucket[aux][b].sz);

        // distribuir até chegarem ao último nível
        for(int i=aux; i<lv-1; i++){
            act[i+1] = size[0];
            while(bucket[i][act[i]].sz){
                int tail_idx = bucket[i][act[i]].tail;
                par v = pool.pool[tail_idx].data;
                pool.pop(&bucket[i][act[i]]);
                actLv[i]--;

                int novo;
                if (i+1==lv-1) novo = v.first & mod;
                else novo = (v.first/size[lv-i-2]) & mod;

                pool.insert(v, &bucket[i+1][novo]);
                qBucket[v.second].first  = i + 1;
                qBucket[v.second].second = novo;
                actLv[i+1]++;
                act[i+1] = min(act[i+1], novo);
            }
        }
    }
 
    par extract_min(){
        update();
        assert(bucket[lv-1][act[lv-1]].sz);
        assert(act[lv-1]>=0 && act[lv-1]<size[0]);
        
        par min_elem = pool.pool[bucket[lv-1][act[lv-1]].tail].data;
        pool.pop(&bucket[lv-1][act[lv-1]]);
        actLv[lv-1]--;
        sz--;
        op.exmin++;
        return min_elem;
    }
 
    bool empty() {
        if (sz) return false;
        return true;
    }

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        if(pool.idxs[u] != -1) {
            pair<int, int> loc = qBucket[u];
            pool.remove(u, &bucket[loc.first][loc.second]);
            actLv[loc.first]--;
            sz--;
            op.dk++;
        }
        insert(u, new_du, w);
    }
};