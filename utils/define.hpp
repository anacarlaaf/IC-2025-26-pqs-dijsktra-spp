#include <vector>
using namespace std;

typedef long long ll ;
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
#endif