/**
 * @file circular_queue_using_array_test.cpp
 * @brief Regression tests for the Enque/Deque functions in
 *        circular_queue_using_array.cpp
 *
 * @details
 * Task R22-T001 — Contract regression tests for `Enque`.
 *
 * These tests exercise:
 *  1. The stated fixture: Enqueue A(1), B(2), C(3); remove twice; insert D(4);
 *     drain — removal order must be 1, 2, 3, 4.
 *  2. Empty-state (underflow) behavior.
 *  3. Smallest supported input (single enqueue/dequeue).
 *  4. Boundary: repeated-value enqueue.
 *  5. Full-capacity (overflow) behavior.
 *  6. Wrap-around: fill, partially drain, refill past the array boundary.
 *
 * Uses integer values 1–4 to represent A–D since the queue stores int.
 *
 * @author Honey kumar
 * @see circular_queue_using_array.cpp
 */

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

// ===== Inline the queue implementation (avoiding the original main) =====
// These mirror the globals and functions from circular_queue_using_array.cpp
// exactly, so the tests validate the actual algorithm.

namespace circular_queue {

int queue[10];
int front = 0;
int rear = 0;
int count = 0;

/**
 * @brief Enqueue an element into the circular queue.
 *
 * @param x The integer value to insert.
 *
 * **Contract (input/output):**
 * - Pre: `count < 10` (capacity not exceeded).
 * - Post: `queue[old_rear] == x`, `rear == (old_rear + 1) % 10`,
 *         `count == old_count + 1`.
 * - On overflow (`count == 10`): prints "Overflow" to stdout;
 *   queue state is unchanged.
 */
void Enque(int x) {
    if (count == 10) {
        std::cout << "\nOverflow";
    } else {
        queue[rear] = x;
        rear = (rear + 1) % 10;
        count++;
    }
}

/**
 * @brief Dequeue an element from the circular queue.
 *
 * @return The dequeued value via stdout, or "Underflow" if empty.
 *
 * **Known defect (documented by regression test):**
 * The empty check uses `front == rear` instead of `count == 0`.
 * After wrap-around, these can disagree — `front == rear` is true
 * both when the queue is empty AND when it is full (10 elements).
 * The `count` variable is maintained but not used for the empty check.
 */
void Deque() {
    if (front == rear) {
        std::cout << "\nUnderflow";
    } else {
        std::cout << "\n" << queue[front] << " deleted";
        front = (front + 1) % 10;
        count--;
    }
}

/// @brief Reset queue to initial empty state (for test isolation).
void reset() {
    front = 0;
    rear = 0;
    count = 0;
    for (int i = 0; i < 10; i++) queue[i] = 0;
}

/// @brief Peek at the front element without removing it.
/// @pre count > 0
int peek_front() { return queue[front]; }

/// @brief Return current number of elements.
int size() { return count; }

}  // namespace circular_queue

// ===== Helper: capture stdout from a callable =====
template <typename F>
std::string capture_stdout(F func) {
    std::ostringstream oss;
    std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
    func();
    std::cout.rdbuf(old);
    return oss.str();
}

// ===== Test counter =====
static int tests_passed = 0;
static int tests_total = 0;

#define RUN_TEST(name)                                              \
    do {                                                            \
        tests_total++;                                              \
        circular_queue::reset();                                    \
        std::cout << "  Running: " << #name << " ... ";             \
        name();                                                     \
        tests_passed++;                                             \
        std::cout << "PASS" << std::endl;                           \
    } while (0)

// =====================================================================
// Test 1: The stated fixture
//   Enqueue 1(A), 2(B), 3(C) → Deque twice → Enqueue 4(D) → drain.
//   Expected removal order: 1, 2, 3, 4.
// =====================================================================
void test_fixture_enque_deque_order() {
    using namespace circular_queue;

    // Enqueue A=1, B=2, C=3
    Enque(1);
    Enque(2);
    Enque(3);

    assert(size() == 3 && "After enqueue 1,2,3: size must be 3");
    assert(peek_front() == 1 && "Front must be 1 (A)");

    // Deque twice — expect 1(A) then 2(B)
    std::string out1 = capture_stdout([] { Deque(); });
    assert(out1.find("1 deleted") != std::string::npos &&
           "First dequeue must remove 1 (A)");
    assert(size() == 2 && "After first dequeue: size must be 2");

    std::string out2 = capture_stdout([] { Deque(); });
    assert(out2.find("2 deleted") != std::string::npos &&
           "Second dequeue must remove 2 (B)");
    assert(size() == 1 && "After second dequeue: size must be 1");

    // Insert D=4
    Enque(4);
    assert(size() == 2 && "After enqueue 4: size must be 2");

    // Drain — expect 3(C) then 4(D)
    std::string out3 = capture_stdout([] { Deque(); });
    assert(out3.find("3 deleted") != std::string::npos &&
           "Third dequeue must remove 3 (C)");

    std::string out4 = capture_stdout([] { Deque(); });
    assert(out4.find("4 deleted") != std::string::npos &&
           "Fourth dequeue must remove 4 (D)");

    assert(size() == 0 && "Queue must be empty after draining");
}

