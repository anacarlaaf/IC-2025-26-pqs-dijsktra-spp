#include <bits/stdc++.h>
#include "../../utils/define.hpp"

using namespace std;

struct _4Ary_heap{
    int B = 4;
    vector<par> pq;

    _4Ary_heap(int n){
        pq.reserve(n);  // só reserva capacidade
    }

    void corrigeSubindo(int i){
        while(i > 0){
            int p = (i - 1) / B;
            if(pq[p].first <= pq[i].first) break;
            swap(pq[p], pq[i]);
            i = p;
        }
    }

    void corrigeDescendo(int j){
        int tam = pq.size();
        while(true){
            int first = B * j + 1;
            if(first >= tam) break;

            int best = first;
            int last = min(first + B - 1, tam - 1);

            for(int k = first + 1; k <= last; k++){
                if(pq[k].first < pq[best].first)
                    best = k;
            }

            if(pq[j].first <= pq[best].first) break;

            swap(pq[j], pq[best]);
            j = best;
        }
    }

    void insert(int u, keyType du){
        pq.emplace_back(du, u);
        corrigeSubindo(pq.size() - 1);
    }

    par extract_min(){
        par menor = pq[0];
        pq[0] = pq.back();
        pq.pop_back();
        if(!pq.empty())
            corrigeDescendo(0);
        return menor;
    }

    bool empty(){
        return pq.empty();
    }

    void clear(){
        pq.clear();
    }
};