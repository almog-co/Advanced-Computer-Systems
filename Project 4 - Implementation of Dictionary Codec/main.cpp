#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

using namespace std;

// Parse input file

// Parse command line arguments
// Format: filename -arg1 value1 -arg2 value2 ...
unordered_map<string, string> parseArgs(int argc, char* argv[]) {
    if (argc == 1) {
        cout << "No arguments provided" << endl;
        exit(1);
    }
    unordered_map<string, string> args;
    args["filename"] = argv[1];

    for (int i = 2; i < argc; i++) {
        string arg = argv[i];
        if (arg[0] != '-') {
            cout << "Invalid argument: " << arg << endl;
            exit(1);
        }
        arg = arg.substr(1);
        if (i == argc - 1) {
            cout << "No value provided for argument: " << arg << endl;
            exit(1);
        }
        string value = argv[i + 1];
        args[arg] = value;
        i++;
    }

    return args;
}


int main(int argc, char* argv[]) {
    // Parse command line arguments
    unordered_map<string, string> args = parseArgs(argc, argv);

    // Print arguments
    for (unordered_map<string, string>::iterator arg = args.begin(); arg != args.end(); arg++) {
        cout << arg->first << ": " << arg->second << endl;
    }
    
}