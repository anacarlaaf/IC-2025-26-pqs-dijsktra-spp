#include<bits/stdc++.h>
using namespace std;
using ll = unsigned long long;
using keyType = ll;
using par = pair<keyType, int>;
 
// g++ -fsanitize=address,undefined -g -o a.out _klv.cpp
 
const ll oo = 1e16;
const int maxn = 1e6+2;
vector<par> adj[maxn];


struct bkt{
    int tail, sz;
};

struct element{
    par data;
    int prev, prox;
};

struct pool_list{

    element *pool;
    int *free_list;
    int *idxs;
    int free_top;

    pool_list() : free_list(nullptr), idxs(nullptr), free_top(0) {}
    pool_list(int c) {
        pool = new element[c];
        free_list = new int[c];
        idxs = new int[c];
        for(int i=0;i<c;i++) {
            free_list[i] = i; // todos livre;
            idxs[i] = -1;
        }
        free_top = c-1;
    }

    int alocar(par novo){
        int idx = free_list[free_top--];
        pool[idx].data = novo;
        pool[idx].prev = pool[idx].prox = -1;;
        return idx;
    }
    
    int insert(par novo, int cauda){ 
        int idx =  alocar(novo);
        idxs[novo.second] = idx;
        if (cauda!=-1) {
            pool[cauda].prox = idx;
            pool[idx].prev = cauda;
        }
        return idx; // retorna nova cauda;
    }

    int pop(int cauda){
        int ant = pool[cauda].prev;

        if (ant != -1) {
            pool[ant].prox = -1;
        }

        free_top++;
        free_list[free_top] = cauda;

        return ant;
    }

    void remove(int u){
        int idx = idxs[u];
        int ant = pool[idx].prev;
        int next = pool[idx].prox;

        if (ant != -1) pool[ant].prox = next;
        if (next != -1) pool[next].prev = ant;

        free_top++;
        free_list[free_top] = idx;
        idxs[u] = -1;
    }

    void clear(int n) {
        for(int i = 0; i < n; i++) { free_list[i] = i; idxs[i] = -1; }
        free_top = n - 1;
    }
};
 
 


struct _2lv_bucket_queue_DK{
    bkt *top_bucket, *bottom_bucket;
    pool_list pool;
    int *qBucket;

    int sz=0, n=0;
    ll at = 0, ab = 0; // at = top bucket ativo, ab = bottom bucket ativo
    ll b_size = 0;     // tamanho dos buckets

    // inicia os buckets
    
    _2lv_bucket_queue_DK(keyType c, int n_){                  // c = maior peso
        n = n_;
        b_size = sqrt(c + 1) + 1;
        ll aux = 1;
        while(aux < b_size) aux <<= 1;
        b_size = aux;
        top_bucket = new bkt[b_size];
        bottom_bucket = new bkt[b_size];
        qBucket = new int[n];
        pool = pool_list(n);
        for(ll i=0;i<b_size;i++){
            top_bucket[i].sz = bottom_bucket[i].sz = 0;
            top_bucket[i].tail = bottom_bucket[i].tail = -1;
        }
    };

    ~_2lv_bucket_queue_DK() {
        delete[] top_bucket;
        delete[] bottom_bucket;
        delete[] qBucket;
        delete[] pool.pool;
        delete[] pool.free_list;
        delete[] pool.idxs;
    }
 
    void insert(int v, keyType dist){
        ll i =  (dist / b_size) & (b_size-1);  // se i = top bucket ativo, insere no bottom
        ll j = dist & (b_size-1);            // se não, insere no top
    
        if (i == at && j >= ab) {
            bottom_bucket[j].tail = pool.insert({dist,v}, bottom_bucket[j].tail);
            bottom_bucket[j].sz++;
            qBucket[v] = b_size+j; // pseudo índice do bottom bucket; 
        }
        else{
            top_bucket[i].tail = pool.insert({dist,v}, top_bucket[i].tail);
            top_bucket[i].sz++;
            qBucket[v] = i;
        }
        sz++;
    }
 
