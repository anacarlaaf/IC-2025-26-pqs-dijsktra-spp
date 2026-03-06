#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct _4lv_bucket_queue{
    cbuffer *bucket[4];
    int actLv[4];
    ll size[4];
    ll act[4];         // bucket ativos
    int sz;
 
    // inicia os buckets
    _4lv_bucket_queue(keyType c, int n){                  // c = maior peso
        int sqrtSize = pow(c + 1.0, 0.25) + 1;
        ll aux = 1;
        while(aux < sqrtSize) aux <<= 1;
        size[0] = aux;
        for(int i=0;i<4;i++){
            bucket[i] = new cbuffer[aux];
            for(int j=0;j<size[0];j++)
                bucket[i][j] = cbuffer(n/aux);
        }
        memset(act, 0, sizeof(act));
        memset(actLv, 0, sizeof(actLv));
        size[1] = size[0];
        size[2] = size[0]*size[0];
        size[3] = size[2]*size[0];
        sz = 0;
    };
 
    void insert(int v, keyType dist, keyType w){
        sz++;

        ll level = (dist / size[3]) & (size[0]-1);
        if(level != act[0]) {
            bucket[0][level].push({dist, v});
            actLv[0]++;
            return;
        }
        
        level = (dist / size[2]) & (size[0]-1);
        if (level != act[1]){
            bucket[1][level].push({dist, v});
            actLv[1]++;
            return;
        }
        
        level = (dist / size[1]) & (size[0]-1);
        ll level3 = dist & (size[0]-1);
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

        int start = act[lv];
        int b = start;
        do {
            if (!bucket[lv][b].empty()) break;
            b = (b + 1) & (size[0]-1);
        } while (b != start);
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

                ll novo = v.first & (size[0]-1);
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
            for(int j=0;j<size[0];j++) while(!bucket[i][j].empty()) bucket[i][j].pop();
            actLv[i] = 0;
            act[i] = 0;
        }
        sz = 0;
    }
};