// Generate a random graph in DIMACS format (supports sparse and dense graphs)

// g++ -O2 -o gen_dense random-dense-graph-generator.cpp
// ./gen_dense <num_vertices> <qtd_arestas> <peso_max> <seed>
// ./gen_dense 1000 800000 100 42 > ../data/txts/grafo_2.txt

#include <bits/stdc++.h>
#include <random>
#define int long long
using namespace std;

signed main(signed argc, char **argv) {
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << " number_of_vertices num_edges max_weight seed" << endl;
        cerr << "  number_of_vertices : number of nodes" << endl;
        cerr << "  num_edges          : total number of directed edges (>= n-1)" << endl;
        cerr << "  max_weight         : maximum edge weight (>= 0)" << endl;
        cerr << "  seed               : random seed" << endl;
        return 1;
    }

    int n       = atoll(argv[1]);
    int m_target = atoll(argv[2]);
    int max_weight = atoll(argv[3]);
    int seed    = atoll(argv[4]);

    // Maximum possible directed edges without self-loops
    int max_edges = n * (n - 1);

    if (m_target < n - 1) {
        cerr << "Error: num_edges must be at least n-1 (" << n - 1 << ") to ensure connectivity." << endl;
        return 1;
    }
    if (m_target > max_edges) {
        cerr << "Error: num_edges exceeds maximum possible (" << max_edges << ") for n=" << n << "." << endl;
        return 1;
    }

    mt19937_64 gen(seed);
    auto random_integer = [&](int l, int r) {
        uniform_int_distribution<uint64_t> dis(l, r);
        return (int)dis(gen);
    };

    // Use flat edge set for O(1) duplicate checking: key = (u-1)*n + (v-1)
    unordered_set<int> edge_set;
    edge_set.reserve(m_target * 2);

    auto hasEdge = [&](int i, int j) -> bool {
        return edge_set.count((i - 1) * n + (j - 1)) > 0;
    };
    auto markEdge = [&](int i, int j) {
        edge_set.insert((i - 1) * n + (j - 1));
    };

    // adj only used for spanning tree construction and reachability check
    vector<vector<int>> adj(n + 1);

    // --- Build a random spanning tree rooted at 1 (ensures connectivity) ---
    // Use random Prüfer-like approach: each node i picks a random parent in [1, i-1]
    {
        vector<vector<int>> undirected(n + 1);
        for (int i = 2; i <= n; i++) {
            int j = random_integer(1, i - 1);
            undirected[i].push_back(j);
            undirected[j].push_back(i);
        }

        // DFS from a random root, orient edges away from root, then swap root with 1
        vector<vector<int>> directed(n + 1);
        int root = random_integer(1, n);

        function<void(int, int)> dfs = [&](int u, int par) {
            for (int v : undirected[u]) {
                if (v == par) continue;
                directed[u].push_back(v);
                dfs(v, u);
            }
        };
        dfs(root, -1);

        // Remap: swap labels so that node 1 is the root
        auto remap = [&](int x) -> int {
            if (x == 1) return root;
            if (x == root) return 1;
            return x;
        };

        for (int u = 1; u <= n; u++) {
            for (int v : directed[u]) {
                int ru = remap(u), rv = remap(v);
                adj[ru].push_back(rv);
                markEdge(ru, rv);
            }
        }
    }

    // Output header
    cout << n << " " << m_target << "\n";

    // Print spanning tree edges
    for (int u = 1; u <= n; u++) {
        for (int v : adj[u]) {
            cout << u << " " << v << " " << random_integer(0, max_weight) << "\n";
        }
    }

    // --- Add remaining random edges ---
    int remaining = m_target - (int)edge_set.size();

    // Density check: if graph is very dense (>50%), use complement sampling
    double density = (double)m_target / (double)max_edges;

    if (density <= 0.5) {
        // Sparse strategy: pick random pairs until we get a new edge
        // To avoid infinite loops near saturation, track attempts
        while (remaining > 0) {
            int u = random_integer(1, n);
            int v = random_integer(1, n);
            if (u == v || hasEdge(u, v)) continue;
            markEdge(u, v);
            cout << u << " " << v << " " << random_integer(0, max_weight) << "\n";
            remaining--;
        }
    } else {
        // Dense strategy: enumerate all missing edges, shuffle, pick what we need
        vector<pair<int,int>> missing;
        missing.reserve(max_edges - (int)edge_set.size());
        for (int u = 1; u <= n; u++) {
            for (int v = 1; v <= n; v++) {
                if (u != v && !hasEdge(u, v)) {
                    missing.push_back({u, v});
                }
            }
        }
        shuffle(missing.begin(), missing.end(), gen);
        for (int k = 0; k < remaining; k++) {
            auto [u, v] = missing[k];
            cout << u << " " << v << " " << random_integer(0, max_weight) << "\n";
        }
    }

    // --- Verify reachability from node 1 ---
    vector<bool> vis(n + 1, false);
    queue<int> q;
    q.push(1);
    vis[1] = true;
    // Rebuild full adj for BFS
    vector<vector<int>> full_adj(n + 1);
    for (auto key : edge_set) {
        int u = key / n + 1;
        int v = key % n + 1;
        full_adj[u].push_back(v);
    }
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : full_adj[u]) {
            if (!vis[v]) { vis[v] = true; q.push(v); }
        }
    }
    assert(accumulate(vis.begin(), vis.end(), 0LL) == n);

    return 0;
}