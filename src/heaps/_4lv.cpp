#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct _4lv_bucket_queue_DK{
    vector<list<par>> bucket[4];
    vector<nptr> ptr;
    int actLv[4];
    ll size[4];
    ll act[4];         // bucket ativos
    int sz;
 
    // inicia os buckets
    _4lv_bucket_queue_DK(keyType c, int n){                  // c = maior peso
        int sqrtSize = pow(c + 1.0, 0.25);
        size[0] = sqrtSize; // potência de 2
        for(int i=0;i<4;i++) bucket[i].resize(size[0], {});
        ptr.resize(n, {-1, -1, list<par>::iterator{}});
        memset(act, 0, sizeof(act));
        memset(actLv, 0, sizeof(actLv));
        size[1] = size[0];
        size[2] = size[0]*size[0];
        size[3] = size[2]*size[0];
        sz = 0;
    };
 
    void insert(int v, keyType dist, keyType w){
        sz++;

        ll level = dist / size[3] % size[0];
        if(level != act[0]) {
            bucket[0][level].push_back({dist, v});
            ptr[v].lvl = 0;
            ptr[v].idx = level;
            ptr[v].it = prev(bucket[0][level].end());
            actLv[0]++;
            return;
        }
        
        level = dist / size[2] % size[0];
        if (level != act[1]){
            bucket[1][level].push_back({dist, v});
            ptr[v].lvl = 1;
            ptr[v].idx = level;
            ptr[v].it = prev(bucket[1][level].end());
            actLv[1]++;
            return;
        }
        
        level = dist / size[1] % size[0];
        ll level3 = dist % size[0];
        if (level != act[2] || level3 < act[3]){
            bucket[2][level].push_back({dist, v});
            ptr[v].lvl = 2;
            ptr[v].idx = level;
            ptr[v].it = prev(bucket[2][level].end());
            actLv[2]++;
        }
        else{
            bucket[3][level3].push_back({dist, v});
            ptr[v].lvl = 3;
            ptr[v].idx = level3;
            ptr[v].it = prev(bucket[3][level3].end());
            actLv[3]++;
        }
    }
 
    void update(){
        
        // procura bucket não vazio no nível mais baixo
        while(act[3] < size[0] && bucket[3][act[3]].empty()) act[3]++;
        if(act[3] < size[0]) return;

        // se não encontrar, expandir
        // procurar nível não vazio
        int lv = 2;
        while(lv>=0 && actLv[lv] <= 0) lv--;

        // atualiza bucket ativo
        int b = act[lv];
        do{
            if(b >= size[0]) b = 0;
            if(!bucket[lv][b].empty()) break;
            b++;
        } while(b!=act[lv]);
        act[lv] = b;
        assert(b < size[0]);
        assert(!bucket[lv][b].empty());

        // distribuir até chegarem ao último nível
        for(int i=lv; i<3; i++){
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

        assert(!bucket[3][act[3]].empty());
        assert(act[3]>=0 && act[3]<size[0]);
        par min_elem = bucket[3][act[3]].front();
        bucket[3][act[3]].pop_front();
        ptr[min_elem.second].lvl = -1;
        ptr[min_elem.second].idx = -1;
        actLv[3]--;
        sz--;
        return min_elem;
    }
 
    bool empty() {
        if (sz) return false;
        return true;
    }

    void clear(){
        for(int i=0;i<4;i++) {
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

struct _4lv_bucket_queue{
    vector<queue<par>> bucket[4];
    int actLv[4];
    ll size[4];
    ll act[4];         // bucket ativos
    int sz;
 
    // inicia os buckets
    _4lv_bucket_queue(keyType c){                  // c = maior peso
        int sqrtSize = pow(c + 1.0, 0.25);
        size[0] = sqrtSize; // potência de 2
        for(int i=0;i<4;i++) bucket[i].resize(size[0], {});
        memset(act, 0, sizeof(act));
        memset(actLv, 0, sizeof(actLv));
        size[1] = size[0];
        size[2] = size[0]*size[0];
        size[3] = size[2]*size[0];
        sz = 0;
    };
 
    void insert(int v, keyType dist, keyType w){
        sz++;

        ll level = dist / size[3] % size[0]; // equivale a (dist/size[0]) % size[0]
        if(level != act[0]) {
            bucket[0][level].push({dist, v});
            actLv[0]++;
            return;
        }
        
        level = dist / size[2] % size[0];
        if (level != act[1]){
            bucket[1][level].push({dist, v});
            actLv[1]++;
            return;
        }
        
        level = dist / size[1] % size[0];
        ll level3 = dist % size[0];
        if (level != act[2] || level3 < act[3]){
            bucket[2][level].push({dist, v});
            actLv[2]++;
        }
        else{
            bucket[3][level3].push({dist, v});
            actLv[3]++;
        }
    }
 
    void update(){
        
        // procura bucket não vazio no nível mais baixo
        while(act[3] < size[0] && bucket[3][act[3]].empty()) act[3]++;
        if(act[3] < size[0]) return;

        // se não encontrar, expandir
        // procurar nível não vazio
        int lv = 2;
        while(lv>=0 && actLv[lv] <= 0) lv--;

        // atualiza bucket ativo
        int b = act[lv];
        do{
            if(b >= size[0]) b = 0;
            if(!bucket[lv][b].empty()) break;
            b++;
        } while(b!=act[lv]);
        act[lv] = b;
        assert(b < size[0]);
        assert(!bucket[lv][b].empty());

        // distribuir até chegarem ao último nível
        for(int i=lv; i<3; i++){
            act[i+1] = size[0];
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

        assert(!bucket[3][act[3]].empty());
        assert(act[3]>=0 && act[3]<size[0]);
        par min_elem = bucket[3][act[3]].front();
        bucket[3][act[3]].pop();
        actLv[3]--;
        sz--;
        return min_elem;
    }
 
    bool empty() {
        if (sz) return false;
        return true;
    }

    void clear(){
        for(int i=0;i<4;i++) {
            for (auto &q : bucket[i]) while (!q.empty()) q.pop();
            actLv[i] = 0;
            act[i] = 0;
        }
    }
};