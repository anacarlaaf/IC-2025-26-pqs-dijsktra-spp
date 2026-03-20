#include "bits/stdc++.h"
#include "heaps/fibonacci_heap.hpp"
#include "heaps/bucket_heaps_dk.hpp"
#include "heaps/bucket_heaps_vector.hpp"
#include "heaps/bin_heap.hpp"

using namespace std;

struct heap_inter {
    virtual void insert(int u, keyType du, keyType w) = 0;
    virtual par extract_min() = 0;
    virtual bool empty() = 0;
    virtual void decrease_key(int u, keyType w, keyType old_du, keyType new_du) = 0;
    virtual ~heap_inter() = default;
};

// Sem suporte a decrease_key

struct binheapCPP : heap_inter{
    priority_queue<par, vector<par>, greater<par>> pq;

    void insert(int u, keyType du, keyType w){
        pq.push({du, u});
    }

    par extract_min(){
        par v = pq.top();
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

    par extract_min(){
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

struct _1lvbq : heap_inter{
    
    _1lv_bucket_queue bq;

    _1lvbq(keyType _c, int n) : bq(_c, n) {}
    
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
        cerr << "Estrutura sem suporte a decrease_key";
        exit(1);
    }
};

struct _2lvbq : heap_inter{
    
    _2lv_bucket_queue bq;

    _2lvbq(keyType _c, int n) : bq(_c, n) {}
    
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
        cerr << "Estrutura sem suporte a decrease_key";
        exit(1);
    }
};

struct _klvbq : heap_inter{
    
    _klv_bucket_queue bq;

    _klvbq(keyType _c, int k) : bq(_c, k) {}
    
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
        cerr << "Estrutura sem suporte a decrease_key";
        exit(1);
    }
};


// Cem suporte a decrease_key

struct rb_tree : heap_inter{
    set<par> pq;
    
    void insert(int u, keyType du, keyType w) {
        pq.insert({du, u});
    }

    par extract_min() {
        par min = *(pq.begin());
        pq.erase(min);
        return min;
    }

    bool empty() {
        return pq.empty();
    }

    void clear() {
        pq.clear();
    }

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du) {
        pq.erase({old_du, u});
        pq.insert({new_du, u});
    }
};

struct fibonacci : heap_inter {
    fibonacci_heap fh;

    fibonacci(int n) : fh(n) {}

    void insert(int u, keyType du, keyType w)  {
        fh.insert(du, u);
    }

    par extract_min()  {
        return fh.extract_min();
    }

    bool empty()  {
        return fh.empty();
    }

    void clear()  {
        fh.clear();
    }
    
    void decrease_key(int u, keyType w, keyType old_du, keyType new_du)  { 
        keyType current_key;
        if (fh.get_key(u, current_key) == -1) fh.insert(new_du, u);
        else fh.decrease_key(u, new_du);
    }
};

//Bucket heaps com suporte a DK

struct _1lvbqDK : heap_inter{
    
    _1lv_bucket_queue_DK bq;

    _1lvbqDK(keyType _c, int n) : bq(_c, n) {}
    
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

struct _2lvbqDK : heap_inter{
    
    _2lv_bucket_queue_DK bq;

    _2lvbqDK(keyType _c, int n) : bq(_c, n) {}

    
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


struct _klvbqDK : heap_inter{
    
    _klv_bucket_queue_DK bq;

    _klvbqDK(keyType _c, int n, int k) : bq(_c, n, k) {}
    
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