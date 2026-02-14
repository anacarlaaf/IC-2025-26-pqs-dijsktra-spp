#include<bits/stdc++.h>
using namespace std;
using ll = long long;
using keyType = ll;
using par = pair<keyType, int>;

// g++ -fsanitize=address,undefined -g -o a.out _4_lv.cpp

const ll oo = 1e16;
const int maxn = 1e6+2;
vector<par> adj[maxn];

struct nptr {
    int lvl;
    int idx;
    list<par>::iterator it;
};


struct _4_lv_bkt{
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
    _4_lv_bkt(keyType c, int n){                  // c = maior peso
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

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){

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

void init_dijkstra(auto &d, auto &p, _4_lv_bkt &q, const int n, const int s) {
    d = vector<ll>(n, oo);
    p = vector<ll>(n, -1ll);
    d[s] = 0;
    q.insert(s, 0, 0);
}
 

void dijkstra(const auto &adj, auto &d, auto &p, _4_lv_bkt &q) {
    while(!q.empty()) {
        auto [du, u] = q.extract_min();
 
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

    _4_lv_bkt pq(bigW, n);
    
    vector<ll> d, p;
    init_dijkstra(d, p, pq, n, 0);
    dijkstra(adj, d, p, pq);
    for(int i = 0; i < n; i++) cout << d[i] << " ";
    cout << endl;
 
    return 0;
}