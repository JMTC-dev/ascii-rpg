# Lesson 16a: Unit-Testing Your Game Logic – Write Code That Can't Lie

By now your ASCII-RPG is growing quickly: combat calculations, inventory stacks, AI decisions... Bugs can sneak in anywhere.  **Unit tests** give you a safety net so new features don't break old ones.

> Time required: 25-30 minutes

---
## 1.  What Is a Unit Test?

A *unit* is the smallest chunk of code that makes sense in isolation – usually one function.  A *unit test* runs that function with known inputs and automatically checks the output.

Think of it as an exam for your code.  Pass = green.  Fail = red with a message telling you exactly what broke.

---
## 2.  Choosing a C Testing Approach

Full-blown frameworks (Check, Unity, CMocka) exist, but for a small tutorial project a **header-only assert style** is perfect.  We will:

1. Write test functions that call your game logic.
2. Use the standard macro `assert()` (from `<assert.h>`).
3. Compile with a `-DUNIT_TEST` flag so tests stay out of the final game binary.

---
## 3.  Example – Testing `ApplyDamage()`

Suppose `combat.c` implements:

```c
// combat.h
#ifndef COMBAT_H
#define COMBAT_H
int ApplyDamage(int hp, int dmg);
#endif

// combat.c
#include "combat.h"
int ApplyDamage(int hp, int dmg) {
    hp -= dmg;
    if (hp < 0) hp = 0;
    return hp;
}
```

Create `combat_test.c`:

```c
#include <assert.h>
#include "combat.h"

static void test_zero_damage() {
    assert(ApplyDamage(10, 0) == 10);
}

static void test_lethal_damage() {
    assert(ApplyDamage(5, 20) == 0);
}

static void test_regular_damage() {
    assert(ApplyDamage(10, 3) == 7);
}

int main() {
    test_zero_damage();
    test_lethal_damage();
    test_regular_damage();
    return 0;   // all good – process exits with 0
}
```

Run the test:

```bash
gcc -DUNIT_TEST -g combat.c combat_test.c -o combat_test
./combat_test && echo "✅  All tests passed!"
```

If any assertion fails, the program aborts and prints the line number.

---
## 4.  Automating Multiple Tests

Put each test file next to the module it checks: `inventory_test.c`, `ai_test.c`, etc.  Then create a mini makefile (or shell script) called `run_tests.sh`:

```bash
#!/usr/bin/env bash
set -e  # stop on first failure

TESTS=(combat_test inventory_test ai_test)
for t in "${TESTS[@]}"; do
    echo "Running $t…"
    gcc -DUNIT_TEST -g ${t%.c}.c ${t}.c -o $t
    ./$t
done

echo "🎉  All unit tests passed!"
```

Make it executable and run:

```bash
chmod +x run_tests.sh
./run_tests.sh
```

> ✔️  Tip: add `./run_tests.sh` to your Git pre-push hook so broken code never lands in the repo.

---
## 5.  Keeping Tests Out of Release Builds

Inside any module that needs test helpers:

```c
#ifdef UNIT_TEST
// extra debug or helper code only visible during tests
#endif
```

When you compile the *real* game, simply omit `-DUNIT_TEST` and nothing extra ships in your binary.

---
## 6.  Challenge Exercises

1. Write a unit test verifying that picking up an item increases `inventoryCount` and the correct slot's quantity.
2. Add an *edge-case* test for `ApplyDamage()` where damage equals `INT_MAX`.
3. Create a failing test first (*TDD style*), then write code to make it pass.

Once you're confident with automated tests, head back to **Lesson 17 – Inventory & Items**. 