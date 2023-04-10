#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <string>
#include <stdarg.h>

using namespace std;

#define INITIAL_ROW_COUNT 4
#define DATABASE_TYPE_INT 0
#define DATABASE_TYPE_STRING 1

struct DatabaseTableSchema {
    int columnCount;
    string* columnNames;
    int *columnTypes;
};

/* Stores data in columnCount array's of size rowCount each
 *
 * Example: 1 -> [Name1, Balance1]
 *          2 -> [Name2, Balance2]
 *          ...
 * ArrayName: [Name1, Name2, ...]
 * ArrayBalance: [Balance1, Balance2, ...]
 */
class DatabaseTable {
public:
    DatabaseTableSchema schema;
    DatabaseTable(DatabaseTableSchema schema) {
        this->schema = schema;
        this->data = new void*[schema.columnCount];
        this->usedSpace = 0;
        this->totalSpace = INITIAL_ROW_COUNT;

        for (int i = 0; i < schema.columnCount; i++) {
            if (schema.columnTypes[i] == DATABASE_TYPE_INT) {
                this->data[i] = new int[INITIAL_ROW_COUNT];
            } else if (schema.columnTypes[i] == DATABASE_TYPE_STRING) {
                this->data[i] = new string[INITIAL_ROW_COUNT];
            }
        }
    }

    // Insert in the order of the schema
    // Example: insert("Bob", 1000) if schema is Name, Balance
    void insert(int count, ...) {
        va_list args;
        va_start(args, count);

        // If the table is full, double the size of the table and copy the data over
        if (usedSpace == totalSpace) {
            totalSpace *= 2;
            for (int i = 0; i < schema.columnCount; i++) {
                if (schema.columnTypes[i] == DATABASE_TYPE_INT) {
                    int* temp = new int[totalSpace];
                    for (int j = 0; j < usedSpace; j++) {
                        temp[j] = ((int*)data[i])[j];
                    }
                    delete[] (int*)data[i];
                    data[i] = temp;
                } else if (schema.columnTypes[i] == DATABASE_TYPE_STRING) {
                    string* temp = new string[totalSpace];
                    for (int j = 0; j < usedSpace; j++) {
                        temp[j] = ((string*)data[i])[j];
                    }
                    delete[] (string*)data[i];
                    data[i] = temp;
                }
            }
        }

        // Insert the data
        for (int i = 0; i < schema.columnCount; i++) {
            if (schema.columnTypes[i] == DATABASE_TYPE_INT) {
                ((int*)data[i])[usedSpace] = va_arg(args, int);
            } else if (schema.columnTypes[i] == DATABASE_TYPE_STRING) {
                ((string*)data[i])[usedSpace] = va_arg(args, char*);
            }
        }

        usedSpace++;

        va_end(args);
    }

    /* Formats data in the following way:
     * 
     * id   Name    Balance 
     * 0    Bob        1000
     * 1    Alice      2000
     * ...
     */
    void print() {
        int columnWidth = 8;
        cout << printWidthHelper(columnWidth, "id");
        for (int i = 0; i < schema.columnCount; i++) {
            cout << printWidthHelper(columnWidth, schema.columnNames[i]);
        }
        cout << endl;

        for (int i = 0; i < usedSpace; i++) {
            cout << printWidthHelper(columnWidth, to_string(i));
            for (int j = 0; j < schema.columnCount; j++) {
                if (schema.columnTypes[j] == DATABASE_TYPE_INT) {
                    cout << printWidthHelper(columnWidth, to_string(((int*)data[j])[i]));
                } else if (schema.columnTypes[j] == DATABASE_TYPE_STRING) {
                    cout << printWidthHelper(columnWidth, ((string*)data[j])[i]);
                }
            }
            cout << endl;
        }
    }

private:
    // Points to an array of pointers to the data
    void** data;
    int usedSpace;
    int totalSpace;

    string printWidthHelper(int width, string str) {
        if (str.length() > width) {
            return str.substr(0, width);
        }

        string result = str;
        for (int i = 0; i < width - str.length(); i++) {
            result += " ";
        }
        return result;
    }
};

#endif