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

using namespace std;

#define NUM_WORKER_THREADS 1

/**********************************************************
 * Class declarations
 **********************************************************/

class WorkerThread {
    public:
        WorkerThread(const string& _query) {
            this->p_launched = false;
            this->query = _query;
        }

        void launch(const vector<string>& lines, const int actual_start_index) {
            parseQuery();
            this->p_launched = true;
            // what function should be launched by each thread?
            
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
        // this function will parse the query that is associated with this thread
        // void or should it return something?
        // maybe it should return parameters needed
        void parseQuery() {
            string q = this->query;
            // need to figure out what format queries will be in
            // in order to parse
        }

        bool p_launched;
        string query;
        thread p_thread;
};


int main(int argc, char* argv[]) {
    ReadWriteLockingTable rwLockMap;

    if (rwLockMap.readLockID(12)) {
        cout << "12 successfully locked" << endl;
    }

    
}