#include <iostream>
using std::cin;
using std::cout;

int queue[10];
int front = 0;
int rear = 0;
int count = 0;

/**
 * @brief Enqueue an element into the circular queue.
 *
 * @param x The integer value to insert at the rear of the queue.
 *
 * **Contract:**
 * - Pre-condition : `count < 10` (queue is not full).
 * - Post-condition: `queue[old_rear] == x`,
 *                   `rear == (old_rear + 1) % 10`,
 *                   `count == old_count + 1`.
 * - Overflow      : When `count == 10`, prints "\nOverflow" to stdout
 *                   and leaves the queue unchanged.
 */
void Enque(int x) {
    if (count == 10) {
        cout << "\nOverflow";
    } else {
        queue[rear] = x;
        rear = (rear + 1) % 10;
        count++;
    }
}

void Deque() {
    if (front == rear) {
        cout << "\nUnderflow";
    }

    else {
        cout << "\n" << queue[front] << " deleted";
        front = (front + 1) % 10;
        count--;
    }
}

void show() {
    for (int i = 0; i < count; i++) {
        cout << queue[(i + front) % 10] << "\t";
    }
}

int main() {
    int ch, x;
    do {
        cout << "\n1. Enque";
        cout << "\n2. Deque";
        cout << "\n3. Print";
        cout << "\nEnter Your Choice : ";
        cin >> ch;
        if (ch == 1) {
            cout << "\nInsert : ";
            cin >> x;
            Enque(x);
        } else if (ch == 2) {
            Deque();
        } else if (ch == 3) {
            show();
        }
    } while (ch != 0);

    return 0;
}
