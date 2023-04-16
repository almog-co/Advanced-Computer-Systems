#ifndef QUERY_H_
#define QUERY_H_

/*
 * This file parses a custom query language and executes the query on the database.
 * The language is as follows:
 *  - begin_tx              : Starts a transaction
 *  - commit_tx             : Commits a transaction
 *  - abort_tx              : Aborts a transaction
 *  - readId(id, col)       : Reads a value from a row
 *  - readVal(term, col)    : Reads a value from a row (search term is a string
 *  - write(id, col, value) : Writes a value to a row
 *  - print(a)              : Prints the value of a variable
 *  - +, -, *, /            : Arithmetic operators
 *  - ==, <, >, <=, >=      : Comparison operators
 *  - if, else              : Conditional statements
*/

/* Example query to increment the balance of a user by 100:
 *
 * begin_tx
 * b = readId(0, Balance)
 * write(0, Balance, 100 + b) 
 * commit_tx
*/

#include <string>
#include <utility>
#include <vector>
#include <iostream>

using namespace std;

// Main function that parses the query and executes it
void parseQuery(const string& query);

void parseQuery(const string& query) {
    // Vector to store variables and their values (e.g. a = 5)
    vector<pair<string, int>> variables;


}

#endif