#ifndef READWRITELOCKINGTABLE_H
#define READWRITELOCKINGTABLE_H

#include <unordered_map>
#include <iostream>
#include <mutex>

using namespace std;

// Class declaration for read/write locking data structure
class ReadWriteLockingTable {
    public:
        ReadWriteLockingTable() {
            this->count = 0;
        }

        // return whether id is read locked or not
        bool isReadLocked(int id) {
            //lock_guard<mutex> lock(m);
            // if id is not in table, it is unlocked
            if (table.find(id) == table.end()) {
                //cout << id << " is not read locked" << endl;
                return false;
            }

            return table[id].readLocked;
        }

        // return whether id is write locked or not
        bool isWriteLocked(int id) {
            //lock_guard<mutex> lock(m);
            // if id is not in table, it is unlocked
            if (table.find(id) == table.end()) {
                //cout << id << " is not write locked" << endl;
                return false;
            }

            return table[id].writeLocked;
        }

        // returns true if successfully locked, false if already locked
        bool readLockID(int id) {
            //cout << "readLockID called on " << id << endl;
            //lock_guard<mutex> lock(m);
            // if id isn't already in table, add it and lock read
            if (table.find(id) == table.end()) {
                table[id].readLocked = true;
                table[id].writeLocked = false;
                count += 1;
                return true;
            }

            // if id exists in table and is unlocked, lock read
            if (table[id].readLocked == false) { // first is read, second is write
                table[id].readLocked = true;
                return true;
            }

            // if id is currently locked, return false
            if (table[id].readLocked == true) {
                return false;
            }

            // if none of the above cases are met, something went wrong
            return false;
        }

        // returns true if successfully locked, false if already locked
        bool writeLockID(int id) {
            //cout << "writeLockID called on " << id << endl;
            //lock_guard<mutex> lock(m);
            // if id isn't already in table, add it and lock write
            if (table.find(id) == table.end()) {
                table[id].readLocked = false;
                table[id].writeLocked = true;
                count += 1;
                return true;
            }

            // if id exists in table and is unlocked, lock write
            if (table[id].writeLocked == false) { // first is read, second is write
                table[id].writeLocked = true;
                return true;
            }

            // if id is currently locked, return false
            if (table[id].writeLocked == true) {
                return false;
            }

            // if none of the above cases are met, something went wrong
            return false;
        }

        // function to unlock read
        bool readUnlock(int id) {
            //lock_guard<mutex> lock(m);
            // if id is not in table, it is unlocked
            if (table.find(id) == table.end()) {
                return true;
            }
            // unlock read
            table[id].readLocked = false;

            return true;
        }

        // function to unlock write
        bool writeUnlock(int id) {
            //lock_guard<mutex> lock(m);
            // if id is not in table, it is unlocked
            if (table.find(id) == table.end()) {
                return true;
            }
            // unlock write
            table[id].writeLocked = false;

            return true;
        }


    private:
        // track locking info
        struct LockInfo {
            bool readLocked = false;
            bool writeLocked = false;
        };

        // shared mutex allows for multiple readers to read simultaneously, but only one writer at a time
        // shared_mutex not supported on my machine, so using standard mutex instead
        mutex m;

        // table structure: key = id, value = {read locked?, write locked?}
        unordered_map<int, LockInfo> table;
        // track number of entries in table
        int count;
};



#endif