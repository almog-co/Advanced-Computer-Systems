#include "../database.h"
#include <vector>
#include <iostream>
#include <string>

/*
 * This file runs unit tests on the DatabaseTable class in Database.h
 */

bool insertTestValues(DatabaseTable& table) {
    vector<string> names{"Bob", "John", "Jane", "Joe", "Jill", "Jack", "Jenny", "Jen", "Jenna", "Jenifer", "Andy"};
    vector<int> balances{1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000};

    if (names.size() != balances.size()) {
        return false;
    }

    for (int i = 0; i < names.size(); i++) {
        table.insert(2, names[i].c_str(), balances[i]);
    }

    // Check that the values were inserted correctly
    for (int i = 0; i < names.size(); i++) {
        if (table.getStringColumn(i, "Name") != names[i]) {
            return false;
        }
        if (table.getIntColumn(i, "Balance") != balances[i]) {
            return false;
        }
    }

    return true;
}

int main() {

    DatabaseTableSchema schema = {
        .columnCount = 2,
        .columnNames = new string[2]{"Name", "Balance"},
        .columnTypes = new int[2]{DATABASE_TYPE_STRING, DATABASE_TYPE_INT}
    };

    DatabaseTable table(schema);

    // Test insert
    if (!insertTestValues(table)) {
        cout << "Insert test failed" << endl;
        return 1;
    } else {
        cout << "Insert test passed" << endl;
    }
    
    return 0;
}