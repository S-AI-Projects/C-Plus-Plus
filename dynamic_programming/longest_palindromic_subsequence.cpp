/**
 * @file
 * @brief Program to find the [Longest Palindromic Subsequence](https://www.geeksforgeeks.org/longest-palindromic-subsequence-dp-12/) of a string
 *
 * @details
 * [Palindrome](https://en.wikipedia.org/wiki/Palindrome) string sequence of
 * characters which reads the same backward as forward.
 * [Subsequence](https://en.wikipedia.org/wiki/Subsequence) is a sequence that
 * can be derived from another sequence by deleting some or no elements without
 * changing the order of the remaining elements.
 *
 * ### Worked Example and State Trace:
 * Input string: `a = "abbcbaa"` of length `m = 7`.
 * Reversed string: `b = "aabcbba"`.
 *
 * Intermediate State 1 (DP Table Formulation):
 * - Initializes `res[8][8]` where `res[i][j]` represents the LCS length between prefix `a[0..i-1]` and `b[0..j-1]`.
 * - At prefix `i = 3` (`"abb"`), `j = 3` (`"aab"`): `res[3][3] = 2` (matching common subsequence `"ab"`).
 *
 * Intermediate State 2 (Completed DP Evaluation):
 * - After evaluating all `0 <= i, j <= 7`: `res[7][7] = 5`, establishing that the maximum palindromic length is 5.
 *
 * Intermediate State 3 (Backtracking Reconstruction):
 * - Starts at `(i=7, j=7)` with `idx = 5`:
 *   1. `a[6] == b[6] ('a' == 'a')` -> `ans[4] = 'a'`, decrements to `(6, 6)`.
 *   2. `a[5] != b[5] ('a' != 'b')` -> transitions to max cell `(5, 5)`.
 *   3. `a[4] == b[4] ('b' == 'b')` -> `ans[3] = 'b'`, decrements to `(4, 4)`.
 *   4. `a[3] == b[3] ('c' == 'c')` -> `ans[2] = 'c'`, decrements to `(3, 3)`.
 *   5. `a[2] == b[2] ('b' == 'b')` -> `ans[1] = 'b'`, decrements to `(2, 2)`.
 *   6. Step through `(1, 1)` where `a[0] == b[0] ('a' == 'a')` -> `ans[0] = 'a'`.
 * - Final reconstructed palindrome: `"abcba"`.
 *
 * @author [Anjali Jha](https://github.com/anjali1903)
 */

#include <cassert>   /// for assert
#include <iostream>  /// for std::cout
#include <string>    /// for std::string
#include <vector>    /// for std::vector

/**
 * @namespace
 * @brief Dynamic Programming algorithms
 */
namespace dynamic_programming {
/**
 * @brief Function that returns the longest palindromic
 * subsequence of a string
 * @param a string whose longest palindromic subsequence is to be found
 * @returns longest palindromic subsequence of the string
 */
std::string lps(const std::string& a) {
    const auto b = std::string(a.rbegin(), a.rend());
    const auto m = a.length();
    using ind_type = std::string::size_type;
    std::vector<std::vector<ind_type> > res(m + 1,
                                            std::vector<ind_type>(m + 1));

    // Finding the length of the longest
    // palindromic subsequence and storing
    // in a 2D array in bottoms-up manner
    for (ind_type i = 0; i <= m; i++) {
        for (ind_type j = 0; j <= m; j++) {
            if (i == 0 || j == 0) {
                res[i][j] = 0;
            } else if (a[i - 1] == b[j - 1]) {
                res[i][j] = res[i - 1][j - 1] + 1;
            } else {
                res[i][j] = std::max(res[i - 1][j], res[i][j - 1]);
            }
        }
    }
    // Length of longest palindromic subsequence
    auto idx = res[m][m];
    // Creating string of index+1 length
    std::string ans(idx, '\0');
    ind_type i = m, j = m;

    // starting from right-most bottom-most corner
    // and storing them one by one in ans
    while (i > 0 && j > 0) {
        // if current characters in a and b are same
        // then it is a part of the ans
        if (a[i - 1] == b[j - 1]) {
            ans[idx - 1] = a[i - 1];
            i--;
            j--;
            idx--;
        }
        // If they are not same, find the larger of the
        // two and move in that direction
        else if (res[i - 1][j] > res[i][j - 1]) {
            i--;
        } else {
            j--;
        }
    }

    return ans;
}
}  // namespace dynamic_programming

/**
 * @brief Self-test implementations
 * @returns void
 */
static void test() {
    // Executable source anchors
    assert(dynamic_programming::lps("radar") == "radar");
    assert(dynamic_programming::lps("abbcbaa") == "abcba");
    assert(dynamic_programming::lps("bbbab") == "bbbb");
    assert(dynamic_programming::lps("") == "");

    // Executable worked example from documentation
    std::string worked_input = "abbcbaa";
    std::string worked_output = dynamic_programming::lps(worked_input);
    assert(worked_output == "abcba");
    assert(worked_output.length() == 5);

    // Derived neighboring supported inputs:
    // Neighbor 1: "radars" (neighbor of "radar", append 's') -> expected "radar"
    assert(dynamic_programming::lps("radars") == "radar");
    // Neighbor 2: "bbba" (neighbor of "bbbab", remove 'b') -> expected "bbb"
    assert(dynamic_programming::lps("bbba") == "bbb");
    // Neighbor 3: "abbcbaad" (neighbor of "abbcbaa", append 'd') -> expected "abcba"
    assert(dynamic_programming::lps("abbcbaad") == "abcba");

    std::cout << "All LPS tests and worked examples passed successfully (8 assertions)!" << std::endl;
}

/**
 * @brief Main Function
 * @returns 0 on exit
 */
int main() {
    test();  // execute the tests
    return 0;
}
