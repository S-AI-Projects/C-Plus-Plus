/**
 * @file
 * @brief   Implementation of [Segment Tree]
 *          (https://en.wikipedia.org/wiki/Segment_tree) data structure
 *
 * @details
 * A segment tree, also known as a statistic tree, is a tree data structure used
 * for storing information about intervals, or segments. Its classical version
 * allows querying which of the stored segments contain a given point, but our
 * modification allows us to perform (query) any binary operation on any range
 * in the array in O(logN) time. Here, we have used addition (+).
 * For range updates, we have used lazy propagation.
 *
 * * Space Complexity : O(NlogN) \n
 * * Build Time Complexity : O(NlogN) \n
 * * Query Time Complexity : O(logN) \n
 *
 * @author  [Madhav Gaba](https://github.com/madhavgaba)
 * @author  [Soham Roy](https://github.com/sohamroy19)
 */

#include <cassert>   /// for assert
#include <cmath>     /// for log2
#include <cstdint>   /// for std::uint64_t
#include <iostream>  /// for IO operations
#include <vector>    /// for std::vector

/**
 * @brief   Constructs the initial segment tree
 *
 * @details ConsTree() depends only on its arguments: it reads `arr` (const),
 * writes `segtree` at the nodes covering `arr[low..high]`, and uses no time,
 * randomness, I/O or global state. Building the same input twice gives the
 * same tree. Nodes that `arr` does not reach are left as they were, so a
 * reused `segtree` can keep old values in those slots; query() never visits
 * them, so query results are not affected.
 *
 * @param   arr input to construct the tree out of
 * @param   segtree the segment tree
 * @param   low inclusive lowest index of arr to begin at
 * @param   high inclusive highest index of arr to end at
 * @param   pos index of segtree to fill (eg. root node)
 * @returns void
 */
void ConsTree(const std::vector<int64_t> &arr, std::vector<int64_t> *segtree,
              uint64_t low, uint64_t high, uint64_t pos) {
    if (low == high) {
        (*segtree)[pos] = arr[low];
        return;
    }

    uint64_t mid = (low + high) / 2;
    ConsTree(arr, segtree, low, mid, 2 * pos + 1);
    ConsTree(arr, segtree, mid + 1, high, 2 * pos + 2);
    (*segtree)[pos] = (*segtree)[2 * pos + 1] + (*segtree)[2 * pos + 2];
}

/**
 * @brief   Returns the sum of all elements in a range
 *
 * @param   segtree the segment tree
 * @param   lazy for lazy propagation
 * @param   qlow lower index of the required query
 * @param   qhigh higher index of the required query
 * @param   low lower index of query for this function call
 * @param   high higher index of query for this function call
 * @param   pos index of segtree to consider (eg. root node)
 * @return  result of the range query for this function call
 */
int64_t query(std::vector<int64_t> *segtree, std::vector<int64_t> *lazy,
              uint64_t qlow, uint64_t qhigh, uint64_t low, uint64_t high,
              uint64_t pos) {
    if (low > high || qlow > high || low > qhigh) {
        return 0;
    }

    if ((*lazy)[pos] != 0) {
        (*segtree)[pos] += (*lazy)[pos] * (high - low + 1);

        if (low != high) {
            (*lazy)[2 * pos + 1] += (*lazy)[pos];
            (*lazy)[2 * pos + 2] += (*lazy)[pos];
        }
        (*lazy)[pos] = 0;
    }

    if (qlow <= low && qhigh >= high) {
        return (*segtree)[pos];
    }

    uint64_t mid = (low + high) / 2;

    return query(segtree, lazy, qlow, qhigh, low, mid, 2 * pos + 1) +
           query(segtree, lazy, qlow, qhigh, mid + 1, high, 2 * pos + 2);
}

/**
 * @brief   Updates a range of the segment tree
 *
 * @param   segtree the segment tree
 * @param   lazy for lazy propagation
 * @param   start lower index of the required query
 * @param   end higher index of the required query
 * @param   delta integer to add to each element of the range
 * @param   low lower index of query for this function call
 * @param   high higher index of query for this function call
 * @param   pos index of segtree to consider (eg. root node)
 * @returns void
 */
