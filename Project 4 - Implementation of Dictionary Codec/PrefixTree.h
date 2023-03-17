#ifndef PREFIX_TREE_H
#define PREFIX_TREE_H

#include <vector>
#include <iostream>

using namespace std;

class PrefixTreeNode {
public:
    char val;
    bool isWord;
    vector<PrefixTreeNode*> children;
    vector<int> originalFileIndices;

    PrefixTreeNode() {
        val = ' ';
        isWord = false;
        children = vector<PrefixTreeNode*>(26, nullptr);
        originalFileIndices = vector<int>();
    }

    PrefixTreeNode(char val) {
        this->val = val;
        isWord = false;
        children = vector<PrefixTreeNode*>(26, nullptr);
        originalFileIndices = vector<int>();
    }

    PrefixTreeNode(char val, int index) {
        this->val = val;
        isWord = false;
        children = vector<PrefixTreeNode*>(26, nullptr);
        originalFileIndices = vector<int>();
        originalFileIndices.push_back(index);
    }

    bool hasChild(char c) {
        return children[c - 'a'] != nullptr;
    }

    PrefixTreeNode* getChild(char c) {
        return children[c - 'a'];
    }

    void addChild(char c) {
        children[c - 'a'] = new PrefixTreeNode(c);
    }

    void print() {
        cout << val << " : ";
        for (int i = 0; i < children.size(); i++) {
            if (children[i] != nullptr) {
                cout << children[i]->val << " ";
            }
        }
        cout << endl;
    }
};

class PrefixTree {
public:
    // making root public for merging function
    PrefixTreeNode* root;

    PrefixTree() {
        root = new PrefixTreeNode();
    }

    // Merge another tree into this tree
    void merge(PrefixTree* other) {
        mergeHelper(root, other->root);
    }

    // Inserts a word into the tree
    void insert(const string& word, int index) {
        PrefixTreeNode* curr = root;
        for (int i = 0; i < word.length(); i++) {
            char c = word[i];
            if (!curr->hasChild(c)) {
                curr->addChild(c);
            }
            curr = curr->getChild(c);
            curr->originalFileIndices.push_back(index);
        }
        curr->isWord = true;
    }
    
    // Searches for a word in the tree
    // Returns all indices of the word in the original file
    vector<int> search(const string& word) {
        PrefixTreeNode* curr = root;
        for (int i = 0; i < word.length(); i++) {
            char c = word[i];
            if (!curr->hasChild(c)) {
                return vector<int>();
            }
            curr = curr->getChild(c);
        }
        if (!curr->isWord) {
            return vector<int>();
        }
        return curr->originalFileIndices;
    }

    // Searches for a prefix in the tree
    // Returns vector of pairs of all words in the tree that start with the prefix and their indices
    // Example return for "ap" : {{"apple", {1, 2, 3}}, {"app", {4, 5}}}
    vector<pair<string, vector<int>>> searchPrefix(const string& prefix) {
        PrefixTreeNode* curr = root;
        for (int i = 0; i < prefix.length(); i++) {
            char c = prefix[i];
            if (!curr->hasChild(c)) {
                return vector<pair<string, vector<int>>>();
            }
            curr = curr->getChild(c);
        }
        vector<pair<string, vector<int>>> words;
        searchPrefixHelper(curr, prefix, words);
        return words;
    }

    // Print the tree in format:
    // Parent : Child1 Child2 Child3 ...
    void print() {
        PrefixTreeNode* curr = root;
        printHelper(curr);
    }

private:
    void printHelper(PrefixTreeNode* curr) {
        curr->print();
        for (int i = 0; i < curr->children.size(); i++) {
            if (curr->children[i] != nullptr) {
                printHelper(curr->children[i]);
            }
        }
    }

    void searchPrefixHelper(PrefixTreeNode* curr, const string& prefix, vector<pair<string, vector<int>>>& words) {
        if (curr->isWord) {
            words.push_back(make_pair(prefix, curr->originalFileIndices));
        }
        for (int i = 0; i < curr->children.size(); i++) {
            if (curr->children[i] != nullptr) {
                searchPrefixHelper(curr->children[i], prefix + curr->children[i]->val, words);
            }
        }
    }

    // Given two nodes, merge the second node into the first node
    void mergeHelper(PrefixTreeNode* node1, PrefixTreeNode* node2) {
        // base case
        if (node2 == nullptr) return;

        if (node1 == nullptr) {
            node1 = new PrefixTreeNode(node2->val);
        }

        // merge original file indices
        for (int i = 0; i < node2->originalFileIndices.size(); i++) {
            node1->originalFileIndices.push_back(node2->originalFileIndices[i]);
        }

        // if the nodes represent the same word, keep isWord flag as true
        if (node1->val == node2->val && node2->isWord) {
            node1->isWord = true;
        }

        // merge child nodes
        for (int i = 0; i < node2->children.size(); i++) {
            if (node2->children[i] != nullptr) {
                if (node1->children[i] == nullptr) {
                    node1->children[i] = new PrefixTreeNode(node2->children[i]->val);
                }
                mergeHelper(node1->children[i], node2->children[i]);
            }
        }
    }
};

#endif