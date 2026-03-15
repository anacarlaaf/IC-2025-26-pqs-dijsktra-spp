#include "bits/stdc++.h"
#include "heaps/_1lv.hpp"
#include "heaps/_2lv.hpp"
#include "heaps/_4lv.hpp"
// #include "heaps/_klv.hpp"
#include "heaps/bin_heap.hpp"
#include "heaps/radix_heap.h"

using namespace std;

struct heap_inter {
    virtual void insert(int u, keyType du, keyType w) = 0;
    virtual par extract_min() = 0;
    virtual bool empty() = 0;
    virtual void clear() = 0;
    virtual void decrease_key(int u, keyType w, keyType old_du, keyType new_du) = 0;
    virtual ~heap_inter() = default;
};

// Sem suporte a decrease_key

struct binheapCPP : heap_inter{
    priority_queue<parInt, vector<parInt>, greater<parInt>> pq;

    
    void insert(int u, keyType du, keyType w){
        pq.push({du, u});
    }

    parInt extract_min(){
        parInt v = pq.top();
        pq.pop();
        return v;
    }

    bool empty(){
        return pq.empty();
    }

    void clear(){
        while(!pq.empty()) pq.pop();
    }

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        cerr << "Estrutura sem suporte a decrease_key";
        exit(1);
    }
};

struct binheap : heap_inter{

    bin_heap pq;

    binheap (int n) : pq(n) {}

    void insert(int u, keyType du, keyType w){
        pq.insert(u, du);
    }

    parInt extract_min(){
        return pq.extract_min();
    }

    bool empty(){
        return pq.empty();
    }

    void clear(){
        pq.clear();
    }

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        cerr << "Estrutura sem suporte a decrease_key";
        exit(1);
    }

};


struct _2lvbqDK : heap_inter{
    
    _2lv_bucket_queue_DK bq;

    _2lvbqDK(keyType _c, int n) : bq(_c, n) {}

    void clear() {
        bq.clear();
    }
    
    void insert(int u, keyType du, keyType w) {
       bq.insert(u, du, w);
    }

    par extract_min() {
        return bq.extract_min();
    }

    bool empty() {
        return bq.empty();
    }

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        bq.decrease_key(u, w, old_du, new_du);
    }
};
