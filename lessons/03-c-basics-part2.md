# Lesson 3: C Basics Part 2 - Loops, Arrays, and Functions

Welcome back! Now that you understand variables and decisions, let's learn about repetition, storing multiple values, and organizing code.

## Loops - Repeating Actions

Games are full of repetition: animation frames, checking input, updating enemies. Let's learn how to repeat code efficiently.

### The While Loop

The simplest loop continues while a condition is true:

```c
#include <stdio.h>

int main() {
    int countdown = 5;
    
    printf("Game starting in...\n");
    
    while (countdown > 0) {
        printf("%d...\n", countdown);
        countdown--;  // Decrease by 1
    }
    
    printf("GO!\n");
    
    return 0;
}
```

How it works:
1. Check the condition `(countdown > 0)`
2. If true, run the code in the braces
3. Go back to step 1
4. If false, skip past the loop

### The For Loop

When you know how many times to repeat, use a for loop:

```c
#include <stdio.h>

int main() {
    // Print 5 enemies
    printf("Spawning enemies:\n");
    
    for (int i = 1; i <= 5; i++) {
        printf("Enemy %d appears!\n", i);
    }
    
    return 0;
}
```

Understanding the `for` loop: `for (setup; condition; update)`
- **Setup**: `int i = 1;` runs once at the very beginning. It's where you create your counter variable.
- **Condition**: `i <= 5;` is checked *before* each repetition. If it's true, the loop runs. If it's false, the loop stops.
- **Update**: `i++` runs *after* each repetition. It's how you get closer to the end condition.

## Arrays - Storing Multiple Values

What if you need to track health for multiple enemies? Arrays let you store many values of the same type.

### Creating and Using Arrays

```c
#include <stdio.h>

int main() {
    // Create an array of 3 enemy health values
    int enemyHealth[3];
    
    // Set values individually
    enemyHealth[0] = 50;   // First enemy (index 0)
    enemyHealth[1] = 75;   // Second enemy (index 1)
    enemyHealth[2] = 100;  // Third enemy (index 2)
    
    // Access values
    printf("First enemy health: %d\n", enemyHealth[0]);
    
    return 0;
}
```

**CRITICAL CONCEPT: Zero-Based Indexing**
In C (and most programming languages), array indexes start at **0**.
- The first item is at index `[0]`.
- The second item is at index `[1]`.
- For an array of size `N`, the last item is at index `[N-1]`.
This is a very common source of bugs for new programmers! Always remember to start counting from zero.

### Arrays and Loops - Perfect Together

```c
#include <stdio.h>

int main() {
    int enemyHealth[5] = {50, 50, 50, 50, 50};
    
    // Display all enemy health
    printf("Enemy Status:\n");
    for (int i = 0; i < 5; i++) {
        printf("Enemy %d: %d HP\n", i + 1, enemyHealth[i]);
    }
    
    return 0;
}
```

## Structs - Grouping Related Data

Arrays store many **of the same thing**. What if you want to store **different kinds of data that belong to ONE thing**—for example a player's health and attack power? A `struct` (structure) lets you bundle several variables under one label.

```c
#include <stdio.h>

// 1. Define a new type called 'Monster'
typedef struct {
    int health;
    int attackPower;
    char symbol;
} Monster;   // Monster is now a *type* you can use like int or char

int main() {
    // 2. Create a variable of that type
    Monster goblin;

    // 3. Fill in the fields using dot-notation
    goblin.health = 50;
    goblin.attackPower = 10;
    goblin.symbol = 'g';

    // 4. Access the data
    printf("A goblin ('%c') appears!\n", goblin.symbol);
    printf("It has %d HP and %d attack power.\n", goblin.health, goblin.attackPower);

    return 0;
}
```

Key points:
- The keyword `typedef struct { ... } TypeName;` creates a handy alias so you can write `TypeName` instead of `struct TypeName` everywhere.
- Access fields with the dot operator (`goblin.health`).
- You can nest structs inside structs (e.g., a `Position` struct inside `Player`).

### Arrays of Structs
Structs and arrays work great together:
```c
Monster goblinArmy[5];
goblinArmy[0].health = 50;
```

### Passing Structs to Functions

This `healMonster` function takes a monster, heals it, and returns the *new, updated* monster data.

