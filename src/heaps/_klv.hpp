#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct _klv_bucket_queue{
    cbuffer **bucket; //
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
        while(aux < sqrtSize) aux <<= 1;
        sqrtSize = aux;

        size   = new ll[k];
        act    = new ll[k];
        actLv  = new int[k];
        bucket = new cbuffer*[k];

        size[0] = sqrtSize;
        aux = sqrtSize;
        for(int i=0;i<k;i++) {
            act[i]    = 0;
            actLv[i]  = 0;
            bucket[i] = new cbuffer[sqrtSize];
            for(int j=0;j<size[0];j++) bucket[i][j] = cbuffer(10);
            if(i > 0) {
                size[i] = aux;
                aux *= size[0];
            }
        }

        sz = 0;
    };

    ~_klv_bucket_queue() {
        for(int i = 0; i < lv; i++)
            delete[] bucket[i];
        delete[] bucket;
        delete[] actLv;
        delete[] size;
        delete[] act;
    }
 
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