# Project 4 - Implementation of Dictionary Codec
By: Almog Cohen and Rojan Karn

## Project Introduction
A dictionary codec is a data compression technique used to reduce the data footprint and speed up search/scan operations on data with low cardinality. Dictionary encoding involves scanning the data to create a dictionary of unique data items and replacing each item with its dictionary ID. This project will use a custom Prefix Tree (or trie) data structure to accelerate dictionary look-up and apply SIMD instructions to further speed up search/scan operations. By implementing this technique, the project will contribute to the optimization of real-world data analytics systems.

## Program Structure
### PrefixTree.h
The PrefixTreeNode class represents a single node in a prefix tree, which is a tree data structure used for efficiently storing and searching for strings. The PrefixTreeNode class contains a character value, a boolean flag to indicate if the node represents the end of a word, a vector of child nodes (for each character in the alphabet), and a vector of original file indices (to track where the word was found in the original document). It also provides methods for adding and retrieving child nodes, as well as for printing the node and checking if a particular character has a child node.

The PrefixTree class represents the overall prefix tree data structure, and contains a root node of type PrefixTreeNode. It provides methods for inserting words into the tree (which returns an encoded index of the word), searching for words (which returns all indices of the word in the original file), searching for prefixes (which returns a vector of pairs of all words in the tree that start with the prefix and their indices), and merging another prefix tree into this one. The class also includes two search methods that are designed to make use of SIMD instructions for faster searching.

### Main Program
The program is designed to read in a file and build a prefix tree data structure from its contents. The prefix tree is used to search for prefixes and complete words within the file.

The program reads in a file, converts all text to lowercase, and stores each line in a vector of strings. This vector is then divided into multiple smaller vectors, with each vector assigned to a separate worker thread. Each worker thread is responsible for building a separate prefix tree based on the lines assigned to it. Once all the worker threads have completed building their individual prefix trees, they are merged together into one large prefix tree which represents the dictionary.

There are also functions defined for performing a "vanilla" search of the file for a specific prefix or word. The vanillaSearchPrefix() function performs a simple prefix search of the file, while the vanillaSearchWord() function performs a simple word search. These functions are used as a baseline for comparison against the prefix tree search.

Once the Prefix Tree is built and ready to search, a user interface comes up in the terminal prompting the user to search/scan the dictionary. They can choose to either search for an entire word or search for a prefix. After they entire their word/prefix, the program prints out the indices that the word/prefix appears at in the input file. It also prints out the time it took for the search/scan operation to complete. This user interface continues to run for the user to search for multiple words until the user prompts the program to quit.

## Prerequisites
1. immintrin library
2. g++ compiler with C++ version 11

## Usage
### Multi Threading
The number of threads used to run the program can be changed by editing the line in main.cpp:
```
#define NUM_WORKER_THREADS <number of threads>
```

### Compiling and Running
The program can be run in two different modes: vanilla or optimized. Vanilla mode does not make use of the Prefix Tree or multi-threading and instead simply performs the search/scan operations iteratively. Vanilla mode provides the baseline performance values that the optimized performance will be compared against.
Optimized mode uses the Prefix Tree data structure and multi-threading to perform the search/scan operations. This mode will showcase the efficiency improvements from vanilla mode.

To compile and run the program in **optimized** mode:
```
g++ main.cpp -std=c++11 -mavx2 && ./a.out <INPUT_FILE.txt>
```

To compile and run the program in **vanilla** mode:
```
g++ main.cpp -std=c++11 -mavx2 && ./a.out <INPUT_FILE.txt> -mode vanilla
```

## Experimental Results



## Analysis and Conclusion

