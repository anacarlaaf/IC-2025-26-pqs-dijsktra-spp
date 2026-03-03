#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct _klv_bucket_queue_DK{
    vector<vector<list<par>>> bucket; //
    vector<nptr> ptr;
    vector<int> actLv; //
    vector<ll> size;   //
    vector<ll> act;    //     // bucket ativos
    int sz;
    int lv;
 
    // inicia os buckets
    _klv_bucket_queue_DK(keyType c, int n, int k){                  // c = maior peso
        lv = k;
        int sqrtSize = pow(c + 1.0, 1.0/k);
        size.resize(k, 0);
        size[0] = sqrtSize; // potência de 2
        bucket.resize(k, {});
        actLv.resize(k, 0);
        act.resize(k, 0);

        for(int i=0;i<k;i++) {
            bucket[i].resize(size[0], {});
            act[i] = 0;
            actLv[i] = 0;
        }
        ptr.resize(n, {-1, -1, list<par>::iterator{}});

        ll aux = size[0];
        for(int i=1;i<k;i++){
            size[i] = aux;
            aux *= size[0];
        }
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
            if(b >= size[0]) b = 0;
            if(!bucket[aux][b].empty()) break;
            b++;
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
        for(int i=0;i<lv;i++) {
            for (auto &l : bucket[i]) l.clear();
            actLv[i] = 0;
            act[i] = 0;
        }
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
    vector<vector<queue<par>>> bucket; //
    vector<int> actLv; //
    vector<ll> size;   //
    vector<ll> act;    //     // bucket ativos
    int sz;
    int lv;
 
    // inicia os buckets
    _klv_bucket_queue(keyType c, int k){                  // c = maior peso
        lv = k;
        int sqrtSize = pow(c + 1.0, 1.0/k);
        size.resize(k, 0);
        size[0] = sqrtSize; // potência de 2
        bucket.resize(k, {});
        actLv.resize(k, 0);
        act.resize(k, 0);

        for(int i=0;i<k;i++) {
            bucket[i].resize(size[0], {});
            act[i] = 0;
            actLv[i] = 0;
        }

        ll aux = size[0];
        for(int i=1;i<k;i++){
            size[i] = aux;
            aux *= size[0];
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
            if(b >= size[0]) b = 0;
            if(!bucket[aux][b].empty()) break;
            b++;
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
        for(int i=0;i<lv;i++) {
            for (auto &q : bucket[i]) while (!q.empty()) q.pop();
            actLv[i] = 0;
            act[i] = 0;
        }
    }
};