void update(std::vector<int64_t> *segtree, std::vector<int64_t> *lazy,
            int64_t start, int64_t end, int64_t delta, uint64_t low,
            uint64_t high, uint64_t pos) {
    if (low > high) {
        return;
    }

    if ((*lazy)[pos] != 0) {
        (*segtree)[pos] += (*lazy)[pos] * (high - low + 1);

        if (low != high) {
            (*lazy)[2 * pos + 1] += (*lazy)[pos];
            (*lazy)[2 * pos + 2] += (*lazy)[pos];
        }
        (*lazy)[pos] = 0;
    }

    if (start > high || end < low) {
        return;
    }

    if (start <= low && end >= high) {
        (*segtree)[pos] += delta * (high - low + 1);

        if (low != high) {
            (*lazy)[2 * pos + 1] += delta;
            (*lazy)[2 * pos + 2] += delta;
        }

        return;
    }

    uint64_t mid = (low + high) / 2;

    update(segtree, lazy, start, end, delta, low, mid, 2 * pos + 1);
    update(segtree, lazy, start, end, delta, mid + 1, high, 2 * pos + 2);
    (*segtree)[pos] = (*segtree)[2 * pos + 1] + (*segtree)[2 * pos + 2];
}

/**
 * @brief   Self-test implementation
 *
 * @returns void
 */
static void test() {
    auto max = static_cast<int64_t>(2 * pow(2, ceil(log2(7))) - 1);
    assert(max == 15);

    std::vector<int64_t> arr{1, 2, 3, 4, 5, 6, 7}, lazy(max), segtree(max);
    ConsTree(arr, &segtree, 0, 7 - 1, 0);

    assert(query(&segtree, &lazy, 1, 5, 0, 7 - 1, 0) == 2 + 3 + 4 + 5 + 6);

    update(&segtree, &lazy, 2, 4, 1, 0, 7 - 1, 0);
    assert(query(&segtree, &lazy, 1, 5, 0, 7 - 1, 0) == 2 + 4 + 5 + 6 + 6);

    update(&segtree, &lazy, 0, 6, -2, 0, 7 - 1, 0);
    assert(query(&segtree, &lazy, 0, 4, 0, 7 - 1, 0) == -1 + 0 + 2 + 3 + 4);
}

/**
 * @brief   Sum arr[lo..hi] directly, as an independent reference
 * @param   arr values
 * @param   lo first index (included)
 * @param   hi last index (included)
 * @returns the sum
 */
static int64_t brute_sum(const std::vector<int64_t> &arr, uint64_t lo,
                         uint64_t hi) {
    int64_t sum = 0;
    for (uint64_t k = lo; k <= hi; ++k) sum += arr[k];
    return sum;
}

/**
 * @brief   Check every range query of a built tree against brute_sum()
 * @param   arr values the tree was built from
 * @param   segtree built tree (queried on a copy, so it is not changed)
 * @returns the number of assertions checked
 */
static int check_all_ranges(const std::vector<int64_t> &arr,
                            const std::vector<int64_t> &segtree) {
    int count = 0;
    const uint64_t n = arr.size();
    for (uint64_t lo = 0; lo < n; ++lo) {
        for (uint64_t hi = lo; hi < n; ++hi) {
            std::vector<int64_t> tree = segtree, lazy(segtree.size(), 0);
            assert(query(&tree, &lazy, lo, hi, 0, n - 1, 0) ==
                   brute_sum(arr, lo, hi));
            ++count;
        }
    }
    return count;
}

/**
 * @brief   Repeatability and input-isolation tests for ConsTree()
 *
 * @returns void
 */
