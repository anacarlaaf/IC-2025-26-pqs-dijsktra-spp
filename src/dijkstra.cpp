#include<bits/stdc++.h>
#include"heaps.cpp"

using namespace std;
typedef heap_inter* pq;      // heaps gerais

const keyType oo = 1e17;

struct shortest_path {    
    vector<keyType> dist;
    vector<int> pai;
    vector<bool> in_heap; // mapeia presença no heap para evitar inicialização de todos os vértices

    void init_dijkstra(pq &q, const int size, const int s, bool dk) {
        dist = vector<keyType>(size, oo);
        pai = vector<int>(size, -1);
        if(dk) { // só inicializa se há suporte para decrease_key
            in_heap = vector<bool>(size, 0); 
            in_heap[0] = true;
        }

        dist[s] = 0;
        
        if (q) q->clear();
        q->insert(0, s, 0);  
    }

    void dijkstra_dk(const auto &adj, pq &q) {
        keyType old;
        while(!q->empty()) {
            auto [du, u] = q->extract_min();
            in_heap[u] = false; // removido do heap
            if(dist[u] < du) continue;
    
            for(auto [w, v] : adj[u]) {
                if(dist[v] > dist[u] + w) {
                    old = dist[v];
                    pai[v] = u;
                    dist[v] = dist[u] + w;
                    if (!in_heap[v]){ // se NÃO está no heap, insere  
                        q->insert(v, dist[v], w);
                        in_heap[v] = true;
                    }
                    else q->relax(v, w, old, dist[v]); // se está, relaxa
                }
            }
        }
    };

    void dijkstra_ndk(const auto &adj, pq &q) {
        keyType old;
        while(!q->empty()) {
            auto [du, u] = q->extract_min();
            if(dist[u] < du) continue;
    
            for(auto [w, v] : adj[u]) {
                if(dist[v] > dist[u] + w) {
                    old = dist[v];
                    pai[v] = u;
                    dist[v] = dist[u] + w;
                    q->insert(v, dist[v], w);
                }
            }
        }
    };

    void clear(){
        dist.clear();
        pai.clear();
        in_heap.clear();
    }
};