    void update(){
        // procura um bottom bucket não vazio
        while(ab < b_size && bottom_bucket[ab].sz==0) ab++;
        if(ab < b_size) return;

         
        // expand: se nao encontrar, distribui elementos de outro top bucket
        ll start = at;
        do {
            if(top_bucket[at].sz != 0) break;
            at++;
            if(at == b_size) at = 0;
        }  while(at != start);
        if(!top_bucket[at].sz) return;
 
        // distribui no bottom_bucket apenas os atuais, ignorando as novas inserções 
        int aux = top_bucket[at].sz;
        ab = b_size;
        int mod_b_size = b_size-1;
        for(int i=0;i<aux;i++){
            par a = pool.pool[top_bucket[at].tail].data; 
            top_bucket[at].tail = pool.pop(top_bucket[at].tail);
            top_bucket[at].sz--;

            ll nova_ab = a.first & (mod_b_size); // a.first % b_size;
            ab = min(ab, nova_ab);
            bottom_bucket[nova_ab].tail = pool.insert(a, bottom_bucket[nova_ab].tail);
            qBucket[a.second] = b_size+nova_ab;
            bottom_bucket[nova_ab].sz++;
        }
    }
 
    par extract_min(){
        update();
        par min_elem = pool.pool[bottom_bucket[ab].tail].data;
        bottom_bucket[ab].tail = pool.pop(bottom_bucket[ab].tail);
        bottom_bucket[ab].sz--;
        sz--;
        return min_elem;
    }
 
    bool empty() {
        return sz == 0;
    }

    void clear(){
        for(ll i = 0; i < b_size; i++){
            top_bucket[i].tail = bottom_bucket[i].tail = -1;
            top_bucket[i].sz = bottom_bucket[i].sz = 0;
        }
        pool.clear(n);
        at = ab = sz = 0;
    }

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        if(pool.idxs[u] != -1) {
            ll k = qBucket[u];
            int idx = pool.idxs[u];

            // Se u é o tail do bucket, atualiza o tail para o prev
            if(k < b_size) {
                if(top_bucket[k].tail == idx)
                    top_bucket[k].tail = pool.pool[idx].prev;
                top_bucket[k].sz--;
            } else {
                int kb = k & (b_size - 1);
                if(bottom_bucket[kb].tail == idx)
                    bottom_bucket[kb].tail = pool.pool[idx].prev;
                bottom_bucket[kb].sz--;
            }

            pool.remove(u);
            sz--;
        }
        insert(u, new_du);
    }
};
  
void init_dijkstra(auto &d, auto &p, _2lv_bucket_queue_DK &q, const int n, const int s) {
    d = vector<ll>(n, oo);
    p = vector<ll>(n, -1);
    d[s] = 0;
    if (!q.empty()) {
        q.clear();
    }
    q.insert(s, 0);
}
 
void dijkstra(const auto &adj, auto &d, auto &p, _2lv_bucket_queue_DK &q) {
    while(!q.empty()) {
        auto [du, u] = q.extract_min();
        if(d[u] < du) continue;

        for(auto [w, v] : adj[u]) {
            if(d[v] > d[u] + w) {
                d[v] = d[u] + w;
                q.insert(v, d[v]);
            }
        }
    }
};
 
signed main() {
    ios::sync_with_stdio(false); cin.tie(0);
 
    int n, m;
    cin >> n >> m;
 
    keyType bigW = -1;
    while(m--) {
        int a, b;
        keyType w;
        cin >> a >> b >> w;
        a--; b--;
        adj[a].push_back({w, b});
        bigW = max(bigW, w);
    }
 
    _2lv_bucket_queue_DK pq(bigW, n);
    
    vector<ll> d, p;
    init_dijkstra(d, p, pq, n, 0);
    dijkstra(adj, d, p, pq);
    for(int i = 0; i < n; i++) cout << d[i] << " ";
    cout << endl;
 
    return 0;
}