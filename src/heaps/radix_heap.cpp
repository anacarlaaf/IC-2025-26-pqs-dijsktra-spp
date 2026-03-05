#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct radix_heap{
    vector<par> b[65];
    int qtd_buckets = 65;
    int sz = 0;
    keyType mi = 0;

    void clear() {
        for (int i = 0; i < 65; i++) b[i].clear();
        sz = 0;
        mi = 0;
    } 

    int bucket(keyType key) {
        if (key == mi) return 0;
        return 64 - __builtin_clzll(key - mi);
    }
    
    void insert(int u, keyType du) {
        assert(du >= mi);
        int idx = bucket(du);
        b[idx].push_back({du, u});
        sz++;
    }

    void update(){
        int i = 1;
        while(i < qtd_buckets && b[i].empty()) i++;
        
        mi = b[i][0].first;
        for (size_t j = 1; j < b[i].size(); j++) {
            if (b[i][j].first < mi) {
                mi = b[i][j].first;
            }
        }
        
        vector<par> temp = move(b[i]);
        b[i].clear();
        
        for (par p : temp) {
            if (p.first != mi) {
                int idx = bucket(p.first);
                b[idx].push_back(p);
            } else {
                b[0].push_back(p);
            }
        }
    }
 
    par extract_min() {
        if (b[0].empty()) update();
        par min_el = b[0].back();
        b[0].pop_back();
        sz--;
        return min_el;
    }
 
    bool empty() {
        return sz == 0;
    }
};