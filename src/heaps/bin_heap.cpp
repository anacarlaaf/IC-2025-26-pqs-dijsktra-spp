#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

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

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        return;
    }

};