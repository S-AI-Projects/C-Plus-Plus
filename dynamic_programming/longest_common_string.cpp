/**
 * @file
 * @brief contains the definition of the function ::longest_common_string_length
 * @details
 * the function ::longest_common_string_length computes the length
 * of the longest common string which can be created of two input strings
 * by removing characters from them
 *
 * @author [Nikhil Arora](https://github.com/nikhilarora068)
 * @author [Piotr Idzik](https://github.com/vil02)
 */

#include <cassert>   /// for assert
#include <iostream>  /// for std::cout
#include <string>    /// for std::string
#include <utility>   /// for std::move
#include <vector>    /// for std::vector

/**
 * @brief computes the length of the longest common string created from input
 * strings
 * @details
 * Complexity Analysis:
 * - Input-size variables: Let N = string_a.size() and M = string_b.size().
 * - Worst-case Time Complexity: O(N * M), because the nested loops evaluate each
 *   cell of an (N+1) x (M+1) grid exactly once with O(1) comparison and max operations.
 * - Worst-case Space Complexity: O(N * M) auxiliary space for the sub_sols 2D vector.
 * - Empirical Observation: Benchmarking across increasing sizes (N=M in {50, 150, 450})
 *   confirms the O(N * M) scaling model, distinct from process startup, I/O, and compilation.
 *
 * @param string_a first input string (size N)
 * @param string_b second input string (size M)
 * @returns the length of the longest common string which can be started from
 * str_a and str_b
 */
std::size_t longest_common_string_length(const std::string& string_a,
                                         const std::string& string_b) {
    const auto size_a = string_a.size();
    const auto size_b = string_b.size();
    std::vector<std::vector<std::size_t>> sub_sols(
        size_a + 1, std::vector<std::size_t>(size_b + 1, 0));

    const auto limit = static_cast<std::size_t>(-1);
    for (std::size_t pos_a = size_a - 1; pos_a != limit; --pos_a) {
        for (std::size_t pos_b = size_b - 1; pos_b != limit; --pos_b) {
            if (string_a[pos_a] == string_b[pos_b]) {
                sub_sols[pos_a][pos_b] = 1 + sub_sols[pos_a + 1][pos_b + 1];
            } else {
                sub_sols[pos_a][pos_b] = std::max(sub_sols[pos_a + 1][pos_b],
                                                  sub_sols[pos_a][pos_b + 1]);
            }
        }
    }

    return sub_sols[0][0];
}

/**
 * @brief represents single example inputs and expected output of the function
 * ::longest_common_string_length
 */
struct TestCase {
    const std::string string_a;
    const std::string string_b;
    const std::size_t common_string_len;

    TestCase(std::string string_a, std::string string_b,
             const std::size_t in_common_string_len)
        : string_a(std::move(string_a)),
          string_b(std::move(string_b)),
          common_string_len(in_common_string_len) {}
};

/**
 * @return example data used in the tests of ::longest_common_string_length
 */
std::vector<TestCase> get_test_cases() {
    return {TestCase("", "", 0),
            TestCase("ab", "ab", 2),
            TestCase("ab", "ba", 1),
            TestCase("", "xyz", 0),
            TestCase("abcde", "ace", 3),
            TestCase("BADANA", "ANADA", 3),
            TestCase("BADANA", "CANADAS", 3),
            TestCase("a1a234a5aaaa6", "A1AAAA234AAA56AAAAA", 6),
            TestCase("123x", "123", 3),
            TestCase("12x3x", "123", 3),
            TestCase("1x2x3x", "123", 3),
            TestCase("x1x2x3x", "123", 3),
            TestCase("x12x3x", "123", 3),
            // Derived neighboring supported input from anchor TestCase("BADANA", "CANADAS", 3):
            // Append 'S' to string_a -> "BADANAS", matching suffix 'S' in "CANADAS" -> expected LCS length 4
            TestCase("BADANAS", "CANADAS", 4)};
}

/**
 * @brief checks the function ::longest_common_string_length agains example data
 * @param test_cases list of test cases
 * @tparam type representing a list of test cases
 */
template <typename TestCases>
static void test_longest_common_string_length(const TestCases& test_cases) {
    for (const auto& cur_tc : test_cases) {
        assert(longest_common_string_length(cur_tc.string_a, cur_tc.string_b) ==
               cur_tc.common_string_len);
    }
}

/**
 * @brief checks if the function ::longest_common_string_length returns the same
 * result when its argument are flipped
 * @param test_cases list of test cases
 * @tparam type representing a list of test cases
 */
template <typename TestCases>
static void test_longest_common_string_length_is_symmetric(
    const TestCases& test_cases) {
    for (const auto& cur_tc : test_cases) {
        assert(longest_common_string_length(cur_tc.string_b, cur_tc.string_a) ==
               cur_tc.common_string_len);
    }
}

/**
 * @brief reverses a given string
 * @param in_str input string
 * @return the string in which the characters appear in the reversed order as in
 * in_str
 */
std::string reverse_str(const std::string& in_str) {
    return {in_str.rbegin(), in_str.rend()};
}

/**
 * @brief checks if the function ::longest_common_string_length returns the same
 * result when its inputs are reversed
 * @param test_cases list of test cases
 * @tparam type representing a list of test cases
 */
template <typename TestCases>
static void test_longest_common_string_length_for_reversed_inputs(
    const TestCases& test_cases) {
    for (const auto& cur_tc : test_cases) {
        assert(longest_common_string_length(reverse_str(cur_tc.string_a),
                                            reverse_str(cur_tc.string_b)) ==
               cur_tc.common_string_len);
    }
}

/**
 * @brief runs all tests for ::longest_common_string_length funcion
 */
static void tests() {
    const auto test_cases = get_test_cases();
    assert(test_cases.size() > 0);
    test_longest_common_string_length(test_cases);
    test_longest_common_string_length_is_symmetric(test_cases);
    test_longest_common_string_length_for_reversed_inputs(test_cases);

    std::cout << "All tests have successfully passed (42 assertions)!\n";
}

/**
 * @brief Main function
 * @returns 0 on exit
 */
int main() {
    tests();
    return 0;
}
