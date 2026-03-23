#include <bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

// ================================================================
// Nó do radix heap
// ================================================================
struct RadixHeapNode {
    int      value;
    keyType  priority;

    RadixHeapNode* prev;
    RadixHeapNode* next;

    RadixHeapNode(int v, keyType p)
        : value(v), priority(p), prev(nullptr), next(nullptr) {}
};

// ================================================================
// Radix heap
// ================================================================
struct radix_heap {
    static constexpr size_t BITS = sizeof(keyType) * 8;

    size_t  tam;
    keyType m_minimum;
    keyType m_bucketMask;
    RadixHeapNode* m_buckets[BITS + 1];

    // ---------- msbSet ----------
    // versão genérica (fallback)
    static size_t msbSet_generic(keyType mask) {
        size_t i = 0;
        while (mask != 0) { mask >>= 1; i++; }
        return i;
    }

    static size_t msbSet(keyType mask) {
#if defined(__has_builtin) && __has_builtin(__builtin_clz)
        if constexpr (sizeof(keyType) == sizeof(unsigned int)) {
            return mask == 0 ? 0 : (BITS - __builtin_clz((unsigned int)mask));
        } else if constexpr (sizeof(keyType) == sizeof(unsigned long)) {
            return mask == 0 ? 0 : (BITS - __builtin_clzl((unsigned long)mask));
        } else if constexpr (sizeof(keyType) == sizeof(unsigned long long)) {
            return mask == 0 ? 0 : (BITS - __builtin_clzll((unsigned long long)mask));
        }
#endif
        return msbSet_generic(mask);
    }

    // ---------- construtor / destrutor ----------
    radix_heap() : tam(0), m_minimum(0), m_bucketMask(0) {
        fill(m_buckets, m_buckets + BITS + 1, nullptr);
    }

    ~radix_heap() {
        for (size_t i = 0; i <= BITS; i++) {
            RadixHeapNode* it = m_buckets[i];
            while (it != nullptr) {
                RadixHeapNode* nxt = it->next;
                delete it;
                it = nxt;
            }
        }
    }

    // ---------- insert interno ----------
    void insert_node(RadixHeapNode* node) {
        size_t ind = msbSet(node->priority ^ m_minimum);

        node->next = m_buckets[ind];
        if (m_buckets[ind] != nullptr)
            m_buckets[ind]->prev = node;
        m_buckets[ind] = node;

        if (ind != 0)
            m_bucketMask |= (keyType(1) << (BITS - ind));
    }

    // ---------- interface pública ----------
    RadixHeapNode* insert(int u, keyType du) {
        tam++;
        RadixHeapNode* node = new RadixHeapNode(u, du);
        insert_node(node);
        return node;
    }

    par extract_min() {
        tam--;

        // bucket 0 já tem o mínimo pronto
        if (m_buckets[0] != nullptr) {
            par result = {m_buckets[0]->priority, m_buckets[0]->value};
            RadixHeapNode* tmp = m_buckets[0]->next;
            delete m_buckets[0];
            m_buckets[0] = tmp;
            if (m_buckets[0] != nullptr)
                m_buckets[0]->prev = nullptr;
            return result;
        }

        // encontra o bucket não-vazio de menor índice via bucketMask
        size_t ind = BITS + 1 - msbSet(m_bucketMask);

        RadixHeapNode* bucket = m_buckets[ind];
        m_buckets[ind] = nullptr;
        if (ind != 0)
            m_bucketMask ^= (keyType(1) << (BITS - ind));

        // acha o mínimo dentro do bucket
        RadixHeapNode* mn = bucket;
        for (RadixHeapNode* it = bucket; it != nullptr; it = it->next)
            if (it->priority < mn->priority)
                mn = it;

        // remove mn da lista
        if (mn->prev != nullptr)
            mn->prev->next = mn->next;
        else
            bucket = mn->next;

        if (mn->next != nullptr)
            mn->next->prev = mn->prev;

        par result = {mn->priority, mn->value};
        m_minimum   = mn->priority;
        delete mn;

        // redistribui os nós restantes do bucket
        while (bucket != nullptr) {
            RadixHeapNode* nxt = bucket->next;
            bucket->prev = nullptr;
            insert_node(bucket);
            bucket = nxt;
        }

        return result;
    }

    bool empty() { return tam == 0; }

    void clear() {
        for (size_t i = 0; i <= BITS; i++) {
            RadixHeapNode* it = m_buckets[i];
            while (it != nullptr) {
                RadixHeapNode* nxt = it->next;
                delete it;
                it = nxt;
            }
            m_buckets[i] = nullptr;
        }
        tam = 0;
        m_minimum    = 0;
        m_bucketMask = 0;
    }
};