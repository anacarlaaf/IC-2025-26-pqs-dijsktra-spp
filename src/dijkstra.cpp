#include<bits/stdc++.h>
#include"heaps.cpp"

using namespace std;
typedef heap_inter* pq;      // heaps gerais

const keyType oo = 1e17;

struct shortest_path {    
    vector<keyType> dist;
    vector<int> pai;

    void init_dijkstra(pq &q, const int size, const int s, bool dk) {
        dist = vector<keyType>(size, oo);
        pai = vector<int>(size, -1);
        dist[s] = 0;
        
        q->insert(s, 0, 0);  
    }

    void dijkstra_dk(const auto &adj, pq &q) {
        while(!q->empty()) {
            auto [du, u] = q->extract_min();
    
            for(auto [w, v] : adj[u]) {
                if(dist[v] > dist[u] + w) {
                    q->decrease_key(v, w, dist[v], dist[u]+w);
                    pai[v] = u;
                    dist[v] = dist[u] + w;
                }
            }
        }
    };

    void dijkstra_ndk(const auto &adj, pq &q) {
        while(!q->empty()) {
            auto [du, u] = q->extract_min();
            if(dist[u] < du) continue;
    
            for(auto [w, v] : adj[u]) {
                if(dist[v] > dist[u] + w) {
                    pai[v] = u;
                    dist[v] = dist[u] + w;
                    q->insert(v, dist[v], w);
                }
            }
        }
    };

    pair<pair<int,int>,int> dijkstra_dk_perf(const auto &adj, pq &q) {
        pair<pair<int,int>,int> qtds = {{0,0},0};
        // {{insert, dk}, extractMin}
        int dk;

        while(!q->empty()) {
            qtds.second++;
            auto [du, u] = q->extract_min();
    
            for(auto [w, v] : adj[u]) {
                if(dist[v] > dist[u] + w) {
                    dk = q->decrease_key_perf(v, w, dist[v], dist[u]+w);
                    if(dk) qtds.first.second++;
                    else qtds.first.first++;
                    pai[v] = u;
                    dist[v] = dist[u] + w;
                }
            }
        }
        return qtds;
    };

    pair<int,int> dijkstra_ndk_perf(const auto &adj, pq &q) {
        pair<int,int> qtds = {0,0};
        // {insert, extractMin}

        while(!q->empty()) {
            auto [du, u] = q->extract_min();
            qtds.second++;
            if(dist[u] < du) continue;
    
            for(auto [w, v] : adj[u]) {
                if(dist[v] > dist[u] + w) {
                    pai[v] = u;
                    dist[v] = dist[u] + w;
                    q->insert(v, dist[v], w);
                    qtds.first++;
                }
            }
        }

        return qtds;
    };

    void clear(){
        dist.clear();
        pai.clear();
    }
};