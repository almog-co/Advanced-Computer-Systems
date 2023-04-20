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

#define NUM_WORKER_THREADS 1


/**********************************************************
 * Class declarations
 **********************************************************/

class WorkerThread {
    public:
        WorkerThread() {
            this->p_launched = false;
            this->finishedAllQueries = true;
        }

        void launch(const vector<string>& _query) {
            this->queries = _query;
            this->p_launched = true;
            this->finishedAllQueries = false;

            int i = 0;
            while (i < this->queries.size()) {
                // check if the previous query was completed before starting the new one
                if (i > 0) { // if it is not the first thread to be deployed, it needs to wait for previous one
                    this->p_thread.join();
                    this->p_thread = thread(parseQuery, this->queries[i]); // terminating due to uncaught exception of type std::invalid_argument: stoi: no conversion
                    i++;
                } else { // first thread does not need to join
                    this->p_thread = thread(parseQuery, this->queries[i]); // terminating due to uncaught exception of type std::invalid_argument: stoi: no conversion
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


// function that locks all IDs (if IDs are already locked, it keeps trying until it works)
void tryPreLock(vector<pair<int, string>> ids, ReadWriteLockingTable& rwTable) {

    bool lockingDone = false;
    int lockedIDindex = 0;

    while (!lockingDone) {
        bool allLocked = true;
        for (int i = 0; i < ids.size(); i++) {
            if (ids[i].second == "READ") { // if reading, need to lock write
                if (!rwTable.isReadLocked(ids[i].first)) { // if this ID is read unlocked
                    if (rwTable.writeLockID(ids[i].first)) cout << "WRITE LOCKED" << endl; // write lock the ID
                    cout << "Locked read for ID " << i << endl;
                } else { // if ID is read locked
                    allLocked = false; // indicate that all IDs need to be unlocked
                    lockedIDindex = i; // this is the index that was locked, unlock all IDs before it
                    break;
                }
            } else if (ids[i].second == "WRITE" || ids[i].second == "BOTH") {
                // need to lock both read and write
                if (!rwTable.isReadLocked(ids[i].first) && !rwTable.isWriteLocked(ids[i].first)) { // if this ID is both read and write unlocked
                    if (rwTable.readLockID(ids[i].first)) cout << "read locked" << endl; // read lock
                    if (rwTable.writeLockID(ids[i].first)) cout << "write locked" << endl; // write lock
                    cout << "Locked read and write for ID " << i << endl;
                } else { // an ID is either read or write locked
                    allLocked = false;
                    lockedIDindex = i;
                    break;
                }
            }
        }

        // check if everything has been locked
        if (allLocked) {
            lockingDone = true;
            break; // exit while loop
        } else {
            // unlock all of the IDs that have just been locked prior
            // only reverse locks that have just been done
            cout << "unlocking all" << endl;
            for (int i = 0; i < lockedIDindex; i++) {
                if (ids[i].second == "READ") {
                    rwTable.writeUnlock(ids[i].first);
                } else if (ids[i].second == "WRITE" || ids[i].second == "BOTH") {
                    rwTable.readUnlock(ids[i].first);
                    rwTable.writeUnlock(ids[i].first);
                }
            }
            continue;
        }

        // try locking again
        // add time delay here?
    } // end while loop


}

vector<vector<string>> readTransactionFile(const string& filename) {
    vector<string> transactions;

    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Could not open file: " << filename << endl;
        exit(1);
    }

    const int MAX_LENGTH = 524288;
    char* line = new char[MAX_LENGTH];
    
    string transaction = "";
    while(file.getline(line, MAX_LENGTH) && strlen(line) > 0) {
        // Convert to lowercase
        transform(line, line + strlen(line), line, ::tolower);

        // if the transaction is complete, add the built string to the vector
        if (strcmp(line, "commit_tx") == 0) {
            transaction += line;
            transactions.push_back(transaction);
            transaction = ""; // clear string
            continue; // next line
        }

        // add current line to current transaction
        transaction += line;
        transaction += " "; // add a space
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


int main(int argc, char* argv[]) {

    // Read entire file into memory. Number of vectors == number of threads
    cout << "Reading file into memory..." << endl;
    vector< vector<string> > txChunks = readTransactionFile("Tests/transaction.txt");
    cout << "Done reading file into memory" << endl;
    cout << endl;

    // txChunks holds all of the transactions divided into chunks for each thread to handle
    cout << "transaction: " << txChunks[0][1] << endl;

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
            cout << "check in first loop" << endl;
            // pass in a chunk of queries to the thread
            vector<string> queries = txChunks[i];

            // Launch thread to process each query
            worker_threads[i]->launch(queries);
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
        cout << "entered last loop" << endl;
        all_threads_finished = true;
        for (int i = 0; i < NUM_WORKER_THREADS; i++) {
            if (!worker_threads[i]->isCompleted()) {
                all_threads_finished = false;
            } else {
                // Join each thread
                cout << "attempting to join" << endl;
                worker_threads[i]->join();
                // Should threads return anything?
            }
        }
    }
    cout << "Done waiting for threads to finish" << endl;
    
}