#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"
 
struct _1lv_bucket_queue_DK{
    list<int> *b;
    list<int>::iterator *ptr;
 
    int nbuckets = 0;
    int a = 0, r = 0, sz=0, n=0;
    keyType c = 0;

    _1lv_bucket_queue_DK(keyType c_, int n_){
        c = c_ + 1;
        n = n_;
        nbuckets = c;
        b = new list<int>[nbuckets];
        ptr = new list<int>::iterator[n];
    };
 
    void clear() {
        for(int i=0;i<nbuckets;i++) b[i].clear();
        for(int i=0;i<n;i++) ptr[i] = list<int>::iterator{};
        a = 0, r = 0, sz=0;
    }
    
    void insert(int u, keyType du, keyType w) {
        int id = (a+w) % nbuckets;
        b[id].push_back(u);
        ptr[u] = prev(b[id].end());
        sz++;
    }

    void update() {
        if (b[a].size()) return;

        int bg = a;
        do {
            a = (a + 1) % nbuckets;
            if (a == 0) r++;
            if (b[a].size()) return;
        } while (a != bg);

        assert(false);
    }
 
    par extract_min() {
        update();
        int u = b[a].front();
        keyType du = r * nbuckets + a;
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
            int id = old_du % nbuckets;
            b[id].erase(ptr[u]);
            ptr[u] = list<int>::iterator{};
            sz--;
        }
        insert(u, new_du, w);
    }

};

struct _1lv_bucket_queue{
    queue<int> *b;
 
    int nbuckets = 0;
    int a = 0, r = 0, sz=0;
    keyType c = 0;

    _1lv_bucket_queue(keyType c_){
        c = c_ + 1;
        nbuckets = c;
        b = new queue<int>[nbuckets];
    };
 
    void clear() {
        for(int i=0;i<nbuckets;i++) while(!b[i].empty()) b[i].pop();
        a = 0, r = 0, sz=0;
    }
    
    void insert(int u, keyType du, keyType w) {
        int id = (a+w) % nbuckets;
        b[id].push(u);
        sz++;
    }

    void update() {
        if (b[a].size()) return;

        int bg = a;
        do {
            a = (a + 1) % nbuckets;
            if (a == 0) r++;
            if (b[a].size()) return;
        } while (a != bg);

        assert(false);
    }
 
    par extract_min() {
        update();
        int u = b[a].front();
        keyType du = r * nbuckets + a;
        b[a].pop();
        sz--;
        return make_pair(du, u);
    }
 
    bool empty() {
        return sz==0;
    }
};