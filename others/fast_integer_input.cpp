/**
 * @file
 * @brief Read integers from stdin continuously as they are entered without
 * waiting for the `\n` character
 */
#include <cstdio>
#include <iostream>

/**
 * Read one signed decimal integer from the current stdin position. The input
 * must begin with an optional '-' followed by at least one digit, fit in int,
 * and end with a non-digit (or EOF); whitespace and '+' are not skipped.
 *
 * Worked examples (the accumulator is updated once per digit):
 * - Input `0\n`: digit `0` changes 0 to 0; newline stops the loop; result 0.
 * - Input `-2048\n`: '-' records the sign; digits give 2 → 20 → 204 → 2048;
 *   newline stops the loop and applying the sign returns -2048.
 *
 * @param number receives the parsed integer
 */
void fastinput(int *number) {
    // variable to indicate sign of input integer
    bool negative = false;
    int c;
    *number = 0;

    // extract current character from buffer
    c = std::getchar();
    if (c == '-') {
        // number is negative
        negative = true;

        // extract the next character from the buffer
        c = std::getchar();
    }

    // Keep on extracting characters if they are integers
    // i.e ASCII Value lies from '0'(48) to '9' (57)
    for (; (c > 47 && c < 58); c = std::getchar())
        *number = *number * 10 + c - 48;

    // if scanned input has a negative sign, negate the
    // value of the input number
    if (negative)
        *(number) *= -1;
}

/** Main function */
int main() {
    int number;
    fastinput(&number);
    std::cout << number << std::endl;
    return 0;
}
