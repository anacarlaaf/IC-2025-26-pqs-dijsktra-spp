#include <bits/stdc++.h>
#include "../../utils/define.hpp"

using namespace std;


struct bin_heap{
    int tam=0;
    vector<par> pq;

    bin_heap(int n){
        pq = vector<par>(n);
    };

    void corrigeSubindo(){
        int i = tam-1;
        par aux;
        while(i>=1 && pq[(i-1)/2].first > pq[i].first){
            aux = pq[(i-1)/2];
            pq[(i-1)/2] = pq[i];
            pq[i] = aux;
            i = (i-1)/2;
        }
    }

    void corrigeDescendo(){
        int j = 0, f;
        par aux;
        while(2*j+1<tam){
            f = 2*j+1;

            if(f+1<tam && pq[f].first > pq[f+1].first) f=f+1;
            if(pq[j].first <= pq[f].first) j = tam;
            else{
                aux = pq[j];
                pq[j] = pq[f];
                pq[f] = aux;
                j = f;
            }

        }
    }

    void insert(int u, keyType du, keyType w){
        pq[tam] = (make_pair(du,u));
        tam++;
        corrigeSubindo();
    }

    par extract_min(){
        par menor = pq[0];
        pq[0] = pq[tam-1];
        tam--;
        corrigeDescendo();
        pq.pop_back();
        return menor;
    }

    bool empty(){
        return tam == 0;
    }

    void clear(){
        pq.clear();
        tam = 0;
    }
};


struct dAry_heap{
    int tam = 0;
    int B = 11;
    vector<par> pq;

    dAry_heap(int n){
        pq = vector<par>(n);
    };

    void corrigeSubindo(){
        int i = tam - 1;
        while(i >= 1){
            int p = (i - 1) / B;
            if(pq[p].first <= pq[i].first) break;
            swap(pq[p], pq[i]);
            i = p;
        }
    }

    void corrigeDescendo(){
        int j = 0;
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

    void insert(int u, keyType du, keyType w){
        pq[tam] = (make_pair(du,u));
        tam++;
        corrigeSubindo();
    }

    par extract_min(){
        par menor = pq[0];
        pq[0] = pq[tam - 1];
        tam--;
        corrigeDescendo();
        pq.pop_back();
        return menor;
    }

    bool empty(){
        return tam == 0;
    }

    void clear(){
        pq.clear();
        tam = 0;
    }
};
