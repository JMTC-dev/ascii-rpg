# Lesson 4: Understanding Pointers and Dynamic Memory

This is the most important lesson in the C programming section. Understanding pointers is what separates a beginner from an intermediate C programmer. It's a challenging topic, so we'll take it slow, use lots of analogies, and build up the concepts step-by-step.

## How Computer Memory Works: A Mental Model

Imagine your computer's memory (RAM) as a gigantic street of tiny, numbered mailboxes.
- Each mailbox has a unique **address** (e.g., mailbox #1, #2, #3, ... up to billions).
- Each mailbox can hold a small amount of **data** (a single byte).
- A **variable** is like a label you put on one or more of these mailboxes. When you declare `int health = 100;`, C finds four empty mailboxes in a row (since an `int` is usually 4 bytes), puts the number 100 inside them, and slaps a "health" label on the first one.

```c
#include <stdio.h>

int main() {
    int health = 100;
    
    // The & operator gets the address (the house number)
    printf("Value of health: %d\n", health);
    printf("Address of health: %p\n", &health);
    
    return 0;
}
```
The `%p` format specifier is used to print memory addresses. The output might be `0x7ffc...` - that's the address where the `health` variable is stored.

## What Is a Pointer?

A **pointer** is a special kind of variable. Instead of holding a value like `100` or `'@'`, it holds a **memory address**. It's a variable whose value is the location of another variable.

-   **A normal variable (`int health`)**: A mailbox containing the number 100.
-   **A pointer (`int* health_ptr`)**: A mailbox containing the *address* of the `health` mailbox.

### Creating and Using Pointers

We use two special operators:
-   `&` (Address-of operator): Gets the memory address of a variable.
-   `*` (Dereference operator): Gets the value *at* a memory address.

```c
#include <stdio.h>

int main() {
    int gold = 50;        // A regular variable holding the value 50.
    int *goldPointer;     // A pointer variable. The * means it holds an address.
    
    goldPointer = &gold;  // Store the ADDRESS of 'gold' in the pointer.
    
    printf("Value of gold:          %d\n", gold);
    printf("Address of gold:        %p\n", &gold);
    printf("Value of goldPointer:   %p\n", goldPointer); // Should be same as above
    
    // To get the value the pointer points to, we DEREFERENCE it with *
    printf("Value AT address held by goldPointer: %d\n", *goldPointer);
    
    // We can also modify the original variable THROUGH the pointer
    *goldPointer = 75; // Go to the address stored in goldPointer, and change the value there to 75
    printf("New value of gold:      %d\n", gold);
    
    return 0;
}
```

## Why Do We Need Pointers?

Pointers solve two major problems in C.

### 1. Changing a Variable Inside a Function

Normally, when you pass a variable to a function, the function gets a *copy*. It can't change the original.

```c
#include <stdio.h>

// This function receives a COPY of the gold variable.
void addGold(int playerGold) {
    playerGold += 10; // This only changes the local copy.
}

// This function receives the ADDRESS of the gold variable.
void addGoldWithPointer(int *playerGoldPtr) {
    // Go to the address and add 10 to the value there.
    *playerGoldPtr += 10; 
}

int main() {
    int gold = 50;
    
    addGold(gold);
    printf("Gold after addGold: %d\n", gold); // Still 50!
    
    addGoldWithPointer(&gold); // Pass the MEMORY ADDRESS of gold
    printf("Gold after addGoldWithPointer: %d\n", gold); // It's 60!
    
    return 0;
}
```

By passing the address (a pointer), we allow the function to reach outside of itself and modify the original variable.

### 2. Working With Large Data Efficiently

Imagine you have a giant `struct` with player data. Copying it to a function every time would be slow. Passing a pointer (just the address) is much faster. We will see this in future lessons.

## NULL Pointers

A pointer can be set to `NULL`. This is a special value that means "this pointer doesn't point to anything." It's a good practice to initialize pointers to `NULL` to prevent them from pointing to random, invalid memory locations.

```c
int *myPointer = NULL;

// Later, you can check if it's safe to use
if (myPointer != NULL) {
    // It's safe to dereference
    *myPointer = 10; 
}
```

## Dynamic Memory Basics (malloc & free)

So far every variable we've used lives **on the stack** – the space C automatically allocates for each function call.  Sometimes you need memory that:
1. Survives after the current function returns, **and**
2. Has a size you only know at run-time (user input, file size, etc.)

That memory lives on the **heap** and we request it manually with `malloc` ( _memory-allocate_ ) and release it with `free`.

```c
#include <stdio.h>
#include <stdlib.h>   // malloc, free

int main() {
    int enemyCount;
    printf("How many enemies? ");
    scanf("%d", &enemyCount); // Note: scanf is simple but can be unsafe.
                              // We'll learn safer input methods later.

    // 1) Ask for enough ints to store every enemy's HP
    int *enemyHP = malloc(enemyCount * sizeof(int));

    if (enemyHP == NULL) {      // ALWAYS check the result!
        printf("Out of memory!\n");
        return 1;
    }

    // 2) Use the memory like a normal array
    for (int i = 0; i < enemyCount; i++) {
        enemyHP[i] = 100;       // Everyone starts with 100 HP
    }

    // ... game logic ...

    // 3) When you're done, give it back to the OS
    free(enemyHP);
    enemyHP = NULL;             // Good hygiene – avoid dangling pointers
    return 0;
}
```

Key rules for safe dynamic memory:
- Every `malloc` / `calloc` / `realloc` **must** have exactly one matching `free`.
- After calling `free(ptr)`, set the pointer to `NULL` so accidental dereferences crash immediately instead of corrupting data.
- Never `free` the same pointer twice ( _double-free_ bug ).
- Check if the pointer returned by `malloc` is `NULL` – it means allocation failed.

### Debugging memory errors

On Linux/macOS, the tool **Valgrind** will catch leaks and invalid frees:
```bash
valgrind --leak-check=full ./mygame
```
Raylib works fine under Valgrind when you build it with `-g` and without optimisation (`-O0`).  We'll revisit Valgrind in Lesson 10b (Debugging).

### Quick tip – calloc
`calloc(count, size)` works like `malloc` but also **zeros** the memory. Great for structs where you want default 0 values.

## "Try This" Challenges

1.  **Pointer Practice:**
    a. Create a `float` variable named `attackPower` and set it to `15.5f`.
    b. Create a pointer `float* powerPtr` that stores the address of `attackPower`.
    c. Print the value of `attackPower` directly.
    d. Print the value of `attackPower` by dereferencing `powerPtr`.
    e. Use the pointer `powerPtr` to change the value of `attackPower` to `20.0f`.
    f. Print the new value of `attackPower` (the original variable) to confirm it changed.

2.  **Function to Modify:** Write a function `void addGold(int* gold_ptr, int amount)` that takes a pointer to a player's gold and adds `amount` to it.
    *   In `main`, create a `playerGold` variable.
    *   Call `addGold()` by passing the *address* of `playerGold`.
    *   Print the gold amount before and after to see it change.

3.  **Dynamic Struct:**
    a. Define a `Monster` struct with `int health;` and `int attack;`.
    b. In `main`, use `malloc` to dynamically create a single `Monster`.
    c. Check if the `malloc` was successful.
    d. If it was, use the `->` operator to set its health and attack values.
    e. Print the monster's stats.
    f. Don't forget to `free` the monster's memory at the end!

## Commit Your Progress

This was the toughest lesson yet, and you made it through. Understanding pointers **and dynamic memory** is a huge milestone for any C programmer. Be proud! Let's commit this major achievement.

```bash
git add .
git commit -m "Complete Lesson 4: Pointers, Memory, and Dynamic Allocation"
```

## Summary

In this lesson, you learned:
- Memory is like a street of **addresses**.
- A **pointer** is a variable that stores a memory address.
- The `&` operator gets the **address** of a variable.
- The `*` operator **dereferences** a pointer to get the value it points to.
- Pointers allow functions to **modify original variables**.
- `NULL` is a safe value for a pointer that points to nothing.
- **Dynamic memory** lives on the heap – use `malloc`/`free` responsibly.
- Tools like **Valgrind** help you detect leaks and invalid memory access.

## Next Steps

Phew! That was a lot. In the next lesson, we'll see how pointers and functions relate to creating larger, more organized programs using header files.

Let's move on to [Lesson 5: Functions, Headers, and Program Structure](05-functions-headers.md). 