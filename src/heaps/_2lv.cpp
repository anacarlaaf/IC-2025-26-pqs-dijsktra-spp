#include<bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct _2lv_bucket_queue_DK{
    list<par> *top_bucket, *bottom_bucket;
    list<par>::iterator *ptr;

    int qtdA = 0, qtdB = 0, n=0;
    ll at = 0, ab = 0; // at = top bucket ativo, ab = bottom bucket ativo
    ll b_size = 0;     // tamanho dos buckets

    // inicia os buckets
    
    _2lv_bucket_queue_DK(keyType c, int n_){                  // c = maior peso
        n = n_;
        b_size = sqrt(c + 1) + 1; // quantidade de buckets
        top_bucket = new list<par>[b_size];
        bottom_bucket = new list<par>[b_size];
        ptr = new list<par>::iterator[n];
    };
 
    void insert(int v, keyType dist, keyType w){
        ll i =  dist / b_size % b_size;  // se i = top bucket ativo, insere no bottom
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
            top_bucket[i].clear();
            bottom_bucket[i].clear();
        }

        for(int i=0;i<n;i++) ptr[i] = list<par>::iterator{};

        qtdA = 0; qtdB = 0;
        at = 0; ab = 0; 
    }

    void decrease_key(int u, keyType w, keyType old_du, keyType new_du){
        if(ptr[u] != list<par>::iterator{}){
            ll i = old_du / b_size % b_size;      // se i = top bucket ativo, ta em cima,
            ll j = old_du % b_size;               // se não, tá em baixo
        
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
    queue<par> *bucket;
    //queue<par> *top_bucket, *bottom_bucket;

    int sz=0;
    ll bb, tb;
    ll at = 0, ab = 0; // at = top bucket ativo, ab = bottom bucket ativo
    ll b_size = 0;     // tamanho dos buckets

    // inicia os buckets
    
    _2lv_bucket_queue(keyType c){                  // c = maior peso
        b_size = sqrt(c + 1) + 1;
        ll aux = 1;
        while(aux < b_size) aux <<= 1;
        b_size = aux;
        bucket = new queue<par>[2*b_size];
        tb = b_size;
        // top_bucket = new queue<par>[b_size];
        // bottom_bucket = new queue<par>[b_size];
    };
 
    void insert(int v, keyType dist, keyType w){
        ll i =  (dist / b_size) & (b_size-1); // se i = top bucket ativo, insere no bottom
        ll j = dist & (b_size-1);            // se não, insere no top
    
        if (i == at && j >= ab) {
            bucket[b_size+j].push({dist,v});
        }
        else{
            bucket[i].push({dist,v});
        }
        sz++;
    }
 
    void update(){
        // procura um bottom bucket não vazio
        while(ab < b_size && bucket[b_size+ab].empty()) ab++;
        if(ab < b_size) return;

         
        // expand: se nao encontrar, distribui elementos de outro top bucket
        ll start = at;
        do {
            if(bucket[at].size()) break;
            at++;
            if(at == b_size) at = 0;
        }  while(at != start);
        if(!bucket[at].size()) return;
 
        // distribui no bottom_bucket apenas os atuais, ignorando as novas inserções 
        int aux = bucket[at].size();
        ab = b_size;
        for(int i=0;i<aux;i++){
            auto a = bucket[at].front(); bucket[at].pop();
            ll nova_ab = a.first & (b_size-1); // a.first % b_size;
            ab = min(ab, nova_ab);
            bucket[b_size+nova_ab].push(a);  // insere no bottom bucket
        }
    }
 
    par extract_min(){
        update();
        par min_elem = bucket[b_size+ab].front();
        bucket[b_size+ab].pop();
        sz--;
        return min_elem;
    }
 
    bool empty() {
        return sz == 0;
    }

    void clear(){
        for(int i=0;i<b_size;i++){
            while(!bucket[i].empty()) bucket[i].pop();
            while(!bucket[b_size+i].empty()) bucket[b_size+i].pop();
        }

        at = 0; ab = 0; sz=0;
    }
};