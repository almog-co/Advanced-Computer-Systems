# Project 2: Matrix-Matrix Multiplication with SIMD Instructions & Cache Miss Minimization
By: Rojan Karn and Almog Cohen

## Project Introduction
This project implements a C/C++ module that carries out high-speed matrix-matrix multiplication by explicitly utilizing x86 SIMD instructions and minimizing cache miss rate via restructuring data access patterns. Matrix-matrix multiplication is a key data processing kernel in many real-life applications, such as machine learning, computer vision, signal processing and scientific computing. The project demonstrates the importance of exploiting data-level parallelism and minimizing cache miss rate.

## Program Structure
structure here

## Prerequisites
g++ compiler with C++ version 17

## Usage
The program takes a single command-line argument, the name of the input file.

The program is designed to be compiled and executed using the following command:
```
g++ -o matrix main.cpp -std=c++17 -march=native -O3 -msse4.2 -mavx && ./matrix
```

### Changing matrix size
The size of the square matrices being multiplied can be changed by modifying the line:
```
#define MATRIX_SIZE 10000
```

## Experimental Results
include raw data, graph (?), hardware env

## Analysis and Conclusion
