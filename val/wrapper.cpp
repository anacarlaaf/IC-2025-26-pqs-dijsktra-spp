#include "../helpers/external/doctest.h"
#include "dijkstra.cpp"

#include "common.cpp"

#include "../helpers/dijkstra.hpp"
#include "../helpers/common.hpp"

#include <vector>
#include <string>
#include <cmath>
#include <limits>

// ---------------------------------------------------------------------------
// Wrapper que adapta shortest_path (dijkstra.cpp) à interface esperada por
// checkReturns: execute(source) → {dist, pai}  e  get_shortest_path(u, pred)
// ---------------------------------------------------------------------------
template<typename wT>
struct sp_wrapper {
    using adjT = std::vector<std::vector<std::pair<int, wT>>>;

    adjT adj;
    int  n;
    const wT oo = std::numeric_limits<wT>::max() / 10;

    // heap usado internamente – substitua pelo heap desejado (dk ou ndk)
    pq   q;
    bool use_dk;   // true → dijkstra_dk, false → dijkstra_ndk
    shortest_path sp;

    sp_wrapper(const adjT &adj, pq heap_instance, bool use_dk = false)
        : adj(adj), n(adj.size()), q(heap_instance), use_dk(use_dk) {}

    std::pair<std::vector<keyType>, std::vector<int>> execute(int s) {
        // limpa estado anterior e reinicializa
        sp.clear();
        q->clear();
        sp.init_dijkstra(q, n, s, use_dk);

        if (use_dk)
            sp.dijkstra_dk(adj, q);
        else
            sp.dijkstra_ndk(adj, q);

        return {sp.dist, sp.pai};
    }

    std::vector<int> get_shortest_path(int u, const std::vector<int> &pred) {
        if (sp.dist[u] >= oo) return {};

        std::vector<int> path;
        path.reserve(n);
        while (u != -1) {
            path.push_back(u);
            u = pred[u];
        }
        std::reverse(path.begin(), path.end());
        return path;
    }
};

// ---------------------------------------------------------------------------
// Macro auxiliar para instanciar o wrapper com o heap adequado.
// Ajuste make_heap_ndk() / make_heap_dk() conforme as factories do seu projeto.
// ---------------------------------------------------------------------------
// Exemplo de uso (assumindo que existem factories globais):
//   pq make_heap_ndk();   // retorna heap sem decrease-key
//   pq make_heap_dk();    // retorna heap com decrease-key

TEST_CASE("King graphs with a wall") {
    using distT = double;
    std::vector<std::string> paths = {
        "../tests/graphs/grid5.gr",
        "../tests/graphs/grid25.gr",
        "../tests/graphs/grid100.gr"
    };
    for (std::string path : paths) {
        auto [adj, m] = readGraph<distT>(path);

        spp::dijkstra<distT> dijkstra_ref(adj);
        sp_wrapper<distT>    bmssp(adj, make_heap_ndk(), /*use_dk=*/false);

        n = adj.size();
        int grid_sz = sqrt(n);
        for (int source : {0, grid_sz - 1, grid_sz * (grid_sz - 1), n - 2, n / 2}) {
            checkReturns<distT>(source, dijkstra_ref, bmssp, false);
        }
    }
}

TEST_CASE("Small random graphs") {
    using distT = long long;
    std::vector<std::string> paths = {
        "../tests/graphs/random32D3.gr",
        "../tests/graphs/random256D3.gr",
        "../tests/graphs/random1024D3.gr",
        "../tests/graphs/random4096D3.gr"
    };
    for (std::string path : paths) {
        auto [adj, m] = readGraph<distT>(path);

        spp::dijkstra<distT> dijkstra_ref(adj);
        sp_wrapper<distT>    bmssp(adj, make_heap_ndk(), /*use_dk=*/false);

        n = adj.size();
        for (int source : {0, n / 3, n / 2, n / 3 * 2, n - 1}) {
            checkReturns<distT>(source, dijkstra_ref, bmssp, false);
        }
    }
}

TEST_CASE("King graphs with a wall - CD") {
    using distT = double;
    std::vector<std::string> paths = {
        "../tests/graphs/grid5.gr",
        "../tests/graphs/grid25.gr",
        "../tests/graphs/grid100.gr"
    };
    for (std::string path : paths) {
        auto [adj, m] = readGraph<distT>(path);

        spp::dijkstra<distT> dijkstra_ref(adj);
        sp_wrapper<distT>    bmssp(adj, make_heap_dk(), /*use_dk=*/true);

        n = adj.size();
        int grid_sz = sqrt(n);
        for (int source : {0, grid_sz - 1, grid_sz * (grid_sz - 1), n - 2, n / 2}) {
            checkReturns<distT>(source, dijkstra_ref, bmssp, true);
        }
    }
}

TEST_CASE("Small random graphs - CD") {
    using distT = long long;
    std::vector<std::string> paths = {
        "../tests/graphs/random32D3.gr",
        "../tests/graphs/random256D3.gr",
        "../tests/graphs/random1024D3.gr",
        "../tests/graphs/random4096D3.gr"
    };
    for (std::string path : paths) {
        auto [adj, m] = readGraph<distT>(path);

        spp::dijkstra<distT> dijkstra_ref(adj);
        sp_wrapper<distT>    bmssp(adj, make_heap_dk(), /*use_dk=*/true);

        n = adj.size();
        for (int source : {0, n / 3, n / 2, n / 3 * 2, n - 1}) {
            checkReturns<distT>(source, dijkstra_ref, bmssp, true);
        }
    }
}