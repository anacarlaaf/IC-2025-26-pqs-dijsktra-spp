#include<iostream>
#include<chrono>
#include"define.hpp"
#include<papi.h>

using namespace std;

#include <iostream>
#include <chrono>

using namespace std;

struct timer {
    chrono::high_resolution_clock::time_point begin, end;

    timer() {
        start();
    }

    void start() {
        begin = chrono::high_resolution_clock::now();
    }

    void stop() {
        end = chrono::high_resolution_clock::now();
    }

    int elapsed() {
        return chrono::duration_cast<chrono::milliseconds>(end - begin).count();
    }
};



struct cache {
    chrono::high_resolution_clock::time_point begin, end;

    // PAPI
    int EventSet = PAPI_NULL;
    long long values[3];
    bool has_L3 = false;

    cache() {
        start();
    }

    void start() {
        // Tempo
        begin = chrono::high_resolution_clock::now();

        // Inicializa PAPI
        if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
            cerr << "Erro ao inicializar PAPI\n";
            exit(1);
        }

        PAPI_create_eventset(&EventSet);

        // Adiciona eventos
        PAPI_add_event(EventSet, PAPI_L1_DCM);
        PAPI_add_event(EventSet, PAPI_L2_DCM);

        if (PAPI_query_event(PAPI_L3_TCM) == PAPI_OK) {
            PAPI_add_event(EventSet, PAPI_L3_TCM);
            has_L3 = true;
        }

        PAPI_start(EventSet);
    }

    void stop() {
        end = chrono::high_resolution_clock::now();
        PAPI_stop(EventSet, values);
    }

    int elapsed() {
        return chrono::duration_cast<chrono::milliseconds>(end - begin).count();
    }

    cache_miss cache_misses() {
        cache_miss cm;
        cm.l1 = values[0];
        cm.l2 = values[1];
        cm.l3 = values[2];
        return cm;
    }

    ~cache() {
        PAPI_cleanup_eventset(EventSet);
        PAPI_destroy_eventset(&EventSet);
    }
};