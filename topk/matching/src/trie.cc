#include <queue>

#include "trie.h"

int dup = 0;

TrieNode::~TrieNode() {
    if (child) {
        delete child;
        child = NULL;
    }
    if (next) {
        delete next;
        next = NULL;
    }
}

TrieNode* TrieNode::insert_child(const char ch) {
    TrieNode* node = child;
    while (node && node->ch != ch)
        node = node->next;
    if (node == NULL) {
        node = new TrieNode(ch, depth + 1, this);
        node->next = child;
        child = node;
    }
    return node;
}

int TrieNode::preorder(int id, Trie* trie) {
    this->id = id;
    if (rid != -1)
        trie->rids.push_back(std::make_pair(id, rid));
    for (TrieNode* node = child; node; node = node->next)
        id = node->preorder(id + 1, trie);
    this->last = id;
    return id;
}

void TrieNode::get_records(std::unordered_set<int>& records, unsigned K, Trie* trie) {
    if (records.size() == K) return;
    auto iter = std::lower_bound(trie->rids.begin(), trie->rids.end(),
        std::make_pair(id, -1));
    while (iter != trie->rids.end() && iter->first <= last) {
        records.insert(iter->second);
        if (records.size() == K) return;
        ++iter;
    }
}

Trie::~Trie() {
    if (root) {
        delete root;
        root = NULL;
    }
}

TrieNode* Trie::insert(const std::string& str, const int rid) {
    TrieNode* node = root;
    for (auto& ch : str) {
        node = node->insert_child(ch);
    }
    node->rid = rid;
    return node;
}

void Trie::build_index() {
    root->preorder(1, this);
    std::queue<TrieNode*> Q;
    Q.push(root);
    while (!Q.empty()) {
        TrieNode* node = Q.front();
        Q.pop();
        index[node->ch][node->depth].push_back(node);
        for (node = node->child; node; node = node->next)
            Q.push(node);
    }
}
