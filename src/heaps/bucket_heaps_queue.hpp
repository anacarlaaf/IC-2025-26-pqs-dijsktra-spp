#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct _1lv_bucket_queue{
    stack<par> *b;
 
    int b_size = 0;
    int a = 0, sz=0;

    _1lv_bucket_queue(keyType c_, int n){
        b_size = c_;
        ll aux = 1;
        while(aux < b_size) aux <<=1;
        b_size = aux;
        b = new stack<par>[b_size];
    };

    ~_1lv_bucket_queue() {
        delete[] b;
    }
 
    void clear() {
        for(int i=0;i<b_size;i++) while(!b[i].empty()) b[i].pop();
        a = 0, sz=0;
    }
    
    void insert(int u, keyType du, keyType w) {
        int id = (a+w) & (b_size-1);
        b[id].push({du,u});
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
        par u = b[a].top();
        b[a].pop();
        sz--;
        return u;
    }
 
    bool empty() {
        return sz==0;
    }
};

struct _2lv_bucket_queue{
    stack<par> *bucket[2]; // 2 arrays de buckets alocados em um mesmo array
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
        bucket[0] = new stack<par>[b_size];
        bucket[1] = new stack<par>[b_size];
    };

    ~_2lv_bucket_queue(){
        delete[] bucket[0];
        delete[] bucket[1];
    }
 
    void insert(int v, keyType dist, keyType w){
        ll i =  (dist / b_size) & (b_size-1); // se i = top bucket ativo, insere no bottom
        ll j = dist & (b_size-1);            // se não, insere no top
    
        if (i == at && j >= ab) {
            bucket[1][j].push({dist,v});
        }
        else{
            bucket[0][i].push({dist,v});
        }
        sz++;
    }
 
    void update(){
        // procura um bottom bucket não vazio
        while(ab < b_size && bucket[1][ab].empty()) ab++;
        if(ab < b_size) return;

         
        // expand: se nao encontrar, distribui elementos de outro top bucket
        ll start = at;
        do {
            if(bucket[0][at].size()) break;
            at++;
            if(at == b_size) at = 0;
        }  while(at != start);
        if(!bucket[0][at].size()) return;
 
        // distribui no bottom_bucket apenas os atuais, ignorando as novas inserções 
        int aux = bucket[0][at].size();
        ab = b_size;
        for(int i=0;i<aux;i++){
            auto a = bucket[0][at].top(); bucket[0][at].pop();
            ll nova_ab = a.first & (b_size-1); // a.first % b_size;
            ab = min(ab, nova_ab);
            bucket[1][nova_ab].push(a);  // insere no bottom bucket
        }
    }
 
    par extract_min(){
        update();
        par min_elem = bucket[1][ab].top();
        bucket[1][ab].pop();
        sz--;
        return min_elem;
    }
 
    bool empty() {
        return sz == 0;
    }

    void clear(){
        for(int i=0;i<b_size;i++){
            while(!bucket[0][i].empty()) bucket[0][i].pop();
            while(!bucket[1][i].empty()) bucket[1][i].pop();
        }

        at = 0; ab = 0; sz=0;
    }
};


struct _klv_bucket_queue{
    stack<par> **bucket; //
    int *actLv; //
    ll *size;   //
    ll *act;    //     // bucket ativos
    int sz;
    int lv;
    int n;
 
    // inicia os buckets
    _klv_bucket_queue(keyType c, int k){                  // c = maior peso
        lv = k;
        int sqrtSize = pow(c + 1.0, 1.0/k) + 1;
        ll aux = 1;
        while(aux < sqrtSize) aux <<=1;

        size   = new ll[k];
        act    = new ll[k];
        actLv  = new int[k];
        bucket = new stack<par>*[k];

        size[0] = aux;
        aux = size[0];
        for(int i=0;i<k;i++) {
            act[i]    = 0;
            actLv[i]  = 0;
            bucket[i] = new stack<par>[size[0]];
            if(i > 0) {
                size[i] = aux;
                aux *= size[0];
            }
        }

        sz = 0;
    };

    ~_klv_bucket_queue(){
        for(int i=0;i<lv;i++){
            delete[] bucket[i];
        }
        delete[] actLv;
        delete[] size;
        delete[] act;
        
    };
 
    void insert(int v, keyType dist, keyType w){
        sz++;
        
        ll lvs;
        int aux = size[0]-1;
        for(int i=0;i<lv-2;i++){
            lvs = (dist/size[lv-i-1]) & aux;
            if(lvs != act[i]){
                bucket[i][lvs].push({dist, v});
                actLv[i]++;
                return;
            }
        }

        lvs = (dist/size[lv-2]) & (size[0]-1);
        ll last_lvs = dist & aux;
        if (lvs != act[lv-2] || last_lvs < act[lv-1]){
            bucket[lv-2][lvs].push({dist, v});
            actLv[lv-2]++;
        }
        else{
            bucket[lv-1][last_lvs].push({dist, v});
            actLv[lv-1]++;
        }
    }
 
    void update(){
        
        // procura bucket não vazio no nível mais baixo
        while(act[lv-1] < size[0] && bucket[lv-1][act[lv-1]].empty()) act[lv-1]++;
        if(act[lv-1] < size[0]) return;


        // se não encontrar, expandir
        // procurar nível não vazio
        int aux = lv-2;
        while(aux>=0 && actLv[aux] <= 0) aux--;

        // atualiza bucket ativo
        int b = act[aux];
        int mod = size[0]-1;
         do{
            if(!bucket[aux][b].empty()) break;
            b = (b + 1) & mod;
        } while(b!=act[aux]);
        act[aux] = b;
        assert(b < size[0]);
        assert(!bucket[aux][b].empty());

        // distribuir até chegarem ao último nível
        for(int i=aux; i<lv-1; i++){
            act[i+1] = size[0];
            while(!bucket[i][act[i]].empty()){
                par v = bucket[i][act[i]].top();
                bucket[i][act[i]].pop();
                actLv[i]--;

                ll novo;
                if(i+1 == lv-1)
                    novo = v.first & mod;
                else
                    novo = (v.first / size[lv-i-2]) & mod;
                
                bucket[i+1][novo].push(v);
                actLv[i+1]++;
                act[i+1] = min(act[i+1], novo);
            }
        }
    }
 
    par extract_min(){
        update();

        assert(!bucket[lv-1][act[lv-1]].empty());
        assert(act[lv-1]>=0 && act[lv-1]<size[0]);
        par min_elem = bucket[lv-1][act[lv-1]].top();
        bucket[lv-1][act[lv-1]].pop();
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
                while(!bucket[i][j].empty()) bucket[i][j].pop();
            actLv[i] = 0;
            act[i]   = 0;
        }
        sz = 0;
    }
    
};