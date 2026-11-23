/**
 * @file
 * @brief Implements [Palindrome
 * Partitioning](https://leetcode.com/problems/palindrome-partitioning-ii/)
 * algorithm, giving you the minimum number of partitions you need to make
 *
 * @details
 * palindrome partitioning uses dynamic programming and goes to all the possible
 * partitions to find the minimum you are given a string and you need to give
 * minimum number of partitions needed to divide it into a number of palindromes
 * [Palindrome Partitioning]
 * (https://www.geeksforgeeks.org/palindrome-partitioning-dp-17/) overall time
 * complexity O(n^2) For example: example 1:- String : "nitik" Output : 2 => "n
 * | iti | k" For example: example 2:- String : "ababbbabbababa" Output : 3 =>
 * "aba | b | bbabb | ababa"
 * @author [Sujay Kaushik] (https://github.com/sujaykaushik008)
 */

#include <algorithm>  // for std::min
#include <cassert>    // for std::assert
#include <climits>    // for INT_MAX
#include <iostream>   // for io operations
#include <string>     // for std::string
#include <utility>    // for std::pair
#include <vector>     // for std::vector

/**
 * @namespace dynamic_programming
 * @brief Dynamic Programming algorithms
 */
namespace dynamic_programming {

/**
 * @namespace palindrome_partitioning
 * @brief Functions for [Palindrome
 * Partitioning](https://leetcode.com/problems/palindrome-partitioning-ii/)
 * algorithm
 */
namespace palindrome_partitioning {

/**
 * Function implementing palindrome partitioning algorithm using lookup table
 * method. The string is used verbatim (case-sensitive, with no normalization);
 * an empty or already-palindromic string requires zero cuts.
 * @param str input string
 * @returns minimum number of cuts needed to partition str into palindromes
 */
int pal_part(const std::string &str) {
    int n = str.size();

    if (n == 0) {
        return 0;
    }

    // creating lookup table for minimum number of cuts
    std::vector<std::vector<int> > cuts(n, std::vector<int>(n, 0));

    // creating lookup table for palindrome checking
    std::vector<std::vector<bool> > is_palindrome(n,
                                                  std::vector<bool>(n, false));

    // initialization
    for (int i = 0; i < n; i++) {
        is_palindrome[i][i] = true;
        cuts[i][i] = 0;
    }

    for (int len = 2; len <= n; len++) {
        for (int start_index = 0; start_index < n - len + 1; start_index++) {
            int end_index = start_index + len - 1;

            if (len == 2) {
                is_palindrome[start_index][end_index] =
                    (str[start_index] == str[end_index]);
            } else {
                is_palindrome[start_index][end_index] =
                    (str[start_index] == str[end_index]) &&
                    is_palindrome[start_index + 1][end_index - 1];
            }

            if (is_palindrome[start_index][end_index]) {
                cuts[start_index][end_index] = 0;
            } else {
                cuts[start_index][end_index] = INT_MAX;
                for (int partition = start_index; partition <= end_index - 1;
                     partition++) {
                    cuts[start_index][end_index] =
                        std::min(cuts[start_index][end_index],
                                 cuts[start_index][partition] +
                                     cuts[partition + 1][end_index] + 1);
                }
            }
        }
    }

    return cuts[0][n - 1];
}
}  // namespace palindrome_partitioning
}  // namespace dynamic_programming

/**
 * @brief Test Function
 * @return void
 */
static void test() {
    const std::vector<std::pair<std::string, int>> cases = {
        {"", 0}, {"a", 0}, {"aba", 0}, {"ab", 1}, {"abba", 0},
        {"nitik", 2}, {"ababbbabbababa", 3}, {"abdc", 3}};

    for (const auto &test_case : cases) {
        const int actual =
            dynamic_programming::palindrome_partitioning::pal_part(
                test_case.first);
        std::cout << "{\"input\":\"" << test_case.first
                  << "\",\"expected\":" << test_case.second
                  << ",\"actual\":" << actual << "}\n";
        assert(test_case.second == actual);
    }
}

/**
 * @brief Main function
 * @returns 0 on exit
 */
int main() {
    test();  // execute the test
    return 0;
}
