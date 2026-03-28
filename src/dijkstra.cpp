#include<bits/stdc++.h>
#include"heaps.cpp"
#include"../utils/perf.hpp"

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

    void dijkstra_dk_perf(const auto &adj, pq &q, ofstream& file, string carac) {
        CacheStats cache;

        while(!q->empty()) {
            cache.start();
            auto [du, u] = q->extract_min();
            cache.stop();

            file << carac << " extractMin " << cache.r_cycles << " " << cache.r_instructions << " " << cache.r_branch_instr << " " << cache.r_branch_miss << "\n";
    
            for(auto [w, v] : adj[u]) {
                if(dist[v] > dist[u] + w) {
                    cache.start();
                    q->decrease_key(v, w, dist[v], dist[u]+w);
                    cache.stop();
                    file << carac << " dk " << cache.r_cycles << " " << cache.r_instructions << " " << cache.r_branch_instr << " " << cache.r_branch_miss << "\n";

                    pai[v] = u;
                    dist[v] = dist[u] + w;
                }
            }
        }
    };

    void dijkstra_ndk_perf(const auto &adj, pq &q, ofstream& file, string carac) {

        CacheStats cache;

        while(!q->empty()) {
            cache.start();
            auto [du, u] = q->extract_min();
            cache.stop();

            file << carac << " extractMin " << cache.r_cycles << " " << cache.r_instructions << " " << cache.r_branch_instr << " " << cache.r_branch_miss << "\n";
            
            if(dist[u] < du) continue;
    
            for(auto [w, v] : adj[u]) {
                if(dist[v] > dist[u] + w) {
                    pai[v] = u;
                    dist[v] = dist[u] + w;
                    cache.start();
                    q->insert(v, dist[v], w);
                    cache.stop();

                    file << carac << " insert " << cache.r_cycles << " " << cache.r_instructions << " " << cache.r_branch_instr << " " << cache.r_branch_miss << "\n";
                }
            }
  
        }
    };

    void clear(){
        dist.clear();
        pai.clear();
    }
};