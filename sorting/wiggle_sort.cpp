/**
 * \addtogroup sorting Sorting Algorithms
 * @{
 * \file
 * \brief [Wiggle Sort Algorithm]
 * (https://leetcode.com/problems/wiggle-sort-ii/) Implementation
 *
 * \author [Roshan Kanwar](http://github.com/roshan0708)
 *
 * \details
 * Wiggle Sort sorts the array into a wave like array.
 * An array ‘arr[0..n-1]’ is sorted in wave form,
 * if arr[0] >= arr[1] <= arr[2] >= arr[3] <= arr[4] >= …..
 *
 * \example
 * arr = [1,1,5,6,1,4], after wiggle sort arr will become equal to [1,1,6,1,5,4]
 * arr = [2,8,9,1,7], after wiggle sort arr will become equal to [8,2,9,1,7]
 */

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <iostream>  /// for io operations
#include <random>
#include <utility>
#include <vector>

/**
 * @namespace sorting
 * @brief Sorting algorithms
 */
namespace sorting {
/**
 * @namespace wiggle_sort
 * @brief Functions for [Wiggle
 * Sort](https://leetcode.com/problems/wiggle-sort-ii/) algorithm
 */
namespace wiggle_sort {

/**
 *
 * @brief Function used for sorting the elements in wave form.
 * @details
 * The returned copy satisfies out[0] >= out[1] <= out[2] >= out[3] ... and
 * preserves every input value. The input vector is not modified.
 * Checking whether the even indexed elements are greater than
 * their adjacent odd elements.
 * Traversing all even indexed elements of the input arr.
 * If current element is smaller than the previous odd element, swap them.
 * If current element is smaller than the next odd element, swap them.
 *
 * @param arr input array (unsorted elements)
 *
 */
template <typename T>  // this allows to have vectors of ints, double, float,
                       // etc
std::vector<T> wiggleSort(const std::vector<T> &arr) {
    uint32_t size = arr.size();

    std::vector<T> out(
        arr);  // create a copy of input vector. this way, the original input
               // vector does not get modified. a sorted array is is returned.

    for (int i = 0; i < size; i += 2) {
        if (i > 0 && out[i - 1] > out[i]) {
            std::swap(out[i], out[i - 1]);  // swapping the two values
        }

        if (i < size - 1 && out[i] < out[i + 1]) {
            std::swap(out[i], out[i + 1]);  // swapping the two values
        }
    }

    return out;  // returns the sorted vector
}
}  // namespace wiggle_sort
}  // namespace sorting

/**
 *
 * @brief Utility function used for printing the elements.
 * Prints elements of the array after they're sorted using wiggle sort
 * algorithm.
 *
 * @param arr array containing the sorted elements
 *
 */
template <typename T>
static void displayElements(const std::vector<T> &arr) {
    uint32_t size = arr.size();

    std::cout << "Sorted elements are as follows: ";

    std::cout << "[";

    for (int i = 0; i < size; i++) {
        std::cout << arr[i];
        if (i != size - 1) {
            std::cout << ", ";
        }
    }

    std::cout << "]" << std::endl;
}

/**
 * Test function
 * @returns void
 */
static void test() {
    using IntVector = std::vector<int>;
    const std::vector<std::pair<IntVector, IntVector>> fixtures = {
        {{3, 1, 2}, {3, 1, 2}},
        {{2, 2, 1}, {2, 1, 2}},
        {{1, 2, 3}, {2, 1, 3}},
        {{3, 2, 1}, {3, 1, 2}},
        {{}, {}},
        {{7}, {7}},
    };
    size_t checked_cases = 0;

    const auto check_invariants = [&checked_cases](const IntVector &input) {
        const IntVector original = input;
        const IntVector output = sorting::wiggle_sort::wiggleSort(input);
        IntVector input_values = input;
        IntVector output_values = output;
        std::sort(input_values.begin(), input_values.end());
        std::sort(output_values.begin(), output_values.end());
        assert(output_values == input_values);  // multiplicities are preserved
        assert(input == original);              // source is not mutated
        for (size_t index = 0; index + 1 < output.size(); index++) {
            if (index % 2 == 0) {
                assert(output[index] >= output[index + 1]);
            } else {
                assert(output[index] <= output[index + 1]);
            }
        }
        checked_cases++;
    };

    for (size_t fixture_index = 0; fixture_index < fixtures.size(); fixture_index++) {
        const auto &fixture = fixtures[fixture_index];
        const IntVector output = sorting::wiggle_sort::wiggleSort(fixture.first);
        assert(output == fixture.second);
        std::cout << "FIXTURE " << fixture_index << " input=[";
        for (size_t index = 0; index < fixture.first.size(); index++) {
            std::cout << (index ? "," : "") << fixture.first[index];
        }
        std::cout << "] output=[";
        for (size_t index = 0; index < output.size(); index++) {
            std::cout << (index ? "," : "") << output[index];
        }
        std::cout << "]\n";
        check_invariants(fixture.first);
    }

    std::mt19937 generator(0x5A17U);
    std::uniform_int_distribution<int> size_distribution(0, 30);
    std::uniform_int_distribution<int> value_distribution(-10, 10);
    for (int sample = 0; sample < 100; sample++) {
        IntVector input(static_cast<size_t>(size_distribution(generator)));
        for (int &value : input) {
            value = value_distribution(generator);
        }
        check_invariants(input);
    }
    std::cout << "Wiggle cases passed: " << checked_cases << '\n';
}

/** Driver Code */
int main() {
    test();
    return 0;
}

/** @} */
