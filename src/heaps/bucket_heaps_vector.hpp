#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct _1lv_bucket_queue{
    vector<par> *b;

    ops op;
    int b_size = 0;
    int a = 0, sz=0;

    _1lv_bucket_queue(keyType c) {
        b_size = 1 << (32 - __builtin_clz((int)c-1));
        b = new vector<par>[b_size];
    }

    ~_1lv_bucket_queue() {
        delete[] b;
    }
    
    void insert(int u, keyType du, keyType w) {
        int id = (a+w) & (b_size-1);
        b[id].push_back({du,u});
        sz++;
        op.ins++;
    }

    void update() {
        if (b[a].size()) return;

        int bg = a;
        int aux = b_size-1;
        do {
            a = (a + 1) & (aux);
            if (b[a].size()) return;
            op.bkemp++;
        } while (a != bg);

        assert(false);
    }
 
    par extract_min() {
        update();
        par u = b[a].back();
        b[a].pop_back();
        sz--;
        op.exmin++;
        return u;
    }
 
    bool empty() {
        return sz==0;
    }
};

struct _2lv_bucket_queue{
    vector<par> *bucket[2]; // 2 arrays de buckets alocados em um mesmo array
    //queue<par> *top_bucket, *bottom_bucket;

    ops op;
    int sz=0;
    ll at = 0, ab = 0; // at = top bucket ativo, ab = bottom bucket ativo
    ll b_size = 0;     // tamanho dos buckets

    // inicia os buckets
    _2lv_bucket_queue(keyType c) {
        int aux = sqrt((int)c);
        b_size = 1 << (32 - __builtin_clz(aux-1));
        bucket[0] = new vector<par>[b_size];
        bucket[1] = new vector<par>[b_size];
    }

    ~_2lv_bucket_queue(){
        delete[] bucket[0];
        delete[] bucket[1];
    }
 
    void insert(int v, keyType dist, keyType w){
        ll i =  (dist / b_size) & (b_size-1); // se i = top bucket ativo, insere no bottom
        ll j = dist & (b_size-1);            // se não, insere no top
        op.ins++;
        if (i == at && j >= ab) {
            bucket[1][j].push_back({dist,v});
        }
        else{
            bucket[0][i].push_back({dist,v});
        }
        sz++;
    }
 
    void update(){
        // procura um bottom bucket não vazio
        while(ab < b_size && bucket[1][ab].empty()) {
            ab++;
            op.bkemp++;
        }
        if(ab < b_size) return;

        op.upd++;
        // expand: se nao encontrar, distribui elementos de outro top bucket
        ll start = at;
        do {
            if(bucket[0][at].size()) break;
            at++;
            if(at == b_size) at = 0;
            op.bkemp++;
        }  while(at != start);
        if(!bucket[0][at].size()) return;
 
        // distribui no bottom_bucket apenas os atuais, ignorando as novas inserções 
        int aux = bucket[0][at].size();
        ab = b_size;
        for(int i=0;i<aux;i++){
            auto a = bucket[0][at].back(); bucket[0][at].pop_back();
            ll nova_ab = a.first & (b_size-1); // a.first % b_size;
            ab = min(ab, nova_ab);
            bucket[1][nova_ab].push_back(a);  // insere no bottom bucket
        }
    }
 
    par extract_min(){
        update();
        par min_elem = bucket[1][ab].back();
        bucket[1][ab].pop_back();
        sz--;
        op.exmin++;
        return min_elem;
    }
 
    bool empty() {
        return sz == 0;
    }
};


struct _klv_bucket_queue{
    vector<par> **bucket;
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
 
