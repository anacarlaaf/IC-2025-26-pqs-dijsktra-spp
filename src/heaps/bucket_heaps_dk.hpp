#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct _1lv_bucket_queue_DK{
    bkt *bucket;
    pool_list pool;
    int *qBucket;

    int sz=0, n=0;
    ll a=0; // at = top bucket ativo, ab = bottom bucket ativo
    ll b_size = 0;     // tamanho dos buckets

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
        pool.del();
    }

    void insert(int v, keyType dist, keyType w){
        ll id = (a + w) & (b_size - 1);  // usa dist, não a+w
        pool.insert({dist, v}, &bucket[id]);
        qBucket[v] = id;
        sz++;
    }

    void update(){
        if (bucket[a].sz) return;

        int bg = a;
        int aux = b_size-1;
        do {
            a = (a + 1) & (aux);
            if (bucket[a].sz) return;
        } while (a != bg);

        assert(false);
    }

    par extract_min(){
        update();
        par min_elem = pool.pool[bucket[a].tail].data;
        pool.pop(&bucket[a]);
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
            pool.remove(u, &bucket[k]);
            sz--;
        }
        insert(u, new_du, w);
    }
};

struct _2lv_bucket_queue_DK{
    bkt *top_bucket, *bottom_bucket;
    pool_list pool;
    int *qBucket;

    int sz=0, n=0;
    ll at = 0, ab = 0; // at = top bucket ativo, ab = bottom bucket ativo
    ll b_size = 0;     // tamanho dos buckets

    // inicia os buckets
    
    _2lv_bucket_queue_DK(keyType c, int n_){                  // c = maior peso
        n = n_;
        b_size = sqrt(c + 1) + 1;
        ll aux = 1;
        while(aux < b_size) aux <<= 1;
        b_size = aux;
        top_bucket = new bkt[b_size];
        bottom_bucket = new bkt[b_size];
        qBucket = new int[n];
        pool = pool_list(n);
        for(int i=0;i<b_size;i++){
            top_bucket[i].sz = bottom_bucket[i].sz = 0;
            top_bucket[i].tail = bottom_bucket[i].tail = -1;
        }
    };

    ~_2lv_bucket_queue_DK() {
        delete[] top_bucket;
        delete[] bottom_bucket;
        delete[] qBucket;
        pool.del();
    }
 
    void insert(int v, keyType dist, keyType w){
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
            pool.pop(&top_bucket[at]);

            ll nova_ab = a.first & (mod_b_size); // a.first % b_size;
            ab = min(ab, nova_ab);
            pool.insert(a, &bottom_bucket[nova_ab]);
            qBucket[a.second] = b_size+nova_ab;
        }
    }
 
    par extract_min(){
        update();
        par min_elem = pool.pool[bottom_bucket[ab].tail].data;
        pool.pop(&bottom_bucket[ab]);
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

            if(k < b_size) {
                pool.remove(u, &top_bucket[qBucket[u]]);
            } else {
                int kb = k & (b_size - 1);
                pool.remove(u, &bottom_bucket[kb]);
            }
            sz--;
        }
        insert(u, new_du, w);
    }
};

struct _klv_bucket_queue_DK{

    pool_list pool;
    pair<int,int> *qBucket;
    bkt **bucket; //
    int *actLv; //
    ll *size;   //
    ll *act;    //     // bucket ativos
    int sz;
    int lv;
 
    // inicia os buckets
    _klv_bucket_queue_DK(keyType c, int n_, int k){                  // c = maior peso
        lv = k;
        int sqrtSize = pow(c + 1.0, 1.0/k) + 1;
        ll aux = 1;
        while(aux < sqrtSize) aux <<=1;

        size   = new ll[k];
        act    = new ll[k];
        actLv  = new int[k];
        bucket = new bkt*[k];
        qBucket = new pair<int,int>[n_];
        pool = pool_list(n_);

        size[0] = aux;
        for(int i=0;i<k;i++) {
            act[i]    = 0;
            actLv[i]  = 0;
            bucket[i] = new bkt[size[0]];
            if(i > 0) {
                size[i] = aux;
                aux *= size[0];
            }

            for(int j = 0; j < size[0]; j++){
                bucket[i][j].sz   = 0;
                bucket[i][j].tail = -1;
            }
        }


        sz = 0;
    };

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
        while(act[lv-1] < size[0] && bucket[lv-1][act[lv-1]].sz==0) act[lv-1]++;
        if(act[lv-1] < size[0]) return;


        // se não encontrar, expandir
        // procurar nível não vazio
        int aux = lv-2;
        while(aux>=0 && actLv[aux] <= 0) aux--;

        // atualiza bucket ativo
        int mod = size[0]-1;
        int b = act[aux];
         do{
            if(bucket[aux][b].sz) break;
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

                ll novo;
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
        return min_elem;
    }
 
    bool empty() {
        if (sz) return false;
        return true;
    }

    void clear(){
        for(int i = 0; i < lv; i++) {
            for(int j = 0; j < size[0]; j++)
                while(bucket[i][j].sz) pool.pop(&bucket[i][j]);
            actLv[i] = 0;
            act[i]   = 0;
        }
        sz = 0;
    }

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        if(pool.idxs[u] != -1) {
            pair<int, int> loc = qBucket[u];
            pool.remove(u, &bucket[loc.first][loc.second]);
            actLv[loc.first]--;
            sz--;
        }
        insert(u, new_du, w);
    }
};