#ifndef MATRIX_H
#define MATRIX_H

// =================================
// Includes
// =================================

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cmath>

using namespace std;

// =================================
// Matrix Class Definition
// =================================

template<typename T>
class Matrix {
    
private:
    int max_digits() {
        int max_digits = 0;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                int digits = floor(log10(abs(matrix[i][j]))) + 1;
                max_digits = max(max_digits, digits);
            }
        }
        return max_digits;
    }

    int round_to_nearest_multiple_of_8(int n) {
        int result = n;
        if (n % 8 != 0) {
            result = ((n / 8) + 1) * 8;
        }
        return result;
    }

public:
    int size;
    int padding_size;
    T** matrix;
    T** matrix_by_col;

    // Constructor for matrix of size n
    // Initializes all values to 0
    Matrix(const int n) {
        // Padding to multiple of 8
        this->padding_size = round_to_nearest_multiple_of_8(n) - n;
        this->size = n + padding_size;

        matrix = new T * [size];
        matrix_by_col = new T * [size];
        for (int i = 0; i < size; i++) {
            matrix[i] = new T[size];
            matrix_by_col[i] = new T[size];
            for (int j = 0; j < size; j++) {
                matrix[i][j] = 0;
                matrix_by_col[i][j] = 0;
            }
        }
    }

    // Constructor for matrix from file
    // First line of file is the size of the matrix
    Matrix(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error: File not found" << endl;
            exit(1);
        }
        file >> size;
        padding_size = round_to_nearest_multiple_of_8(size) - size;
        size += padding_size;

        // Set everything to 0 at first
        matrix = new T * [size];
        matrix_by_col = new T * [size];
        for (int i = 0; i < size; i++) {
            matrix[i] = new T[size];
            matrix_by_col[i] = new T[size];
            for (int j = 0; j < size; j++) {
                matrix[i][j] = 0;
                matrix_by_col[i][j] = 0;
            }
        }

        // Read in values from file
        for (int i = 0; i < size - padding_size; i++) {
            for (int j = 0; j < size - padding_size; j++) {
                file >> matrix[i][j];
                matrix_by_col[j][i] = matrix[i][j];
            }
        }

    }

    // Destructor for matrix
    ~Matrix() {
        for (int i = 0; i < size; i++) {
            delete[] matrix[i];
            delete[] matrix_by_col[i];
        }
        delete[] matrix;
        delete[] matrix_by_col;
    }

    // Print without padding
    void print() {
        int digits = max_digits();
        for (int i = 0; i < size - padding_size; i++) {
            for (int j = 0; j < size - padding_size; j++) {
                cout << setw(digits) << matrix[i][j] << " ";
            }
            cout << endl;
        }
    }

    // Print with padding
    void print_with_padding() {
        int digits = max_digits();
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                cout << setw(digits) << matrix[i][j] << " ";
            }
            cout << endl;
        }
    }
};

#endif