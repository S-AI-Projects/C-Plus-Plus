// C++ program to find minimum number of swaps required to sort an array
#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

/**
 * @brief Function returns the minimum number of swaps
 * required to sort the array
 * @details Supported boundaries:
 * - `n == 0` and `n == 1` are the smallest supported collections; both need
 *   0 swaps. With `n == 0`, `arr` is never read.
 * - Only the first `n` elements of `arr` are used, and `arr` is not modified
 *   (the values are copied into `arrPos` first).
 * - The result is the minimum only when the `n` values are distinct. With
 *   repeated values, ties are broken by original index, so the result is a
 *   valid number of swaps but can be larger than the minimum: for
 *   {2, 2, 1} it returns 2, although one swap gives {1, 2, 2}.
 * - Negative `n` is not supported (`new std::pair<int, int>[n]` cannot take a
 *   negative size).
 * @param arr array of values
 * @param n number of elements of `arr` to use
 * @returns the number of swaps needed to sort the first `n` values ascending
 */
int minSwaps(int arr[], int n) {
    // Create an array of pairs where first
    // element is array element and second element
    // is position of first element
    std::pair<int, int> *arrPos = new std::pair<int, int>[n];
    for (int i = 0; i < n; i++) {
        arrPos[i].first = arr[i];
        arrPos[i].second = i;
    }

    // Sort the array by array element values to
    // get right position of every element as second
    // element of pair.
    std::sort(arrPos, arrPos + n);

    // To keep track of visited elements. Initialize
    // all elements as not visited or false.
    std::vector<bool> vis(n, false);

    // Initialize result
    int ans = 0;

    // Traverse array elements
    for (int i = 0; i < n; i++) {
        // already swapped and corrected or
        // already present at correct pos
        if (vis[i] || arrPos[i].second == i)
            continue;

        // find out the number of node in
        // this cycle and add in ans
        int cycle_size = 0;
        int j = i;
        while (!vis[j]) {
            vis[j] = 1;

            // move to next node
            j = arrPos[j].second;
            cycle_size++;
        }

        // Update answer by adding current cycle.
        if (cycle_size > 0) {
            ans += (cycle_size - 1);
        }
    }

    delete[] arrPos;

    // Return result
    return ans;
}

/**
 * @brief Check one minSwaps() call against a hand-calculated answer and
 * check that the input array is unchanged
 * @param name label of the case
 * @param values input values
 * @param n number of elements passed to minSwaps()
 * @param expected number of swaps worked out by hand
 */
static void check(const char *name, std::vector<int> values, int n,
                  int expected) {
    const std::vector<int> before = values;
    const int actual = minSwaps(values.data(), n);
    std::cerr << "[minSwaps] " << name << ": expected " << expected
              << " actual " << actual << std::endl;
    assert(actual == expected);
    assert(values == before);  // the input array is not modified
}

/**
 * @brief Boundary tests for minSwaps()
 * @returns the number of checks run
 */
static int test() {
    int count = 0;
    // smallest supported collections
    check("empty, n=0", {}, 0, 0);
    count += 2;
    check("single value, n=1", {5}, 1, 0);
    count += 2;

    // valid interior, distinct values
    check("already ordered [1, 2, 3]", {1, 2, 3}, 3, 0);
    count += 2;
    check("[3, 1, 2] -> one 3-cycle", {3, 1, 2}, 3, 2);
    count += 2;
    check("reversed [3, 2, 1] -> one 2-cycle", {3, 2, 1}, 3, 1);
    count += 2;
    check("program example", {6, 7, 8, 1, 2, 3, 9, 12}, 8, 3);
    count += 2;

    // only the first n elements are used
    check("prefix n=2 of [3, 1, 2]", {3, 1, 2}, 2, 1);
    count += 2;

    // repeated values: other side of the distinct-values boundary
    check("repeated values [2, 2, 2]", {2, 2, 2}, 3, 0);
    count += 2;
    check("repeated values [2, 2, 1] (minimum is 1)", {2, 2, 1}, 3, 2);
    count += 2;

    std::cerr << "[minSwaps] " << count << " checks passed" << std::endl;
    return count;
}

// program to test
int main() {
    test();
    int arr[] = {6, 7, 8, 1, 2, 3, 9, 12};
    int n = (sizeof(arr) / sizeof(int));
    std::cout << minSwaps(arr, n);
    return 0;
}
