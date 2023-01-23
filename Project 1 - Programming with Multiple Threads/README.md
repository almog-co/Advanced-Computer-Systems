# Project 1: Programming with Multiple Threads (C++)
By: Almog Cohen and Rojan Karn


## Project Introduction

This project aims to provide hands-on experience in multithreading by implementing a C++ module that utilizes multiple threads to compress an input data stream. The compression is achieved by utilizing the open-source ZSTD library (https://facebook.github.io/zstd/). The module will divide the input data stream into 16KB blocks and compress them individually. The compressed blocks will be written to the output file in the same order as the input data stream. The project will use one thread for reading the input data, dispatching the blocks to worker threads for compression, receiving the compressed blocks from worker threads and writing them to the output file. The goal is to demonstrate the ability to implement multithreading in a real-world application.


## Program Structure/Usage



## Experimental Results



## Analysis and Conclusion


