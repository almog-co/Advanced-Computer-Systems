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
        }

        void launch(const string& _query) {
            this->query = _query;
            this->p_launched = true;
            this->p_thread = thread(parseQuery, this->query);
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


    private:
        bool p_launched;
        string query;
        thread p_thread;
};



int main(int argc, char* argv[]) {
    ReadWriteLockingTable rwLockMap;

    if (rwLockMap.readLockID(12)) {
        cout << "12 successfully locked" << endl;
    }

    ifstream file("transaction.txt");

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
            string query;
            string line;
            while (getline(file, line)) {
                query += line + "\n";
                // a query is complete when the transaction is committed
                if (line == "commit_tx") {
                    break;
                }
            }

            // Launch thread to encode the file
            worker_threads[i]->launch(query);

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
                // Join each thread
                worker_threads[i]->join();
                // Should threads return anything?
            }
        }
    }
    
}