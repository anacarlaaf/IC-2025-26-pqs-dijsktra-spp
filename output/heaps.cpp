#include <vector>
#include <queue>
#include <set>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <list>
#include <assert.h>
#include <cstring>
#include "heaps/fibonacci_heap.cpp"
#include "heaps/radix_heap.h"

using namespace std;

typedef struct nptr {
    int lvl;
    int idx;
    list<par>::iterator it;
} nptr;

struct heap_inter {
    virtual void insert(int u, keyType du, keyType w) = 0;
    virtual par extract_min() = 0;
    virtual bool empty() = 0;
    virtual void clear() = 0;
    virtual void relax(int u, keyType w, keyType old_du, keyType new_du) = 0;
    virtual ~heap_inter() = default;
};

struct bin_heap : heap_inter{
    int tam=0;
    vector<par> pq;

    bin_heap(int n){
        pq = vector<par>(n);
    };

    void corrigeSubindo(){
        int i = tam-1;
        par aux;
        while(i>=1 && pq[(i-1)/2].first > pq[i].first){
            aux = pq[(i-1)/2];
            pq[(i-1)/2] = pq[i];
            pq[i] = aux;
            i = (i-1)/2;
        }
    }

    void corrigeDescendo(){
        int j = 0, f;
        par aux;
        while(2*j+1<tam){
            f = 2*j+1;

            if(f+1<tam && pq[f].first > pq[f+1].first) f=f+1;
            if(pq[j].first <= pq[f].first) j = tam;
            else{
                aux = pq[j];
                pq[j] = pq[f];
                pq[f] = aux;
                j = f;
            }

        }
    }

    void insert(int u, keyType du, keyType w){
        pq[tam] = (make_pair(du,u));
        tam++;
        corrigeSubindo();
    }

    par extract_min(){
        par menor = pq[0];
        pq[0] = pq[tam-1];
        tam--;
        corrigeDescendo();
        pq.pop_back();
        return menor;
    }

    bool empty(){
        return tam == 0;
    }

    void clear(){
        pq.clear();
        tam = 0;
    }

    void relax(int u, keyType w, keyType old_du, keyType new_du){
        insert(u, new_du, 0);
    }

};


struct b_heap : heap_inter{
    int tam = 0;
    int B = 11;
    vector<par> pq;

    b_heap(int n){
        pq = vector<par>(n);
    };

    void corrigeSubindo(){
        int i = tam - 1;
        while(i >= 1){
            int p = (i - 1) / B;
            if(pq[p].first <= pq[i].first) break;
            swap(pq[p], pq[i]);
            i = p;
        }
    }

    void corrigeDescendo(){
        int j = 0;
        while(true){
            int first = B * j + 1;
            if(first >= tam) break;

            int best = first;
            int last = min(first + B - 1, tam - 1);

            for(int k = first + 1; k <= last; k++){
                if(pq[k].first < pq[best].first)
                    best = k;
            }

            if(pq[j].first <= pq[best].first) break;

            swap(pq[j], pq[best]);
            j = best;
        }
    }

    void insert(int u, keyType du, keyType w){
        pq[tam] = (make_pair(du,u));
        tam++;
        corrigeSubindo();
    }

    par extract_min(){
        par menor = pq[0];
        pq[0] = pq[tam - 1];
        tam--;
        corrigeDescendo();
        pq.pop_back();
        return menor;
    }

    bool empty(){
        return tam == 0;
    }

    void clear(){
        pq.clear();
        tam = 0;
    }

