#include "../query.h"

#include <fstream>

int main() {
    // set query to file contents in transaction.txt
    string query;
    ifstream file("transaction.txt");
    string line;
    while (getline(file, line)) {
        query += line + "\n";
    }

    parseQuery(query);
}