/**
 * @file
 * @brief Regression test for size accounting in the circular linked list
 * (cll.h / cll.cpp). Uses only the public interface; every check is named and
 * compares a literal expected value. Expected values were hand-calculated in
 * work/R22-T040/inputs.md. Built as its own target (see CMakeLists.txt) so it
 * does not collide with main_cll.cpp's main().
 */
#include <cstdio>
#include <cstdlib>

#include "cll.h"

static int checks = 0;

/** Named, literal-value check: prints ok/FAIL with expected and actual. */
static void check_eq(const char *name, long expected, long actual) {
    ++checks;
    if (expected != actual) {
        std::fprintf(stderr, "FAIL %s: expected %ld, actual %ld\n", name,
                     expected, actual);
        std::exit(1);
    }
    std::printf("ok   %s: expected %ld, actual %ld\n", name, expected, actual);
}

int main() {
    {  // smallest valid input: empty list (constructor: head = NULL cll.cpp:9,
       // total = 0 cll.cpp:10)
        cll list;
        check_eq("empty size", 0, list.get_size());          // cll.cpp:74
        check_eq("empty find_item", 0, list.find_item(5));   // cll.cpp:79-81
    }
    {  // smallest non-empty input
        cll list;
        list.insert_tail(5);  // empty-list branch cll.cpp:59-61, total++ cll.cpp:70
        check_eq("one-element size", 1, list.get_size());  // protects total++
        // NOTE: find_item(5) is NOT checked here. It returns false because
        // the loop at cll.cpp:84 skips the last node (documented defect, see
        // work/R22-T040/inputs.md); this test does not change behavior.
    }
    {  // ordinary input: 1, 2, 3 (tail inserts keep insertion order)
        cll list;
        list.insert_tail(1);  // cll.cpp:59-61
        list.insert_tail(2);  // non-empty branch cll.cpp:62-69
        list.insert_tail(3);
        check_eq("ordinary size", 3, list.get_size());
        check_eq("ordinary find_item present", 1, list.find_item(2));  // cll.cpp:85
        check_eq("ordinary find_item absent", 0, list.find_item(9));   // cll.cpp:89
        check_eq("ordinary head value", 1, *list);  // operator* cll.cpp:94
        list.insert_front(0);  // non-empty branch cll.cpp:41-49, total++ cll.cpp:50
        check_eq("after front insert size", 4, list.get_size());
        check_eq("after front insert head value", 0, *list);  // head = newNode cll.cpp:48
    }
    std::printf("CASES_RUN=%d\n", checks);
    return 0;
}
