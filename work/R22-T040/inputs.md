# Task R22-T040 Notes — data_structures/cll (circular linked list)

Line numbers refer to `master` before this change.

## Public interface (`cll.h`)

| Method | Params | Returns | Kind | Line (h / cpp) |
|---|---|---|---|---|
| `cll()` | — | — | sets `head = NULL`, `total = 0` | h:19 / cpp:8–11 |
| `~cll()` | — | — | empty; **nodes are never freed** (leak, noted, not fixed) | h:20 / cpp:13–14 |
| `display()` | — | `void` | prints only | h:21 / cpp:17–30 |
| `insert_front(int)` | value | `void` | mutates: new node becomes head, `total++` | h:26 / cpp:33–51 |
| `insert_tail(int)` | value | `void` | mutates: new node after last, `total++` | h:27 / cpp:54–71 |
| `get_size()` | — | `int` | returns `total` | h:28 / cpp:74 |
| `find_item(int)` | value | `bool` | returns value (and prints "List is empty !" on an empty list) | h:29 / cpp:78–91 |
| `operator*()` | — | `int` | returns `head->data` (UB on empty list) | h:34 / cpp:94 |
| `operator++()` | — | `void` | mutates: removes head; `total--` (even on empty list → -1) | h:37 / cpp:98–110 |

There is no `search` or `delete` method: the search is `find_item`, and the closest thing to delete is `operator++`, which drops the head.

Protected state: `node* head; int total;` (h:40–41). Dependencies: `<cctype>`, `<cstdlib>`, `<cstring>`, `<iostream>` (h:4–7); `using namespace std;` (cpp:5); nodes allocated with `new` (cpp:35, 56).

## Output shape (`display`, cpp:17–30)

- Empty: `List is empty !\n` (cpp:19)
- Non-empty: `CLL list: ` then, for each of `total` nodes, `<data> -> `, then the head value again, newline, then `Total element: <total>\n` (cpp:21–28).
- `insert_front` puts the value first (`head = newNode`, cpp:48); `insert_tail` puts it last (cpp:67–68).
- After `insert_tail(1,2,3)` then `insert_front(0)`:
  ```
  CLL list: 0 -> 1 -> 2 -> 3 -> 0
  Total element: 4
  ```
  (confirmed by a run)

## Existing tests

`main_cll.cpp` only prints PASS/FAIL text and never fails the process. It is built by `data_structures/cll/CMakeLists.txt` (`add_executable(cll cll.cpp main_cll.cpp)`), which `data_structures/CMakeLists.txt:20` includes. This is not a glob, so the new `cll_test.cpp` gets its own `add_executable(cll_test ...)` line, which avoids a second `main` in the `cll` target.

## Hand-calculated expectations

| Case | Operations | Expected | Precondition line |
|---|---|---|---|
| Smallest valid | new list | `get_size()==0`, `find_item(5)==false` | cpp:9–10, cpp:79–81 |
| One element | `insert_tail(5)` | `get_size()==1`, `find_item(5)==true` ⚠️ | cpp:59–61, cpp:70 |
| Ordinary | `insert_tail(1,2,3)` | size 3, order 1,2,3, `find_item(2)==true`, `find_item(9)==false` | cpp:62–70 |
| Front insert | then `insert_front(0)` | size 4, order 0,1,2,3 | cpp:41–50 |

## ⚠️ Defect found in `find_item` (documented, NOT fixed)

`find_item` loops `while (current->next != head)` (cpp:84), which stops **before** examining the last node.

Smallest reproduction: `cll l; l.insert_tail(5); l.find_item(5)` returns **false**, expected **true**.
Also on 1,2,3: `find_item(3)` returns **false**.

Per the task rules (behavior unchanged, document defects before any fix), the regression test does **not** assert these two failing expectations. It only asserts `find_item` results that the current code gets right (empty → false, present-but-not-last → true, absent → false). A fix belongs in a separate issue.

## Mutant (Step 4)

Change `total++;` at the end of `insert_tail` (cpp:70) to `total += 2;`.
In one sentence: the list holds one element, but the counter says two, so `get_size()` reports a wrong number.
The input that exposes it: `insert_tail(5)` then `get_size()` returns 2 instead of 1. Named check: `one-element size`.
`total++;` also appears in `insert_front` (cpp:50), so the runner matches the longer, unique insert_tail tail `newNode->next = head;\n    }\n    total++;`.