    void relax(int u, keyType w, keyType old_du, keyType new_du){
        insert(u, new_du, 0);
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

    void relax(int u, keyType w, keyType old_du, keyType new_du) {
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
    
    void relax(int u, keyType w, keyType old_du, keyType new_du)  { 
        fh.decrease_key(u, new_du);
    }
};


struct dial : heap_inter{
    vector<list<int>> b;
    int nbuckets = -1;
    int a = 0, r = 0, sz = 0, c = 0;
    
    dial(int c_){
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

    void relax(int u, keyType w, keyType old_du, keyType new_du){
        insert(u, new_du, w);
    }

};



struct dialdk : heap_inter{
    vector<list<int>> b;
    vector<list<int>::iterator> ptr;

    int nbuckets = -1;
    int a = 0, r = 0, sz = 0, c = 0;
    
    dialdk(int c_){
        c = c_;
        nbuckets = c + 1;
        b.assign(nbuckets, {});
        ptr.resize(nbuckets, b[0].end());
    };

    void clear() {
        a = 0, r = 0, sz = 0;
        for(auto &l: b) l.clear();
    }
    
    void insert(int u, keyType du, keyType w) {
        // assert(w <= c);
        int id = (a + w) % nbuckets;
        b[id].push_back(u);
        ptr[u] = prev(b[id].end());
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

    void relax(int u, keyType w, keyType old_du, keyType new_du, keyType *dist){
        b[dist[u] % c].erase(ptr[u]);
        insert(u, new_du, w);
    }

};

struct _2_lv_bkt : heap_inter{
    vector<queue<par>> top_bucket, bottom_bucket;
 
    ll at = 0, ab = 0; // at = top bucket ativo, ab = bottom bucket ativo
    ll b_size = 0;     // tamanho dos buckets
    int sz = 0;        // quantidade de elementos na estrutura
 
    // inicia os buckets
    
    _2_lv_bkt(keyType c){                  // c = maior peso
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

    void relax(int u, keyType w, keyType old_du, keyType new_du){
        insert(u, new_du, w);
    }

};

struct _4lv_bucket_queue : heap_inter{
    vector<list<par>> bucket[4];
    vector<nptr> ptr;
    int actLv[4];
    ll size[4];
    ll act[4];         // bucket ativos
    int sz;

    int getCsize(int c){
        // Calcula menor potência de 2 maior que c.
        int p = 1;
        while (p <= c) p <<= 1;
        return p;
    }
 
    // inicia os buckets
    _4lv_bucket_queue(keyType c, int n){                  // c = maior peso
        int sqrtSize = pow(c + 1.0, 0.25);
        size[0] = getCsize(sqrtSize); // potência de 2
        for(int i=0;i<4;i++) bucket[i].resize(size[0], {});
        ptr.resize(n, {-1, -1, list<par>::iterator{}});
        memset(act, 0, sizeof(act));
        memset(actLv, 0, sizeof(actLv));
        size[1] = size[0];
        size[2] = size[0]*size[0];
        size[3] = size[2]*size[0];
        sz = 0;
    };
 
    void insert(int v, keyType dist, keyType w){
        ll level0 = (dist / size[3]) & (size[0]-1); // equivale a (dist/size[0]) % size[0]
        ll level1 = (dist / size[2]) & (size[0]-1);
        ll level2 = (dist / size[1]) & (size[0]-1);
        ll level3 = dist & (size[0]-1);
        sz++;

        if(level0 != act[0]) {
            bucket[0][level0].push_back({dist, v});
            ptr[v].lvl = 0;
            ptr[v].idx = level0;
            ptr[v].it = prev(bucket[0][level0].end());
            actLv[0]++;
        }
        else if (level1 != act[1]){
            bucket[1][level1].push_back({dist, v});
            ptr[v].lvl = 1;
            ptr[v].idx = level1;
            ptr[v].it = prev(bucket[1][level1].end());
            actLv[1]++;
        }
        else if (level2 != act[2] || level3 < act[3]){
            bucket[2][level2].push_back({dist, v});
            ptr[v].lvl = 2;
            ptr[v].idx = level2;
            ptr[v].it = prev(bucket[2][level2].end());
            actLv[2]++;
        }
        else{
            bucket[3][level3].push_back({dist, v});
            ptr[v].lvl = 3;
            ptr[v].idx = level3;
            ptr[v].it = prev(bucket[3][level3].end());
            actLv[3]++;
        }
    }
 
    void update(){
        
        // procura bucket não vazio no nível mais baixo
        int b = act[3];
        do {
            if (!bucket[3][b].empty()) {
                act[3] = b;
                return;
            }
            b++;
            if (b >= size[0]) b = 0;
        } while (b != act[3]);


        // se não encontrar, expandir
        // procurar nível não vazio
        int lv = 2;
        while(lv>=0 && actLv[lv] <= 0) lv--;

        // atualiza bucket ativo
        b = act[lv];
        do{
            if(b >= size[0]) b = 0;
            if(!bucket[lv][b].empty()) break;
            b++;
        } while(b!=act[lv]);
        act[lv] = b;
        assert(b < size[0]);
        assert(!bucket[lv][b].empty());

        // distribuir até chegarem ao último nível
        for(int i=lv; i<3; i++){
            act[i+1] = size[0];
            //cout << bucket[i].size() << " " << act[i] << endl;
            while(!bucket[i][act[i]].empty()){
                par v = bucket[i][act[i]].front();
                bucket[i][act[i]].pop_front();
                actLv[i]--;

                ll novo = v.first & (size[0]-1);
                bucket[i+1][novo].push_back(v);
                actLv[i+1]++;
                ptr[v.second].lvl = i+1;
                ptr[v.second].idx = novo;
                ptr[v.second].it = prev(bucket[i+1][novo].end());
                act[i+1] = min(act[i+1], novo);
            }
        }
    }
 
    par extract_min(){
        update();

        assert(!bucket[3][act[3]].empty());
        assert(act[3]>=0 && act[3]<size[0]);
        par min_elem = bucket[3][act[3]].front();
        bucket[3][act[3]].pop_front();
        ptr[min_elem.second].lvl = -1;
        ptr[min_elem.second].idx = -1;
        actLv[3]--;
        sz--;
        return min_elem;
    }
 
    bool empty() {
        if (sz) return false;
        return true;
    }

    void clear(){
        for(int i=0;i<4;i++) {
            bucket[i].clear();
            actLv[i] = 0;
            act[i] = 0;
        }
    }

    void relax(int u, keyType w, keyType old_du, keyType new_du){

        if(ptr[u].lvl == -1){ // elemento não está na lista, então insere
            insert(u, new_du, w);
            return;
        }

        bucket[ptr[u].lvl][ptr[u].idx].erase(ptr[u].it);
        actLv[ptr[u].lvl]--;
        sz--;
        ptr[u] = {-1, -1, list<par>::iterator{}};
        insert(u, new_du, w);
    }

};

struct radix : heap_inter{
    radix_heap::pair_radix_heap<keyType, int> rh;

    void insert(int u, keyType du, keyType w)  {
        rh.push(du, u);
    }

    par extract_min()  {
        keyType k = rh.top_key();
        int ver = rh.top_value();
        rh.pop();
        return {k, ver};
    }

    bool empty()  {
        return rh.empty();
    }

    void clear()  {
        rh.clear();
    }
    
    void relax(int u, keyType w, keyType old_du, keyType new_du)  { 
        rh.push(new_du, u);
    }
};