    _klv_bucket_queue(keyType c, int niveis) {
        k = niveis;
        r = 0;
        size = 0;
        d = ceil(pow((double)(c+1), 1.0 / k))+1;
 
        actBucket  = new int[k]();
        width      = new keyType[k]();
        lowerB     = new keyType[k]();
        upperB     = new keyType[k]();
        sizeLv     = new int[k]();
        bucket     = new vector<par>*[k];
 
        width[0] = 1;
        for(int i=1;i<k;i++) width[i] = width[i-1]*d;
 
        lowerB[k-1] = 0;
        upperB[k-1] = d * width[k-1] - 1;
        for(int i=k-2;i>=0;i--) updBounds(i);
 
        for(int i=0;i<k;i++) bucket[i] = new vector<par>[d];
    }
 
    ~_klv_bucket_queue(){
        for(int i=0;i<k;i++) delete[] bucket[i];
        delete[] bucket;
        delete[] actBucket;
        delete[] width;
        delete[] lowerB;
        delete[] upperB;
        delete[] sizeLv;
    }
 
    void updTopBounds(){
        lowerB[k-1] = r * d * width[k-1];
        upperB[k-1] = lowerB[k-1] + d * width[k-1] - 1;
    }
 
    void updBounds(int lv){
        lowerB[lv] = lowerB[lv+1] + actBucket[lv+1] * width[lv+1];
        upperB[lv] = lowerB[lv] + d * width[lv] - 1;
    }
 
    void insert(int v, keyType dist){
        op.ins++;
        // procura nível em que dist está dentro dos bounds
        for(int i=0;i<k;i++){
            if(lowerB[i] <= dist && dist <= upperB[i]){
                int j = (int)((dist - lowerB[i]) / width[i]);
                assert(0 <= j && j < d);
                bucket[i][j].push_back({dist, v});
                sizeLv[i]++;
                size++;
                return;
            }
        }
 
        // se não estiver nos bounds, wrap-around no top-level
        int j = (dist - upperB[k-1] - 1) / width[k-1];
        assert(0 <= j && j < d);
        bucket[k-1][j].push_back({dist, v});
        sizeLv[k-1]++;
        size++;
    }
 
    void update() {
        op.upd++;
        // procura bucket não vazio no bottom level
        while(actBucket[0] < d && bucket[0][actBucket[0]].empty()){
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
            while(actBucket[f] < d && bucket[f][actBucket[f]].empty()){
                actBucket[f]++;
                op.bkemp++;
            }
 
            if(actBucket[f] == d) {
                r++;
                updTopBounds();
                actBucket[f] = 0;
                while(actBucket[f] < d && bucket[f][actBucket[f]].empty()){
                    actBucket[f]++;
                    op.bkemp++;
                }
                assert(actBucket[f] < d);
            }
            // atualiza bounds dos níveis abaixo antes de expandir
            for(int idx = k-2; idx >= 0; idx--) updBounds(idx);
        }
 
        // expande do nível f até o nível 1
        for(int i = f; i > 0; i--) {
            while(actBucket[i] < d && bucket[i][actBucket[i]].empty()){
                actBucket[i]++;
                op.bkemp++;
            }
 
            assert(actBucket[i] < d);
 
            // atualiza os bounds do nível abaixo
            updBounds(i-1);
            int new_act = d;

            // distribui elementos no nível abaixo
            int src = actBucket[i];
            while(!bucket[i][src].empty()) {
                par elem = bucket[i][src].back();
                bucket[i][src].pop_back();
                sizeLv[i]--;
 
                int target = (elem.first - lowerB[i-1]) / width[i-1];
                assert(0 <= target && target < d);
 
                new_act = min(new_act, target);
                bucket[i-1][target].push_back(elem);
                sizeLv[i-1]++;
            }
 
            if(new_act < d) actBucket[i-1] = new_act;
        }
    }
 
    par extract_min(){
        op.exmin++;
        update();
        int j = actBucket[0];
        assert(j < d && !bucket[0][j].empty());
        par v = bucket[0][j].back();
        bucket[0][j].pop_back();
        sizeLv[0]--;
        size--;
        return v;
    }
 
    bool empty() { return size == 0; }
};