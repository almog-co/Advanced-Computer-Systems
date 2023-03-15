#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <thread>
#include <algorithm>
#include <fstream>
#include "PrefixTree.h"

using namespace std;

#define NUM_WORKER_THREADS 8

void buildPrefixTree(PrefixTree& tree, const vector<string>& lines);

/////////////////////////
//   WorkerThread Class
////////////////////////

class WorkerThread {
    public:
        WorkerThread() {
            this->p_thread_id = -1;
            this->p_launched = false;
        }

        WorkerThread(const int16_t thread_id) {
            this->p_thread_id = thread_id;
            this->p_launched = false;
        }

        void launch(const vector<string>& lines) {
            this->p_thread = thread(buildPrefixTree, workerTree, lines);
            this->p_launched = true;
        }

        void join() {
            this->p_thread.join();
        }

        bool isCompleted() {
            return !(this->p_launched) || this->p_thread.joinable();
        }

        void clear() {
            this->p_thread_id = -1;
            this->p_launched = false;
        }

        int16_t getID() {
            return this->p_thread_id;
        }

        PrefixTree getTree() {
            return this->workerTree;
        }

    private:
        int16_t p_thread_id;
        bool p_launched;
        thread p_thread;
        PrefixTree workerTree;
};

////////////////////////////
//   END WorkerThread Class
///////////////////////////

// function for threads to use to build trees
void buildPrefixTree(PrefixTree& tree, const vector<string>& lines) {
    for (int i = 0; i < lines.size(); i++) {
        tree.insert(lines[i], i);
        if (i % 1000 == 0) cout << "Inserted " << i << " words" << endl;
    }
}

// Parse input file in chunks and return a vector of strings
vector<string> parseFile(const string& filename, const int& chunk_size) {
    vector<string> lines;
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Could not open file: " << filename << endl;
        exit(1);
    }
    string line;
    // while (getline(file, line)) {
    //     lines.push_back(line);
    // }
    // reads file in chunks instead of reading the file out completely
    for (int i = 0; i < chunk_size; i++) {
        if (getline(file, line)) {
            lines.push_back(line);
        } else { // end of file
            //lines.push_back("end of file"); // not sure how to indicate end of file when reading in chunks
            break;
        }
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

// recursive helper function for merging trees
void mergeNodes(PrefixTreeNode* node1, PrefixTreeNode* node2) {
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
            mergeNodes(node1->children[i], node2->children[i]);
        }
    }

}

// function to merge two trees
void mergeTrees(PrefixTree& tree1, PrefixTree& tree2) {
    // merge all nodes from tree2 into tree1
    mergeNodes(tree1.root, tree2.root); // call helper function
    // tree1 is the merged tree
}


// vanilla implementation of search for baseline performance
vector<string> vanillaSearch(const vector<string>& lines, string prefix) {
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

    // count number of lines in the file
    ifstream file(args["filename"]);
    int num_lines = count(istreambuf_iterator<char>(file), istreambuf_iterator<char>(), '\n') + 1;

    // determine how much of the file each thread will work with
    int chunk_size = num_lines / NUM_WORKER_THREADS;
    
    // // Parse input file
    // cout << "Parsing input file..." << endl;
    // vector<string> lines = parseFile(args["filename"], chunk_size);
    // cout << "Successfully parsed input file!" << endl;

    // // Build prefix tree
    // cout << "Building prefix tree..." << endl;
    // PrefixTree tree;
    // for (int i = 0; i < lines.size(); i++) {
    //     tree.insert(lines[i], i);
    //     if (i % 1000 == 0) cout << "Inserted " << i << " words" << endl;
    // }
    // cout << "Successfully built prefix tree!" << endl;

    // // Print prefix tree
    // tree.print();

    // // Search for word "apple"
    // vector<int> indices = tree.search("apple");
    // cout << endl;
    // printVector(indices);

    // // Search for prefix "ap"
    // cout << endl;
    // vector<pair<string, vector<int>>> words = tree.searchPrefix("ap");
    // printVectorOfPairs(words);

    // // Vanilla implementation of "ap" prefix search
    // // will be very slow on main file so comment this out when testing
    // vector<string> van = vanillaSearch(lines, "ap");
    // printVector(van);
    // cout << endl;
    
    // // testing merge function
    // PrefixTree tree2;
    // tree2.insert("dog", 0);
    // tree2.insert("app", 1);
    // tree2.insert("ape", 2);
    // tree2.insert("and", 3);

    // //PrefixTree mergedTree;
    // mergeTrees(tree, tree2);
    // cout << "Merged Tree: " << endl;
    // tree.print();

    // MULTI THREADING:

    // Array of WorkerThread objects
    WorkerThread* worker_threads[NUM_WORKER_THREADS];
    for (int i = 0; i < NUM_WORKER_THREADS; i++) {
        worker_threads[i] = new WorkerThread();
    }

    // vector of PrefixTrees from threads
    vector<PrefixTree> threadTrees;

    int chunk_id = 0;
    
    for (int i = 0; i < NUM_WORKER_THREADS; i++) {
        if (worker_threads[i]->isCompleted()) {
            if (worker_threads[i]->getID() != -1) {

                // Join the thread
                worker_threads[i]->join();

                // Get the threads resulting tree
                PrefixTree tree = worker_threads[i]->getTree();

                // Add the tree to the vector
                threadTrees.push_back(tree);

                // Clear the thread for reuse
                worker_threads[i]->clear();
            } else {
                // parse file in chunks
                vector<string> lines = parseFile(args["filename"], chunk_size);

                // check if end of file
                // does end of file need to be checked for?
                // if we already have a specified number of threads and looping through those threads only
                // parseFile handles checking for end of file

                // Delete old worker thread
                delete worker_threads[i];

                // Launch a worker thread to compress the data
                worker_threads[i] = new WorkerThread(chunk_id);
                worker_threads[i]->launch(lines);

                chunk_id++;

                //cout << "Launched thread " << i << endl;
            }
        }
    }

    // Join all threads remaining
    bool all_threads_completed = false;
    while(!all_threads_completed) {
        all_threads_completed = true;
        for (int i = 0; i < NUM_WORKER_THREADS; i++) {
            if (worker_threads[i]->isCompleted()) {
                if (worker_threads[i]->getID() != -1) {
                    //cout << "Final join thread " << i << endl;

                    // Join the thread
                    worker_threads[i]->join();

                    // Get the tree
                    PrefixTree tree = worker_threads[i]->getTree();

                    // Add tree to tree vector
                    threadTrees.push_back(tree);
                    
                    // Clear the thread for reuse
                    worker_threads[i]->clear();

                    all_threads_completed = false;
                }
            }
        }
    }

    // merge all trees together
    PrefixTree mergedTree;
    for (int i = 0; i < threadTrees.size(); i++) {
        mergeTrees(mergedTree, threadTrees[i]);
    }

    // final tree is in mergedTree now
    cout << endl << "multi threading tree result: " << endl;
    mergedTree.print();

}