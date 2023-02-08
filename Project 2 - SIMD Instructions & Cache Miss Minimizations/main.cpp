#include <iostream>
#include <immintrin.h>
#include <cmath>
#include <chrono>
using namespace std;

#define MATRIX_SIZE 1000

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

// overloaded for different data type
void native_matrix_multiplication(short int* A, short int* B, short int* C) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            short int sum = 0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                sum += A[i * MATRIX_SIZE + k] * B[k * MATRIX_SIZE + j];
            }
            C[i * MATRIX_SIZE + j] = sum;
        }
    }
}

// fucnction from Figure 6 (page 19) of ResearchSquare report (that i texted you)
void avx_matrix_multiplication(float** A, float** B, float** C) {
    __m256 ymm0;
    __m256 ymm1;
    __m256 result;

    int i, j, k;
    float temp[MATRIX_SIZE];

    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            ymm0 = _mm256_load_ps(&A[i][0]);
            ymm1 = _mm256_load_ps(&B[j][0]);
            result = _mm256_mul_ps(ymm0, ymm1);
            for (k = 8; k < MATRIX_SIZE; k += 8) {
                result = _mm256_add_ps(_mm256_mul_ps(_mm256_load_ps(&A[i][k]), _mm256_load_ps(&B[j][k])), result);
                _mm256_store_ps(temp, result);
            }
            C[i][j] += temp[0] + temp[1] + temp[2] + temp[3] + temp[4] + temp[5] + temp[6] + temp[7];
        }
    }

}

// function found on stack exchange - not tested
void avx_matrix_mult(int A[MATRIX_SIZE][MATRIX_SIZE], int B[MATRIX_SIZE][MATRIX_SIZE], int C[MATRIX_SIZE][MATRIX_SIZE]) {
    __m256i vec_multi_res = _mm256_setzero_si256(); //Initialize vector to zero
    __m256i vec_A = _mm256_setzero_si256(); //Initialize vector to zero
    __m256i vec_B = _mm256_setzero_si256(); //Initialize vector to zero

    int i, j, k;
    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; ++j) {
            //Stores one element in mat1 and use it in all computations needed before proceeding
            //Stores as vector to increase computations per cycle
            vec_A = _mm256_set1_epi32(A[i][j]);

            for (k = 0; k < MATRIX_SIZE; k += 8) {
                vec_B = _mm256_loadu_si256((__m256i*)&B[j][k]); //Stores row of second matrix (eight in each iteration)
                vec_multi_res = _mm256_loadu_si256((__m256i*)&C[i][k]); //Loads the result matrix row as a vector
                vec_multi_res = _mm256_add_epi32(vec_multi_res ,_mm256_mullo_epi32(vec_A, vec_B));//Multiplies the vectors and adds to th the result vector

                _mm256_storeu_si256((__m256i*)&C[i][k], vec_multi_res); //Stores the result vector into the result array
            }
        }
    }
}



