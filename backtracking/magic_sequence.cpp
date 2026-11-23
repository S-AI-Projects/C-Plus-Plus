/*
 * @brief [Magic sequence](https://www.csplib.org/Problems/prob019/)
 * implementation
 *
 * @details Solve the magic sequence problem with backtracking
 *
 * "A magic sequence of length $n$ is a sequence of integers $x_0
 * \ldots x_{n-1}$ between $0$ and $n-1$, such that for all $i$
 * in $0$ to $n-1$, the number $i$ occurs exactly $x_i$ times in
 * the sequence. For instance, $6,2,1,0,0,0,1,0,0,0$ is a magic
 * sequence since $0$ occurs $6$ times in it, $1$ occurs twice, etc."
 * Quote taken from the [CSPLib](https://www.csplib.org/Problems/prob019/)
 * website
 *
 * @author [Jxtopher](https://github.com/Jxtopher)
 */

#include <algorithm>  /// for std::count
#include <cassert>    /// for assert
#include <iostream>   /// for IO operations
#include <list>       /// for std::list
#include <numeric>    /// for std::accumulate
#include <sstream>    /// for std::ostringstream
#include <string>     /// for std::string
#include <vector>     /// for std::vector

/**
 * @namespace backtracking
 * @brief Backtracking algorithms
 */
namespace backtracking {
/**
 * @namespace magic_sequence
 * @brief Functions for the [Magic
 * sequence](https://www.csplib.org/Problems/prob019/) implementation
 */
namespace magic_sequence {
using sequence_t =
    std::vector<unsigned int>;  ///< Definition of the sequence type
/**
 * @brief Print the magic sequence
 * @param s working memory for the sequence
 */
void print(const sequence_t& s) {
    for (const auto& item : s) std::cout << item << " ";
    std::cout << std::endl;
}

/**
 * @brief Check if the sequence is magic
 * @param s working memory for the sequence
 * @returns true if it's a magic sequence
 * @returns false if it's NOT a magic sequence
 */
bool is_magic(const sequence_t& s) {
    for (unsigned int i = 0; i < s.size(); i++) {
        if (std::count(s.cbegin(), s.cend(), i) != s[i]) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Sub-solutions filtering
 * @param s working memory for the sequence
 * @param depth current depth in tree
 * @returns true if the sub-solution is valid
 * @returns false if the sub-solution is NOT valid
 */
bool filtering(const sequence_t& s, unsigned int depth) {
    return std::accumulate(s.cbegin(), s.cbegin() + depth,
                           static_cast<unsigned int>(0)) <= s.size();
}

/**
 * @brief Solve the Magic Sequence problem
 * @param s working memory for the sequence
 * @param ret list of the valid magic sequences
 * @param depth current depth in the tree
 */
void solve(sequence_t* s, std::list<sequence_t>* ret, unsigned int depth = 0) {
    if (depth == s->size()) {
        if (is_magic(*s)) {
            ret->push_back(*s);
        }
    } else {
        for (unsigned int i = 0; i < s->size(); i++) {
            (*s)[depth] = i;
            if (filtering(*s, depth + 1)) {
                solve(s, ret, depth + 1);
            }
        }
    }
}

}  // namespace magic_sequence
}  // namespace backtracking

/**
 * @brief Capture what print() writes to std::cout
 * @param s sequence to print
 * @returns the exact text print() wrote
 */
static std::string capture_print(
    const backtracking::magic_sequence::sequence_t& s) {
    std::ostringstream buffer;
    std::streambuf* old_buf = std::cout.rdbuf(buffer.rdbuf());
    backtracking::magic_sequence::print(s);
    std::cout.rdbuf(old_buf);
    return buffer.str();
}

/**
 * @brief Make a newline visible in the test report
 * @param text text to show
 * @returns the text with each newline written as a backslash and n
 */
static std::string visible(const std::string& text) {
    std::string out;
    for (const char c : text) {
        if (c == '\n') {
            out += "\\n";
        } else {
            out += c;
        }
    }
    return out;
}

/**
 * @brief Regression check for print(): compares the exact printed text and
 * checks separately that the input sequence was not modified
 * @param name label of the case
 * @param s input sequence
 * @param expected text worked out by hand before running the code
 * @param count number of checks run so far, increased on each assertion
 */
static void check_print(const std::string& name,
                        const backtracking::magic_sequence::sequence_t& s,
                        const std::string& expected, int* count) {
    const backtracking::magic_sequence::sequence_t before = s;
    const std::string actual = capture_print(s);
    std::cerr << "[print] " << name << ": expected \"" << visible(expected)
              << "\" actual \"" << visible(actual) << "\"" << std::endl;
    // returned value: the exact text written to std::cout
    assert(actual == expected);
    ++*count;
    // observable mutation: print takes a const reference, the input must be
    // unchanged afterwards
    assert(s == before);
    ++*count;
}

/**
 * @brief Self-test implementations
 * @returns void
 */
static void test() {
    int count = 0;

    // test a valid magic sequence
    backtracking::magic_sequence::sequence_t s_magic = {6, 2, 1, 0, 0,
                                                        0, 1, 0, 0, 0};
    assert(backtracking::magic_sequence::is_magic(s_magic));
    ++count;

    // test a non-valid magic sequence
    backtracking::magic_sequence::sequence_t s_not_magic = {5, 2, 1, 0, 0,
                                                            0, 1, 0, 0, 0};
    assert(!backtracking::magic_sequence::is_magic(s_not_magic));
    ++count;

    // print() contract: every value followed by one space, then a newline

    // ordinary input: the fixture magic sequence (n = 10)
    check_print("ordinary n=10 fixture", s_magic, "6 2 1 0 0 0 1 0 0 0 \n",
                &count);

    // non-magic input is printed the same way: print() does not validate
    check_print("non-magic fixture", s_not_magic, "5 2 1 0 0 0 1 0 0 0 \n",
                &count);

    // neighbouring supported input (n = 11), worked out by hand:
    // 0 appears 7 times, 1 twice, 2 once, 7 once, everything else 0 times
    backtracking::magic_sequence::sequence_t s_neighbour = {7, 2, 1, 0, 0, 0,
                                                            0, 1, 0, 0, 0};
    assert(backtracking::magic_sequence::is_magic(s_neighbour));
    ++count;
    check_print("neighbour n=11", s_neighbour, "7 2 1 0 0 0 0 1 0 0 0 \n",
                &count);

    // smallest input: an empty sequence prints only the newline
    check_print("smallest (empty)", {}, "\n", &count);

    // smallest non-empty input: one value
    check_print("single value", {0}, "0 \n", &count);

    // repeated-value boundary: identical values are all printed, none merged
    check_print("repeated values", {3, 3, 3, 3}, "3 3 3 3 \n", &count);

    std::cerr << "[print] " << count << " checks passed" << std::endl;
}

/**
 * @brief Main function
 * @returns 0 on exit
 */
int main() {
    test();  // run self-test implementations

    // solve magic sequences of size 2 to 11 and print the solutions
    for (unsigned int i = 2; i < 12; i++) {
        std::cout << "Solution for n = " << i << std::endl;
        // valid magic sequence list
        std::list<backtracking::magic_sequence::sequence_t> list_of_solutions;
        // initialization of a sequence
        backtracking::magic_sequence::sequence_t s1(i, i);
        // launch of solving the problem
        backtracking::magic_sequence::solve(&s1, &list_of_solutions);
        // print solutions
        for (const auto& item : list_of_solutions) {
            backtracking::magic_sequence::print(item);
        }
    }
    return 0;
}
