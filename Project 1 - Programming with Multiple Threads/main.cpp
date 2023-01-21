#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include "zstd.h"

using namespace std;

// class ZSTDThread {
//     public:
//         ZSTDThread(const string& data, const int id) {
//             this->data = data;
//             this->id = id;
//         }

//         void launch() {
//             // Launch thread
//         }
//     private:
//         int id;
//         string data;   
// };

// global variables for I/O files
ifstream inputFile;
fstream outputFile("output.zst", ios::binary);

// 16KB block size
const int BLOCK_SIZE = 16 * 1024;

// function for reading input data
void readData() {
    char buffer[BLOCK_SIZE]; // 16 KB buffer
    while (inputFile.read(buffer, BLOCK_SIZE)) {
        // read input file
    }
}

// function for compressing input data
void compressData(char* buffer) {
    // compress using ZSTD
    // send compressed data to main
}


void writeData(char* compressed_data) {
    // write the compressed data to output file
}


int main(int argc, char* argv[]) {
    cout << ZSTD_versionNumber() << endl;

    // Check for arguments
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <file>" << endl;
        return 1;
    }

    
    // Verify file can be opened
    if (!inputFile.is_open()) {
        cout << "Error: Could not open input file " << argv[1] << endl;
        return 1;
    }

    if (!outputFile.is_open()) {
        cout << "Error: Could not open output file" << endl;
        return 1;
    }


    // Read file in 16KB chunks, compress each chunk using zstd
    size_t chunkSize = 16000;
    char inputBuffer[chunkSize];
    char outputBuffer[chunkSize];

    // possibly move this loop to readData function
    while (inputFile) {
        inputFile.read(inputBuffer, chunkSize);

        // Break if reading no new data
        if (inputFile.gcount() == 0) {
            break;
        }

        // Set the input/output buffer for compression
        ZSTD_inBuffer zstdInputBuffer = {
            inputBuffer,
            chunkSize,
            0
        };

        ZSTD_outBuffer zstdOutputBuffer = {
            outputBuffer,
            chunkSize,
            0
        };

        // Create the compression context
        ZSTD_CCtx* zstdContext = ZSTD_createCCtx();

        // Compress the data
        size_t compressedSize = ZSTD_compressStream2(
            zstdContext,
            &zstdOutputBuffer,
            &zstdInputBuffer,
            ZSTD_e_end
        );

        // Free the compression context
        ZSTD_freeCCtx(zstdContext);

        outputFile << outputBuffer;
    }

    // create worker threads
    std::thread reader_thread(readData);
    std::vector<std::thread> worker_threads;
    int num_threads = 5;

    for (int i = 0; i < num_threads; i++) {
        std::thread t(compressData);
        worker_threads.push_back(t);
    }

    // join threads
    reader_thread.join();
    for (int i = 0; i < worker_threads.size(); i++) {
        worker_threads[i].join();
    }

    // Close the files
    inputFile.close();
    outputFile.close();

}