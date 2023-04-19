#include <iostream>
#include <vector>
#include "../ReadWriteLockingTable.h"
#include "../main.cpp"

using namespace std;

// function to verify if all IDs are read and write locked
bool verifyLocks(const vector<pair<int, string>>& ids, ReadWriteLockingTable& rwTable) {
    for (auto id : ids) {
        if (id.second == "READ") {
            if (!rwTable.isWriteLocked(id.first)) {
                return false;
            }
        } else if (id.second == "WRITE" || id.second == "BOTH") {
            if (!rwTable.isReadLocked(id.first) || !rwTable.isWriteLocked(id.first)) {
                cout << "ID " << id.first << " caused this failure" << endl;
                return false;
            }
        }
    }
    return true;
}

int main() {
    ReadWriteLockingTable readwriteTable;

    // create a vector of IDs to test
    vector<pair<int, string>> ids = {{1, "READ"}, {2, "WRITE"}, {3, "BOTH"}, {4, "READ"}, {5, "WRITE"}};

    readwriteTable.readLockID(1);
    readwriteTable.readLockID(2);
    readwriteTable.readLockID(3);
    readwriteTable.readLockID(4);
    readwriteTable.readLockID(5);

    readwriteTable.readUnlock(1);
    readwriteTable.readUnlock(2);
    readwriteTable.readUnlock(3);
    readwriteTable.readUnlock(4);
    readwriteTable.readUnlock(5);

    // call the tryPreLock function with the test IDs
    tryPreLock(ids, readwriteTable);

    // verify if all IDs are read and write locked
    if (verifyLocks(ids, readwriteTable)) {
        cout << "All IDs are locked successfully." << endl;
    } else {
        cout << "Locking failed." << endl;
    }

    return 0;
}
