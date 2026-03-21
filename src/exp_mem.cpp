#include "dijkstra.cpp"
#include "bits/stdc++.h"
#include "../utils/perf.hpp"
#include <sys/resource.h>
using namespace std;

// sudo sysctl -w kernel/perf_event_paranoid=1 (pra permitir ler cache miss)
// g++ -std=c++20 exp_mem.cpp -o exp_mem
// g++ -fsanitize=address,undefined -g -std=c++20 exp_mem.cpp -o exp_mem

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

long long getPeakMemory() {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        long long peakMemory = usage.ru_maxrss * 1024;
        return peakMemory;
    }
    return 0;
}

void exp(string input, string fila, char restart){


    // Experimentos --------------------------------

    // Abre o arquivo

    string filename = "../data/txts/"+input+".txt";
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo: " << filename << endl;
        return;
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

    cout << "\nNome: " << input << "\n";
    cout << "Vértices: " << qtd_ver << "\n";
    cout << "Arestas: " << qtd_edges << "\n";
    cout << "Maior peso: " << max_weight << "\n";
    cout << "Carregando..." << "\n";

    // Experimentos

    // Gabarito (usa fila de prioridade do C++ para comparar resultados) ----------

    int st = 10;
    shortest_path gabarito;

    pq pq_cpp = create_pq(BINHCPP, qtd_ver, max_weight, 6);
    gabarito.init_dijkstra(pq_cpp, qtd_ver, st, false);
    gabarito.dijkstra_ndk(g, pq_cpp);
    delete pq_cpp;

    // ----------------------------------------------------------------------------

    int tam = fila.size();
    PQS p;
    int lv_int = 1;
    char lv = '1';
    if(fila.compare(1,4, "LVBQ")==0 && fila[0]-'0'>2){
        if(fila[tam-1]=='K') p = _KLVBQDK;
        else p = _KLVBQ;
        lv = fila[0];
        lv_int = lv-'0';
    }
    else {
        auto it = toPq.find(fila);
        if (it == toPq.end()) {
            cerr << "Fila inválida: " << fila << "\n";
            exit(1);
        }
        p = toPq[fila];
    }
    bool dk=false;
    if (p==FIBH || p==RBT || p==_KLVBQDK || p==_2LVBQDK || p==_1LVBQDK) dk=true;
    pq q;

    long long mem_st = getPeakMemory(); // mede mem do gabarito e grafo para isolar o uso de mem de dijkstra+fila

    shortest_path sp;
    for(int i = 0; i < 10; i++) {
        if(dk){
            q = create_pq(p, qtd_ver, max_weight, lv_int);
            sp.init_dijkstra(q, qtd_ver, st, dk);
            sp.dijkstra_dk(g, q);
        }
        else{
            q = create_pq(p, qtd_ver, max_weight, lv_int);
            sp.init_dijkstra(q, qtd_ver, st, dk);
            sp.dijkstra_ndk(g, q);
        }

        if(fila[0]=='K') fila = '_'+lv+"BQTH";

        if (i == 0) {
            for(int j = 0; j < qtd_ver; j++){
                if(gabarito.dist[j] != sp.dist[j]){
                    cerr << "\nERRO: distâncias calculadas com a fila " << fila << " incorretas.\n";
                    exit(1);
                }
            }
        }
        sp.clear();
        delete q;
    }

    long long mem_fim = getPeakMemory();

    cout << "\n------------------------------\n";

    gabarito.clear();

    string output = "../data/outs/all_mem.csv";

    if (restart == '1') {
        // Primeira execução: trunca e escreve header
        ofstream fileO(output);
        fileO << "nome n m c fila mem\n";
        fileO.close();
    } else if (restart != '0') {
        cout << "Valor inválido de restart (0 ou 1)\n";
        exit(1);
    }

    // Sempre abre em modo append para escrever o resultado
    ofstream fileO(output, ios::app);
    fileO << fixed << setprecision(6);
    long long delta = mem_fim - mem_st;
    cout << "Uso de memória: " << delta << " bytes\n";
    fileO << input << " " << qtd_ver << " " << qtd_edges << " "
        << max_weight << " " << fila << " " << delta << "\n";
    fileO.close();
    }

int main(int argc, char* argv[]) {
    if (argc != 4) {  // programa + 2 argumentos
        cout << "Uso: programa <input> <fila> <restart> <>\n";
        return 1;
    }
    string input = argv[1];
    string fila= argv[2];
    char restart = argv[3][0];
    

    timer tot_timer;
    tot_timer.start();
    exp(input, fila, restart);
    tot_timer.stop();

    cout << "Tempo total de execução: " << tot_timer.elapsed_wall_ms() << " ms\n";
    return 0;
}