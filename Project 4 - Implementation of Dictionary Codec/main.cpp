#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include "PrefixTree.h"

using namespace std;

// Parse input file and return a vector of strings
vector<string> parseFile(const string& filename) {
    vector<string> lines;
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Could not open file: " << filename << endl;
        exit(1);
    }
    string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }
    file.close();
    return lines;
}

// Parse command line arguments
// Format: filename -arg1 value1 -arg2 value2 ...
unordered_map<string, string> parseArgs(int argc, char* argv[]) {
    if (argc == 1) {
        cout << "No arguments provided" << endl;
        exit(1);
    }
    unordered_map<string, string> args;
    args["filename"] = argv[1];

    for (int i = 2; i < argc; i++) {
        string arg = argv[i];
        if (arg[0] != '-') {
            cout << "Invalid argument: " << arg << endl;
            exit(1);
        }
        arg = arg.substr(1);
        if (i == argc - 1) {
            cout << "No value provided for argument: " << arg << endl;
            exit(1);
        }
        string value = argv[i + 1];
        args[arg] = value;
        i++;
    }

    return args;
}

// Print vector of any type
template <typename T>
void printVector(const vector<T>& v) {
    for (int i = 0; i < v.size(); i++) {
        cout << v[i] << " ";
    }
    cout << endl;
}

// Print vector of pairs of any type
template <typename T1, typename T2>
void printVectorOfPairs(const vector<pair<T1, T2>>& v) {
    for (int i = 0; i < v.size(); i++) {
        cout << v[i].first << " : ";
        printVector(v[i].second);
    }
    cout << endl;
}

// vanilla implementation of search for baseline performance
vector<string> vanillaSearch(const vector<string> lines, string prefix) {
    vector<string> results;

    for (int i = 0; i < lines.size(); i++) {
        if (lines[i].substr(0, prefix.size()) == prefix) {
            results.push_back(lines[i]);
        }
    }

    return results;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    unordered_map<string, string> args = parseArgs(argc, argv);
    
    // Parse input file
    cout << "Parsing input file..." << endl;
    vector<string> lines = parseFile(args["filename"]);
    cout << "Successfully parsed input file!" << endl;

    // Build prefix tree
    cout << "Building prefix tree..." << endl;
    PrefixTree tree;
    for (int i = 0; i < lines.size(); i++) {
        tree.insert(lines[i], i);
        if (i % 1000 == 0) cout << "Inserted " << i << " words" << endl;
    }
    cout << "Successfully built prefix tree!" << endl;

    // Print prefix tree
    tree.print();

    // Search for word "apple"
    vector<int> indices = tree.search("apple");
    cout << endl;
    printVector(indices);

    // Search for prefix "ap"
    cout << endl;
    vector<pair<string, vector<int>>> words = tree.searchPrefix("ap");
    printVectorOfPairs(words);
    
}