# Project 1: Programming with Multiple Threads (C++)
By: Almog Cohen and Rojan Karn


## Project Introduction

This project aims to provide hands-on experience in multithreading by implementing a C++ module that utilizes multiple threads to compress an input data stream. The compression is achieved by utilizing the open-source ZSTD library (https://facebook.github.io/zstd/). The module will divide the input data stream into 16KB blocks and compress them individually. The compressed blocks will be written to the output file in the same order as the input data stream. The project will use one thread for reading the input data, dispatching the blocks to worker threads for compression, receiving the compressed blocks from worker threads and writing them to the output file. The goal is to demonstrate the ability to implement multithreading in a real-world application.


## Program Structure
This program is designed to take in a file and compress it using the Zstandard library. The program creates 1 worker thread that reads the data in 16 KB chunks and launches multiple worker threads to compress the data. The worker threads then write the compressed data to a file in the same order as the original data.

The program uses a WorkerThread class to manage the worker threads. The class has methods for launching, joining and checking if the thread has completed execution.

The program uses the compressBufferData function to compress the input buffer, puts the compressed data into the WorkerThread's output buffer, and provides the compressedSize for that data.

The compressed data is stored in an array of output buffers, parallel to an array of compressed data sizes.

## Usage
The program takes a single command-line argument, the name of the input file. The input file must be in binary mode.

The program is designed to be compiled and executed using the following command:
```
g++ -std=c++11 -o compress.out -L/usr/local/lib/ -lzstd compress.cpp
./compress.out <input file>
```

The output will be written to a file named "compressed_data.zst" in the current working directory.

The program uses a single thread to read data from the file, dispatch 16KB blocks to other worker threads for compression, receive compressed blocks from other worker threads, and write compressed blocks (in the correct order) to the output file.

It is important to note that the program uses the ZSTD library and it should be installed on the system before executing the program, otherwise the compilation will fail.


## Experimental Results



## Analysis and Conclusion


