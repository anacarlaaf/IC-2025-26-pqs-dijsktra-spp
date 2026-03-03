#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct _2lv_bucket_queue_DK{
    vector<list<par>> top_bucket, bottom_bucket;
    vector<list<par>::iterator> ptr;

    int qtdA = 0, qtdB = 0;
    ll at = 0, ab = 0; // at = top bucket ativo, ab = bottom bucket ativo
    ll b_size = 0;     // tamanho dos buckets

    // inicia os buckets
    
    _2lv_bucket_queue_DK(keyType c, int n){                  // c = maior peso
        b_size = sqrt(c + 1) + 1; // quantidade de buckets
        top_bucket.resize(b_size);
        bottom_bucket.resize(b_size);
        ptr.resize(n, list<par>::iterator{});
    };
 
    void insert(int v, keyType dist, keyType w){
        ll i =  dist / b_size % b_size; // se i = top bucket ativo, insere no bottom
        ll j = dist % b_size;            // se não, insere no top
    
        if (i == at && j >= ab) {
            bottom_bucket[j].push_back({dist,v});
            ptr[v] = prev(bottom_bucket[j].end());
            qtdB++;
        }
        else{
            top_bucket[i].push_back({dist,v});
            ptr[v] = prev(top_bucket[i].end());
            qtdA++;
        }
    }
 
    void update(){
        // procura um bottom bucket não vazio
        while(ab < b_size && bottom_bucket[ab].empty()) ab++;
        if(ab < b_size) return;

         
        // expand: se nao encontrar, distribui elementos de outro top bucket
        ll start = at;
        do {
            if(top_bucket[at].size()) break;
            at++;
            if(at == b_size) at = 0;
        }  while(at != start);
        if(!top_bucket[at].size()) return;
 
        // distribui no bottom_bucket apenas os atuais, ignorando as novas inserções 
        int aux = top_bucket[at].size();
        ab = b_size;
        for(int i=0;i<aux;i++){
            auto a = top_bucket[at].front(); top_bucket[at].pop_front();
            qtdA--;
            ll nova_ab = a.first % b_size; // a.first % b_size;
            ab = min(ab, nova_ab);
            bottom_bucket[nova_ab].push_back(a);  // insere no bottom bucket
            ptr[a.second] = prev(bottom_bucket[nova_ab].end());
            qtdB++;
        }
    }
 
    par extract_min(){
        update();
        par min_elem = bottom_bucket[ab].front();
        bottom_bucket[ab].pop_front();
        ptr[min_elem.second] = list<par>::iterator{};
        qtdB--;
        return min_elem;
    }
 
    bool empty() {
        return qtdA+qtdB == 0;
    }

    void clear(){
        for(int i=0;i<b_size;i++){
            while(!top_bucket[i].empty()) top_bucket[i].pop_front();
            while(!bottom_bucket[i].empty()) bottom_bucket[i].pop_front();
        }

        qtdA = 0; qtdB = 0;
        at = 0; ab = 0;
    }

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        if(ptr[u] != list<par>::iterator{}){
            ll i = old_du / b_size % b_size;      // se i = top bucket ativo, ta em cima,
            ll j = old_du % b_size;            // se não, tá em baixo
        
            if (i == at && j >= ab) {
                bottom_bucket[j].erase(ptr[u]);
                qtdB--;
            }
            else{
                top_bucket[i].erase(ptr[u]);
                qtdA--;
            }
            ptr[u] = list<par>::iterator{};
        }

        insert(u, new_du, w);
    }

};

struct _2lv_bucket_queue{
    vector<queue<par>> top_bucket, bottom_bucket;

    int qtdA = 0, qtdB = 0;
    ll at = 0, ab = 0; // at = top bucket ativo, ab = bottom bucket ativo
    ll b_size = 0;     // tamanho dos buckets

    // inicia os buckets
    
    _2lv_bucket_queue(keyType c){                  // c = maior peso
        b_size = sqrt(c + 1) + 1; // quantidade de buckets
        top_bucket.resize(b_size);
        bottom_bucket.resize(b_size);
    };
 
    void insert(int v, keyType dist, keyType w){
        ll i =  dist / b_size % b_size; // se i = top bucket ativo, insere no bottom
        ll j = dist % b_size;            // se não, insere no top
    
        if (i == at && j >= ab) {
            bottom_bucket[j].push({dist,v});
            qtdB++;
        }
        else{
            top_bucket[i].push({dist,v});
            qtdA++;
        }
    }
 
    void update(){
        // procura um bottom bucket não vazio
        while(ab < b_size && bottom_bucket[ab].empty()) ab++;
        if(ab < b_size) return;

         
        // expand: se nao encontrar, distribui elementos de outro top bucket
        ll start = at;
        do {
            if(top_bucket[at].size()) break;
            at++;
            if(at == b_size) at = 0;
        }  while(at != start);
        if(!top_bucket[at].size()) return;
 
        // distribui no bottom_bucket apenas os atuais, ignorando as novas inserções 
        int aux = top_bucket[at].size();
        ab = b_size;
        for(int i=0;i<aux;i++){
            auto a = top_bucket[at].front(); top_bucket[at].pop();
            qtdA--;
            ll nova_ab = a.first % b_size; // a.first % b_size;
            ab = min(ab, nova_ab);
            bottom_bucket[nova_ab].push(a);  // insere no bottom bucket
            qtdB++;
        }
    }
 
    par extract_min(){
        update();
        par min_elem = bottom_bucket[ab].front();
        bottom_bucket[ab].pop();
        qtdB--;
        return min_elem;
    }
 
    bool empty() {
        return qtdA+qtdB == 0;
    }

    void clear(){
        for(int i=0;i<b_size;i++){
            while(!top_bucket[i].empty()) top_bucket[i].pop();
            while(!bottom_bucket[i].empty()) bottom_bucket[i].pop();
        }

        qtdA = 0; qtdB = 0;
        at = 0; ab = 0;
    }
};