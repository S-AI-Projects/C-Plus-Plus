/**
 * @file
 * @brief Implementation of [the inverse square root
 * Root](https://medium.com/hard-mode/the-legendary-fast-inverse-square-root-e51fee3b49d9).
 * @details
 * Two implementation to calculate inverse inverse root,
 * from Quake III Arena (C++ version) and with a standard library (`cmath`).
 * This algorithm is used to calculate shadows in Quake III Arena.
 */

#include <cassert>   /// for assert
#include <cmath>     /// for `std::sqrt`
#include <cstdint>
#include <iostream>  /// for IO operations
#include <limits>    /// for numeric_limits
/**
 * @brief This is the function that calculates the fast inverse square root.
 * The following code is the fast inverse square root implementation from
 * Quake III Arena (Adapted for C++). More information can be found at
 * [Wikipedia](https://en.wikipedia.org/wiki/Fast_inverse_square_root)
 * @tparam T floating type
 * @tparam iterations inverse square root, the greater the number of
 * iterations, the more exact the result will be (1 or 2).
 * @param x value to calculate
 * @return the inverse square root
 *
 * @details Complexity: O(1) per call. The work does not depend on the value
 * of `x`: one multiply for `x2`, one shift and one subtraction for the
 * initial guess, then 4 multiplies and 1 subtraction per Newton iteration
 * (`iterations` is 1 or 2). Computing N values therefore takes O(N) time,
 * where N is the number of calls. This is a count of operations, not a
 * timing claim; work/R22-T029 has a timing experiment that is consistent
 * with it.
 *
 * Supported input: positive, finite, normal values. Observed maximum
 * relative error against `1 / std::sqrt(x)` for x in [1e-3, 1e3]:
 * about 0.175% with 1 iteration and 4.6e-6 with 2 iterations.
 */
template <typename T = double, char iterations = 2>
inline T Fast_InvSqrt(T x) {
    using Tint = typename std::conditional<sizeof(T) == 8, std::int64_t,
                                           std::int32_t>::type;
    T y = x;
    T x2 = y * 0.5;

    Tint i =
        *reinterpret_cast<Tint *>(&y);  // Store floating-point bits in integer

    i = (sizeof(T) == 8 ? 0x5fe6eb50c7b537a9 : 0x5f3759df) -
        (i >> 1);  // Initial guess for Newton's method

    y = *reinterpret_cast<T *>(&i);  // Convert new bits into float

    y = y * (1.5 - (x2 * y * y));  // 1st iteration Newton's method
    if (iterations == 2) {
        y = y * (1.5 - (x2 * y * y));  // 2nd iteration, the more exact result
    }
    return y;
}

/**
 * @brief This is the function that calculates the fast inverse square root.
 * The following code is the fast inverse square root with standard lib (cmath)
 * More information can be found at
 * [LinkedIn](https://www.linkedin.com/pulse/fast-inverse-square-root-still-armin-kassemi-langroodi)
 * @tparam T floating type
 * @param number value to calculate
 * @return the inverse square root
 */
template <typename T = double>
T Standard_InvSqrt(T number) {
    T squareRoot = sqrt(number);
    return 1.0f / squareRoot;
}

/**
 * @brief Self-test implementations
 * @returns void
 */
static void test() {
    const float epsilon = 1e-3f;

    /* Tests with multiple values */
    assert(std::fabs(Standard_InvSqrt<float>(100.0f) - 0.0998449f) < epsilon);
    assert(std::fabs(Standard_InvSqrt<double>(36.0f) - 0.166667f) < epsilon);
    assert(std::fabs(Standard_InvSqrt(12.0f) - 0.288423f) < epsilon);
    assert(std::fabs(Standard_InvSqrt<double>(5.0f) - 0.447141f) < epsilon);

    assert(std::fabs(Fast_InvSqrt<float, 1>(100.0f) - 0.0998449f) < epsilon);
    assert(std::fabs(Fast_InvSqrt<double, 1>(36.0f) - 0.166667f) < epsilon);
    assert(std::fabs(Fast_InvSqrt(12.0f) - 0.288423) < epsilon);
    assert(std::fabs(Fast_InvSqrt<double>(5.0f) - 0.447141) < epsilon);

    /* Neighbouring input worked out by hand: 1 / sqrt(25) = 0.2, checked
     * against the relative error bounds for 1 and 2 iterations */
    assert(std::fabs(Fast_InvSqrt<float, 1>(25.0f) - 0.2f) < 0.2f * 1.8e-3f);
    assert(std::fabs(Fast_InvSqrt<double, 2>(25.0) - 0.2) < 0.2 * 5e-6);
    std::cerr << "[Fast_InvSqrt] 10 checks passed" << std::endl;
}

/**
 * @brief Main function
 * @returns 0 on exit
 */
int main() {
    test();  // run self-test implementations
    std::cout << "The Fast inverse square root of 36 is: "
              << Fast_InvSqrt<float, 1>(36.0f) << std::endl;
    std::cout << "The Fast inverse square root of 36 is: "
              << Fast_InvSqrt<double, 2>(36.0f) << " (2 iterations)"
              << std::endl;
    std::cout << "The Fast inverse square root of 100 is: "
              << Fast_InvSqrt(100.0f)
              << " (With default template type and iterations: double, 2)"
              << std::endl;
    std::cout << "The Standard inverse square root of 36 is: "
              << Standard_InvSqrt<float>(36.0f) << std::endl;
    std::cout << "The Standard inverse square root of 100 is: "
              << Standard_InvSqrt(100.0f)
              << " (With default template type: double)" << std::endl;
}
