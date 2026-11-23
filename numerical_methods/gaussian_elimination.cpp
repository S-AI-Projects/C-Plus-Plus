/**
 * \file
 * \brief [Gaussian elimination
 * method](https://en.wikipedia.org/wiki/Gaussian_elimination)
 */
#include <cassert>   /// for assert
#include <iostream>  /// for IO operations

/**
 * @brief Forward elimination: turn the augmented matrix into upper-triangular
 * form, in place
 * @details For each pivot row `step`, every row below it has
 * `a = mat[row][step] / mat[step][step]` times the pivot row subtracted, which
 * makes `mat[row][step]` zero. Columns `step..mat_size` are updated; the last
 * column is the right-hand side. There is no row swapping, so every pivot
 * `mat[step][step]` must be non-zero when it is used.
 * @param mat augmented matrix with `mat_size` rows and `mat_size + 1` columns
 * @param mat_size number of equations
 */
void forward_elimination(double **mat, int mat_size) {
    for (int step = 0; step < mat_size - 1; step++) {
        for (int row = step + 1; row < mat_size; row++) {
            double a = (mat[row][step] / mat[step][step]);

            for (int j = step; j <= mat_size; j++)
                mat[row][j] = mat[row][j] - (a * mat[step][j]);
        }
    }
}

/**
 * @brief Tests for forward_elimination() with hand-calculated results
 * @details Preconditions, from main(): `mat_size >= 1` (line `std::cin >>
 * mat_size;` sizes every allocation), the matrix is augmented with
 * `mat_size + 1` columns (the input loop `j <= mat_size`), and each pivot is
 * non-zero (the division `mat[row][step] / mat[step][step]`).
 */
static void test() {
    int count = 0;

    // smallest valid input, 1 equation: 2x = 6. Nothing to eliminate.
    double r1[] = {2, 6};
    double *m1[] = {r1};
    forward_elimination(m1, 1);
    assert(r1[0] == 2 && r1[1] == 6);
    count += 1;

    // ordinary input, 2 equations: 2x + y = 5, x - y = 1 (x = 2, y = 1).
    // a = 1 / 2, row 1 becomes {1 - 1, -1 - 0.5, 1 - 2.5} = {0, -1.5, -1.5}.
    double a0[] = {2, 1, 5}, a1[] = {1, -1, 1};
    double *m2[] = {a0, a1};
    forward_elimination(m2, 2);
    assert(a0[0] == 2 && a0[1] == 1 && a0[2] == 5);  // pivot row unchanged
    assert(a1[0] == 0 && a1[1] == -1.5 && a1[2] == -1.5);
    count += 2;

    // ordinary input, 3 equations: x + y + z = 6, 2y + 5z = -4,
    // 2x + 5y - z = 27 (x = 5, y = 3, z = -2).
    // step 0: row 1 unchanged (a = 0); row 2 -= 2 * row 0 -> {0, 3, -3, 15}
    // step 1: row 2 -= 1.5 * row 1 -> {0, 0, -10.5, 21}
    double b0[] = {1, 1, 1, 6}, b1[] = {0, 2, 5, -4}, b2[] = {2, 5, -1, 27};
    double *m3[] = {b0, b1, b2};
    forward_elimination(m3, 3);
    assert(b0[0] == 1 && b0[1] == 1 && b0[2] == 1 && b0[3] == 6);
    assert(b1[0] == 0 && b1[1] == 2 && b1[2] == 5 && b1[3] == -4);
    assert(b2[0] == 0 && b2[1] == 0 && b2[2] == -10.5 && b2[3] == 21);
    count += 3;

    std::cerr << "[gaussian_elimination] " << count << " checks passed"
              << std::endl;
}

/** Main function */
int main() {
    test();  // run self-test implementations

    int mat_size, i, j;

    std::cout << "Matrix size: ";
    std::cin >> mat_size;

    // create a 2D matrix by dynamic memory allocation
    double **mat = new double *[mat_size + 1], **x = new double *[mat_size];
    for (i = 0; i <= mat_size; i++) {
        mat[i] = new double[mat_size + 1];
        if (i < mat_size)
            x[i] = new double[mat_size + 1];
    }

    // get the matrix elements from user
    std::cout << std::endl << "Enter value of the matrix: " << std::endl;
    for (i = 0; i < mat_size; i++) {
        for (j = 0; j <= mat_size; j++) {
            std::cin >>
                mat[i][j];  // Enter (mat_size*mat_size) value of the matrix.
        }
    }

    // perform Gaussian elimination
    forward_elimination(mat, mat_size);

    std::cout << std::endl
              << "Matrix using Gaussian Elimination method: " << std::endl;
    for (i = 0; i < mat_size; i++) {
        for (j = 0; j <= mat_size; j++) {
            x[i][j] = mat[i][j];
            std::cout << mat[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl
              << "Value of the Gaussian Elimination method: " << std::endl;
    for (i = mat_size - 1; i >= 0; i--) {
        double sum = 0;
        for (j = mat_size - 1; j > i; j--) {
            x[i][j] = x[j][j] * x[i][j];
            sum = x[i][j] + sum;
        }
        if (x[i][i] == 0)
            x[i][i] = 0;
        else
            x[i][i] = (x[i][mat_size] - sum) / (x[i][i]);

        std::cout << "x" << i << "= " << x[i][i] << std::endl;
    }

    for (i = 0; i <= mat_size; i++) {
        delete[] mat[i];
        if (i < mat_size)
            delete[] x[i];
    }
    delete[] mat;
    delete[] x;

    return 0;
}
