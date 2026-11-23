// Returns the sorted vector after performing SlowSort
// It is a sorting algorithm that is of humorous nature and not useful.
// It's based on the principle of multiply and surrender, a tongue-in-cheek joke
// of divide and conquer. It was published in 1986 by Andrei Broder and Jorge
// Stolfi in their paper Pessimal Algorithms and Simplexity Analysis. This
// algorithm multiplies a single problem into multiple subproblems It is
// interesting because it is provably the least efficient sorting algorithm that
// can be built asymptotically, and with the restriction that such an algorithm,
// while being slow, must still all the time be working towards a result.

#include <algorithm>  /// for std::sort, std::is_sorted, std::is_permutation
#include <cassert>    /// for assert
#include <iostream>   /// for IO operations
#include <string>     /// for std::string, std::to_string
#include <utility>    /// for std::pair
#include <vector>     /// for std::vector

/**
 * @brief Sort `a[i..j]` (both ends included) in ascending order, in place
 * @details Elements outside `a[i..j]` are not touched, and nothing is
 * returned. To sort an array of `n` elements, call `SlowSort(a, 0, n - 1)`.
 *
 * Worked example, `a = {3, 1, 2}`, `SlowSort(a, 0, 2)`:
 * - midpoint `m = 1`; sort `a[0..1]` -> `{1, 3, 2}`; `a[2..2]` is one element
 * - `a[2] = 2 < a[1] = 3`, so swap: the largest value 3 moves to the end,
 *   giving `{1, 2, 3}`
 * - sort the rest, `a[0..1]` = `{1, 2}`, which is already in order
 *
 * Result: `{1, 2, 3}`. This is the "multiply and surrender" step: each call
 * puts the maximum of `a[i..j]` at `a[j]` and then sorts `a[i..j-1]` again.
 * @param a array to sort
 * @param i first index of the range
 * @param j last index of the range (included)
 */
void SlowSort(int a[], int i, int j) {
    if (i >= j)
        return;
    int m = i + (j - i) / 2;  // midpoint, implemented this way to avoid
                              // overflow
    int temp;
    SlowSort(a, i, m);
    SlowSort(a, m + 1, j);
    if (a[j] < a[m]) {
        temp = a[j];  // swapping a[j] & a[m]
        a[j] = a[m];
        a[m] = temp;
    }
    SlowSort(a, i, j - 1);
}

/**
 * @brief Independent oracle: sort a copy with std::sort (introsort, a
 * different method that does not call SlowSort)
 * @param v input values
 * @returns the values in ascending order
 */
static std::vector<int> oracle(std::vector<int> v) {
    std::sort(v.begin(), v.end());
    return v;
}

/**
 * @brief Run SlowSort on the whole vector and compare it with an expected
 * result: the output must be ordered and keep the same values with the same
 * multiplicities
 * @param input values to sort
 * @param expected expected ascending result
 * @returns the number of assertions checked
 */
static int check_full(const std::vector<int>& input,
                      const std::vector<int>& expected) {
    std::vector<int> a = input;
    SlowSort(a.data(), 0, static_cast<int>(a.size()) - 1);
    assert(std::is_sorted(a.begin(), a.end()));
    assert(std::is_permutation(a.begin(), a.end(), input.begin()));
    assert(a == expected);
    return 3;
}

/**
 * @brief Print a vector for the test report
 * @param v values
 * @returns the values as text, e.g. "[1, 2, 3]"
 */
static std::string show(const std::vector<int>& v) {
    std::string out = "[";
    for (size_t k = 0; k < v.size(); ++k) {
        out += (k ? ", " : "") + std::to_string(v[k]);
    }
    return out + "]";
}

/**
 * @brief Cross-check SlowSort against a fixed table and the oracle
 * @returns void
 */
static void test() {
    int count = 0;

    // fixed expected-result table, worked out by hand
    const std::vector<std::pair<std::vector<int>, std::vector<int>>> table = {
        {{3, 1, 2}, {1, 2, 3}},
        {{2, 2, 1}, {1, 2, 2}},
        {{1, 2, 3}, {1, 2, 3}},
        {{3, 2, 1}, {1, 2, 3}},
        {{7}, {7}},  // smallest non-empty collection
        {{}, {}},    // smallest collection: empty range, j = -1
    };
    for (const auto& row : table) {
        std::vector<int> a = row.first;
        SlowSort(a.data(), 0, static_cast<int>(a.size()) - 1);
        std::cerr << "[SlowSort] " << show(row.first) << ": expected "
                  << show(row.second) << " actual " << show(a) << std::endl;
        count += check_full(row.first, row.second);
    }

    // every sequence of length 0..4 over the values {1, 2, 3}: 121 inputs
    int enumerated = 0;
    for (int len = 0; len <= 4; ++len) {
        int total = 1;
        for (int k = 0; k < len; ++k) total *= 3;
        for (int code = 0; code < total; ++code) {
            std::vector<int> v;
            int c = code;
            for (int k = 0; k < len; ++k) {
                v.push_back(1 + c % 3);
                c /= 3;
            }
            count += check_full(v, oracle(v));
            ++enumerated;
        }
    }
    std::cerr << "[SlowSort] enumerated " << enumerated
              << " inputs against the std::sort oracle" << std::endl;

    // mutation contract: only a[i..j] changes
    std::vector<int> part = {9, 3, 1, 2, 0};
    SlowSort(part.data(), 1, 3);
    assert((part == std::vector<int>{9, 1, 2, 3, 0}));
    ++count;

    // regression for the old sample call SlowSort(arr, 0, size): j is
    // included, so that call also sorts arr[size], one past the end. Shown
    // safely with an extra slot holding a smaller value: it moves into the
    // "sorted" part, and 3 is pushed out past the end.
    std::vector<int> padded = {3, 1, 2, -100};
    SlowSort(padded.data(), 0, 3);  // the old call with size = 3
    assert((padded == std::vector<int>{-100, 1, 2, 3}));
    ++count;
    std::vector<int> fixed = {3, 1, 2, -100};
    SlowSort(fixed.data(), 0, 3 - 1);  // the corrected call
    assert((fixed == std::vector<int>{1, 2, 3, -100}));
    ++count;

    std::cerr << "[SlowSort] " << count << " checks passed" << std::endl;
}

// Sample Main function

int main() {
    test();

    int size;
    std::cout << "\nEnter the number of elements : ";

    std::cin >> size;

    int *arr = new int[size];

    std::cout << "\nEnter the unsorted elements : ";

    for (int i = 0; i < size; ++i) {
        std::cout << "\n";
        std::cin >> arr[i];
    }

    SlowSort(arr, 0, size - 1);  // j is the last index, which is included

    std::cout << "Sorted array\n";

    for (int i = 0; i < size; ++i) {
        std::cout << arr[i] << " ";
    }

    delete[] arr;
    return 0;
}