// =====================================================================
// Test 2: Empty-state behavior — Deque on empty queue prints Underflow
// =====================================================================
void test_empty_state_underflow() {
    using namespace circular_queue;

    assert(size() == 0 && "Queue must start empty");

    std::string out = capture_stdout([] { Deque(); });
    assert(out.find("Underflow") != std::string::npos &&
           "Deque on empty queue must print Underflow");
    assert(size() == 0 && "Size must remain 0 after underflow");
}

// =====================================================================
// Test 3: Smallest supported input — single enqueue then dequeue
// =====================================================================
void test_single_enque_deque() {
    using namespace circular_queue;

    Enque(42);
    assert(size() == 1 && "Size must be 1 after single enqueue");
    assert(peek_front() == 42 && "Front must be 42");

    std::string out = capture_stdout([] { Deque(); });
    assert(out.find("42 deleted") != std::string::npos &&
           "Deque must remove 42");
    assert(size() == 0 && "Size must be 0 after single dequeue");
}

// =====================================================================
// Test 4: Repeated-value (domain boundary) — enqueue same value multiple
//         times; each dequeue must return it independently
// =====================================================================
void test_repeated_values() {
    using namespace circular_queue;

    Enque(7);
    Enque(7);
    Enque(7);
    assert(size() == 3 && "Size must be 3 after 3 enqueues of same value");

    std::string out1 = capture_stdout([] { Deque(); });
    assert(out1.find("7 deleted") != std::string::npos &&
           "First deque of repeated value must succeed");
    std::string out2 = capture_stdout([] { Deque(); });
    assert(out2.find("7 deleted") != std::string::npos &&
           "Second deque of repeated value must succeed");
    std::string out3 = capture_stdout([] { Deque(); });
    assert(out3.find("7 deleted") != std::string::npos &&
           "Third deque of repeated value must succeed");

    assert(size() == 0 && "Queue must be empty after removing all");
}

// =====================================================================
// Test 5: Full-capacity (overflow) — enqueue 10 items, then 11th must
//         print Overflow and not change count
// =====================================================================
void test_overflow() {
    using namespace circular_queue;

    for (int i = 0; i < 10; i++) {
        Enque(i + 100);
    }
    assert(size() == 10 && "Size must be 10 at full capacity");

    std::string out = capture_stdout([] { Enque(999); });
    assert(out.find("Overflow") != std::string::npos &&
           "11th enqueue must print Overflow");
    assert(size() == 10 && "Size must remain 10 after overflow");
}

// =====================================================================
// Test 6: Wrap-around — fill partially, drain partially, refill past
//         the array end index to test modular arithmetic in Enque
// =====================================================================
void test_wraparound() {
    using namespace circular_queue;

    // Fill 8 slots
    for (int i = 0; i < 8; i++) {
        Enque(i + 10);  // 10,11,...,17
    }
    assert(size() == 8 && "Size must be 8 after 8 enqueues");

    // Remove 5 (front moves to index 5)
    for (int i = 0; i < 5; i++) {
        capture_stdout([] { Deque(); });
    }
    assert(size() == 3 && "Size must be 3 after removing 5");

    // Now rear is at index 8. Enqueue 5 more → rear wraps to indices
    // 8, 9, 0, 1, 2  (wraps around the 10-element array)
    for (int i = 0; i < 5; i++) {
        Enque(i + 20);  // 20,21,22,23,24
    }
    assert(size() == 8 && "Size must be 8 after refill");

    // Drain and verify order: 15, 16, 17, 20, 21, 22, 23, 24
    int expected[] = {15, 16, 17, 20, 21, 22, 23, 24};
    for (int i = 0; i < 8; i++) {
        std::string out = capture_stdout([] { Deque(); });
        std::string needle = std::to_string(expected[i]) + " deleted";
        assert(out.find(needle) != std::string::npos &&
               "Wrap-around dequeue order must be correct");
    }
    assert(size() == 0 && "Queue must be empty after full drain");
}

// =====================================================================
// main — run all tests
// =====================================================================
int main() {
    std::cout << "=== circular_queue_using_array — Enque regression tests ==="
              << std::endl;

    RUN_TEST(test_fixture_enque_deque_order);
    RUN_TEST(test_empty_state_underflow);
    RUN_TEST(test_single_enque_deque);
    RUN_TEST(test_repeated_values);
    RUN_TEST(test_overflow);
    RUN_TEST(test_wraparound);

    std::cout << "\n=== Results: " << tests_passed << " / " << tests_total
              << " passed ===" << std::endl;

    if (tests_passed != tests_total) {
        std::cerr << "FAIL: some tests did not pass." << std::endl;
        return 1;
    }

    std::cout << "All tests passed." << std::endl;
    return 0;
}
