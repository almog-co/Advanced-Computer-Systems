#include <iostream>
#include <immintrin.h>
#include <cmath>

#define MATRIX_SIZE 4

void native_matrix_multiplication(float* A, float* B, float* C) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            float sum = 0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                sum += A[i * MATRIX_SIZE + k] * B[k * MATRIX_SIZE + j];
            }
            C[i * MATRIX_SIZE + j] = sum;
        }
    }
}

int main() {
    float A[MATRIX_SIZE * MATRIX_SIZE] = {1, 2, 3, 4,
                                          5, 6, 7, 8,
                                          9, 10, 11, 12,
                                          13, 14, 15, 16};
    float B[MATRIX_SIZE * MATRIX_SIZE] = {1, 2, 3, 4,
                                          5, 6, 7, 8,
                                          9, 10, 11, 12,
                                          13, 14, 15, 16};
    float C[MATRIX_SIZE * MATRIX_SIZE] = {0};

    native_matrix_multiplication(A, B, C);

    for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        std::cout << C[i] << " ";
        if ((i + 1) % MATRIX_SIZE == 0) {
            std::cout << std::endl;
        }
    }

    return 0;
}
