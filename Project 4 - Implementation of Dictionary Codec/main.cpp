/**********************************************************
 * Includes
 **********************************************************/

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <fstream>
#include <thread>
#include "PrefixTree.h"

using namespace std;

/**********************************************************
 * Preprocessor variables
 **********************************************************/

#define NUM_WORKER_THREADS 4

/**********************************************************
 * Function definitions
 **********************************************************/

void buildPrefixTree(PrefixTree* tree, const vector<string>& lines, const int actual_start_index);

/**********************************************************
 * Class declarations
 **********************************************************/

class WorkerThread {
    public:
        WorkerThread() {
            this->p_launched = false;
            this->workerTree = new PrefixTree();
        }

        void launch(const vector<string>& lines, const int actual_start_index) {
            this->p_launched = true;
            this->p_thread = thread(buildPrefixTree, this->workerTree, lines, actual_start_index);
        }

        void join() {
            this->p_thread.join();
        }

        bool isCompleted() {
            return !(this->p_launched) || this->p_thread.joinable();
        }

        bool isLaunched() {
            return this->p_launched;
        }

        PrefixTree* getTree() {
            return this->workerTree;
        }

    private:
        bool p_launched;
        thread p_thread;
        PrefixTree* workerTree;
};

/**********************************************************
 * Function declarations
 **********************************************************/

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
vector<string> vanillaSearch(const vector<string>& lines, string prefix) {
    vector<string> results;

    for (int i = 0; i < lines.size(); i++) {
        if (lines[i].substr(0, prefix.size()) == prefix) {
            results.push_back(lines[i]);
        }
    }

    return results;
}

// Read in file into memory. Number of vectors == number of threads
vector<vector<string>> vectorizeFile(const string& filename) {
    vector< vector<string> > lines;
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Could not open file: " << filename << endl;
        exit(1);
    }
    
    // Read in file line by line
    vector<string> all_lines;
    string line;
    while (getline(file, line)) {
        // Convert to lowercase
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        all_lines.push_back(line);
    }

    // Determine how many lines each thread will work with
    int chunk_size = all_lines.size() / NUM_WORKER_THREADS;

    // If the number of lines is not evenly divisible by the number of threads, then
    // the last thread will have to work with more lines
    int remainder = all_lines.size() % NUM_WORKER_THREADS;

    // If number of threads is greater than number of lines, then
    // the last threads will have to work with 0 lines
    if (all_lines.size() < NUM_WORKER_THREADS) {
        chunk_size = 1;
    }

    // Vectorize file into chunks
    for (int i = 0; i < NUM_WORKER_THREADS; i++) {
        int begin = i * chunk_size;
        int end = begin + chunk_size;

        if (end > all_lines.size()) { end = all_lines.size(); }
        if (begin > all_lines.size()) { lines.push_back(vector<string>()); continue; }
        if (i == NUM_WORKER_THREADS - 1) { end += remainder; }
        
        vector<string> chunck(all_lines.begin() + begin, all_lines.begin() + end);
        lines.push_back(chunck);
    }

    // Verify that the number of lines in the file is equal to the number of lines in the vector
    int num_lines = 0;
    for (int i = 0; i < lines.size(); i++) {
        num_lines += lines[i].size();
    }

    if (num_lines != all_lines.size()) {
        cout << "Error: number of lines in file does not equal number of lines in vector" << endl;
        exit(1);
    }

    file.close();

    return lines;
}

// function for threads to use to build trees
// this function may be causing a segmentation fault when using biggertext.txt and multiple threads
void buildPrefixTree(PrefixTree* tree, const vector<string>& lines, const int actual_start_index) {
    for (int i = 0; i < lines.size(); i++) {
        tree->insert(lines[i], i + actual_start_index);
    }
}

/**********************************************************
 * Main function
 **********************************************************/

int main(int argc, char* argv[]) {
    // Parse command line arguments
    unordered_map<string, string> args = parseArgs(argc, argv);

    // Read in file into memory. Number of vectors == number of threads
    vector< vector<string> > lines = vectorizeFile(args["filename"]);

    // Array of WorkerThread objects
    WorkerThread* worker_threads[NUM_WORKER_THREADS];
    for (int i = 0; i < NUM_WORKER_THREADS; i++) {
        worker_threads[i] = new WorkerThread();
    }

    // Encode the file using the worker threads
    for (int i = 0; i < NUM_WORKER_THREADS; i++) {
        // Verify that the thread is not already running
        if (worker_threads[i]->isLaunched() == false) {
            // Get number of lines in the previous chunks
            int num_lines = 0;
            for (int j = 0; j < i; j++) {
                num_lines += lines[j].size();
            }

            // Launch thread to encode the file
            worker_threads[i]->launch(lines[i], num_lines);
        } else {
            cout << "Error launching thread" << endl;
            exit(1);
        }
    }

    // List of trees for each thread
    vector<PrefixTree*> trees;

    // Wait for all threads to finish
    bool all_threads_finished = false;
    while (!all_threads_finished) {
        all_threads_finished = true;
        for (int i = 0; i < NUM_WORKER_THREADS; i++) {
            if (!worker_threads[i]->isCompleted()) {
                all_threads_finished = false;
            } else {
                // Add the tree from the thread to the list of trees
                worker_threads[i]->join();
                trees.push_back(worker_threads[i]->getTree());
            }
        }
    }

    // Combine the resulting PrefixTrees from each thread
    PrefixTree tree;
    for (int i = 0; i < trees.size(); i++) {
        tree.merge(trees[i]);
    }

    // USER INTERFACE
    cout << "User Search:" << endl << endl;
    int search_option;

    while(1) {
        cout << "Enter search option: " << endl;
        cout << "(1) Search word" << endl;
        cout << "(2) Search prefix" << endl;
        cout << "(3) Quit program" << endl; 
        cout << "Option: ";
        cin >> search_option;
        cout << endl;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Error: invalid input" << endl << endl;
            continue;
        }

        if (search_option == 1) {
            string search_word;
            cout << "Enter word you would like to search for: ";
            cin >> search_word;
            transform(search_word.begin(), search_word.end(), search_word.begin(), ::tolower);
            vector<int> searchWord_indices;
            searchWord_indices = tree.search(search_word);
            cout << "Indices for word: " << search_word << ": " << endl;
            printVector(searchWord_indices);
        } else if (search_option == 2) {
            string search_prefix;
            cout << "Enter prefix you would like to search for: ";
            cin >> search_prefix;
            transform(search_prefix.begin(), search_prefix.end(), search_prefix.begin(), ::tolower);
            vector<pair<string, vector<int>>> searchPrefix_indices;
            searchPrefix_indices = tree.searchPrefix(search_prefix);
            cout << "Results for prefix: " << search_prefix << ": " << endl;
            printVectorOfPairs(searchPrefix_indices);
        } else if (search_option == 3) {
            cout << "Exiting program..." << endl;
            break;
        } else {
            cout << "Invalid option. Please try again." << endl;
        }
    }


}