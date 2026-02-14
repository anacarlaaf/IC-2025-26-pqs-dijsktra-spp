#include<bits/stdc++.h>
using namespace std;
using ll = long long;
using keyType = ll;
using par = pair<keyType, int>;
 
const ll oo = 1e16;
const int maxn = 1e6+2;
vector<par> adj[maxn];
 
 
struct _1lv_bucket_queue{
    int pot2[30];
    vector<list<int>> b;
    vector<list<int>::iterator> ptr;
    bitset<10000000> mask;
 
    int nbuckets = -1;
    int a = 0, r = 0, c = 0;
 
    int getCsize(int c){
        // Calcula menor potência de 2 maior que c.
        int p = 1;
        while (p <= c) p <<= 1;
        return p;
    }
 
    _1lv_bucket_queue(int c_, int n){
        // Inicia o 1-level bucket queue
        c = getCsize(c_);
        nbuckets = c;
        b.assign(nbuckets, {});
        ptr.resize(n, b[0].end());
    };
 
    void clear() {
        a = 0, r = 0;
        for(auto &l: b) l.clear();
        mask.reset();
    }
    
    void insert(int u, keyType du, keyType w) {
        int id = (a+w) & (nbuckets - 1);
        b[id].push_back(u);
        ptr[u] = prev(b[id].end());
        mask[id] = 1;
    }

    void update() {
        if (mask[a]) return;

        int bg = a;
        do {
            a = (a + 1) & (nbuckets - 1);
            if (a == 0) r++;
            if (mask[a]) return;
        } while (a != bg);

        assert(false);
    }
 
    par extract_min() {
        update();
        int u = b[a].front();
        keyType du = r * nbuckets + a;
        b[a].pop_front();
        if(b[a].empty()) mask[a] = 0;
        return make_pair(du, u);
    }
 
    bool empty() {
        return mask.none();
    }
 
    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        if(ptr[u] != b[0].end()){
            int id = old_du & (nbuckets - 1);
            b[id].erase(ptr[u]);
            if(b[id].empty()) mask[id]=0;
        }
        insert(u, new_du, w);
    }
 
};
 
void init_dijkstra(auto &d, auto &p, _1lv_bucket_queue &q, const int n, const int s) {
    d = vector<ll>(n, oo);
    p = vector<ll>(n, -1ll);
    d[s] = 0;
    q.insert(s, 0, 0);
}
 
 
void dijkstra(const auto &adj, auto &d, auto &p, _1lv_bucket_queue &q) {
 
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
 
    vector<vector<pair<int, int>>> adj(n);
    int bigW = -1;
    while(m--) {
        int a, b, w; cin >> a >> b >> w;
        a--; b--;
        adj[a].push_back({w, b});
        bigW = max(bigW, w);
    }
 
    _1lv_bucket_queue pq(bigW, n);
    
    vector<ll> d, p;
    init_dijkstra(d, p, pq, n, 0);
    dijkstra(adj, d, p, pq);
    for(int i = 0; i < n; i++) cout << d[i] << " ";
    cout << endl;
 
    return 0;
}