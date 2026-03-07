#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct quad_heap{
    int tam = 0;
    par *pq;

    quad_heap(int n){
        pq = new par[n];
    }

    void corrigeSubindo(){
        int i = tam - 1;
        par aux;

        while(i >= 1 && pq[(i-1)>>2].first > pq[i].first){
            aux = pq[(i-1)>>2];
            pq[(i-1)>>2] = pq[i];
            pq[i] = aux;

            i = (i-1)>>2;
        }
    }

    void corrigeDescendo(){
        int j = 0, f, first_child;
        par aux;

        while((j<<2) + 1 < tam){

            first_child = (j<<2) + 1;
            f = first_child;

            for(int k=1;k<4;k++){
                int child = first_child + k;
                if(child < tam && pq[child].first < pq[f].first)
                    f = child;
            }

            if(pq[j].first <= pq[f].first){
                j = tam;
            }
            else{
                aux = pq[j];
                pq[j] = pq[f];
                pq[f] = aux;

                j = f;
            }
        }
    }

    void insert(int u, keyType du){
        pq[tam] = make_pair(du,u);
        tam++;
        corrigeSubindo();
    }

    par extract_min(){
        par menor = pq[0];
        pq[0] = pq[tam-1];
        tam--;
        corrigeDescendo();
        return menor;
    }

    bool empty(){
        return tam == 0;
    }

    void clear(){
        tam = 0;
    }
};