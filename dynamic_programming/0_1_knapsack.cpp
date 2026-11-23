/**
 * @file
 * @brief Implementation of [0-1 Knapsack Problem]
 * (https://en.wikipedia.org/wiki/Knapsack_problem)
 *
 * @details
 * Given weights and values of n items, put these items in a knapsack of
 * capacity `W` to get the maximum total value in the knapsack. In other words,
 * given two integer arrays `val[0..n-1]` and `wt[0..n-1]` which represent
 * values and weights associated with n items respectively. Also given an
 * integer W which represents knapsack capacity, find out the maximum value
 * subset of `val[]` such that sum of the weights of this subset is smaller than
 * or equal to W. You cannot break an item, either pick the complete item or
 * don’t pick it (0-1 property)
 *
 * ### Algorithm
 * The idea is to consider all subsets of items and calculate the total weight
 * and value of all subsets. Consider the only subsets whose total weight is
 * smaller than `W`. From all such subsets, pick the maximum value subset.
 *
 * @author [Anmol](https://github.com/Anmol3299)
 * @author [Pardeep](https://github.com/Pardeep009)
 */

#include <array>
#include <cassert>
#include <iostream>
#include <vector>

/**
 * @namespace dynamic_programming
 * @brief Dynamic Programming algorithms
 */
namespace dynamic_programming {
/**
 * @namespace Knapsack
 * @brief Implementation of 0-1 Knapsack problem
 */
namespace knapsack {
/**
 * @brief Picking up all those items whose combined weight is below
 * the given capacity and calculating the value of those picked items. Trying all
 * possible combinations will yield the maximum knapsack value.
 * @tparam n size of the weight and value array
 * @param capacity capacity of the carrying bag
 * @param weight array representing the weight of items
 * @param value array representing the value of items
 * @return maximum value obtainable with a given capacity.
 */
template <size_t n>
int maxKnapsackValue(const int capacity, const std::array<int, n> &weight,
                     const std::array<int, n> &value) {
    std::vector<std::vector<int> > maxValue(n + 1,
                                            std::vector<int>(capacity + 1, 0));
    // outer loop will select no of items allowed
    // inner loop will select the capacity of the knapsack bag
    int items = sizeof(weight) / sizeof(weight[0]);
    for (size_t i = 0; i < items + 1; ++i) {
        for (size_t j = 0; j < capacity + 1; ++j) {
            if (i == 0 || j == 0) {
                // if no of items is zero or capacity is zero, then maxValue
                // will be zero
                maxValue[i][j] = 0;
            } else if (weight[i - 1] <= j) {
                // if the ith item's weight(in the actual array it will be at i-1)
                // is less than or equal to the allowed weight i.e. j then we
                // can pick that item for our knapsack. maxValue will be the
                // obtained either by picking the current item or by not picking
                // current item

                // picking the current item
                int profit1 = value[i - 1] + maxValue[i - 1][j - weight[i - 1]];

                // not picking the current item
                int profit2 = maxValue[i - 1][j];

                maxValue[i][j] = std::max(profit1, profit2);
            } else {
                // as the weight of the current item is greater than the allowed weight, so
                // maxProfit will be profit obtained by excluding the current item.
                // Protected condition: an item that does not fit must keep the
                // best value of the earlier items (not reset it); see the
                // "item heavier than capacity" regression in test().
                maxValue[i][j] = maxValue[i - 1][j];
            }
        }
    }

    // returning maximum value
    return maxValue[items][capacity];
}
}  // namespace knapsack
}  // namespace dynamic_programming

/**
 * @brief Function to test the above algorithm
 * @returns void
 */
static void test() {
    // Test 1
    const int n1 = 3;                             // number of items
    std::array<int, n1> weight1 = {10, 20, 30};   // weight of each item
    std::array<int, n1> value1 = {60, 100, 120};  // value of each item
    const int capacity1 = 50;                     // capacity of carrying bag
    const int max_value1 = dynamic_programming::knapsack::maxKnapsackValue(
        capacity1, weight1, value1);
    const int expected_max_value1 = 220;
    assert(max_value1 == expected_max_value1);
    std::cout << "Maximum Knapsack value with " << n1 << " items is "
              << max_value1 << std::endl;

    // Test 2
    const int n2 = 4;                               // number of items
    std::array<int, n2> weight2 = {24, 10, 10, 7};  // weight of each item
    std::array<int, n2> value2 = {24, 18, 18, 10};  // value of each item
    const int capacity2 = 25;                       // capacity of carrying bag
    const int max_value2 = dynamic_programming::knapsack::maxKnapsackValue(
        capacity2, weight2, value2);
    const int expected_max_value2 = 36;
    assert(max_value2 == expected_max_value2);
    std::cout << "Maximum Knapsack value with " << n2 << " items is "
              << max_value2 << std::endl;

    // Test 3: item heavier than capacity (neighbour of test 1). Item 4
    // (weight 60) cannot fit in capacity 50, so the answer must stay 220
    // (items 2 and 3: weight 20 + 30 = 50, value 100 + 120).
    const int n3 = 4;
    const std::array<int, n3> weight3 = {10, 20, 30, 60};
    const std::array<int, n3> value3 = {60, 100, 120, 500};
    const std::array<int, n3> weight3_copy = weight3, value3_copy = value3;
    const int max_value3 = dynamic_programming::knapsack::maxKnapsackValue(
        50, weight3, value3);
    const int expected_max_value3 = 220;
    assert(max_value3 == expected_max_value3);  // item heavier than capacity
    // same result for a fresh equivalent input, and the inputs are unchanged
    const int max_value3_again =
        dynamic_programming::knapsack::maxKnapsackValue(50, weight3_copy,
                                                        value3_copy);
    assert(max_value3_again == expected_max_value3);
    assert(weight3 == weight3_copy && value3 == value3_copy);

    // Test 4: smallest input for the same condition. Capacity 1 holds only
    // item 1 (weight 1, value 10); item 2 (weight 2) does not fit.
    const std::array<int, 2> weight4 = {1, 2};
    const std::array<int, 2> value4 = {10, 20};
    const int max_value4 =
        dynamic_programming::knapsack::maxKnapsackValue(1, weight4, value4);
    const int expected_max_value4 = 10;
    assert(max_value4 == expected_max_value4);  // smallest heavy-item case
    std::cerr << "[maxKnapsackValue] 6 checks passed" << std::endl;
}

/**
 * @brief Main function
 * @returns 0 on exit
 */
int main() {
    // Testing
    test();
    return 0;
}
