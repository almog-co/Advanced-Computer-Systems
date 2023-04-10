#include "../database.h"

int main() {

    DatabaseTableSchema schema = {
        .columnCount = 2,
        .columnNames = new string[2]{"Name", "Balance"},
        .columnTypes = new int[2]{DATABASE_TYPE_STRING, DATABASE_TYPE_INT}
    };

    DatabaseTable table(schema);

    table.insert(2, "Bob", 1000);
    table.insert(2, "John", 2000);
    table.insert(2, "Jane", 3000);
    table.insert(2, "Joe", 4000);
    table.insert(2, "Jill", 5000);
    table.insert(2, "Jack", 6000);
    table.insert(2, "Jenny", 7000);
    table.insert(2, "Jen", 8000);
    table.insert(2, "Jenna", 9000);
    table.insert(2, "Jenifer", 10000);
    table.insert(2, "Andy", 11000);

    table.print();

    return 0;
}