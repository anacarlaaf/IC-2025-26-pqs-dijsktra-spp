#include<bits/stdc++.h>
using namespace std;
using ll = long long;
using keyType = ll;
using par = pair<keyType, int>;

const ll oo = 1e16;
const int maxn = 1e6+2;
vector<par> adj[maxn];


struct _2_lv_bkt{
    vector<list<par>> top_bucket, bottom_bucket;
    vector<list<par>::iterator> ptr;

    int qtdA = 0, qtdB = 0;
    ll at = 0, ab = 0; // at = top bucket ativo, ab = bottom bucket ativo
    ll b_size = 0;     // tamanho dos buckets

    int getCsize(int c){
        // Calcula menor potência de 2 maior que c.
        int p = 1;
        while (p <= c) p <<= 1;
        return p;
    }
 
    // inicia os buckets
    
    _2_lv_bkt(keyType c, int n){                  // c = maior peso
        b_size = getCsize(sqrt(c + 1) + 1); // quantidade de buckets
        top_bucket.resize(b_size);
        bottom_bucket.resize(b_size);
        ptr.resize(n, list<par>::iterator{});
    };
 
    void insert(int v, keyType dist, keyType w){
        // bit trick (equivale a dist / b_size % b_size;)
        int k = __builtin_ctz(b_size);
        ll i = (dist >> k) & (b_size - 1); // se i = top bucket ativo, insere no bottom
        ll j = dist & (b_size - 1);            // se não, insere no top
    
        if (i == at && j >= ab) {
            bottom_bucket[j].push_back({dist,v});
            ptr[v] = prev(bottom_bucket[j].end());
            qtdB++;
        }
        else{
            top_bucket[i].push_back({dist,v});
            ptr[v] = prev(top_bucket[i].end());
            qtdA++;
        }
    }
 
    void update(){
        // procura um bottom bucket não vazio
        ll start = ab;
        do {
            if(bottom_bucket[ab].size()) break;
            ab++;
            if(ab == b_size) ab = 0;
        }  while(ab != start);
        if(bottom_bucket[ab].size()) return;

         
        // expand: se nao encontrar, distribui elementos de outro top bucket
        start = at;
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
            auto a = top_bucket[at].front(); top_bucket[at].pop_front();
            qtdA--;
            ll nova_ab = a.first & (b_size-1); // a.first % b_size;
            ab = min(ab, nova_ab);
            bottom_bucket[nova_ab].push_back(a);  // insere no bottom bucket
            ptr[a.second] = prev(bottom_bucket[nova_ab].end());
            qtdB++;
        }
    }
 
    par extract_min(){
        update();
        par min_elem = bottom_bucket[ab].front();
        bottom_bucket[ab].pop_front();
        ptr[min_elem.second] = bottom_bucket[0].end();
        qtdB--;
        return min_elem;
    }
 
    bool empty() {
        return qtdA+qtdB == 0;
    }

    void clear(){
        for(int i=0;i<b_size;i++){
            while(!top_bucket[i].empty()) top_bucket[i].pop_front();
            while(!bottom_bucket[i].empty()) bottom_bucket[i].pop_front();
        }

        qtdA = 0; qtdB = 0;
        at = 0; ab = 0;
    }

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        if(ptr[u] != list<par>::iterator{}){
            int k = __builtin_ctz(b_size);
            ll i = (old_du >> k) & (b_size - 1);      // se i = top bucket ativo, ta em cima,
            ll j = old_du & (b_size - 1);            // se não, tá em baixo
        
            if (i == at && j >= ab) {
                bottom_bucket[j].erase(ptr[u]);
                qtdB--;
            }
            else{
                top_bucket[i].erase(ptr[u]);
                qtdA--;
            }
        }

        insert(u, new_du, w);
    }

};

void init_dijkstra(auto &d, auto &p, _2_lv_bkt &q, const int n, const int s) {
    d = vector<ll>(n, oo);
    p = vector<ll>(n, -1ll);
    d[s] = 0;
    q.insert(s, 0, 0);
}
 
 
void dijkstra(const auto &adj, auto &d, auto &p, _2_lv_bkt &q) {
    while(!q.empty()) {
        auto [du, u] = q.extract_min();
        if(d[u] < du) continue;
 
        for(auto [w, v] : adj[u]) {
            if(d[v] > d[u] + w) {
                p[v] = u;
                q.decrease_key(v, w, d[v], d[u]+w);
                d[v] = d[u] + w;
            }
        }
    }
};
 
signed main() {
    ios::sync_with_stdio(false); cin.tie(0);
 
    int n, m;
    cin >> n >> m;
 
    int bigW = -1;
    while(m--) {
        int a, b, w; cin >> a >> b >> w;
        a--; b--;
        adj[a].push_back({w, b});
        bigW = max(bigW, w);
    }
 
    _2_lv_bkt pq(bigW, n);
    
    vector<ll> d, p;
    init_dijkstra(d, p, pq, n, 0);
    dijkstra(adj, d, p, pq);
    for(int i = 0; i < n; i++) cout << d[i] << " ";
    cout << endl;
 
    return 0;
}