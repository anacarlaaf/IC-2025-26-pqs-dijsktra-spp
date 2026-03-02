#include "bits/stdc++.h"
#include "heaps/fibonacci_heap.cpp"
#include "heaps/_1lv.cpp"
#include "heaps/_2lv.cpp"
#include "heaps/_4lv.cpp"
#include "heaps/_klv.cpp"
#include "heaps/bin_heap.cpp"

using namespace std;

struct heap_inter {
    virtual void insert(int u, keyType du, keyType w) = 0;
    virtual par extract_min() = 0;
    virtual bool empty() = 0;
    virtual void clear() = 0;
    virtual void decrease_key(int u, keyType w, keyType old_du, keyType new_du) = 0;
    virtual ~heap_inter() = default;
};

struct binheap : heap_inter{

    bin_heap pq;

    binheap (int n) : pq(n) {}

    void insert(int u, keyType du, keyType w){
        pq.insert(u, du, w);
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
        return;
    }

};


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
        fh.decrease_key(u, new_du);
    }
};


struct _1lvbq1 : heap_inter{
    vector<list<int>> b;
    int nbuckets = -1;
    int a = 0, r = 0, sz = 0, c = 0;
    
    _1lvbq1(int c_){
        c = c_;
        nbuckets = c + 1;
        b.assign(nbuckets, {});
    };

    void clear() {
        a = 0, r = 0, sz = 0;
        for(auto &l: b) l.clear();
    }
    void insert(int u, keyType du, keyType w) {
        // assert(w <= c);
        int id = (a + w) % nbuckets;
        b[id].push_back(u);
        sz++;
    }
    void update() {
        if(b[a].size()) return;

        int bg = a;
        do {
            a++;
            if(a == nbuckets) a = 0, r++;

            if(b[a].size()) return;
        } while(a != bg);
        assert(false);
    }

    par extract_min() {
        update();
        int u = b[a].front();
        int du = r * nbuckets + a;
        b[a].pop_front();
        sz--;
        
        return make_pair(du, u);
    }

    bool empty() {
        return sz == 0;
    }

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        insert(u, new_du, w);
    }

};



struct _1lvbq2 : heap_inter{
    
    _1lv_bucket_queue bq;

    _1lvbq2(keyType _c, int n) : bq(_c, n) {}

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

struct _2lvbq1 : heap_inter{
    vector<queue<par>> top_bucket, bottom_bucket;
 
    ll at = 0, ab = 0; // at = top bucket ativo, ab = bottom bucket ativo
    ll b_size = 0;     // tamanho dos buckets
    int sz = 0;        // quantidade de elementos na estrutura
 
    // inicia os buckets
    
    _2lvbq1(keyType c){                  // c = maior peso
        b_size = sqrt(c + 1) + 1;     // tamanho dos buckets
        top_bucket.resize(b_size);
        bottom_bucket.resize(b_size);
    };
 
    void insert(int v, keyType dist, keyType w){
        ll i = dist / b_size % b_size;   // se i = top bucket ativo, insere no bottom
        ll j = dist % b_size;            // se não, insere no top
    
        if (i == at && j >= ab) bottom_bucket[j].push({dist,v});
        else top_bucket[i].push({dist,v});
 
        sz++;
    }
 
    void update(){
        // procura o próximo bottom bucket não vazio
        while (ab < b_size && bottom_bucket[ab].empty()) ab++;
        if(ab < b_size) return;
 
        // Expand: se nao encontrar, distribui elementos de outro top bucket
        ll start = at;
        do {
            if(top_bucket[at].size()) break;
            at++;
            if(at == b_size) at = 0;
        }  while(at != start);
        if(!top_bucket[at].size()) return;
 
        // distribui no bottom_bucket apenas os atuais, ignorando as novas inserções
        int aux = top_bucket[at].size();
        ab = b_size;
        for(int i=0;i<aux;i++){
            auto a = top_bucket[at].front(); top_bucket[at].pop();
            ll nova_ab = a.first % b_size;
            ab = min(ab, nova_ab);
            bottom_bucket[nova_ab].push(a);  // insere no bottom bucket
        }
     }
 
    par extract_min(){
        update();
        par min_elem = bottom_bucket[ab].front();
        bottom_bucket[ab].pop();
        sz --;
        return min_elem;
    }
 
    bool empty() {
        return sz == 0;
    }

    void clear(){
        for(int i=0;i<b_size;i++){
            while(!top_bucket[i].empty()) top_bucket[i].pop();
            while(!bottom_bucket[i].empty()) bottom_bucket[i].pop();
        }
    }

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        return;
    }

};

struct _2lvbq2 : heap_inter{
    
    _2lv_bucket_queue bq;

    _2lvbq2(keyType _c, int n) : bq(_c, n) {}

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

struct _4lvbq : heap_inter{
    
    _4lv_bucket_queue bq;

    _4lvbq(keyType _c, int n) : bq(_c, n) {}

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

struct _klvbq : heap_inter{
    
    _klv_bucket_queue bq;

    _klvbq(keyType _c, int n, int k) : bq(_c, n, k) {}

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