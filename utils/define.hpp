#include <bits/stdc++.h>
using namespace std;

typedef unsigned long long ll ;
typedef ll keyInt;
typedef ll keyType;


typedef pair<keyInt,int> parInt;
typedef pair<keyType,int> par;

typedef vector<vector<parInt>> graphInt;
typedef vector<vector<par>> graph;


#ifndef NPTR_DEFINED
#define NPTR_DEFINED
typedef struct nptr {
    int lvl;
    int idx;
    list<par>::iterator it;
} nptr;


enum PQS {
    BINHCPP,
    RBT,
    FIBH,
    BINH,
    RH,
    _1LVBQ,
    _2LVBQ,
    _4LVBQ,
    _KLVBQ,
    _1LVBQDK,
    _2LVBQDK,
    _4LVBQDK,
    _KLVBQDK,
};

map<string, PQS> toPq = {
    {"RBT", RBT},
    {"FIBH", FIBH},
    {"BINH", BINH},
    {"RH", RH},
    {"1LVBQ", _1LVBQ},
    {"2LVBQ", _2LVBQ},
    {"4LVBQ", _4LVBQ},
    {"KLVBQ", _KLVBQ},
    {"1LVBQDK", _1LVBQDK},
    {"2LVBQDK", _2LVBQDK},
    {"4LVBQDK", _4LVBQDK},
    {"KLVBQDK", _KLVBQDK},
};

#endif