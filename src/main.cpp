#include "dijkstra.cpp"
#include "../utils/perf.cpp"
#include <iostream>
#include <fstream>
using namespace std;

//g++ -std=c++20 main.cpp -o main
enum PQS {
    RB_TREE,
    FIBONACCI_HEAP,
    BINARY_HEAP,
    B_HEAP,
    DIAL,
    TWO_LV_BKT_HEAP,
    RADIX_HEAP,
};

string pq_string(PQS q) {
    switch(q) {
        case RB_TREE: return "RB_TREE";
        case FIBONACCI_HEAP: return "FIBONACCI_HEAP";
        case BINARY_HEAP: return "BINARY_HEAP";
        case B_HEAP: return "B_HEAP";
        case DIAL: return "DIAL";
        case TWO_LV_BKT_HEAP: return "TWO_LV_BKT_HEAP";
        case RADIX_HEAP : return "RADIX_HEAP";
        default: return "UNKNOWN";
    }
}

pq create_pq(PQS q, int n, keyType c) {
    switch(q) {
        case BINARY_HEAP: return new bin_heap(n);
        case B_HEAP : return new b_heap(n);
        case RB_TREE: return new rb_tree();
        case FIBONACCI_HEAP: return new fibonacci(n);
        case DIAL: return new dial(c);
        case TWO_LV_BKT_HEAP: return new _2_lv_bkt(c);
        case RADIX_HEAP : return new radix();
        default: return nullptr;
    }
}

void exec_time(){

    timer otimer;
    PQS pqs[7] = {BINARY_HEAP, B_HEAP, RB_TREE, FIBONACCI_HEAP, DIAL, TWO_LV_BKT_HEAP, RADIX_HEAP};
    string data[4] = {"BAY", "COL", "FLA", "E"};

    string output = "../data/outs/time.csv";
    ofstream fileO(output);

    fileO << fixed << setprecision(6);
    fileO <<"nome n m c ";
    fileO.flush();

    for (PQS p : pqs){
        fileO << pq_string(p) << " ";
        fileO.flush();
    }
    fileO << "\n";
    fileO.flush();

    for (string f : data){
        
        string filename = "../data/txts/"+f+".txt";
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Erro ao abrir o arquivo: " << filename << endl;
            continue;
        }

        string line;

        // Lê quantidade de vértices e arestas
        getline(file, line);
        istringstream iss(line);
        int qtd_ver, qtd_edges;
        iss >> qtd_ver >> qtd_edges;

        int edge_count = 0;
        keyType max_weight = -1;
        graph g;
        g.resize(qtd_ver + 1);

        // Lê arestas
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
            edge_count++;
        }
        file.close();

        fileO << f << " " <<  qtd_ver << " " << qtd_edges <<  " " << max_weight << " ";
        fileO.flush();

        cout << "\nNome: " << f << "\n";
        cout << "Vértices: " << qtd_ver << "\n";
        cout << "Arestas: " << qtd_edges << "\n";
        cout << "Arestas processadas: " << edge_count << "\n";
        cout << "Maior peso: " << max_weight << "\n";

        cout << "Fila                 Tempo de execução\n";

        int elap_sum = 0;

        // Exprimentos de priority queues
        bool dk = false;
        for(PQS p : pqs) {
            
            
            if (p==RB_TREE or p==FIBONACCI_HEAP) dk = true;
            else dk = false;

            shortest_path sp;
            elap_sum = 0;
            for(int i = 0; i < 10; i++) {
                pq q = create_pq(p, qtd_ver, max_weight);
                if(!q) continue;

                otimer.start();

                if(dk){
                    sp.init_dijkstra(q, qtd_ver, 0, dk);
                    sp.dijkstra_dk(g, q);
                    otimer.stop();
                }
                else{
                    sp.init_dijkstra(q, qtd_ver, 0, dk);
                    sp.dijkstra_ndk(g, q);
                    otimer.stop();
                }

                elap_sum += otimer.elapsed();
                
                delete q;
                sp.clear();
            }

            float elap_avg = elap_sum / 10.00;
            printf("%-20s %10.6f ms\n", pq_string(p).c_str(), elap_avg);
            fileO << elap_avg << " ";
            fileO.flush();
        }
        fileO << "\n";
        fileO.flush();

        cout << "\n------------------------------\n";
    }
    fileO.close();
}

