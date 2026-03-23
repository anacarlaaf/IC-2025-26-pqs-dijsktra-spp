#include <bits/stdc++.h>
using namespace std;
#include "../../utils/define.hpp"

struct PairingHeapNode {
    par value;
    PairingHeapNode* prev;
    PairingHeapNode* next;
    PairingHeapNode* child;

    PairingHeapNode(par v) : value(v), prev(nullptr), next(nullptr), child(nullptr) {}
};

struct pairing_heap {
    PairingHeapNode* root;

    pairing_heap() : root(nullptr) {}

    // ---------- helpers internos ----------

    PairingHeapNode* merge_nodes(PairingHeapNode* a, PairingHeapNode* b) {
        if (a == nullptr) return b;
        if (b == nullptr) return a;
        if (a->value.first <= b->value.first) {
            link(a, b);
            return a;
        } else {
            link(b, a);
            return b;
        }
    }

    void link(PairingHeapNode* parent, PairingHeapNode* child_node) {
        if (parent->child != nullptr) {
            child_node->next = parent->child;
            parent->child->prev = child_node;
        }
        child_node->prev = parent;
        parent->child = child_node;
    }

    void unlink(PairingHeapNode* node) {
        if (node->prev->child == node)
            node->prev->child = node->next;
        else
            node->prev->next = node->next;

        if (node->next != nullptr)
            node->next->prev = node->prev;

        node->prev = nullptr;
        node->next = nullptr;
    }

    PairingHeapNode* pair(PairingHeapNode* node) {
        if (node == nullptr) return nullptr;

        size_t children = 1;
        PairingHeapNode* it = node;
        while (it->next != nullptr) {
            it = it->next;
            children++;
        }

        PairingHeapNode* result;

        if (children % 2 == 1) {
            PairingHeapNode* a = it;
            it = it->prev;
            a->prev = a->next = nullptr;
            result = a;
        } else {
            PairingHeapNode* a = it;
            PairingHeapNode* b = it->prev;
            it = it->prev->prev;
            a->prev = a->next = b->prev = b->next = nullptr;
            result = merge_nodes(a, b);
        }

        for (size_t i = 0; i < (children - 1) / 2; i++) {
            PairingHeapNode* a = it;
            PairingHeapNode* b = it->prev;
            it = it->prev->prev;
            a->prev = a->next = b->prev = b->next = nullptr;
            result = merge_nodes(merge_nodes(a, b), result);
        }

        return result;
    }

    void release(PairingHeapNode* node) {
        PairingHeapNode* it = node;
        if (it == nullptr) return;

        for (;;) {
            if (it->child != nullptr) { it = it->child; continue; }
            if (it->next  != nullptr) { it = it->next;  continue; }

            for (;;) {
                PairingHeapNode* curr = it;
                PairingHeapNode* prev = it->prev;
                delete it;

                if (prev == nullptr) return;
                if (curr == prev->child && prev->next != nullptr) {
                    it = prev->next;
                    break;
                } else {
                    it = prev;
                }
            }
        }
    }

    // ---------- interface pública ----------

    par top() {
        return root->value;
    }

    PairingHeapNode* insert(int u, keyType du) {
        PairingHeapNode* node = new PairingHeapNode({du, u});
        root = (root == nullptr) ? node : merge_nodes(root, node);
        return node;
    }

    par extract_min() {
        par menor = root->value;
        PairingHeapNode* children = root->child;
        delete root;
        root = pair(children);
        return menor;
    }

    // Diminui a chave do nó para new_du
    void decrease_key(PairingHeapNode* node, keyType new_du) {
        node->value.first = new_du;
        if (node->prev != nullptr) {
            unlink(node);
            root = merge_nodes(root, node);
        }
    }

    bool empty() {
        return root == nullptr;
    }

    void clear() {
        release(root);
        root = nullptr;
    }

    ~pairing_heap() {
        release(root);
    }
};