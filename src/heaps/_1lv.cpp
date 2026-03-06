#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct _1lv_bucket_queue{
    cbuffer *b;
 
    int b_size = 0;
    int a = 0, sz=0;

    _1lv_bucket_queue(keyType c_, int n){
        b_size = c_;
        ll aux = 1;
        while(aux < b_size) aux <<=1;
        b_size = aux;
        b = new cbuffer[b_size];
        for(int i=0;i<b_size;i++) b[i] = cbuffer(n/b_size);
    };
 
    void clear() {
        for(int i=0;i<b_size;i++) while(!b[i].empty()) b[i].pop();
        a = 0, sz=0;
    }
    
    void insert(int u, keyType du, keyType w) {
        int id = (a+w) & (b_size-1);
        b[id].push({du,u});
        sz++;
    }

    void update() {
        if (b[a].size()) return;

        int bg = a;
        do {
            a = (a + 1) & (b_size-1);
            if (b[a].size()) return;
        } while (a != bg);

        assert(false);
    }
 
    par extract_min() {
        update();
        par u = b[a].front();
        b[a].pop();
        sz--;
        return u;
    }
 
    bool empty() {
        return sz==0;
    }
};