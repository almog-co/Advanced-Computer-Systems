#include <iostream>
#include <string>
#include <chrono>
#include <immintrin.h>

using namespace std;

void printdb(int* database, int size) {
    for (int i = 0; i < size; i++) {
        cout << database[i] << endl;
    }
}

// Search if balance is in database. If so return index, else return -1
int searchSIMD(int* database, long size, long padded_size, int balance) {
    int number = 0;
    for (long i = 0; i < padded_size; i += 8) {
        __m256i input_reg = _mm256_set1_epi32(balance);
        __m256i data_reg = _mm256_loadu_si256((__m256i*) (database + i));
        __m256i cmp_reg = _mm256_cmpeq_epi32(input_reg, data_reg);
        int mask = _mm256_movemask_ps((__m256) cmp_reg);

        if (mask != 0) {
            number++;
        }
        
        // if (mask != 0) {
        //     return i + __builtin_ctz(mask);
        // }
    }

    return number;
}

int searchRegular(int* database, long size, int balance) {
    int number = 0;
    for (long i = 0; i < size; i++) {
        // if (database[i] == balance) {
        //     return i;
        // }
        if (database[i] == balance) {
            number++;
        }
    }

    return number;
}

int main(int argc, char** argv) {
    long size = 999999999;

    // Pad database to be a multiple of 8
    long padded_size = size;
    if (size % 8 != 0) {
        padded_size = size + (8 - (size % 8));
    }

    int* database = new int[padded_size];

    for (int i = 0; i < padded_size; i++) {
        database[i] = rand() % 100;
    }

    cout << size << " " << padded_size << endl;

    cout << "Finished generating database." << endl;

    int balance = database[rand() % size];

    auto startSIMD = chrono::high_resolution_clock::now();
    int indexSIMD = searchSIMD(database, size, padded_size, balance);
    auto endSIMD = chrono::high_resolution_clock::now();

    // Output the time it took to search
    cout << indexSIMD << endl;
    cout << "SIMD: " << chrono::duration_cast<chrono::milliseconds>(endSIMD - startSIMD).count() << "ms" << endl;

    auto startRegular = chrono::high_resolution_clock::now();
    int indexRegular = searchRegular(database, size, balance);
    auto endRegular = chrono::high_resolution_clock::now();

    // Output the time it took to search
    cout << indexRegular << endl;
    cout << "Regular: " << chrono::duration_cast<chrono::milliseconds>(endRegular - startRegular).count() << "ms" << endl;

}