```c
#include <stdio.h>

// Let's define a simple Monster struct
typedef struct {
    int health;
    int maxHealth;
} Monster;

// This function takes a Monster, heals it, and returns the *new, updated* monster data.
Monster healMonster(Monster m, int amount) { // Takes a Monster directly
    m.health += amount;
    if (m.health > m.maxHealth) {
        m.health = m.maxHealth; // Cap health at max
    }
    return m; // Returns the modified copy
}

int main() {
    Monster goblin = {50, 100}; // A goblin with 50/100 HP
    printf("Goblin has %d HP.\n", goblin.health);

    goblin = healMonster(goblin, 30); // Re-assign the goblin with the healed version

    printf("Goblin now has %d HP.\n", goblin.health);
    return 0;
}
```
> We'll learn a more efficient way to do this with **pointers** in Lesson 4. Passing a copy like this is perfectly fine for now!

## Functions - Organizing Your Code

Functions are reusable pieces of code. Think of them as custom commands you can create and call whenever you need them. They are the single most important tool for keeping your code organized and avoiding repetitive code.

### Your First Function

```c
#include <stdio.h>

// This is a function definition
void showWelcomeMessage() {
    printf("====================\n");
    printf("  Welcome to the Arena! \n");
    printf("====================\n");
}

int main() {
    // This is a function call
    showWelcomeMessage();
    
    // You can call it multiple times
    showWelcomeMessage();
    
    return 0;
}
```

### Functions with Parameters

You can pass information into functions:

```c
#include <stdio.h>

void spawnEnemy(int health, int level) {
    printf("Spawning level %d enemy with %d HP.\n", level, health);
}

int main() {
    spawnEnemy(100, 5); // A level 5 boss
    spawnEnemy(25, 1);  // A level 1 goblin
    return 0;
}
```

### Functions that Return Values

Functions can also send data back:

```c
#include <stdio.h>

int calculateAttackDamage(int strength) {
    int damage = strength * 2;
    return damage; // Send this value back
}

int main() {
    int playerStrength = 15;
    int damageDealt = calculateAttackDamage(playerStrength);
    
    printf("You attack for %d damage!\n", damageDealt);
    
    return 0;
}
```

## "Try This" Challenges

1.  **For Loop Power-up:** Write a program that uses a `for` loop to simulate a character charging up a power move. It should print "Charging... 1", "Charging... 2", ..., "Charging... 5", and finally "POWER BLAST!".

2.  **Array Damage Calculation:** Create an integer array named `damageRolls` with these 5 values: `{8, 12, 5, 15, 9}`. Use a `for` loop to iterate through the array, sum up all the damage, and print the total damage dealt.

3.  **Struct Factory:**
    a. Define a `Potion` struct with two integer fields: `healAmount` and `goldCost`.
    b. In `main`, create two potions: `smallPotion` that heals 20 for 50 gold, and `largePotion` that heals 50 for 120 gold.
    c. Print the stats of both potions in a readable format.

4.  **Function for Clamping Health:** Write a function `int clamp(int value, int min, int max)` that ensures a value stays within a range. If `value` is less than `min`, it should return `min`. If `value` is greater than `max`, it should return `max`. Otherwise, it returns `value`.
    *   **Test it in `main`:** Create a `health` variable, damage it so it goes below 0, and use your `clamp` function to make sure it doesn't stay negative. Print the result. (e.g., `health = clamp(health, 0, 100);`).

## Commit Your Progress

You're really getting the hang of C now! Functions, arrays, and loops are major building blocks. Let's commit your excellent work.

```bash
git add .
git commit -m "Complete Lesson 3: C Basics Part 2"
```

## Summary

In this lesson, you learned about:
- **`while` and `for` loops** for repeating code.
- **Arrays** for storing lists of data.
- **Structs** for grouping related data.
- **Functions** for organizing and reusing code.
- Passing **parameters** to functions.
- **Returning values** from functions.

## Next Steps

This was a big one! With loops, arrays, and functions, you can now build a complete, interactive program. In the next lesson, we'll tackle the most powerful and feared topic in C: pointers.

Ready? Let's go to [Lesson 4: Understanding Pointers and Memory](04-pointers-memory.md). 