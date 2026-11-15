/**
 * @file
 * @brief
 * A simple program to check if the given number is
 * [Prime](https://en.wikipedia.org/wiki/Primality_test) or not.
 * @details
 * A prime number is any number that can be divided only by itself and 1. It
 * must be positive and a whole number, therefore any prime number is part of
 * the set of natural numbers. The majority of prime numbers are even numbers,
 * with the exception of 2. This algorithm finds prime numbers using this
 * information. additional ways to solve the prime check problem:
 * https://cp-algorithms.com/algebra/primality_tests.html#practice-problems
 * @author [Omkar Langhe](https://github.com/omkarlanghe)
 * @author [ewd00010](https://github.com/ewd00010)
 */

#include <cassert>   /// for assert
#include <iostream>  /// for IO operations
#include <vector>    /// for vector in boundary tests

/**
 * @brief Mathematical algorithms
 * @namespace
 */
namespace math {
/**
 * @brief Function to check if the given number is prime or not.
 *
 * @param num number to be checked (any signed 64-bit integer).
 * @return true if number is prime (>= 2).
 * @return false if number is not prime (including <= 1, negatives, composites).
 *
 * @details
 * Supported boundary table and source contract:
 * | Domain / Boundary Class | Condition | Expected Output | Contract Clause |
 * | :--- | :--- | :--- | :--- |
 * | Negative numbers | `num < 0` | `false` | Primes are natural numbers >= 2 |
 * | Zero | `num == 0` | `false` | Not prime |
 * | Unit boundary | `num == 1` | `false` | Unit is not prime |
 * | Smallest prime | `num == 2` | `true` | Only even prime |
 * | Smallest odd prime | `num == 3` | `true` | Base prime case |
 * | Smallest even composite | `num == 4` | `false` | Divisible by 2 |
 * | Boundary around 9 | `num == 9` | `false` | Divisible by 3 (odd composite) |
 * | Boundary around 11 | `num == 11` | `true` | Prime (6k - 1 where k=2) |
 * | Exclusive sieve upper bound | `[2, 12)` | 2, 3, 5, 7, 11 | Primes strictly less than 12 |
 */
bool is_prime(int64_t num) {
    /*!
     * Reduce all possibilities of a number which cannot be prime with the first
     * 3 if, else if conditionals. Example: Since no even number, except 2 can
     * be a prime number and the next prime we find after our checks is 5,
     * we will start the for loop with i = 5. then for each loop we increment
     * i by +6 and check if i or i+2 is a factor of the number; if it's a factor
     * then we will return false. otherwise, true will be returned after the
     * loop terminates at the terminating condition which is i*i <= num
     */
    if (num <= 1) {
        return false;
    } else if (num == 2 || num == 3) {
        return true;
    } else if (num % 2 == 0 || num % 3 == 0) {
        return false;
    } else {
        for (int64_t i = 5; i * i <= num; i = i + 6) {
            if (num % i == 0 || num % (i + 2) == 0) {
                return false;
            }
        }
    }
    return true;
}
}  // namespace math

/**
 * @brief Self-test implementations covering baseline and boundary cases.
 * @returns void
 */
static void tests() {
    // Baseline checks
    assert(math::is_prime(1) == false);
    assert(math::is_prime(2) == true);
    assert(math::is_prime(3) == true);
    assert(math::is_prime(4) == false);
    assert(math::is_prime(-4) == false);
    assert(math::is_prime(7) == true);
    assert(math::is_prime(-7) == false);
    assert(math::is_prime(19) == true);
    assert(math::is_prime(50) == false);
    assert(math::is_prime(115249) == true);

    // Explicit boundary around 1 (smallest domain boundaries: 0, 1, 2)
    assert(math::is_prime(-1) == false);
    assert(math::is_prime(0) == false);
    assert(math::is_prime(1) == false);
    assert(math::is_prime(2) == true);

    // Explicit boundary around 2 (1, 2, 3, 4)
    assert(math::is_prime(1) == false);
    assert(math::is_prime(2) == true);
    assert(math::is_prime(3) == true);
    assert(math::is_prime(4) == false);

    // Explicit boundary around 9 (8, 9, 10)
    assert(math::is_prime(8) == false);
    assert(math::is_prime(9) == false);
    assert(math::is_prime(10) == false);

    // Explicit boundary around 11 (10, 11, 12)
    assert(math::is_prime(10) == false);
    assert(math::is_prime(11) == true);
    assert(math::is_prime(12) == false);

    // Sieve through 12 respecting exclusive upper-bound interface [2, 12)
    // The prime numbers in this range are strictly: 2, 3, 5, 7, 11
    std::vector<int64_t> primes_up_to_12;
    for (int64_t n = 2; n < 12; ++n) {
        if (math::is_prime(n)) {
            primes_up_to_12.push_back(n);
        }
    }
    std::vector<int64_t> expected_sieve = {2, 3, 5, 7, 11};
    assert(primes_up_to_12 == expected_sieve);

    std::cout << "All is_prime tests passed successfully (25 assertions)!" << std::endl;
}

/**
 * @brief Main function
 * @returns 0 on exit
 */
int main() {
    tests();  // perform self-tests implementations
    return 0;
}
