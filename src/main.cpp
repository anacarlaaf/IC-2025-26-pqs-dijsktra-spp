#include "dijkstra.cpp"
#include "../utils/perf.hpp"
#include "bits/stdc++.h"
using namespace std;

// sudo sysctl -w kernel/perf_event_paranoid=1 (pra permitir ler cache miss)
// g++ -std=c++20 main.cpp -o main
// g++ -fsanitize=address,undefined -g -std=c++20 main.cpp -o main

pq create_pq(PQS q, int n, keyType c, int k=0) {
    switch(q) {
        case BINHCPP : return new binheapCPP();
        case BINH: return new binheap(n);
        case FIBH: return new fibonacci(n);
        case _1LVBQ : return new _1lvbq(c, n);
        case _2LVBQ : return new _2lvbq(c, n);
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
    fileO <<"nome n m c fila cpu_time wall_time l1 l2 llc cycles inst tlb dtlb page_faults rss branch_inst branch_miss\n";
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
                char lv = s[0];
                int lv_int = lv-'0';
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
            for(int i = 0; i < 10; i++) {
                if(dk){
                    cache.start();
                    otimer.start();
                    q = create_pq(p, qtd_ver, max_weight, lv_int);
                    sp.init_dijkstra(q, qtd_ver, st, dk);
                    sp.dijkstra_dk(g, q);
                    otimer.stop();
                    cache.stop();
                }
                else{
                    cache.start();
                    otimer.start();
                    q = create_pq(p, qtd_ver, max_weight, lv_int);
                    sp.init_dijkstra(q, qtd_ver, st, dk);
                    sp.dijkstra_ndk(g, q);
                    otimer.stop();
                    cache.stop();
                }

                if(s[0]=='K') s = '_'+lv+"BQTH";
                elap_cpu = otimer.elapsed_cpu_ms();
                elap_wall = otimer.elapsed_wall_ms();

                fileO << f << " " << qtd_ver << " " << qtd_edges << " " << max_weight << " " << s << " " << elap_cpu << " " << elap_wall << 
                " " << cache.r_l1 << " " << cache.r_l2 << " " << cache.r_llc << " " << cache.r_cycles << " " << cache.r_instructions << " " << 
                cache.r_dtlb << " " << cache.r_page_faults << " " << cache.r_rss_kb << " " << cache.r_branch_instr << " " << cache.r_branch_miss << "\n";

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