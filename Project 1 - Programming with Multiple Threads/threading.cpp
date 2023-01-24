/**
 * This program will take in a file and compress it using the Zstandard library.
 * First, it generates 1 worker thread to read the data in 16 KB chunks and launch
 * mulitple worker threads to compress the data. The worker threads will then
 * write the compressed data to a file in the same order as the original data.
 * 
 * @author Almog Cohen
 * @author Rojan Karn
 * @date 01/25/2023
 */

// =================================
// Includes
// =================================

#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <stdlib.h>
#include <zstd.h>

using namespace std;

// =================================
// Constant Definitions
// =================================

#define NUM_WORKER_THREADS 6
#define CHUNK_SIZE 16384

// =================================
// Function Declarations
// =================================

void compressBufferData(char* input_buffer, char* output_buffer, size_t& compressedSize);
void test(int16_t thread_id, char** outBuffer) {
    cout << "Hello World from " << thread_id << endl;
}

// =================================
// Class/Structure Definitions
// =================================

// Class to store std::thread and thread id
class WorkerThread {
    public:
        WorkerThread() {
            this->p_thread_id = -1;
            this->p_launched = false;
            this->p_output_buffer = nullptr;
            this->compressedSize = 0;
        }
    
        WorkerThread(const int16_t thread_id) {
            this->p_thread_id = thread_id;
            this->p_launched = false;
            this->p_output_buffer = new char[CHUNK_SIZE];
            this->compressedSize = 0;
        }

        void launch(char* inBuffer) {
            //this->p_thread = thread(test, this->p_thread_id); // test function
            this->p_thread = thread(compressBufferData, inBuffer, this->p_output_buffer, this->compressedSize); // compress data function
            this->p_thread = thread(compressBufferData, inBuffer, &(this->p_output_buffer), );
            this->p_launched = true;
        }

        void join() {
            this->p_thread.join();
        }

        bool isCompleted() {
            return !(this->p_launched) || this->p_thread.joinable();
        }

        void clear() {
            this->p_thread_id = -1;
            this->p_launched = false;
            this->p_output_buffer = NULL;
            this->compressedSize = 0;
        }

        int16_t getID() {
            return this->p_thread_id;
        }

        char* getOutputBuffer() {
            return this->p_output_buffer;
        }

    private:
        int16_t p_thread_id;
        bool p_launched;
        char* p_output_buffer;
        thread p_thread;
        size_t compressedSize;
};
// =================================
// Main Function
// =================================
int main(int argc, const char * argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input file>", argv[0]);
        return 1;
    }

    // Open the input file in binary mode using ifstream
    ifstream input_file(argv[1], ios::binary);

    // Check if the file was opened successfully
    if (!input_file.is_open()) {
        printf("Error: Could not open file %s", argv[1]);
        return 1;
    }

    // Array to store compressed output
    // 0 => id=0 compressed data, 1 => id=1 compressed data, etc.
    char* compressed_data_output_buffers[int(input_file.tellg() / CHUNK_SIZE) + 1];
    size_t 

    // Array to store compressed output size (parallel to above array)
    size_t compressed_data_sizes[int(input_file.tellg() / CHUNK_SIZE) + 1];

    // Array of WorkerThread objects
    WorkerThread* worker_threads[NUM_WORKER_THREADS];
    for (int i = 0; i < NUM_WORKER_THREADS; i++) {
        worker_threads[i] = new WorkerThread();
    }

    // For each thread, read 16 KB and launch a worker thread to compress it
    while (!input_file.eof()) {
        for (int i = 0; i < NUM_WORKER_THREADS; i++) {
            if (worker_threads[i]->isCompleted()) {
                if (worker_threads[i]->getID() != -1) {
                    cout << "Joining thread " << i << endl;

                    // Join the thread
                    worker_threads[i]->join();

                    // Get the output buffer
                    size_t compressed_size;
                    char* output_buffer = worker_threads[i]->getOutputBuffer(compressed_size);

                    // Add the output buffer to the vector in position id
                    compressed_data_output_buffers[worker_threads[i]->getID()] = output_buffer;

                    // Clear the thread for reuse
                    worker_threads[i]->clear();
                } else {
                    if (input_file.eof())
                        break;

                    cout << "Launching thread " << i << endl;

                    // Read 16 KB from the file
                    char* input_buffer = new char[CHUNK_SIZE];
                    input_file.read(input_buffer, CHUNK_SIZE);

                    // Delete old worker thread
                    delete worker_threads[i];

                    // Launch a worker thread to compress the data
                    worker_threads[i] = new WorkerThread(i);
                    worker_threads[i]->launch(input_buffer);

                    cout << "Launched thread " << i << endl;
                }
            }
        }
    }

    // Join all threads remaining
    bool all_threads_completed = false;
    while(!all_threads_completed) {
        all_threads_completed = true;
        for (int i = 0; i < NUM_WORKER_THREADS; i++) {
            if (worker_threads[i]->isCompleted()) {
                if (worker_threads[i]->getID() != -1) {
                    cout << "Final join thread " << i << endl;

                    // Join the thread
                    worker_threads[i]->join();

                    // Get the output buffer
                    char* output_buffer = worker_threads[i]->getOutputBuffer();

                    // Add the output buffer to the vector in position id
                    compressed_data_output_buffers[worker_threads[i]->getID()] = output_buffer;
                    
                    // Clear the thread for reuse
                    worker_threads[i]->clear();

                    all_threads_completed = false;
                }
            }
        }
    }

    // Write the compressed data to the output file
    ofstream output_file("output.txt");
    for (int i = 0; i < int(input_file.tellg() / CHUNK_SIZE) + 1; i++) {
        output_file.write(compressed_data_output_buffers[i], 6);
    }

    // Close the files
    input_file.close();
    output_file.close();
}
// =================================
// Function Definitions
// =================================

// Function to compress data
void compressBufferData(char* input_buffer, char* output_buffer, size_t& compressedSize) {

    size_t input_size = 16384; // 16KB
    size_t output_size = ZSTD_compressBound(input_size);

    // compression here
    compressedSize = ZSTD_compress(output_buffer, output_size, input_buffer, input_size, 1);

}