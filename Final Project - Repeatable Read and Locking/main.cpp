// Final Project
// Implementing Repeatable Read and Read/Write Locking

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <fstream>
#include <thread>
#include <chrono>
#include "ReadWriteLockingTable.h"
#include "database.h"
#include "query.h"

using namespace std;

#define NUM_WORKER_THREADS 12
#define MAX_ID 15
#define NUM_TRANSACTIONS 120

/**********************************************************
 * Class declarations
 **********************************************************/

class WorkerThread {
    public:
        WorkerThread() {
            this->p_launched = false;
            this->finishedAllQueries = true;
        }

        void launch(const vector<string>& _query, DatabaseTable* _database, ReadWriteLockingTable* _rwTable) {
            this->queries = _query;
            this->p_launched = true;
            this->finishedAllQueries = false;

            int i = 0;
            while (i < this->queries.size()) {
                // check if the previous query was completed before starting the new one
                if (i > 0) { // if it is not the first thread to be deployed, it needs to wait for previous one
                    this->p_thread.join();
                    this->p_thread = thread(parseQuery, this->queries[i], _database, _rwTable);
                    i++;
                } else { // first thread does not need to join
                    this->p_thread = thread(parseQuery, this->queries[i], _database, _rwTable);
                    i++;
                }

                // if the previous query is still being processed, loop until it is ready
            }

            this->finishedAllQueries = true; // all queries have been processed
        }

        void join() {
            if (this->finishedAllQueries) { // can only join this WorkerThread after it has completed all of its queries
                this->p_thread.join();
            } else {
                cout << "Thread cannot be joined because it has not completed all of its queries yet" << endl;
            }
        }

        bool isCompleted() {
            return (!(this->p_launched) || this->p_thread.joinable()) && finishedAllQueries;
        }

        bool isLaunched() {
            return this->p_launched;
        }

        vector<string> getQueries() {
            return this->queries;
        }


    private:
        bool finishedAllQueries;
        bool p_launched;
        vector<string> queries;
        thread p_thread;
};

//ReadWriteLockingTable rwTable;

// function that tries to lock stuff
// ^ this function calls the below function
// function that does shared memory stuff (lock stuff in parallel)
// if can't lock everything at once, unlock them all
// queue?
// everyhting adds ids to queue that wants to be locked
// worker thread that works through the queue
// one thread that just reads through the queue and does the locking


// for reading input files
vector<vector<string>> readTransactionFile(const string& filename) {
    vector<string> transactions;

    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Could not open file: " << filename << endl;
        exit(1);
    }

    string line;
    string transaction = "";

    while (getline(file, line)) {

        // if the transaction is complete, add the built string to the vector
        if (line == "commit_tx") {
            transaction += line;
            transactions.push_back(transaction);
            transaction = ""; // clear string
            continue; // next line
        }

        // add current line to current transaction
        transaction += line;
        transaction += "\n"; // add a space
    }

    // Determine how many lines each thread will work with
    int chunk_size = transactions.size() / NUM_WORKER_THREADS;

    // If the number of lines is not evenly divisible by the number of threads, then
    // the last thread will have to work with more lines
    int remainder = transactions.size() % NUM_WORKER_THREADS;

    // If number of threads is greater than number of lines, then
    // the last threads will have to work with 0 lines
    if (transactions.size() < NUM_WORKER_THREADS) {
        chunk_size = 1;
    }

    // will hold chunks of transactions stored in a vector for each thread
    vector< vector<string> > transactionChunks;

    // Vectorize file into chunks
    for (int i = 0; i < NUM_WORKER_THREADS; i++) {
        int begin = i * chunk_size;
        int end = begin + chunk_size;

        if (end > transactions.size()) { end = transactions.size(); }
        if (begin > transactions.size()) { transactionChunks.push_back(vector<string>()); continue; }
        if (i == NUM_WORKER_THREADS - 1) { end += remainder; }
        
        vector<string> chunck(transactions.begin() + begin, transactions.begin() + end);
        transactionChunks.push_back(chunck);
    }

    // Verify that the number of lines in the file is equal to the number of lines in the vector
    int num_lines = 0;
    for (int i = 0; i < transactionChunks.size(); i++) {
        num_lines += transactionChunks[i].size();
    }

    if (num_lines != transactions.size()) {
        cout << "Error: number of lines in file does not equal number of lines in vector" << endl;
        exit(1);
    }

    file.close();

    return transactionChunks;
}

