#include<bits/stdc++.h>
using namespace std;
using ll = long long;
using keyType = ll;
using par = pair<keyType, int>;

// g++ -fsanitize=address,undefined -g -o a.out _klv.cpp

const ll oo = 1e16;
const int maxn = 1e6+2;
vector<par> adj[maxn];

struct nptr {
    int lvl;
    int idx;
    list<par>::iterator it;
};


struct _klv_bkt{
    vector<vector<list<par>>> bucket; //
    vector<nptr> ptr;
    vector<int> actLv; //
    vector<ll> size;   //
    vector<ll> act;    //     // bucket ativos
    int sz;
    int lv;

    int getCsize(int c){
        // Calcula menor potência de 2 maior que c.
        int p = 1;
        while (p <= c) p <<= 1;
        return p;
    }
 
    // inicia os buckets
    _klv_bkt(keyType c, int n, int k){                  // c = maior peso
        lv = k;
        int sqrtSize = pow(c + 1.0, 1.0/k);
        size.resize(k, 0);
        size[0] = getCsize(sqrtSize); // potência de 2
        bucket.resize(k, {});
        actLv.resize(k, 0);
        act.resize(k, 0);

        for(int i=0;i<k;i++) {
            bucket[i].resize(size[0], {});
            act[i] = 0;
            actLv[i] = 0;
        }
        ptr.resize(n, {-1, -1, list<par>::iterator{}});

        ll aux = size[0];
        for(int i=1;i<k;i++){
            size[i] = aux;
            aux *= size[0];
        }
        sz = 0;
    };
 
    void insert(int v, keyType dist, keyType w){
        ll lvs[lv];
        for(int i=0;i<lv-1;i++) lvs[i] = (dist/size[lv-i-1]) & (size[0]-1); // equivale a (dist/size[0]) % size[0]
        lvs[lv-1] = dist & (size[0]-1);
        sz++;

        for(int i=0;i<lv-2;i++){
            if(lvs[i] != act[i]){
                bucket[i][lvs[i]].push_back({dist, v});
                ptr[v].lvl = i;
                ptr[v].idx = lvs[i];
                ptr[v].it = prev(bucket[i][lvs[i]].end());
                actLv[i]++;
                return;
            }
        }

        if (lvs[lv-2] != act[lv-2] || lvs[lv-1] < act[lv-1]){
            bucket[lv-2][lvs[lv-2]].push_back({dist, v});
            ptr[v].lvl = lv-2;
            ptr[v].idx = lvs[lv-2];
            ptr[v].it = prev(bucket[lv-2][lvs[lv-2]].end());
            actLv[lv-2]++;
        }
        else{
            bucket[lv-1][lvs[lv-1]].push_back({dist, v});
            ptr[v].lvl = lv-1;
            ptr[v].idx = lvs[lv-1];
            ptr[v].it = prev(bucket[lv-1][lvs[lv-1]].end());
            actLv[lv-1]++;
        }
    }
 
    void update(){
        
        // procura bucket não vazio no nível mais baixo
        int b = act[lv-1];
        do {
            if (!bucket[lv-1][b].empty()) {
                act[lv-1] = b;
                return;
            }
            b++;
            if (b >= size[0]) b = 0;
        } while (b != act[lv-1]);


        // se não encontrar, expandir
        // procurar nível não vazio
        int aux = lv-2;
        while(aux>=0 && actLv[aux] <= 0) aux--;

        // atualiza bucket ativo
        b = act[aux];
        do{
            if(b >= size[0]) b = 0;
            if(!bucket[aux][b].empty()) break;
            b++;
        } while(b!=act[aux]);
        act[aux] = b;
        assert(b < size[0]);
        assert(!bucket[aux][b].empty());

        // distribuir até chegarem ao último nível
        for(int i=aux; i<lv-1; i++){
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

        assert(!bucket[lv-1][act[lv-1]].empty());
        assert(act[lv-1]>=0 && act[lv-1]<size[0]);
        par min_elem = bucket[lv-1][act[lv-1]].front();
        bucket[lv-1][act[lv-1]].pop_front();
        ptr[min_elem.second].lvl = -1;
        ptr[min_elem.second].idx = -1;
        actLv[lv-1]--;
        sz--;
        return min_elem;
    }
 
    bool empty() {
        if (sz) return false;
        return true;
    }

    void clear(){
        for(int i=0;i<lv;i++) {
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

void init_dijkstra(auto &d, auto &p, _klv_bkt &q, const int n, const int s) {
    d = vector<ll>(n, oo);
    p = vector<ll>(n, -1ll);
    d[s] = 0;
    q.insert(s, 0, 0);
}
 

void dijkstra(const auto &adj, auto &d, auto &p, _klv_bkt &q) {
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

    _klv_bkt pq(bigW, n, 10);
    
    vector<ll> d, p;
    init_dijkstra(d, p, pq, n, 0);
    dijkstra(adj, d, p, pq);
    for(int i = 0; i < n; i++) cout << d[i] << " ";
    cout << endl;
 
    return 0;
}