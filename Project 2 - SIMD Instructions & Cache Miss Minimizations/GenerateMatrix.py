import random

random.seed(0)

def generate_matrix(filename, size, use_float=False):
    with open(filename, 'w') as f:
        f.write(str(size) + '\n')
        for i in range(size):
            row = []
            for j in range(size):
                if use_float:
                    value = round(random.uniform(1.00, 100.00), 2)
                else:
                    value = random.randint(1, 100)
                row.append(str(value))
            f.write(' '.join(row) + '\n')

if __name__ == '__main__':
    # 3x3 matrix
    generate_matrix('MATRIX_3_INT.txt', 3)
    generate_matrix('MATRIX_3_FLOAT.txt', 3, use_float=True)
    print('Matrix 3x3 generated')

    # 100x100 matrix
    generate_matrix('MATRIX_100_INT.txt', 100)
    generate_matrix('MATRIX_100_FLOAT.txt', 100, use_float=True)
    print('Matrix 100x100 generated')

    # 1000x1000 matrix
    generate_matrix('MATRIX_1000_INT.txt', 1000)
    generate_matrix('MATRIX_1000_FLOAT.txt', 1000, use_float=True)
    print('Matrix 1000x1000 generated')