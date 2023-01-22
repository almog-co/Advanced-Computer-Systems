#include "zstd.h"
#include <cstdlib>
#include <iostream>


void compress_data(char* input_buffer, char* output_buffer, bool &done) {
    // compression is not complete
    done = false;

    size_t input_size = 16384; // 16KB
    size_t output_size = ZSTD_compressBound(input_size);

    // compression here
    int compression_result = ZSTD_compress(output_buffer, output_size, input_buffer, input_size, 1);

    if (compression_result > 0) {
        // compression successful
        done = true;
    } else {
        // compression failed
        done = false;
    }
}

void compressBufferData(char* inBuffer, char* outBuffer, bool* done) {
    // compression is not complete
    *done = false;

    // create a ZSTD compressor object
    ZSTD_CCtx* cctx = ZSTD_createCCtx();
    
    // get the size of the compressed data
    size_t compressedSize = ZSTD_compressCCtx(cctx, outBuffer, 16384, inBuffer, 16384, 0);
    
    // free the compressor object
    ZSTD_freeCCtx(cctx);
    
    // set the done flag to true
    *done = true;
}


int main() {
    // create two 16KB buffers
    char* inBuffer = (char*)malloc(16384);
    char* outBuffer = (char*)malloc(16384);

    // fill the input buffer with random data
    for (int i = 0; i < 16384; i++) {
        inBuffer[i] = rand() % 256;
    }
    
    // set the done flag to false
    bool done = false;
    
    // compress the input buffer data
    compress_data(inBuffer, outBuffer, &done);
    
    // check if the compression is done
    if (done) {
        std::cout << "Compression successful!" << std::endl;
    } else {
        std::cout << "Compression failed!" << std::endl;
    }
    
    // free the buffers
    free(inBuffer);
    free(outBuffer);
    
    return 0;
}