// for generating random test transactions
vector< vector<string> > generateTransactions(int max_id, int num_transactions) {
    vector<string> transactions;

    for (int i = 0; i < num_transactions; i++) {
        int random_id = rand() % max_id;
        string tx = "begin_tx \n";
        tx += "b = readID(" + to_string(random_id) + ", 1) \n";
        tx += "write(" + to_string(random_id) + ", 1, 100 + b) \n";
        tx += "commit_tx";
        transactions.push_back(tx);
    }

    // Determine how many lines each thread will work with
    int chunk_size = transactions.size() / NUM_WORKER_THREADS;

    // If the number of lines is not evenly divisible by the number of threads, then
    // the last thread will have to work with more lines
    int remainder = transactions.size() % NUM_WORKER_THREADS;

    // If number of threads is greater than number of lines, then
    // the last threads will have to work with 0 lines
    if (transactions.size() < NUM_WORKER_THREADS) {
        chunk_size = 1;
    }

    // will hold chunks of transactions stored in a vector for each thread
    vector< vector<string> > transactionChunks;

    // Vectorize file into chunks
    for (int i = 0; i < NUM_WORKER_THREADS; i++) {
        int begin = i * chunk_size;
        int end = begin + chunk_size;

        if (end > transactions.size()) { end = transactions.size(); }
        if (begin > transactions.size()) { transactionChunks.push_back(vector<string>()); continue; }
        if (i == NUM_WORKER_THREADS - 1) { end += remainder; }
        
        vector<string> chunck(transactions.begin() + begin, transactions.begin() + end);
        transactionChunks.push_back(chunck);
    }

    // Verify that the number of lines in the file is equal to the number of lines in the vector
    int num_lines = 0;
    for (int i = 0; i < transactionChunks.size(); i++) {
        num_lines += transactionChunks[i].size();
    }

    if (num_lines != transactions.size()) {
        cout << "Error: number of lines in file does not equal number of lines in vector" << endl;
        exit(1);
    }

    return transactionChunks;
}

int main(int argc, char* argv[]) {

    // Read entire file into memory. Number of vectors == number of threads
    cout << "Reading file into memory..." << endl;
    vector< vector<string> > txChunks = generateTransactions(MAX_ID, NUM_TRANSACTIONS);
    cout << "Done reading file into memory" << endl;
    cout << endl;

    // txChunks holds all of the transactions divided into chunks for each thread to handle

    // Create a database table schema
    DatabaseTableSchema schema = {
        .columnCount = 2,
        .columnNames = new string[2]{"Name", "Balance"},
        .columnTypes = new int[2]{DATABASE_TYPE_STRING, DATABASE_TYPE_INT}
    };

    // Create a database table
    DatabaseTable* database = new DatabaseTable(schema);

    // Insert random balances into the table
    for (int i = 0; i < MAX_ID; i++) {
        int random_balance = rand() % 1000;
        string random_name = "";
        for (int j = 0; j < 5; j++) {
            random_name += (char) (rand() % 26 + 97);
        }
        database->insert(i, random_name.c_str(), random_balance);
    }

    // Print the table
    database->print();

    // Make the locking table
    ReadWriteLockingTable* locking_table = new ReadWriteLockingTable();

    // threading stuff
    // Array of WorkerThread objects
    WorkerThread* worker_threads[NUM_WORKER_THREADS];
    for (int i = 0; i < NUM_WORKER_THREADS; i++) {
        worker_threads[i] = new WorkerThread();
    }

    // Read in queries using the worker threads
    for (int i = 0; i < NUM_WORKER_THREADS; i++) {
        // Verify that the thread is not already running
        if (worker_threads[i]->isLaunched() == false) {
            // pass in a chunk of queries to the thread
            vector<string> queries = txChunks[i];

            // Launch thread to process each query
            worker_threads[i]->launch(queries, database, locking_table);
            cout << "thread launched" << endl;
        } else {
            cout << "Error launching thread" << endl;
            exit(1);
        }
    }
    cout << "Done deploying threads" << endl;

    // Wait for all threads to finish
    bool all_threads_finished = false;
    while (!all_threads_finished) {
        all_threads_finished = true;
        for (int i = 0; i < NUM_WORKER_THREADS; i++) {
            if (!worker_threads[i]->isCompleted()) {
                all_threads_finished = false;
            } else {
                // Join each thread
                worker_threads[i]->join();
                // Should threads return anything?
            }
        }
    }

    database->print();
    
}