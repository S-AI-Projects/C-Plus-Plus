// Scratch tracing copy of partition3's loop (sorting/quick_sort_3.cpp lines
// 59-82). Used only to produce work/R22-T037/trace.txt; never part of the repo
// source. Build: g++ -std=c++17 trace_scratch.cpp -o trace.bin
#include <cstdint>
#include <cstdio>
#include <utility>
#include <vector>

static void dump(const char *label, int32_t mid, int32_t low, int32_t high,
                 const std::vector<int> &a) {
    std::printf("%s\n  mid=%d low=%d high=%d  [", label, mid, low, high);
    for (size_t k = 0; k < a.size(); k++) std::printf(k ? " %d" : "%d", a[k]);
    std::printf("]\n");
}

static void trace(std::vector<int> a) {
    std::printf("=== Input: [");
    for (size_t k = 0; k < a.size(); k++) std::printf(k ? " %d" : "%d", a[k]);
    std::printf("] ===\n");
    int32_t low = 0, high = static_cast<int32_t>(a.size()) - 1, i, j;
    dump("start:", 0, low, high, a);
    if (high - low <= 1) {  // line 59
        if (a[high] < a[low]) std::swap(a[high], a[low]);
        i = low;
        j = high;
        std::printf("shortcut: i=%d j=%d\n", i, j);
    } else {
        int32_t mid = low;
        int pivot = a[high];  // line 69
        int step = 0;
        char label[96];
        while (mid <= high) {  // line 70
            ++step;
            if (a[mid] < pivot) {
                std::swap(a[low++], a[mid++]);
                std::snprintf(label, sizeof label,
                              "step %d (< pivot %d): swap to left, low++, mid++",
                              step, pivot);
            } else if (a[mid] == pivot) {
                mid++;
                std::snprintf(label, sizeof label, "step %d (== pivot %d): mid++",
                              step, pivot);
            } else {
                std::swap(a[mid], a[high--]);
                std::snprintf(label, sizeof label,
                              "step %d (> pivot %d): swap to right, high--", step,
                              pivot);
            }
            dump(label, mid, low, high, a);
        }
        i = low - 1;  // line 81
        j = mid;      // line 82
        std::printf("result: i=%d j=%d\n", i, j);
    }
    std::printf("final array: [");
    for (size_t k = 0; k < a.size(); k++) std::printf(k ? " %d" : "%d", a[k]);
    std::printf("]  i=%d j=%d\n\n", i, j);
}

int main() {
    trace({3, 1, 2});
    trace({2, 2, 1});
    trace({1, 2, 3});
    trace({3, 2, 1});
    trace({1});
    trace({2, 1});
    return 0;
}
