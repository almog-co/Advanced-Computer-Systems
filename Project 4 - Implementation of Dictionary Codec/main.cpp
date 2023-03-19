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
#include <chrono>

using namespace std;

/**********************************************************
 * Preprocessor variables
 **********************************************************/

#define NUM_WORKER_THREADS 16

/**********************************************************
 * Function definitions
 **********************************************************/

void buildPrefixTree(PrefixTree* tree, const vector<string>& lines, const int actual_start_index, vector<pair<string, int>>* mapping);

/**********************************************************
 * Class declarations
 **********************************************************/

class WorkerThread {
    public:
        WorkerThread() {
            this->p_launched = false;
            this->workerTree = new PrefixTree();
            this->mapping = new vector<pair<string, int>>();
        }

        void launch(const vector<string>& lines, const int actual_start_index) {
            this->p_launched = true;
            this->p_thread = thread(buildPrefixTree, this->workerTree, lines, actual_start_index, this->mapping);
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

        vector<pair<string, int>>* getMapping() {
            return this->mapping;
        }

    private:
        bool p_launched;
        vector<pair<string, int>>* mapping;
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

    // check if user specified vanilla search mode
    if (args.find("mode") == args.end()) {
        args["mode"] = "optimized";
    } else {
        args["mode"] = "vanilla";
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

// vanilla implementation of prefix search for baseline performance
// return form matches PrefixTree return form
vector<pair<string, vector<int>>> vanillaSearchPrefix(const string& filename, string prefix) {
    vector<pair<string, vector<int>>> results;
    ifstream file(filename);
    string line;
    int i = 0;

    while (getline(file, line)) {
        // Convert to lowercase
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        if (line.substr(0, prefix.size()) == prefix) {
            vector<int> curr_index;
            curr_index.push_back(i);
            results.push_back(make_pair(line, curr_index));
        }
        i++;
    }

    return results;
}

// vanilla implementation of word search for baseline performance
// return form matches PrefixTree return form
vector<int> vanillaSearchWord(const string& filename, const string& word) {
    vector<int> results;
    ifstream file(filename);
    string line;
    int i = 0;
    while (getline(file, line)) {
        // Convert to lowercase
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        if (line == word) {
            results.push_back(i);
        }
        i++;
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
    //string line;

    const int MAX_LENGTH = 524288;
    char* line = new char[MAX_LENGTH];
    int i = 0;
    while (file.getline(line, MAX_LENGTH) && strlen(line) > 0) {
        // Convert to lowercase
        transform(line, line + strlen(line), line, ::tolower);
        all_lines.push_back(line);
        if (i % 10000000 == 0) {
            cout << "Read in " << i / 10000000 << " million lines" << endl;
        }
        i++;
    }
    
    // while (getline(file, line)) {
    //     // Convert to lowercase
    //     transform(line.begin(), line.end(), line.begin(), ::tolower);
    //     all_lines.push_back(line);
    // }

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
void buildPrefixTree(PrefixTree* tree, const vector<string>& lines, const int actual_start_index, vector<pair<string, int>>* mapping) {
    for (int i = 0; i < lines.size(); i++) {
        int encoded_mapping = tree->insert(lines[i], i + actual_start_index);
        mapping->push_back(make_pair(lines[i], encoded_mapping));
    }
}

/**********************************************************
 * Main function
 **********************************************************/

int main(int argc, char* argv[]) {
    // Parse command line arguments
    unordered_map<string, string> args = parseArgs(argc, argv);

    // Read entire file into memory. Number of vectors == number of threads
    cout << "Reading file into memory..." << endl;
    vector< vector<string> > lines = vectorizeFile(args["filename"]);
    cout << "Done reading file into memory" << endl;
    cout << endl;

    // List of trees for each thread
    vector<PrefixTree*> trees;

    // List of mappings for each thread
    vector<vector<pair<string, int>>*> mappings;

    if (args["mode"] == "optimized") {
        cout << "Building prefix trees..." << endl;
        // Array of WorkerThread objects
        WorkerThread* worker_threads[NUM_WORKER_THREADS];
        for (int i = 0; i < NUM_WORKER_THREADS; i++) {
            worker_threads[i] = new WorkerThread();
        }

        int lines_processed = 0;
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
                lines_processed += lines[i].size();
                if (lines_processed % 10000000 == 0) {
                    cout << "Processed " << lines_processed / 10000000 << " million lines" << endl;
                }
                
            } else {
                cout << "Error launching thread" << endl;
                exit(1);
            }
        }

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
                    mappings.push_back(worker_threads[i]->getMapping());
                }
            }
        }
        cout << "Done building prefix trees" << endl;
        cout << endl;
    }

