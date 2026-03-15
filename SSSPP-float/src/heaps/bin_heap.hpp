#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"


struct bin_heap{
    int tam=0;
    vector<parInt> pq;

    bin_heap() {}          
    bin_heap(int n) {      
        pq.reserve(n);     
    }


    void corrigeSubindo(){
        int i = tam-1;
        parInt aux;
        while(i>=1 && pq[(i-1)/2].first > pq[i].first){
            aux = pq[(i-1)/2];
            pq[(i-1)/2] = pq[i];
            pq[i] = aux;
            i = (i-1)/2;
        }
    }

    void corrigeDescendo(){
        int j = 0, f;
        parInt aux;
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

    void insert(int u, keyType du){
        pq.push_back({du,u});
        tam++;
        corrigeSubindo();
    }

    parInt extract_min(){
        parInt menor = pq[0];
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
        tam = 0;
        pq.clear();
    }

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        return;
    }

};


// struct bin_heap{
//     int tam=0;
//     parInt  *pq;

//     bin_heap(int n){
//         pq = new parInt[n];
//     };

//     void corrigeSubindo(){
//         int i = tam-1;
//         parInt aux;
//         while(i>=1 && pq[(i-1)/2].first > pq[i].first){
//             aux = pq[(i-1)/2];
//             pq[(i-1)/2] = pq[i];
//             pq[i] = aux;
//             i = (i-1)/2;
//         }
//     }

//     ~bin_heap(){
//         delete[] pq;
//     };

//     void corrigeDescendo(){
//         int j = 0, f;
//         parInt aux;
//         while(2*j+1<tam){
//             f = 2*j+1;

//             if(f+1<tam && pq[f].first > pq[f+1].first) f=f+1;
//             if(pq[j].first <= pq[f].first) j = tam;
//             else{
//                 aux = pq[j];
//                 pq[j] = pq[f];
//                 pq[f] = aux;
//                 j = f;
//             }

//         }
//     }

//     void insert(int u, keyType du){
//         pq[tam] = (make_pair(du,u));
//         tam++;
//         corrigeSubindo();
//     }

//     parInt extract_min(){
//         parInt menor = pq[0];
//         pq[0] = pq[tam-1];
//         tam--;
//         corrigeDescendo();
//         return menor;
//     }

//     bool empty(){
//         return tam == 0;
//     }

//     void clear(){
//         tam = 0;
//     }

//     void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
//         return;
//     }

// };