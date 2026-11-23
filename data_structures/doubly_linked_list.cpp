#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>

struct node {
    int val;
    node *prev;
    node *next;
};

class double_linked_list {
    node *start;

 public:
    double_linked_list() : start(nullptr) {}
    const node *head() const { return start; }
    void insert(int x);
    void remove(int x);
    void search(int x);
    void show();
    void reverseShow();
};

void double_linked_list::insert(int x) {
    node *t = start;
    if (start != NULL) {
        while (t->next != NULL) {
            t = t->next;
        }
        node *n = new node;
        t->next = n;
        n->prev = t;
        n->val = x;
        n->next = NULL;
    } else {
        node *n = new node;
        n->val = x;
        n->prev = NULL;
        n->next = NULL;
        start = n;
    }
}

void double_linked_list::remove(int x) {
    node *t = start;
    while (t != NULL && t->val != x) {
        t = t->next;
    }
    if (t == NULL) {
        return;
    }
    if (t->prev == NULL) {
        if (t->next == NULL) {
            start = NULL;
        } else {
            start = t->next;
            start->prev = NULL;
        }
    } else if (t->next == NULL) {
        t->prev->next = NULL;
    } else {
        t->prev->next = t->next;
        t->next->prev = t->prev;
    }
    delete t;
}

void double_linked_list::search(int x) {
    node *t = start;
    int found = 0;
    while (t != NULL) {
        if (t->val == x) {
            std::cout << "\nFound";
            found = 1;
            break;
        }
        t = t->next;
    }
    if (found == 0) {
        std::cout << "\nNot Found";
    }
}

void double_linked_list::show() {
    node *t = start;
    while (t != NULL) {
        std::cout << t->val << "\t";
        t = t->next;
    }
}

void double_linked_list::reverseShow() {
    node *t = start;
    while (t != NULL && t->next != NULL) {
        t = t->next;
    }
    while (t != NULL) {
        std::cout << t->val << "\t";
        t = t->prev;
    }
}

static void test() {
    double_linked_list first;
    first.insert(10);
    first.insert(20);
    first.insert(30);

    // Check values and forward/backward links independently for A→B→C.
    const node *a = first.head();
    assert(a != nullptr && a->val == 10 && a->prev == nullptr);
    const node *b = a->next;
    assert(b != nullptr && b->val == 20 && b->prev == a);
    const node *c = b->next;
    assert(c != nullptr && c->val == 30 && c->prev == b && c->next == nullptr);
    assert(b->next == c && c->prev == b);

    // A new instance has independent empty state and cannot replace first's head.
    double_linked_list second;
    assert(second.head() == nullptr);
    second.insert(99);
    assert(second.head() != nullptr && second.head()->val == 99);
    assert(second.head()->prev == nullptr && second.head()->next == nullptr);
    assert(first.head() == a && a->next == b && b->next == c);

    // Smallest supported nonempty list has one node and no links.
    double_linked_list singleton;
    singleton.insert(7);
    assert(singleton.head() != nullptr && singleton.head()->val == 7);
    assert(singleton.head()->prev == nullptr && singleton.head()->next == nullptr);

    // Repeat the fixture in fresh state to catch state carried between runs.
    double_linked_list repeated;
    repeated.insert(10);
    repeated.insert(20);
    repeated.insert(30);
    assert(repeated.head()->val == 10);
    assert(repeated.head()->next->val == 20);
    assert(repeated.head()->next->next->val == 30);
    std::cout << "Native checks passed: 3-node links, singleton, isolated repeat\n";
}

int main() {
    test();
    int choice, x;
    double_linked_list ob;
    do {
        std::cout << "\n1. Insert";
        std::cout << "\n2. Delete";
        std::cout << "\n3. Search";
        std::cout << "\n4. Forward print";
        std::cout << "\n5. Reverse print";
        std::cout << "\n\nEnter you choice : ";
        std::cin >> choice;
        switch (choice) {
        case 1:
            std::cout << "\nEnter the element to be inserted : ";
            std::cin >> x;
            ob.insert(x);
            break;
        case 2:
            std::cout << "\nEnter the element to be removed : ";
            std::cin >> x;
            ob.remove(x);
            break;
        case 3:
            std::cout << "\nEnter the element to be searched : ";
            std::cin >> x;
            ob.search(x);
            break;
        case 4:
            ob.show();
            break;
        case 5:
            ob.reverseShow();
            break;
        }
    } while (choice != 0);
    return 0;
}
