#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct _klv_bucket_queue_DK{
    list<par> **bucket; //
    nptr *ptr;
    int *actLv; //
    ll *size;   //
    ll *act;    //     // bucket ativos
    int sz;
    int lv;
    int n;
 
    // inicia os buckets
    _klv_bucket_queue_DK(keyType c, int n_, int k){                  // c = maior peso
        lv = k;
        n = n_;
        int sqrtSize = pow(c + 1.0, 1.0/k) + 1;

        size   = new ll[k];
        act    = new ll[k];
        actLv  = new int[k];
        bucket = new list<par>*[k];

        size[0] = sqrtSize;
        ll aux = size[0];
        for(int i=0;i<k;i++) {
            act[i]    = 0;
            actLv[i]  = 0;
            bucket[i] = new list<par>[size[0]];
            if(i > 0) {
                size[i] = aux;
                aux *= size[0];
            }
        }
        
        ptr = new nptr[n];
        for(int i = 0; i < n; i++)
            ptr[i] = {-1, -1, list<par>::iterator{}};

        sz = 0;
    };
 
    void insert(int v, keyType dist, keyType w){
        sz++;
        
        ll lvs;
        for(int i=0;i<lv-2;i++){
            lvs = dist/size[lv-i-1] % size[0];
            if(lvs != act[i]){
                bucket[i][lvs].push_back({dist, v});
                ptr[v].lvl = i;
                ptr[v].idx = lvs;
                ptr[v].it = prev(bucket[i][lvs].end());
                actLv[i]++;
                return;
            }
        }

        lvs = dist/size[lv-2] % size[0];
        ll last_lvs = dist % size[0];
        if (lvs != act[lv-2] || last_lvs < act[lv-1]){
            bucket[lv-2][lvs].push_back({dist, v});
            ptr[v].lvl = lv-2;
            ptr[v].idx = lvs;
            ptr[v].it = prev(bucket[lv-2][lvs].end());
            actLv[lv-2]++;
        }
        else{
            bucket[lv-1][last_lvs].push_back({dist, v});
            ptr[v].lvl = lv-1;
            ptr[v].idx = last_lvs;
            ptr[v].it = prev(bucket[lv-1][last_lvs].end());
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
        do{
            if(!bucket[aux][b].empty()) break;
            b = (b + 1) % size[0];
        } while(b!=act[aux]);
        act[aux] = b;
        assert(b < size[0]);
        assert(!bucket[aux][b].empty());

        // distribuir até chegarem ao último nível
        for(int i=aux; i<lv-1; i++){
            act[i+1] = size[0];
            //cout << bucket[i].size() << " " << act[i] << endl;
            while(!bucket[i][act[i]].empty()){
                par v = bucket[i][act[i]].front();
                bucket[i][act[i]].pop_front();
                actLv[i]--;

                ll novo = v.first % size[0];
                bucket[i+1][novo].push_back(v);
                actLv[i+1]++;
                ptr[v.second].lvl = i+1;
                ptr[v.second].idx = novo;
                ptr[v.second].it = prev(bucket[i+1][novo].end());
                act[i+1] = min(act[i+1], novo);
            }
        }
    }
 
    par extract_min(){
        update();

        assert(!bucket[lv-1][act[lv-1]].empty());
        assert(act[lv-1]>=0 && act[lv-1]<size[0]);
        par min_elem = bucket[lv-1][act[lv-1]].front();
        bucket[lv-1][act[lv-1]].pop_front();
        ptr[min_elem.second].lvl = -1;
        ptr[min_elem.second].idx = -1;
        actLv[lv-1]--;
        sz--;
        ptr[min_elem.second] = {-1, -1, list<par>::iterator{}};
        return min_elem;
    }
 
    bool empty() {
        if (sz) return false;
        return true;
    }

    void clear(){
        for(int i = 0; i < lv; i++) {
            for(int j = 0; j < size[0]; j++)
                bucket[i][j].clear();

            actLv[i] = 0;
            act[i]   = 0;
        }

        for(int i=0;i<n;i++) ptr[i] = {-1, -1, list<par>::iterator{}};
        sz = 0;
    }

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){

        if(ptr[u].lvl == -1){ // elemento não está na lista, então insere
            insert(u, new_du, w);
            return;
        }

        bucket[ptr[u].lvl][ptr[u].idx].erase(ptr[u].it);
        actLv[ptr[u].lvl]--;
        sz--;
        ptr[u] = {-1, -1, list<par>::iterator{}};
        insert(u, new_du, w);
    }

};

struct _klv_bucket_queue{
    queue<par> **bucket; //
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

        size   = new ll[k];
        act    = new ll[k];
        actLv  = new int[k];
        bucket = new queue<par>*[k];

        size[0] = sqrtSize;
        ll aux = size[0];
        for(int i=0;i<k;i++) {
            act[i]    = 0;
            actLv[i]  = 0;
            bucket[i] = new queue<par>[size[0]];
            if(i > 0) {
                size[i] = aux;
                aux *= size[0];
            }
        }

        sz = 0;
    };
 
    void insert(int v, keyType dist, keyType w){
        sz++;
        
        ll lvs;
        for(int i=0;i<lv-2;i++){
            lvs = dist/size[lv-i-1] % size[0];
            if(lvs != act[i]){
                bucket[i][lvs].push({dist, v});
                actLv[i]++;
                return;
            }
        }

        lvs = dist/size[lv-2] % size[0];
        ll last_lvs = dist % size[0];
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
         do{
            if(!bucket[aux][b].empty()) break;
            b = (b + 1) % size[0];
        } while(b!=act[aux]);
        act[aux] = b;
        assert(b < size[0]);
        assert(!bucket[aux][b].empty());

        // distribuir até chegarem ao último nível
        for(int i=aux; i<lv-1; i++){
            act[i+1] = size[0];
            //cout << bucket[i].size() << " " << act[i] << endl;
            while(!bucket[i][act[i]].empty()){
                par v = bucket[i][act[i]].front();
                bucket[i][act[i]].pop();
                actLv[i]--;

                ll novo = v.first % size[0];
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
        par min_elem = bucket[lv-1][act[lv-1]].front();
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