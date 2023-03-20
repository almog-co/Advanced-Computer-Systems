#ifndef PREFIX_TREE_H
#define PREFIX_TREE_H

#include <vector>
#include <iostream>
#include <immintrin.h>

using namespace std;

#define BASE_26_SYMBOLS "abcdefghijklmnopqrstuvwxyz"

// Convert base 26 number to base 10
int base26ToBase10(const string& num) {
    int base10 = 0;
    for (int i = 0; i < num.length(); i++) {
        base10 *= 26;
        base10 += num[i] - 'a';
    }
    return base10;
}

// Convert base 10 number to base 26
string base10ToBase26(int num) {
    string base26 = "";
    while (num > 0) {
        base26 = BASE_26_SYMBOLS[num % 26] + base26;
        num /= 26;
    }
    return base26;
}


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
        //originalFileIndices.push_back(index);
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

    // Inserts a word into the tree. Returns encoded index of the word
    int insert(const string& word, int index) {
        PrefixTreeNode* curr = root;
        string path = "";
        for (int i = 0; i < word.length(); i++) {
            char c = word[i];
            if (!curr->hasChild(c)) {
                curr->addChild(c);
            }
            curr = curr->getChild(c);
            path += BASE_26_SYMBOLS[c - 'a'];
            //curr->originalFileIndices.push_back(index);
        }
        curr->isWord = true;

        return base26ToBase10(path);
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

    // Searches for a word in the mapping of encoded indices to words
    // Returns all indices of the word in the original file
    // Designed to make use of SIMD instructions
    vector<int> searchMappingSIMD(int* mapping, int mapping_size, const string& word) {
        // Convert word to base 10
        int base10 = base26ToBase10(word);

        // Search if base 10 number is in mapping
        vector<int> results;
        for (int i = 0; i < mapping_size; i += 8) {
            __m256i input_reg = _mm256_set1_epi32(base10);
            __m256i data_reg = _mm256_loadu_si256((__m256i*) (mapping + i));
            __m256i cmp_reg = _mm256_cmpeq_epi32(input_reg, data_reg);
            int mask = _mm256_movemask_ps((__m256) cmp_reg);
            if (mask != 0) {
                for (int j = 0; j < 8; j++) {
                    if (mask & (1 << j)) {
                        results.push_back(i + j);
                    }
                }
            }

        }
        return results;
    }

    // Searches for a word in the mapping of encoded indices to words
    // Returns all indices of the word in the original file
    // Designed to make use of SIMD instructions
    vector<int> searchMapping(int* mapping, int mapping_size, const string& word) {
        // Convert word to base 10
        int base10 = base26ToBase10(word);

        // Search if base 10 number is in mapping
        vector<int> results;
        for (int i = 0; i < mapping_size; i++) {
            if (mapping[i] == base10) {
                results.push_back(i);
            }
        }
        return results;
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

    // Searches for a prefix in the tree
    // Returns vector of words in the tree that start with the prefix
    // Example return for "ap" : {"apple", "app"}
    vector<string> searchPrefixWords(const string& prefix) {
        PrefixTreeNode* curr = root;
        for (int i = 0; i < prefix.length(); i++) {
            char c = prefix[i];
            if (!curr->hasChild(c)) {
                return vector<string>();
            }
            curr = curr->getChild(c);
        }
        vector<string> words;
        searchPrefixWordsHelper(curr, prefix, words);
        return words;
    }

    // Searches for a prefix in the mapping of encoded indices to words
    // Returns vector of pairs of all words in the tree that start with the prefix and their indices
    // Designed to make use of SIMD instructions
    vector<pair<string, vector<int>>> searchPrefixMappingSIMD(int* mapping, int mapping_size, const string& prefix) {
        vector<pair<string, vector<int>>> results;
        unordered_map<string, vector<int>> results_map;

        // Convert prefix to base 10
        int base10 = base26ToBase10(prefix);

        // Get all words that start with prefix
        vector<string> words = searchPrefixWords(prefix);

        // Convert all words to base 10
        int* base10_words = new int[words.size()];
        for (int i = 0; i < words.size(); i++) {
            base10_words[i] = base26ToBase10(words[i]);
        }

        // Search if base 10 number is in mapping
        for (int j = 0; j < words.size(); j++) {
            for (int i = 0; i < mapping_size; i += 8) {
                if (i >= mapping_size) break;
                __m256i input_reg = _mm256_set1_epi32(base10_words[j]);
                __m256i data_reg = _mm256_loadu_si256((__m256i*) (mapping + i));
                __m256i cmp_reg = _mm256_cmpeq_epi32(input_reg, data_reg);
                int mask = _mm256_movemask_ps((__m256) cmp_reg);
                if (mask != 0) {
                    for (int k = 0; k < 8; k++) {
                        if (mask & (1 << k)) {
                            results_map[words[j]].push_back(i + k);
                        }
                    }
                }
            }
        }

        // Convert results_map to results format
        for (auto it = results_map.begin(); it != results_map.end(); it++) {
            results.push_back(make_pair(it->first, it->second));
        }

        delete[] base10_words;
        return results;
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

    void searchPrefixWordsHelper(PrefixTreeNode* curr, const string& prefix, vector<string>& words) {
        if (curr->isWord) {
            words.push_back(prefix);
        }
        for (int i = 0; i < curr->children.size(); i++) {
            if (curr->children[i] != nullptr) {
                searchPrefixWordsHelper(curr->children[i], prefix + curr->children[i]->val, words);
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