    // Combine the resulting PrefixTrees from each thread
    PrefixTree tree;
    for (int i = 0; i < trees.size(); i++) {
        tree.merge(trees[i]);
    }

    // Combine the resulting mappings from each thread into one C style array of ints
    int mapping_size = 0;
    for (int i = 0; i < mappings.size(); i++) {
        mapping_size += mappings[i]->size();
    }

    int remainder = mapping_size % 8;
    // Make mapping_size a multiple of 8
    if (mapping_size % 8 != 0) {
        mapping_size += 8 - remainder;
    }

    int* mapping = new int[mapping_size];
    for (int i = 0; i < mappings.size(); i++) {
        for (int j = 0; j < mappings[i]->size(); j++) {
            mapping[i * mappings[i]->size() + j] = mappings[i]->at(j).second;
        }
    }

    // Pad the end with 0s
    for (int i = mapping_size - remainder; i < mapping_size; i++) {
        mapping[i] = 0;
    }

    // vector<pair<string, int>> mapping;
    // for (int i = 0; i < mappings.size(); i++) {
    //     for (int j = 0; j < mappings[i]->size(); j++) {
    //         mapping.push_back(mappings[i]->at(j));
    //     }
    // }

    // Print the mapping
    // for (int i = 0; i < mapping.size(); i++) {
    //     cout << mapping[i].first << " " << mapping[i].second << endl;
    // }

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

            // timing
            auto start = chrono::high_resolution_clock::now();

            // check for search mode - optimized or vanilla
            if (args["mode"] == "optimized") {
                searchWord_indices = tree.searchMappingSIMD(mapping, mapping_size, search_word);
            } else if (args["mode"] == "vanilla") {
                searchWord_indices = vanillaSearchWord(args["filename"], search_word);
            }

            // print results
            cout << "Indices for word: " << search_word << endl;
            if (searchWord_indices.size() == 0) {
                cout << "No results found" << endl;
            } else {
                printVector(searchWord_indices);
            }

            // record timing and print
            auto stop = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed = stop - start;
            cout << "Mode: " << args["mode"] << endl;
            cout << "Duration: " << elapsed.count() << " seconds" << endl << endl;
        } else if (search_option == 2) {
            string search_prefix;
            cout << "Enter prefix you would like to search for: ";
            cin >> search_prefix;
            transform(search_prefix.begin(), search_prefix.end(), search_prefix.begin(), ::tolower);
            vector<pair<string, vector<int>>> searchPrefix_indices;

            // timing
            auto start = chrono::high_resolution_clock::now();

            // check for search mode - optimized or vanilla
            if (args["mode"] == "optimized") {
                searchPrefix_indices = tree.searchPrefixMappingSIMD(mapping, mapping_size, search_prefix);
            } else if (args["mode"] == "vanilla") {
                searchPrefix_indices = vanillaSearchPrefix(args["filename"], search_prefix);
            }

            // print results
            cout << "Results for prefix: " << search_prefix << endl;
            if (searchPrefix_indices.size() == 0) {
                cout << "No results found" << endl;
            } else {
                printVectorOfPairs(searchPrefix_indices);
            }

            // record timing and print
            auto stop = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed = stop - start;
            cout << "Mode: " << args["mode"] << endl;
            cout << "Duration: " << elapsed.count() << " seconds" << endl << endl;

        } else if (search_option == 3) {
            cout << "Exiting program..." << endl;
            break;
        } else {
            cout << "Invalid option. Please try again." << endl;
        }
    }


}