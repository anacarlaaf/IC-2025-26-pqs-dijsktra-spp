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
            op.ins--;
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
            op.ins--;
        }
        insert(u, new_du, w);
    }
};

struct _klv_bucket_queue_DK{
 
    pool_list pool;
    bkt **bucket;
    pair<int,int> *qBucket;
    int *actBucket; // bucket ativo em cada nível
    keyType *width; // largura dos buckets de cada nível
    keyType *lowerB;// lower bounds de cada nível
    keyType *upperB;// upper bounds de cada nível
    int *sizeLv;    // qtd de elementos por nível 
    int r;          // rounds
    int size;       // total na estrutura
    int k;          // níveis
    keyType d;      // qtd de buckets por nível
    ops op;
 
    // inicializa as estruturas
    _klv_bucket_queue_DK(keyType c, int n, int niveis) {
        k = niveis;
        r=0;
        size = 0;
        d = ceil(pow((double)(c+1), 1.0 / k)) + 1;
 
        actBucket  = new int[k]();
        width      = new keyType[k]();
        lowerB     = new keyType[k]();
        upperB     = new keyType[k]();
        qBucket = new pair<int,int>[n];
        sizeLv     = new int[k]();
        bucket  = new bkt*[k];
        pool    = pool_list(n);
 
        width[0] = 1;
        for (int i=1;i<k;i++) width[i] = width[i-1]*d;
        
        lowerB[k-1] = 0;
        upperB[k-1] = d * width[k-1] - 1;
        for(int i=k-2;i>=0;i--) updBounds(i);
 
        for (int i=0;i<k;i++){
            bucket[i] = new bkt[d]();
            for (int j = 0; j < d; j++) bucket[i][j].tail = -1;
        }
        for(int i = 0; i < n; i++) qBucket[i] = {-1, -1};
    }
    
 
    // destroi estruturas
    ~_klv_bucket_queue_DK(){
        for(int i=0;i<k;i++){
            delete[] bucket[i];
        }
        delete[] bucket;
        delete[] actBucket;        
        delete[] width;
        delete[] lowerB;
        delete[] upperB;
        delete[] sizeLv;
        delete[] qBucket;
        pool.del();
    };
 
    void updTopBounds(){
        lowerB[k-1] = r * d * width[k-1];
        upperB[k-1] = lowerB[k-1] + d * width[k-1] - 1;
    }
 
    void updBounds(int lv){
        lowerB[lv] = lowerB[lv+1] + actBucket[lv+1] * width[lv+1];
        upperB[lv] = lowerB[lv] + d * width[lv] - 1;
    }
 
    void insert(int v, keyType dist){
        // procura nível em que dist está dentro dos bounds
        op.ins++;

        for(int i=0;i<k;i++){
            if(lowerB[i] <= dist && dist <= upperB[i]){
                int j = (int)((dist - lowerB[i]) / width[i]);
                assert(0 <= j && j < d);
                pool.insert({dist,v}, &bucket[i][j]);
                qBucket[v] = {i,j};
                sizeLv[i]++;
                size++;
                return;
            }
        }
 
        // se não estiver nos bounds, wrap-around no top-level
        int j = (dist - upperB[k-1] - 1) / width[k-1];
        assert(0 <= j && j < d);
        pool.insert({dist,v}, &bucket[k-1][j]);
        qBucket[v] = {k-1,j};
        sizeLv[k-1]++;
        size++;
    }
 
    void update() {
        // procura bucket não vazio no bottom level

        op.upd++;
        while(actBucket[0] < d && bucket[0][actBucket[0]].sz==0){
            actBucket[0]++;
            op.bkemp++;
        }
        if(actBucket[0] < d) return;

        op.upd++;
 
        // encontra o nível mais baixo não vazio
        int f = 1;
        while(f < k && sizeLv[f] == 0) f++;
 
        // se top-level e ele está vazio, próximo round
        if(f == k-1) {
            while(actBucket[f] < d && bucket[f][actBucket[f]].sz==0){
                actBucket[f]++;
                op.upd++;
            }
 
            if(actBucket[f] == d) {
                r++;
                updTopBounds();
                actBucket[f] = 0;
                while(actBucket[f] < d && bucket[f][actBucket[f]].sz==0){
                    actBucket[f]++;
                    op.upd++;
                }
                assert(actBucket[f] < d);
            }
            // atualiza bounds dos níveis abaixo antes de expandir
            for(int idx = k-2; idx >= 0; idx--) updBounds(idx);
        }
 
        // expande do nível f até o nível 1
        for(int i = f; i > 0; i--) {
            while(actBucket[i] < d && bucket[i][actBucket[i]].sz==0)
                actBucket[i]++;
 
            assert(actBucket[i] < d);
 
            // atualiza os bounds do nível abaixo
            updBounds(i-1);
            int new_act = d;
 
            // distribui elementos no nível abaixo
            int src = actBucket[i];
            while(bucket[i][src].sz) {
                int idx = bucket[i][src].tail;
                par elem = pool.pool[idx].data;
                pool.pop(&bucket[i][src]);
                sizeLv[i]--;
 
                int target = (elem.first - lowerB[i-1]) / width[i-1];
                assert(0 <= target && target < d);
 
                new_act = min(new_act, target);
                pool.insert(elem, &bucket[i-1][target]);
                qBucket[elem.second] = {i-1, target};
                sizeLv[i-1]++;
            }
 
            if(new_act < d) actBucket[i-1] = new_act;
        }
    }
 
    par extract_min(){
        op.exmin++;
        update();
        int j = actBucket[0];
        assert(j < d && bucket[0][j].sz);
        par v = pool.pool[bucket[0][actBucket[0]].tail].data;
        qBucket[v.second] = {-1,-1};
        pool.pop(&bucket[0][actBucket[0]]);
        sizeLv[0]--;
        size--;
        return v;
    }
 
    bool empty() { return size == 0; }
 
    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        if(pool.idxs[u]!=-1){
            pair<int,int> loc = qBucket[u];
            pool.remove(u, &bucket[loc.first][loc.second]);
            sizeLv[loc.first]--;
            size--;
            op.dk++;
            op.ins--;
        }
        insert(u, new_du);
    }
};