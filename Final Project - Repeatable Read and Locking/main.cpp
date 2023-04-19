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

        string getQuery() {
            return this->query;
        }


    private:
        bool p_launched;
        string query;
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
    }


}


// int main(int argc, char* argv[]) {

//     ifstream file("transaction.txt");

//     // threading stuff
//     // Array of WorkerThread objects
//     WorkerThread* worker_threads[NUM_WORKER_THREADS];
//     for (int i = 0; i < NUM_WORKER_THREADS; i++) {
//         worker_threads[i] = new WorkerThread();
//     }

//     // Read in queries using the worker threads
//     for (int i = 0; i < NUM_WORKER_THREADS; i++) {
//         // Verify that the thread is not already running
//         if (worker_threads[i]->isLaunched() == false) {
//             string query;
//             string line;
//             while (getline(file, line)) {
//                 query += line + "\n";
//                 // a query is complete when the transaction is committed
//                 if (line == "commit_tx") {
//                     break;
//                 }
//             }

//             // Launch thread to encode the file
//             worker_threads[i]->launch(query);

//         } else {
//             cout << "Error launching thread" << endl;
//             exit(1);
//         }
//     }

//     // Wait for all threads to finish
//     bool all_threads_finished = false;
//     while (!all_threads_finished) {
//         all_threads_finished = true;
//         for (int i = 0; i < NUM_WORKER_THREADS; i++) {
//             if (!worker_threads[i]->isCompleted()) {
//                 all_threads_finished = false;
//             } else {
//                 // Join each thread
//                 worker_threads[i]->join();
//                 // Should threads return anything?
//             }
//         }
//     }
    
// }