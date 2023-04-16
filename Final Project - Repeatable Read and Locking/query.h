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
#include <sstream>

using namespace std;

typedef enum {
    READ_ID,
    READ_VAL,
    WRITE,
    PRINT,
    ARITHMETIC,
    COMPARISON,
    IF,
    ELSE,
    END_IF,
    END_ELSE,
    END_QUERY
} QueryType;

typedef enum {
    BEGIN_TX,
    COMMIT_TX,
    ABORT_TX,
} TransactionState;

// Main function that parses the query and executes it
void parseQuery(const string& query);

// Works like the split function in Python
// Returns a list of strings that are separated by the delimiter
vector<string> split(string& str, char delimiter);

// Works like the strip function in Python
// Returns a string with the leading and trailing whitespace removed
string strip(string& str);

// Check if a string sequence is in a string.
// Return its index if it is, -1 if it isn't
int contains(string& str, string sequence);

// Given list of variables and expressions, evaluate the expression
// and return the result
int evaluateExpression(const vector<pair<string, int>>& variables, string& expression);

// Given a list of variables and a function call, execute the function and return the result
int executeFunction(const vector<pair<string, int>>& variables, string& function);

void parseQuery(const string& query) {
    // Vector to store variables and their values (e.g. a = 5)
    vector<pair<string, int>> variables;

    // Split the query into lines
    string queryCopy = query;
    vector<string> lines = split(queryCopy, '\n');

    TransactionState state = BEGIN_TX;

    for (int i = 0; i < lines.size(); i++) {
        lines[i] = strip(lines[i]);

        // Check for transaction state change
        if (lines[i] == "begin_tx") {
            state = BEGIN_TX;
        } else if (lines[i] == "commit_tx") {
            state = COMMIT_TX;
            break;
        } else if (lines[i] == "abort_tx") {
            state = ABORT_TX;
            break;
        }

        // Check for variable assignment
        else if (contains(lines[i], "=") != -1) {
            // Split the line into the variable name and the value
            vector<string> tokens = split(lines[i], '=');
            string variableName = strip(tokens[0]);
            
            // Check if right side is a number
            bool isNumber = true;
            for (int j = 0; j < tokens[1].size(); j++) {
                if (!isdigit(tokens[1][j])) {
                    isNumber = false;
                    break;
                }
            }

            // If the right side is a number, store it as an int
            int value;
            if (isNumber) {
                value = stoi(tokens[1]);
            }

            // If the right side is an expression, evaluate it and store its contents
            else {
                // TODO: Evaluate expression
                value = evaluateExpression(variables, tokens[1]);
            }

            // Check if the variable already exists
            bool exists = false;
            for (int j = 0; j < variables.size(); j++) {
                if (variables[j].first == variableName) {
                    variables[j].second = value;
                    exists = true;
                    break;
                }
            }

            // If the variable doesn't exist, add it to the vector
            if (!exists) {
                variables.push_back(make_pair(variableName, value));
            }
        }

        else {
            evaluateExpression(variables, lines[i]);
        }
    }

    // Print the variables and their values
    for (int i = 0; i < variables.size(); i++) {
        cout << variables[i].first << " = " << variables[i].second << endl;
    }
}

vector<string> split(string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

string strip(string& str) {
    string copy = str;
    copy.erase(0, copy.find_first_not_of(' '));
    copy.erase(copy.find_last_not_of(' ') + 1);
    return copy;
}

int contains(string& str, string sequence) {
    int index = str.find(sequence);
    if (index != string::npos) {
        return index;
    }
    return -1;
}

int evaluateExpression(const vector<pair<string, int>>& variables, string& expression) {
    expression = strip(expression);
    // Look for function calls
    int index = contains(expression, "(");
    if (index != -1) {
        // Split the expression into the function name and the arguments
        string functionName = expression.substr(0, index);
        string arguments = expression.substr(index + 1, expression.size() - index - 2);

        // Evaluate the arguments
        vector<string> args = split(arguments, ',');
        for (int i = 0; i < args.size(); i++) {
            args[i] = strip(args[i]);
            args[i] = to_string(evaluateExpression(variables, args[i]));
        }

        // Join the arguments back together
        string argsJoined = "";
        for (int i = 0; i < args.size(); i++) {
            argsJoined += args[i];
            if (i != args.size() - 1) {
                argsJoined += ",";
            }
        }

        // Execute the function
        string function = functionName + "(" + argsJoined + ")";
        return -1; //executeFunction(variables, function);
    }

    // Look for arithmetic operators 
    string operations[4] = {"+", "-", "*", "/"};

    for (int i = 0; i < 4; i++) {
        int index = contains(expression, operations[i]);
        if (index != -1) {
            // Split the expression into two parts
            string left = expression.substr(0, index);
            string right = expression.substr(index + 1);

            // Evaluate the left and right sides
            int leftValue = evaluateExpression(variables, left);
            int rightValue = evaluateExpression(variables, right);

            // Perform the operation
            switch (operations[i][0]) {
                case '+':
                    return leftValue + rightValue;
                case '-':
                    return leftValue - rightValue;
                case '*':
                    return leftValue * rightValue;
                case '/':
                    return leftValue / rightValue;
            }
        }
    }

    // Look for comparison operators
    string comparisons[5] = {"==", "<", ">", "<=", ">="};
    for (int i = 0; i < 5; i++) {
        int index = contains(expression, comparisons[i]);
        if (index != -1) {
            // Split the expression into two parts
            string left = expression.substr(0, index);
            string right = expression.substr(index + 2);

            // Evaluate the left and right sides
            int leftValue = evaluateExpression(variables, left);
            int rightValue = evaluateExpression(variables, right);

            // Perform the comparison
            switch (comparisons[i][0]) {
                case '=':
                    return leftValue == rightValue;
                case '<':
                    if (comparisons[i].size() == 1) {
                        return leftValue < rightValue;
                    } else {
                        return leftValue <= rightValue;
                    }
                case '>':
                    if (comparisons[i].size() == 1) {
                        return leftValue > rightValue;
                    } else {
                        return leftValue >= rightValue;
                    }
            }
        }
    }

    // Look for variables
    for (int i = 0; i < variables.size(); i++) {
        if (variables[i].first == expression) {
            return variables[i].second;
        }
    }

    // If the expression is a number, return it
    return stoi(expression);

}

#endif