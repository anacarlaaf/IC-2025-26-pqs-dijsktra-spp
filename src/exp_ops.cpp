#include "dijkstra.cpp"
#include "../utils/perf.hpp"
#include "bits/stdc++.h"
using namespace std;

// sudo sysctl -w kernel/perf_event_paranoid=1 (pra permitir ler cache miss)
// g++ -std=c++20 exp_ops.cpp -o exp_ops
// g++ -fsanitize=address,undefined -g -std=c++20 exp_opscpp -o exp_ops

pq create_pq(PQS q, int n, keyType c, int k=0) {
    switch(q) {
        case BINHCPP : return new binheapCPP();
        case BINH: return new binheap(n);
        case FIBH: return new fibonacci(n);
        case _1LVBQ : return new _1lvbq(c);
        case _2LVBQ : return new _2lvbq(c);
        case _KLVBQ : return new _klvbq(c, k);
        case _1LVBQDK : return new _1lvbqDK(c, n);
        case _2LVBQDK : return new _2lvbqDK(c, n);
        case _KLVBQDK : return new _klvbqDK(c, n, k);
        default: return nullptr;
    }
}

void exp(){

    // 1. Ler e definir filas e grafos ----------------

    vector<string> pqs;
    vector<string> inputs;

    string inputsFile = "in_graphs.txt";
    string pqsFile = "int_filas.txt";

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

    string output = "../data/outs/all_ops.csv";

    ofstream fileO(output);

    fileO << fixed << setprecision(6);
    fileO <<"nome n m c fila extractMin insert dk \n";
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
            }
        }
        file.close();

        // Log

        cout << "\nNome: " << f << "\n";
        cout << "Vértices: " << qtd_ver << "\n";
        cout << "Arestas: " << qtd_edges << "\n";
        cout << "Maior peso: " << max_weight << "\n";
        cout << "Carregando..." << "\n";

        int elap_cpu, elap_wall;

        // Experimentos

        // Gabarito (usa fila de prioridade do C++ para comparar resultados) ----------

        int st = 10;

        shortest_path gabarito;
        pq pq_cpp = create_pq(BINHCPP, qtd_ver, max_weight, 6);
        gabarito.init_dijkstra(pq_cpp, qtd_ver, st, false);
        gabarito.dijkstra_ndk(g, pq_cpp);
        delete pq_cpp;

        // ----------------------------------------------------------------------------

        for(string s : pqs) {
            int tam = s.size();
            PQS p;
            int lv_int = 1;
            char lv = '1';
            if(s.compare(1,4, "LVBQ")==0 && s[0]-'0'>2){
                if(s[tam-1]=='K') p = _KLVBQDK;
                else p = _KLVBQ;
                lv = s[0];
                lv_int = lv-'0';
            }
            else {
                auto it = toPq.find(s);
                if (it == toPq.end()) {
                    cerr << "Fila inválida: " << s << "\n";
                    exit(1);
                }
                p = toPq[s];
            }
            bool dk=false;
            if (p==FIBH || p==RBT || p==_KLVBQDK || p==_2LVBQDK || p==_1LVBQDK) dk=true;
            pq q;

            timer otimer;
            CacheStats cache;

            shortest_path sp;
            for(int i = 0; i < 1; i++) {
                pair<pair<int,int>,int> ops_dk = {{1,0},0}; //{{insert, dk}, extractMin}
                pair<int,int> ops_ndk = {1,0}; //{insert, extractMin}
                // insert começa com 1 por causa do 0 inserido no init_dijsktra

                if(dk){
                    q = create_pq(p, qtd_ver, max_weight, lv_int);
                    sp.init_dijkstra(q, qtd_ver, st, dk);
                    ops_dk = sp.dijkstra_dk_perf(g, q);
                    fileO << f << " " << qtd_ver << " " << qtd_edges << " " << max_weight
                    << " " << s << " " << ops_dk.second << " " << ops_dk.first.first << " " << ops_dk.first.second << "\n";

                }
                else{
                    q = create_pq(p, qtd_ver, max_weight, lv_int);
                    sp.init_dijkstra(q, qtd_ver, st, dk);
                    ops_ndk = sp.dijkstra_ndk_perf(g, q);
                    fileO << f << " " << qtd_ver << " " << qtd_edges << " " << max_weight
                    << " " << s << " " << ops_ndk.second << " " << ops_ndk.first << " " << 0 << "\n";
                }

                if(s[0]=='K') s = '_'+lv+"BQTH";

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
    cout << "\nTempo total de execução: " << tot_timer.elapsed_wall_ms() << "\n";
    return 0;
}