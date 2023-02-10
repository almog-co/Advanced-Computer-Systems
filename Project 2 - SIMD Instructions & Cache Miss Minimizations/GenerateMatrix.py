import random

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
    #generate_matrix('MATRIX_3_INT.txt', 3)
    #generate_matrix('MATRIX_3_FLOAT.txt', 3, use_float=True)

    #generate_matrix('MATRIX_1000_INT.txt', 1000)
    #generate_matrix('MATRIX_1000_FLOAT.txt', 1000, use_float=True)

    generate_matrix('MATRIX_10000_INT.txt', 10000)
    generate_matrix('MATRIX_10000_FLOAT.txt', 10000, use_float=True)