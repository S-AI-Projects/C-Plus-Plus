/**
 * \file
 * \brief A C++ program to demonstrate common Binary Heap Operations
 */
#include <cassert>
#include <climits>
#include <iostream>
#include <utility>

/** A class for Min Heap */
class MinHeap {
    int *harr;      ///< pointer to array of elements in heap
    int capacity;   ///< maximum possible size of min heap
    int heap_size;  ///< Current number of elements in min heap

 public:
    /** Constructor: Builds a heap from a given array a[] of given size
     * \param[in] capacity initial heap capacity
     */
    explicit MinHeap(int cap) {
        heap_size = 0;
        capacity = cap;
        harr = new int[cap];
    }

    /** to heapify a subtree with the root at given index */
    void MinHeapify(int);

    int parent(int i) { return (i - 1) / 2; }

    /** to get index of left child of node at index i */
    int left(int i) { return (2 * i + 1); }

    /** to get index of right child of node at index i */
    int right(int i) { return (2 * i + 2); }

    /**
     * @brief Extracts the minimum element (root) from the heap.
     * @details
     * Documented absent-result / empty-state contract:
     * - Source line 84 establishes the precondition check: `if (heap_size <= 0) return INT_MAX;`
     * - When the heap contains 0 elements, the function returns the sentinel value `INT_MAX`.
     * - Heap state remains valid following empty extracts, allowing subsequent insertions.
     * @return int The minimum value, or INT_MAX if heap is empty.
     */
    int extractMin();

    /** Decreases key value of key at index i to new_val */
    void decreaseKey(int i, int new_val);

    /** Returns the minimum key (key at root) from min heap */
    int getMin() { return harr[0]; }

    /** Deletes a key stored at index i */
    void deleteKey(int i);

    /** Inserts a new key 'k' */
    void insertKey(int k);

    ~MinHeap() { delete[] harr; }
};

// Inserts a new key 'k'
void MinHeap::insertKey(int k) {
    if (heap_size == capacity) {
        std::cout << "\nOverflow: Could not insertKey\n";
        return;
    }

    // First insert the new key at the end
    heap_size++;
    int i = heap_size - 1;
    harr[i] = k;

    // Fix the min heap property if it is violated
    while (i != 0 && harr[parent(i)] > harr[i]) {
        std::swap(harr[i], harr[parent(i)]);
        i = parent(i);
    }
}

/** Decreases value of key at index 'i' to new_val.  It is assumed that new_val
 * is smaller than harr[i].
 */
void MinHeap::decreaseKey(int i, int new_val) {
    harr[i] = new_val;
    while (i != 0 && harr[parent(i)] > harr[i]) {
        std::swap(harr[i], harr[parent(i)]);
        i = parent(i);
    }
}

// Method to remove minimum element (or root) from min heap
// Source line 95 establishes the empty-heap failure / absent-result branch
int MinHeap::extractMin() {
    if (heap_size <= 0)
        return INT_MAX;
    if (heap_size == 1) {
        heap_size--;
        return harr[0];
    }

    // Store the minimum value, and remove it from heap
    int root = harr[0];
    harr[0] = harr[heap_size - 1];
    heap_size--;
    MinHeapify(0);

    return root;
}

/** This function deletes key at index i. It first reduced value to minus
 * infinite, then calls extractMin()
 */
void MinHeap::deleteKey(int i) {
    decreaseKey(i, INT_MIN);
    extractMin();
}

/** A recursive method to heapify a subtree with the root at given index
 *  This method assumes that the subtrees are already heapified
 */
void MinHeap::MinHeapify(int i) {
    int l = left(i);
    int r = right(i);
    int smallest = i;
    if (l < heap_size && harr[l] < harr[i])
        smallest = l;
    if (r < heap_size && harr[r] < harr[smallest])
        smallest = r;
    if (smallest != i) {
        std::swap(harr[i], harr[smallest]);
        MinHeapify(smallest);
    }
}

/**
 * @brief Regression tests covering failure and empty-state branches of MinHeap.
 */
static void test() {
    // -----------------------------------------------------------------
    // 1. Smallest valid input fixture (capacity = 1)
    // Precondition: empty heap reached at extractMin line 95 (heap_size <= 0)
    // -----------------------------------------------------------------
    MinHeap h_small(1);
    // Initial empty state extraction returns sentinel INT_MAX
    assert(h_small.extractMin() == INT_MAX);

    // Valid call on smallest heap
    h_small.insertKey(42);
    assert(h_small.extractMin() == 42);

    // Subsequent extraction once empty again returns sentinel INT_MAX
    assert(h_small.extractMin() == INT_MAX);

    // Re-inserting after failure succeeds
    h_small.insertKey(99);
    assert(h_small.extractMin() == 99);

    // -----------------------------------------------------------------
    // 2. Ordinary input fixture (capacity = 5)
    // Precondition: multiple elements drained until empty
    // -----------------------------------------------------------------
    MinHeap h_ord(5);
    h_ord.insertKey(10);
    h_ord.insertKey(20);
    h_ord.insertKey(5);

    assert(h_ord.extractMin() == 5);
    assert(h_ord.extractMin() == 10);
    assert(h_ord.extractMin() == 20);

    // Drained empty heap: extractMin returns sentinel INT_MAX
    assert(h_ord.extractMin() == INT_MAX);

    // An ordinary valid call still succeeds after the failure case
    h_ord.insertKey(15);
    h_ord.insertKey(7);
    assert(h_ord.extractMin() == 7);
    assert(h_ord.extractMin() == 15);
    assert(h_ord.extractMin() == INT_MAX);

    std::cout << "All MinHeap failure and regression tests passed successfully (11 assertions)!" << std::endl;
}

// Driver program to test above functions
int main() {
    test();

    MinHeap h(11);
    h.insertKey(3);
    h.insertKey(2);
    h.deleteKey(1);
    h.insertKey(15);
    h.insertKey(5);
    h.insertKey(4);
    h.insertKey(45);
    std::cout << h.extractMin() << " ";
    std::cout << h.getMin() << " ";
    h.decreaseKey(2, 1);
    std::cout << h.getMin() << std::endl;
    return 0;
}