static void test_repeatable() {
    int count = 0;
    const std::vector<int64_t> arr{1, 2, 3, 4, 5, 6, 7};
    const std::vector<int64_t> arr_copy = arr;
    // hand-worked tree for {1..7}: root [0,6] = 28, [0,3] = 10, [4,6] = 18,
    // [0,1] = 3, [2,3] = 7, [4,5] = 11, leaf [6] = 7, leaves 1..6; slots 13
    // and 14 are not used for n = 7 and stay 0
    const std::vector<int64_t> expected7{28, 10, 18, 3, 7, 11, 7, 1,
                                         2,  3,  4,  5, 6, 0,  0};

    // run 1 and run 2, each in fresh state
    std::vector<int64_t> first(15, 0), second(15, 0);
    ConsTree(arr, &first, 0, 7 - 1, 0);
    ConsTree(arr, &second, 0, 7 - 1, 0);
    assert(first == expected7);
    assert(second == expected7);
    assert(arr == arr_copy);  // input isolation: arr is not modified
    count += 3;
    count += check_all_ranges(arr, first);

    // neighbouring supported input: n = 8, a full tree with no unused slots
    const std::vector<int64_t> arr8{1, 2, 3, 4, 5, 6, 7, 8};
    const std::vector<int64_t> expected8{36, 10, 26, 3, 7, 11, 15, 1,
                                         2,  3,  4,  5, 6, 7,  8};
    std::vector<int64_t> tree8(15, 0);
    ConsTree(arr8, &tree8, 0, 8 - 1, 0);
    assert(tree8 == expected8);
    ++count;
    count += check_all_ranges(arr8, tree8);

    // run 3: after another valid input of the same size in the same vector,
    // every used node is overwritten, so the result equals a fresh build
    std::vector<int64_t> reused(15, 0);
    ConsTree({7, 6, 5, 4, 3, 2, 1}, &reused, 0, 7 - 1, 0);
    ConsTree(arr, &reused, 0, 7 - 1, 0);
    assert(reused == expected7);
    ++count;

    // after a larger input (n = 8) in the same vector, slots 13 and 14 keep
    // the old leaves 7 and 8; the n = 7 queries are still correct
    std::vector<int64_t> stale = expected8;
    ConsTree(arr, &stale, 0, 7 - 1, 0);
    assert(stale[13] == 7 && stale[14] == 8);
    assert(std::vector<int64_t>(stale.begin(), stale.begin() + 13) ==
           std::vector<int64_t>(expected7.begin(), expected7.begin() + 13));
    count += 2;
    count += check_all_ranges(arr, stale);

    // the source fixture still holds
    std::vector<int64_t> lazy(15, 0), segtree(15, 0);
    ConsTree(arr, &segtree, 0, 7 - 1, 0);
    assert(query(&segtree, &lazy, 1, 5, 0, 7 - 1, 0) == 2 + 3 + 4 + 5 + 6);
    update(&segtree, &lazy, 2, 4, 1, 0, 7 - 1, 0);
    assert(query(&segtree, &lazy, 1, 5, 0, 7 - 1, 0) == 2 + 4 + 5 + 6 + 6);
    count += 2;

    std::cerr << "[ConsTree] " << count << " checks passed" << std::endl;
}

/**
 * @brief   Main function
 *
 * @return  0 on exit
 */
int main() {
    test();             // run self-test implementations
    test_repeatable();  // run repeatability tests for ConsTree

    std::cout << "Enter number of elements: ";

    uint64_t n = 0;
    std::cin >> n;

    auto max = static_cast<uint64_t>(2 * pow(2, ceil(log2(n))) - 1);
    std::vector<int64_t> arr(n), lazy(max), segtree(max);

    int choice = 0;
    std::cout << "\nDo you wish to enter each number?:\n"
                 "1: Yes\n"
                 "0: No (default initialize them to 0)\n";

    std::cin >> choice;
    if (choice == 1) {
        std::cout << "Enter " << n << " numbers:\n";
        for (int i = 1; i <= n; i++) {
            std::cout << i << ": ";
            std::cin >> arr[i];
        }
    }

    ConsTree(arr, &segtree, 0, n - 1, 0);

    do {
        std::cout << "\nMake your choice:\n"
                     "1: Range update (input)\n"
                     "2: Range query (output)\n"
                     "0: Exit\n";
        std::cin >> choice;

        if (choice == 1) {
            std::cout << "Enter 1-indexed lower bound, upper bound & value:\n";

            uint64_t p = 1, q = 1, v = 0;
            std::cin >> p >> q >> v;
            update(&segtree, &lazy, p - 1, q - 1, v, 0, n - 1, 0);
        } else if (choice == 2) {
            std::cout << "Enter 1-indexed lower bound & upper bound:\n";

            uint64_t p = 1, q = 1;
            std::cin >> p >> q;
            std::cout << query(&segtree, &lazy, p - 1, q - 1, 0, n - 1, 0);
            std::cout << "\n";
        }
    } while (choice > 0);

    return 0;
}
