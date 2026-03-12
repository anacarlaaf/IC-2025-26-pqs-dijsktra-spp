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
        for(int i=0;i<b_size;i++) b[i] = cbuffer(10);
    };

    ~_1lv_bucket_queue() {
        delete[] b;
    }
 
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
        int aux = b_size-1;
        do {
            a = (a + 1) & (aux);
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

struct _1lv_bucket_queue_DK{
    list<int> *b;
    list<int>::iterator *ptr;
 
    int b_size = 0;
    int a = 0, r = 0, sz=0, n=0;
    keyType c = 0;

    _1lv_bucket_queue_DK(keyType c_, int n_){
        c = c_ + 1;
        n = n_;
        b_size = c;
        b = new list<int>[b_size];
        ptr = new list<int>::iterator[n];
    };

    ~_1lv_bucket_queue_DK() {
        delete[] b;
        delete[] ptr;
    }
 
 
    void clear() {
        for(int i=0;i<b_size;i++) b[i].clear();
        for(int i=0;i<n;i++) ptr[i] = list<int>::iterator{};
        a = 0, r = 0, sz=0;
    }
    
    void insert(int u, keyType du, keyType w) {
        int id = (a+w) % b_size;
        b[id].push_back(u);
        ptr[u] = prev(b[id].end());
        sz++;
    }

    void update() {
        if (b[a].size()) return;

        int bg = a;
        do {
            a = (a + 1) % b_size;
            if (a == 0) r++;
            if (b[a].size()) return;
        } while (a != bg);

        assert(false);
    }
 
    par extract_min() {
        update();
        int u = b[a].front();
        keyType du = r * b_size + a;
        b[a].pop_front();
        sz--;
        ptr[u] = list<int>::iterator{};
        return make_pair(du, u);
    }
 
    bool empty() {
        return sz==0;
    }
 
    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        if(ptr[u] != list<int>::iterator{}){
            int id = old_du % b_size;
            b[id].erase(ptr[u]);
            ptr[u] = list<int>::iterator{};
            sz--;
        }
        insert(u, new_du, w);
    }

};