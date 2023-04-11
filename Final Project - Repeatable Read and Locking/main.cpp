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


int main(int argc, char* argv[]) {
    ReadWriteLockingTable rwLockMap;

    if (rwLockMap.readLockID(12)) {
        cout << "12 successfully locked" << endl;
    }

    
}