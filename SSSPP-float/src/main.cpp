#include "dijkstra.cpp"
#include "../utils/perf.hpp"
#include "bits/stdc++.h"
using namespace std;

// sudo sysctl -w kernel/perf_event_paranoid=1 (pra permitir ler cache miss)
// g++ -std=c++20 main.cpp -o main
// g++ -fsanitize=address,undefined -g -std=c++20 main.cpp -o main

pq create_pq(PQS q, int n, keyType c, int k=0, keyType delta) {
    switch(q) {
        case BINHCPP : return new binheapCPP();
        case BINH: return new binheap(n);
        case RH: return new radixHeap();
        case _2LVBQDK : return new _2lvbqDK(c, n, delta);
        case _4LVBQDK : return new _4lvbqDK(c, n);
        default: return nullptr;
    }
}

void exp(){

    timer otimer;
    CacheStats cache;

    // 1. Ler e definir filas e grafos ----------------

    vector<string> pqs;
    vector<string> inputs;

    string inputsFile = "input.txt";
    string pqsFile = "heaps.txt";

    ifstream file_data(inputsFile);
    ifstream file_pqs(pqsFile);

    string line;
    while(getline(file_data, line)){
        istringstream iss(line);
        string s;
        iss >> s;
        inputs.push_back(s);
    }

    while(getline(file_pqs, line)){
        istringstream iss(line);
        string s;
        iss >> s;
        pqs.push_back(s);
    }

    // -----------------------------------------------

    // 2. Formatar arquivo de saída dos resultados----

    string output = "../data/outs/all_time.csv";

    ofstream fileO(output);

    fileO << fixed << setprecision(6);
    fileO <<"nome n m c fila run l1 l2 llc cycles inst\n";
    fileO.flush();

    // ------------------------------------------------

    // 3. Experimentos --------------------------------

    for (string f : inputs){
        
        // Abre o arquivo

        string filename = "../data/txts/"+f+".txt";
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Erro ao abrir o arquivo: " << filename << endl;
            continue;
        }

        string line;
        getline(file, line);
        istringstream iss(line);
        int qtd_ver, qtd_edges;
        iss >> qtd_ver >> qtd_edges; // quantidade de vértices e arestas

        keyType max_weight = 0;
        keyType min_weight = numeric_limits<double>::max();
        graph g;
        g.resize(qtd_ver + 1);

        // Lê grafo

        while (getline(file, line)) {
            istringstream iss(line);
            int a, b;
            keyType c;
            
            if (iss >> a >> b >> c) {

                a--; b--;
                g[a].push_back({c, b});
                g[b].push_back({c, a});

                max_weight = max(max_weight, c);
                min_weight = min(min_weight, c);
            }
        }
        file.close();

        // Log

        cout << "\nNome: " << f << "\n";
        cout << "Vértices: " << qtd_ver << "\n";
        cout << "Arestas: " << qtd_edges << "\n";
        cout << "Maior peso: " << max_weight << "\n";
        cout << "Carregando..." << "\n";

        int elap;

        // Experimentos

        // Gabarito (usa fila de prioridade do C++ para comparar resultados)

        int st = 10;

        shortest_path gabarito;
        pq pq_cpp = create_pq(BINHCPP, qtd_ver, max_weight, 6);
        gabarito.init_dijkstra(pq_cpp, qtd_ver, st, false);
        gabarito.dijkstra_ndk(g, pq_cpp);
        delete pq_cpp;

        for(string s : pqs) {
            
            PQS p = toPq[s];
            bool dk=false;
            for(PQS pq : {RBT, FIBH, _1LVBQDK, _2LVBQDK, _4LVBQDK, _KLVBQDK}) if (p==pq) dk=true;

            shortest_path sp;
            for(int i = 0; i < 10; i++) {
                pq q = create_pq(p, qtd_ver, max_weight, 6);

                if(dk){
                    cache.start();
                    otimer.start();
                    sp.init_dijkstra(q, qtd_ver, st, dk);
                    sp.dijkstra_dk(g, q);
                    otimer.stop();
                    cache.stop();
                }
                else{
                    cache.start();
                    otimer.start();
                    sp.init_dijkstra(q, qtd_ver, st, dk);
                    sp.dijkstra_ndk(g, q);
                    otimer.stop();
                    cache.stop();
                }

                elap = otimer.elapsed();
                fileO << f << " " <<  qtd_ver << " " << qtd_edges <<  " " << max_weight << " " << s << " " << elap << 
                " " << cache.r_l1 << " " << cache.r_l2 << " " << cache.r_llc << " " << cache.r_cycles << " " << cache.r_instructions << "\n";
                
                if (i == 0) {
                    for(int j = 0; j < qtd_ver; j++){
                        if(gabarito.dist[j] != sp.dist[j]){
                            cerr << "\nERRO: distâncias calculadas com a fila " << s << " incorretas.\n";
                            exit(1);
                        }
                    }
                }
                sp.clear();
                delete q;
            }
            fileO.flush();
        }

        cout << "\n------------------------------\n";

        gabarito.clear();
    }
    fileO.close();
}

int main() {
    timer tot_timer;
    tot_timer.start();
    exp();
    tot_timer.stop();
    cout << "\nTempo total de execução: " << tot_timer.elapsed() << "\n";
    return 0;
}