int main() {
    // test stuff
    // float A[MATRIX_SIZE * MATRIX_SIZE] = {1, 2, 3, 4,
    //                                       5, 6, 7, 8,
    //                                       9, 10, 11, 12,
    //                                       13, 14, 15, 16};
    // float B[MATRIX_SIZE * MATRIX_SIZE] = {1, 2, 3, 4,
    //                                       5, 6, 7, 8,
    //                                       9, 10, 11, 12,
    //                                       13, 14, 15, 16};
    // float C[MATRIX_SIZE * MATRIX_SIZE] = {0};

    // native_matrix_multiplication(A, B, C);

    // for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
    //     std::cout << C[i] << " ";
    //     if ((i + 1) % MATRIX_SIZE == 0) {
    //         std::cout << std::endl;
    //     }
    // }
    // end test stuff

    // get user input on data precision
    int dprecision;
    std::cout << "Choose 4-byte Floating Point (1) or 2-byte Fixed Point (2): " << endl;
    cin >> dprecision;


    if (dprecision == 1) {
        // 4-byte floating point data
        float input_matrix1[MATRIX_SIZE * MATRIX_SIZE];
        float input_matrix2[MATRIX_SIZE * MATRIX_SIZE];
        float output_matrix[MATRIX_SIZE * MATRIX_SIZE];


        // build float matrices for native implementation
        // also make empty array for out matrix
        for (uint i = 0; i < (MATRIX_SIZE * MATRIX_SIZE); i++)
        {
            input_matrix1[i] = float(rand()) / (float(RAND_MAX));
            input_matrix2[i] = float(rand()) / (float(RAND_MAX));
            output_matrix[i] = 0;
        }

        // time how long the native implementation takes
        std::chrono::high_resolution_clock::time_point start_native = chrono::high_resolution_clock::now();
        
        // call native implementation function
        native_matrix_multiplication(input_matrix1, input_matrix2, output_matrix);

        // std::cout << "Input Matrix 1" << std::endl;
        // for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        //     std::cout << input_matrix1[i] << " ";
        //     if ((i + 1) % MATRIX_SIZE == 0) {
        //         std::cout << std::endl;
        //     }
        // }

        // std::cout << "Input Matrix 2" << std::endl;
        // for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        //     std::cout << input_matrix2[i] << " ";
        //     if ((i + 1) % MATRIX_SIZE == 0) {
        //         std::cout << std::endl;
        //     }
        // }

        // std::cout << "Output" << std::endl;
        // for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        //     std::cout << output_matrix[i] << " ";
        //     if ((i + 1) % MATRIX_SIZE == 0) {
        //         std::cout << std::endl;
        //     }
        // }
        
        chrono::high_resolution_clock::time_point end_native = chrono::high_resolution_clock::now();
        chrono::duration<double, std::milli> time_native = (end_native - start_native) / 1000;
        cout << "Native implementation took: " << time_native.count() << endl;

        // time SIMD implementation here
        chrono::high_resolution_clock::time_point start_simd = chrono::high_resolution_clock::now(); 

        /////////////////////////////////
        // SOME SIMD FUNCTION CALLED HERE
        /////////////////////////////////

        chrono::high_resolution_clock::time_point end_simd = chrono::high_resolution_clock::now();
        chrono::duration<double, std::milli> time_simd = (end_simd - start_simd) / 1000;
        cout << "SIMD implementation took: " << time_simd.count() << endl;

    } else if (dprecision == 2) {
        // 2-byte fixed point data
        short int input_matrix1[MATRIX_SIZE * MATRIX_SIZE];
        short int input_matrix2[MATRIX_SIZE * MATRIX_SIZE];
        short int output_matrix[MATRIX_SIZE * MATRIX_SIZE];


        // build float matrices for native implementation
        // also make empty array for out matrix
        for (uint i = 0; i < (MATRIX_SIZE * MATRIX_SIZE); i++)
        {
            input_matrix1[i] = (short int)(rand()) / 255;
            input_matrix2[i] = (short int)(rand()) / 255;
            output_matrix[i] = 0;
        }

        // time how long the native implementation takes
        std::chrono::high_resolution_clock::time_point start_native = chrono::high_resolution_clock::now();
        
        // call native implementation function
        native_matrix_multiplication(input_matrix1, input_matrix2, output_matrix);

        // std::cout << "Input Matrix 1" << std::endl;
        // for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        //     std::cout << input_matrix1[i] << " ";
        //     if ((i + 1) % MATRIX_SIZE == 0) {
        //         std::cout << std::endl;
        //     }
        // }

        // std::cout << "Input Matrix 2" << std::endl;
        // for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        //     std::cout << input_matrix2[i] << " ";
        //     if ((i + 1) % MATRIX_SIZE == 0) {
        //         std::cout << std::endl;
        //     }
        // }

        // std::cout << "Output" << std::endl;
        // for (int i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        //     std::cout << output_matrix[i] << " ";
        //     if ((i + 1) % MATRIX_SIZE == 0) {
        //         std::cout << std::endl;
        //     }
        // }
        
        chrono::high_resolution_clock::time_point end_native = chrono::high_resolution_clock::now();
        chrono::duration<double, std::milli> time_native = (end_native - start_native) / 1000;
        cout << "Native implementation took: " << time_native.count() << endl;

        // time SIMD implementation here
        chrono::high_resolution_clock::time_point start_simd = chrono::high_resolution_clock::now(); 

        /////////////////////////////////
        // SOME SIMD FUNCTION CALLED HERE
        /////////////////////////////////

        chrono::high_resolution_clock::time_point end_simd = chrono::high_resolution_clock::now();
        chrono::duration<double, std::milli> time_simd = (end_simd - start_simd) / 1000;
        cout << "SIMD implementation took: " << time_simd.count() << endl;
    }

    return 0;
}
