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
 * Struct declarations
 **********************************************************/

// can edit if needed
struct UserQuery {
    int sender_id;
    int receiver_id;
    int amount;
};


/**********************************************************
 * Class declarations
 **********************************************************/

class WorkerThread {
    public:
        WorkerThread(const UserQuery& _query) {
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
        bool p_launched;
        UserQuery query;
        thread p_thread;
};

void parseQuery() {
    // code for parsing the passed in query

    // check for read/write locks

    // if locked, wait for unlock
    // how should we implement "waiting"?

    // commit transaction
    return;
}


int main(int argc, char* argv[]) {
    ReadWriteLockingTable rwLockMap;

    if (rwLockMap.readLockID(12)) {
        cout << "12 successfully locked" << endl;
    }

    
}