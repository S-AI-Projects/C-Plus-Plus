/**
 * @file binary_addition.cpp
 * @brief Adds two binary numbers and outputs resulting string
 *
 * @details The algorithm for adding two binary strings works by processing them
 * from right to left, similar to manual addition. It starts by determining the
 * longer string's length to ensure both strings are fully traversed. For each
 * pair of corresponding bits and any carry from the previous addition, it
 * calculates the sum. If the sum exceeds 1, a carry is generated for the next
 * bit. The results for each bit are collected in a result string, which is
 * reversed at the end to present the final binary sum correctly. Additionally,
 * the function validates the input to ensure that only valid binary strings
 * (containing only '0' and '1') are processed. If invalid input is detected,
 * it returns an empty string.
 *
 * @par Worked example
 * @code
 * greedy_algorithms::BinaryAddition adder;
 * std::cout << adder.addBinary("1011", "110");  // prints 10001
 * @endcode
 * That is 11 + 6 = 17. The bits are read right to left (`i` counts from the
 * last character); `b` is shorter, so its missing bit is 0:
 *
 * | i | bitA | bitB | carry in | sum | bit written | carry out | result so far |
 * |---|------|------|----------|-----|-------------|-----------|---------------|
 * | 0 | 1    | 0    | 0        | 1   | 1           | 0         | "1"           |
 * | 1 | 1    | 1    | 0        | 2   | 0           | 1         | "10"          |
 * | 2 | 0    | 1    | 1        | 2   | 0           | 1         | "100"         |
 * | 3 | 1    | 0    | 1        | 2   | 0           | 1         | "1000"        |
 *
 * The decisive step is after the loop: the carry is still 1, so `if (carry)`
 * appends one more '1', giving "10001" once reversed. Without that step the
 * answer would be one bit short ("0001").
 * @author [Muhammad Junaid Khalid](https://github.com/mjk22071998)
 */

#include <algorithm>  /// for reverse function
#include <cassert>    /// for tests
#include <iostream>   /// for input and outputs
#include <sstream>    /// for std::ostringstream
#include <string>     /// for string class

/**
 * @namespace
 * @brief Greedy Algorithms
 */
namespace greedy_algorithms {
/**
 * @brief A class to perform binary addition of two binary strings.
 */
class BinaryAddition {
 public:
    /**
     * @brief Adds two binary strings and returns the result as a binary string.
     * @param a The first binary string.
     * @param b The second binary string.
     * @return The sum of the two binary strings as a binary string, or an empty
     * string if either input string contains non-binary characters.
     */
    std::string addBinary(const std::string& a, const std::string& b) {
        if (!isValidBinaryString(a) || !isValidBinaryString(b)) {
            return "";  // Return empty string if input contains non-binary
                        // characters
        }

        std::string result;
        int carry = 0;
        int maxLength = std::max(a.size(), b.size());

        // Traverse both strings from the end to the beginning
        for (int i = 0; i < maxLength; ++i) {
            // Get the current bits from both strings, if available
            int bitA = (i < a.size()) ? (a[a.size() - 1 - i] - '0') : 0;
            int bitB = (i < b.size()) ? (b[b.size() - 1 - i] - '0') : 0;

            // Calculate the sum of bits and carry
            int sum = bitA + bitB + carry;
            carry = sum / 2;  // Determine the carry for the next bit
            result.push_back((sum % 2) +
                             '0');  // Append the sum's current bit to result
        }
        if (carry) {
            result.push_back('1');
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

 private:
    /**
     * @brief Validates whether a string contains only binary characters (0 or 1).
     * @param str The string to validate.
     * @return true if the string is binary, false otherwise.
     */
    bool isValidBinaryString(const std::string& str) const {
        return std::all_of(str.begin(), str.end(),
                           [](char c) { return c == '0' || c == '1'; });
    }
};
}  // namespace greedy_algorithms

/**
 * @brief run self test implementation.
 * @returns void
 */
static void tests() {
    greedy_algorithms::BinaryAddition binaryAddition;

    // Valid binary string tests
    assert(binaryAddition.addBinary("1010", "1101") == "10111");
    assert(binaryAddition.addBinary("1111", "1111") == "11110");
    assert(binaryAddition.addBinary("101", "11") == "1000");
    assert(binaryAddition.addBinary("0", "0") == "0");
    assert(binaryAddition.addBinary("1111", "1111") == "11110");
    assert(binaryAddition.addBinary("0", "10101") == "10101");
    assert(binaryAddition.addBinary("10101", "0") == "10101");
    assert(binaryAddition.addBinary("101010101010101010101010101010",
                                    "110110110110110110110110110110") ==
           "1100001100001100001100001100000");
    assert(binaryAddition.addBinary("1", "11111111") == "100000000");
    assert(binaryAddition.addBinary("10101010", "01010101") == "11111111");

    // Invalid binary string tests (should return empty string)
    assert(binaryAddition.addBinary("10102", "1101") == "");
    assert(binaryAddition.addBinary("ABC", "1101") == "");
    assert(binaryAddition.addBinary("1010", "1102") == "");
    assert(binaryAddition.addBinary("111", "1x1") == "");
    assert(binaryAddition.addBinary("1x1", "111") == "");
    assert(binaryAddition.addBinary("1234", "1101") == "");
}

/**
 * @brief Run the documented worked example and check its printed output,
 * then check each traced intermediate state through the public API
 * @returns void
 */
static void doc_example_tests() {
    int count = 0;

    // the exact code from the file's @code block, with std::cout captured
    std::ostringstream printed;
    std::streambuf* old_buf = std::cout.rdbuf(printed.rdbuf());
    greedy_algorithms::BinaryAddition adder;
    std::cout << adder.addBinary("1011", "110");  // prints 10001
    std::cout.rdbuf(old_buf);
    std::cerr << "[BinaryAddition] doc example printed " << printed.str()
              << std::endl;
    assert(printed.str() == "10001");
    ++count;

    // traced states: after reading the last k bits, the sum of those bits
    // gives the "result so far" (reversed back) plus the carry as a leading 1
    assert(adder.addBinary("1", "0") == "1");        // k = 1: bit 1, carry 0
    assert(adder.addBinary("11", "10") == "101");    // k = 2: bits 01, carry 1
    assert(adder.addBinary("011", "110") == "1001");  // k = 3: 001, carry 1
    assert(adder.addBinary("1011", "0110") == "10001");  // k = 4: final carry
    count += 4;

    // source anchors and a neighbouring input worked out by hand
    assert(adder.addBinary("1010", "1101") == "10111");
    assert(adder.addBinary("1111", "1111") == "11110");
    assert(adder.addBinary("101", "11") == "1000");
    assert(adder.addBinary("101", "111") == "1100");  // neighbour: 5 + 7 = 12
    count += 4;

    std::cerr << "[BinaryAddition] " << count << " checks passed" << std::endl;
}

/**
 * @brief main function
 * @returns 0 on successful exit
 */
int main() {
    tests();              /// To execute tests
    doc_example_tests();  /// To execute the documented example
    return 0;
}
