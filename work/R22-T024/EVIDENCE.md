# R22-T024 — Test an invariant of `Trie` (data_structures/trie_using_hashmap.cpp)

## What `Trie` does (recorded before any change)

- **Public API:** `insert(word)`, `search(word) -> bool`, `startwith(prefix) -> bool`,
  `delete_word(word)` and `predict_words(prefix) -> vector<string>`.
- **State:** a tree of `Node`s, each with `children` (an `unordered_map<char16_t, shared_ptr<Node>>`) and
  a `word_end` flag. `insert` sets `word_end` on the last node, but never on the root
  (the `curr != root_node` check, line 71), so `""` is never a word.
- **Mutation:** only `insert` and `delete_word` change the trie. `search`, `startwith` and
  `predict_words` only read it.
- **External dependencies:** none.

## Invariant (now documented on the class)

For the set W of non-empty words inserted so far:
1. `search(w)` ⇔ w ∈ W. A prefix is not a word unless it was inserted.
2. `startwith(p)` ⇔ p is a prefix of some word in W.
3. `predict_words(p)` returns exactly the words in W that start with p. The order isn't
   specified, so it's compared after sorting.
4. Inserting never removes another word, and inserting the same word again changes nothing.

**Why it holds:** `insert` only adds nodes and sets flags; it never clears or removes them. `search`
returns `word_end` of the node it reaches (line 94). `startwith` only checks that the path exists.

## Readable fixture

| Step | Call | Expected | Actual |
|---|---|---|---|
| insert car, cat, dog | `search("car")` | true | true |
| | `search("ca")` | false (only a prefix) | false |
| | `startwith("ca")` | true | true |
| | `search("cat")` | true | true |
| | `search("dog")` / `search("do")` | true / false | true / false |
| | `startwith("cab")` | false | false |
| insert "car" again | `search("car")`, `search("cat")`, `search("ca")` | true, true, false | same |
| | `predict_words("ca")` (sorted) | [car, cat] | [car, cat] |

## Generated cases

- Fixed seed `std::mt19937(20261123)`, **200 cases**. Each inserts 1 to 6 random words of length 1 to 4
  over {a, b, c}. That stays inside the real input domain: non-empty ASCII strings.
  `delete_word` isn't used because the invariant is about insertion.
- After every insertion, the trie is compared with a reference `std::set<std::string>` (a separate
  model). Every earlier word must still be found, and `search`, `startwith` and `predict_words` must match
  the model for three queries: the new word, its first letter, and a random string.
- At the end of each case, one word is inserted again and checked.

## The checks fail when the invariant is broken

`check.py` builds a **temporary copy** of the file where `search` ignores `word_end`
(`if (curr->word_end)` becomes `if (true)`), so every prefix counts as a word. The mutant fails the named
fixture assertion:

```
Assertion `trie.search("ca") == false' failed.   (exit code -6)
```

`check.py` requires this failure and reports `FAIL` if the mutant passes. The real source is never
changed: the mutant lives in a temp folder that gets deleted.

## Command and result

```
python3 work/R22-T024/check.py
```

This runs `cmake -S . -B build-R22-T024 -DCMAKE_BUILD_TYPE=Debug -DUSE_OPENMP=OFF`, then
`cmake --build build-R22-T024 --target trie_using_hashmap`, then runs
`build-R22-T024/data_structures/trie_using_hashmap`, then compiles and runs the mutant.

Output: `PASS: 9754 checks passed` (9753 assertions + 1 mutant rejection). Exit code 0. The file also
runs clean under `-fsanitize=address,undefined`. In `main()`, the new invariant tests run first,
followed by the original `test()`, which is unchanged.

## Tool versions

- g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- cmake 3.28.3
- Python 3.13.18
