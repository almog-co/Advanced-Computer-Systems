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

using namespace std;


// Class declaration for read/write locking data structure
class ReadWriteLockingTable {
    public:
        ReadWriteLockingTable() {
            this->count = 0;
        }

        // return whether id is read locked or not
        bool isReadLocked(int id) {
            // if id is not in table, it is unlocked
            if (table.find(id) == table.end()) {
                return false;
            }

            return table[id].first;
        }

        // return whether id is write locked or not
        bool isWriteLocked(int id) {
            // if id is not in table, it is unlocked
            if (table.find(id) == table.end()) {
                return false;
            }

            return table[id].second;
        }

        // returns true if successfully locked, false if already locked
        bool readLockID(int id) {
            // if id isn't already in table, add it and lock read
            if (table.find(id) == table.end()) {
                table[id] = make_pair(true, false);
                count += 1;
                return true;
            }

            // if id exists in table and is unlocked, lock read
            if (table[id].first == false) { // first is read, second is write
                table[id].first = true;
                return true;
            }

            // if id is currently locked, return false
            if (table[id].first == true) {
                return false;
            }

            // if none of the above cases are met, something went wrong
            return false;
        }

        // returns true if successfully locked, false if already locked
        bool writeLockID(int id) {
            // if id isn't already in table, add it and lock write
            if (table.find(id) == table.end()) {
                table[id] = make_pair(false, true);
                count += 1;
                return true;
            }

            // if id exists in table and is unlocked, lock write
            if (table[id].second == false) { // first is read, second is write
                table[id].second = true;
                return true;
            }

            // if id is currently locked, return false
            if (table[id].second == true) {
                return false;
            }

            // if none of the above cases are met, something went wrong
            return false;
        }

        // function to unlock read
        bool readUnlock(int id) {
            // if id is not in table, it is unlocked
            if (table.find(id) == table.end()) {
                return true;
            }
            // unlock read
            table[id].first = false;

            return true;
        }

        // function to unlock write
        bool writeUnlock(int id) {
            // if id is not in table, it is unlocked
            if (table.find(id) == table.end()) {
                return true;
            }
            // unlock write
            table[id].second = false;

            return true;
        }


    private:
        // table structure: key = id, value = {read locked?, write locked?}
        unordered_map<int, pair<bool, bool>> table;
        // track number of entries in table
        int count;
};


int main(int argc, char* argv[]) {
    ReadWriteLockingTable rwLockMap;

    if (rwLockMap.readLockID(12)) {
        cout << "12 successfully locked" << endl;
    }

    
}