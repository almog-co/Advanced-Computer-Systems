/**
 * This program can read in matrices from a file and perform matrix multiplication on them.
 * It will test the performance of the naive matrix multiplication algorithm and the SIMD
 * matrix multiplication algorithm.
 * 
 * @author Almog Cohen
 * @author Rojan Karn
 * @date 02/10/2023
 */

// =================================
// Includes
// =================================

#include "Matrix.h"
#include <iostream>
#include <immintrin.h>
#include <chrono>
#include <string>

using namespace std;

// =================================
// Function Declarations
// =================================

template <typename T>
Matrix<T> naive_matrix_multiplication(const Matrix<T>& A, const Matrix<T>& B);

template <typename T>
Matrix<T> simd_matrix_multiplication(const Matrix<T>& A, const Matrix<T>& B);

void print_m256(__m256 a);

void parse_args(int argc, char** argv, string& matrix_a_filename, string& matrix_b_filename, bool& verbose);

// =================================
// Main Function
// =================================

int main(int argc, char** argv) {
    string matrix_a_filename;
    string matrix_b_filename;
    bool verbose;

    // Parse the command line arguments
    parse_args(argc, argv, matrix_a_filename, matrix_b_filename, verbose);
    
    // Read in the matrices from the files
    Matrix<float> A(matrix_a_filename);
    Matrix<float> B(matrix_b_filename);

    // Start timer for simd matrix multiplication
    auto start = chrono::high_resolution_clock::now();
    
    // Perform the matrix multiplication
    Matrix<float> C = simd_matrix_multiplication(A, B);

    // Duration for simd matrix multiplication
    auto duration = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start);

    // Print the results
    cout << "SIMD Matrix Multiplication Duration: " << duration.count() << " milliseconds" << endl;
    if (verbose) {
        cout << "SIMD Matrix Multiplication Result:" << endl;
        C.print();
    }
    cout << endl;

    // Start timer for naive matrix multiplication
    start = chrono::high_resolution_clock::now();

    // Perform the matrix multiplication
    Matrix<float> D = naive_matrix_multiplication(A, B);

    // Duration for naive matrix multiplication
    duration = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start);

    // Print the results
    cout << "Naive Matrix Multiplication Duration: " << duration.count() << " milliseconds" << endl;
    if (verbose) {
        cout << "Naive Matrix Multiplication Result:" << endl;
        D.print();
    }

}

// =================================
// Function Definitions
// =================================

// Helper function to print the values of an __m256 variable
void print_m256(__m256 a) {
    float* p = (float*)&a;
    for (int i = 0; i < 8; i++) {
        cout << p[i] << " ";
    }
    cout << endl;
}

// Naive matrix multiplication algorithm
template <typename T>
Matrix<T> naive_matrix_multiplication(const Matrix<T>& A, const Matrix<T>& B) {
    // Get the actual size of the matrix (without padding)
    int size = A.size - A.padding_size;

    // Create the matrix to store the multiplication result
    Matrix<T> C(size);

    // Go through each row of A and column of B
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {

            // Multiply the values in row i of A and column j of B
            T sum = 0;
            for (int k = 0; k < size; k++) {
                sum += A.matrix[i][k] * B.matrix[k][j];
            }

            // Store the sum of the products of the values
            C.matrix[i][j] = sum;
        }
    }

    return C;
}

// SIMD matrix multiplication algorithm
template <typename T>
Matrix<T> simd_matrix_multiplication(const Matrix<T>& A, const Matrix<T>& B) {
    // Get the actual size of the matrix (without padding)
    int size = A.size - A.padding_size;
    
    // Create the matrix to store the multiplication result
    Matrix<T> C(size);

    // Go through each row of A and column of B
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {

            // Creates a variable that can store 8 T values
            __m256 sum = _mm256_setzero_ps();

            // In groups of 8, multiply the values in row i of A and column j of B
            for (int k = 0; k < size; k += 8) {

                // Get 8 values from row i of A and column j of B
                __m256 a = _mm256_loadu_ps(&A.matrix[i][k]);
                __m256 b = _mm256_loadu_ps(&B.matrix_by_col[j][k]);

                // Sum the products of the 8 values
                sum = _mm256_add_ps(sum, _mm256_mul_ps(a, b));
            }
            // Store the sum of the products of the 8 valyes
            T result[8];
            _mm256_storeu_ps(result, sum);
            C.matrix[i][j] = result[0] + result[1] + result[2] + result[3] + result[4] + result[5] + result[6] + result[7];
        }
    }

    return C;
}

void parse_args(int argc, char** argv, string& matrix_a_filename, string& matrix_b_filename, bool& verbose) {
    // Check if the number of arguments is correct
    if (argc != 3 && argc != 4) {
        cout << "Usage: ./MatrixMultiplication <matrix_a_filename> <matrix_b_filename> [-print]" << endl;
        exit(1);
    }

    // Set the default values
    verbose = false;
    matrix_a_filename = "";
    matrix_b_filename = "";
    int verbose_position = -1;

    // Check if the print flag is set
    if (argc == 4) {
        for (int i = 1; i < 4; i++) {
            if (strcmp(argv[i], "-print") == 0) {
                verbose = true;
                verbose_position = i;
            }
        }
        // If we didn't find the print flag, then the arguments are invalid
        if (!verbose) {
            cout << "Usage: ./MatrixMultiplication <matrix_a_filename> <matrix_b_filename> [-print]" << endl;
            exit(1);
        }

        // Get the filenames of the matrices
        for (int i = 1; i < 4; i++) {
            if (i == verbose_position) continue;
            if (matrix_a_filename == "") {
                matrix_a_filename = argv[i];
            } else {
                matrix_b_filename = argv[i];
            }
        }
    } else {
        verbose = false;
        matrix_a_filename = argv[1];
        matrix_b_filename = argv[2];
    }

    // Check if files exist
    ifstream matrix_a_file(matrix_a_filename);
    ifstream matrix_b_file(matrix_b_filename);
    if (!matrix_a_file.good() || !matrix_b_file.good()) {
        cout << "Error: One or more of the files does not exist." << endl;
        exit(1);
    }

    // Close the files
    matrix_a_file.close();
    matrix_b_file.close();
}