# Project 4 - Implementation of Dictionary Codec
By: Almog Cohen and Rojan Karn

## Project Introduction
A dictionary codec is a data compression technique used to reduce the data footprint and speed up search/scan operations on data with low cardinality. Dictionary encoding involves scanning the data to create a dictionary of unique data items and replacing each item with its dictionary ID. This project will use a custom Prefix Tree (or trie) data structure to accelerate dictionary look-up and apply SIMD instructions to further speed up search/scan operations. By implementing this technique, the project will contribute to the optimization of real-world data analytics systems.

## Program Structure
### Prefix Tree Class


## Prerequisites
1. immintrin library
2. g++ compiler with C++ version 11

## Usage
The program can be run in two different modes: vanilla or optimized. Vanilla mode does not make use of the Prefix Tree or multi-threading and instead simply performs the search/scan operations iteratively. Vanilla mode provides the baseline performance values that the optimized performance will be compared against.
Optimized mode uses the Prefix Tree data structure and multi-threading to perform the search/scan operations. This mode will showcase the efficiency improvements from vanilla mode.

To compile and run the program in **optimized** mode:
```
g++ main.cpp -std=c++11 && ./a.out <INPUT_FILE.txt>
```

To compile and run the program in **vanilla** mode:
```
g++ main.cpp -std=c++11 && ./a.out <INPUT_FILE.txt> -mode vanilla
```

## Experimental Results



## Analysis and Conclusion

