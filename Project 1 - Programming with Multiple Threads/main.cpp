#include <iostream>
#include <thread>
#include <fstream>
#include <zstd.h>

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

int main(int argc, char* argv[]) {
    cout << ZSTD_versionNumber() << endl;

    // Check for arguments
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <file>" << endl;
        return 1;
    }

    ifstream inputFile(argv[1], ios::binary);
    fstream outputFile("output.zst", ios::binary);
    
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

    // Close the files
    inputFile.close();
    outputFile.close();

}