void mcache_miss(){
    cache mc;
    cache_misss cm;
    string f = "E";

    const PQS pqs[7] = {BINARY_HEAP, B_HEAP, RB_TREE, FIBONACCI_HEAP, DIAL, TWO_LV_BKT_HEAP, RADIX_HEAP};
    string output = "../data/outs/cache.csv";
    ofstream fileO(output);

    fileO << fixed << setprecision(6);
    fileO <<"nome n m c ";
    fileO.flush();

    for (PQS p : pqs){
        fileO << pq_string(p) << " ";
        fileO.flush();
    }
    fileO << "\n";
    fileO.flush();
        
    string filename = "../data/txts/"+f+".txt";
    ifstream file(filename);

    string line;

    // Lê quantidade de vértices e arestas
    getline(file, line);
    istringstream iss(line);
    int qtd_ver, qtd_edges;
    iss >> qtd_ver >> qtd_edges;

    int edge_count = 0;
    keyType max_weight = -1;
    graph g;
    g.resize(qtd_ver + 1);

    // Lê arestas
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
        edge_count++;
    }
    file.close();
    

    cout << "\nNome: " << f << "\n";
    cout << "Vértices: " << qtd_ver << "\n";
    cout << "Arestas: " << qtd_edges << "\n";
    cout << "Arestas processadas: " << edge_count << "\n";
    cout << "Maior peso: " << max_weight << "\n";

    fileO << "fila tempo L1 L2 L3\n";
    fileO.flush();
    cout << "Fila                 Tempo de execução  L1 L2 L3\n";

    int elap_sum = 0;
    ll L1_sum = 0;
    ll L2_sum = 0;
    ll L3_sum = 0;

    // Exprimentos de priority queues
    bool dk = false;
    for(PQS p : pqs) {
        
        
        if (p==RB_TREE or p==FIBONACCI_HEAP) dk = true;
        else dk = false;

        // warm-up
        pq q_warmup = create_pq(p, qtd_ver, max_weight);
        shortest_path sp_warmup;
        sp_warmup.init_dijkstra(q_warmup, qtd_ver, 0, dk);
        sp_warmup.dijkstra_dk(g, q_warmup);
        delete q_warmup;
        sp_warmup.clear();
        //

        shortest_path sp;
        elap_sum = 0;
        for(int i = 0; i < 10; i++) {
            pq q = create_pq(p, qtd_ver, max_weight);
            if(!q) continue;

            // warm-up
            sp.init_dijkstra(q, qtd_ver, 0, dk);
            sp.dijkstra_dk(g, q);

            mc.start();

            if(dk){
                sp.init_dijkstra(q, qtd_ver, 0, dk);
                sp.dijkstra_dk(g, q);
                mc.stop();
            }
            else{
                sp.init_dijkstra(q, qtd_ver, 0, dk);
                sp.dijkstra_ndk(g, q);
                mc.stop();
            }

            elap_sum += mc.elapsed();
            cm = mc.cache_misses();
            L1_sum += cm.l1;
            L2_sum += cm.l2;
            L3_sum += cm.l3;
            
            delete q;
            sp.clear();
        }

        float elap_avg = elap_sum / 10.00;
        float L1_avg = L1_sum / 10.00;
        float L2_avg = L2_sum / 10.00;
        float L3_avg = L3_sum / 10.00;

        printf("%-20s %10.6f ms %f %f %f", pq_string(p).c_str(), elap_avg, L1_avg, L2_avg, L3_avg);
        fileO << pq_string(p).c_str() << elap_avg << " " << L1_avg << " " << L2_avg << " " << L3_avg << "\n";
        fileO.flush();
    }
    fileO << "\n";
    fileO.flush();
    fileO.close();
}

int main() {
    exec_time();

    mcache_miss();
    return 0;
}