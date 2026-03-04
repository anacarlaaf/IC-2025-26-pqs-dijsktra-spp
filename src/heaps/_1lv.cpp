#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"
 
struct _1lv_bucket_queue_DK{
    vector<list<int>> b;
    vector<list<int>::iterator> ptr;
 
    int nbuckets = 0;
    int a = 0, r = 0, sz=0;
    keyType c = 0;

    _1lv_bucket_queue_DK(keyType c_, int n){
        c = c_ + 1;
        nbuckets = c;
        b.assign(nbuckets, {});
        ptr.resize(n, b[0].end());
    };
 
    void clear() {
        a = 0, r = 0, c=0, sz=0;
        nbuckets = 0;
        for(auto &l : b) l.clear();
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
        ptr[u] = b[0].end();
        return make_pair(du, u);
    }
 
    bool empty() {
        return sz==0;
    }
 
    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        if(ptr[u] != b[0].end()){
            int id = old_du % nbuckets;
            b[id].erase(ptr[u]);
            ptr[u] = b[0].end();
            sz--;
        }
        insert(u, new_du, w);
    }

};

struct _1lv_bucket_queue{
    vector<queue<int>> b;
 
    int nbuckets = -1;
    int a = 0, r = 0, sz=0;
    keyType c = 0;

    _1lv_bucket_queue(keyType c_){
        c = c_ + 1;
        nbuckets = c;
        b.assign(nbuckets, {});
    };
 
    void clear() {
        a = 0, r = 0, c=0, sz=0;
        nbuckets = 0;
        for(auto &l : b) while(!l.empty()) l.pop();
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