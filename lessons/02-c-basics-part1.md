# Lesson 2: C Basics Part 1 - Your First Steps in Programming

Welcome back! In this lesson, we'll start learning C programming. We'll take it slow and make sure you understand each concept before moving on.

**Before you begin, please make sure you have completed [Lesson 0: Setting Up Your Development Environment](00-setup.md) and [Lesson 0b: Saving Your Progress with Git](00b-git-basics.md). You should have a working C compiler and be comfortable opening a terminal.**

## What is Programming?

Programming is giving instructions to a computer. Just like a recipe tells you how to make a cake step by step, a program tells a computer what to do step by step.

## Your First C Program

Let's start with the traditional first program. Create a new file called `hello.c`:

```c
#include <stdio.h>

int main() {
    printf("Hello, Game Developer!\n");
    return 0;
}
```

Let's understand every single part:

### Line 1: `#include <stdio.h>`
- This brings in the "Standard Input/Output" library
- Think of it like importing tools - we need `printf` to display text
- The `<>` means it's a system library

### Line 3: `int main() {`
- Every C program starts here - it's the entry point
- `int` means this function returns an integer (number)
- `main` is the special name for where programs start
- `()` holds parameters (none in this case)
- `{` starts the function body

### Line 4: `printf("Hello, Game Developer!\n");`
- `printf` = "print formatted" - it displays text
- Text goes in double quotes `""`
- `\n` creates a new line (like pressing Enter)
- `;` ends the statement (like a period in English)

### Line 5: `return 0;`
- Tells the computer "we're done and everything went well"
- `0` means success
- Every statement needs a semicolon

### Line 6: `}`
- Closes the function body
- Every `{` needs a matching `}`

## Compiling and Running

To turn your code into a program:

1. Save the file as `hello.c`
2. Open a terminal/command prompt
3. Navigate to your file's location
4. Compile: `gcc hello.c -o hello`
   - `gcc` is the compiler
   - `hello.c` is your source code
   - `-o hello` names the output file
5. Run it:
   - Mac/Linux: `./hello`
   - Windows: `hello.exe`

You should see: `Hello, Game Developer!`

## Variables - Storing Information

Variables are like labeled boxes that store information. Let's learn the basic types:

### Integers (Whole Numbers)

```c
#include <stdio.h>

int main() {
    // Declare a variable
    int playerHealth;
    
    // Assign a value
    playerHealth = 100;
    
    // Or do both at once
    int playerGold = 50;
    
    // Display the values
    printf("Health: %d\n", playerHealth);
    printf("Gold: %d\n", playerGold);
    
    return 0;
}
```

Key points:
- `int` stores whole numbers. We use it for things that can be counted, like health, gold, or score.
- **Descriptive Names**: `playerHealth` is much clearer than `h` or `p1_hp`. Good naming is one of the most important habits in programming!
- **Placeholders**: `printf` uses `%d` as a placeholder. It looks at the variables you provide after the string and slots them in.
- **Mutability**: Variables can change, which is essential for a game state that's always updating.

### Characters (Single Letters/Symbols)

```c
#include <stdio.h>

int main() {
    char playerSymbol = '@';
    char wallSymbol = '#';
    char floorSymbol = '.';
    
    printf("The player looks like: %c\n", playerSymbol);
    printf("Walls look like: %c\n", wallSymbol);
    printf("Floor looks like: %c\n", floorSymbol);
    
    return 0;
}
```

Key points:
- `char` stores a single character, perfect for representing tiles like walls (`#`) or the player (`@`).
- **Single vs. Double Quotes**: In C, single quotes `'` are for single characters, while double quotes `"` are for strings (sequences of characters). This is a strict rule!
- `%c` in `printf` is the placeholder for a single character.

### Floating-Point Numbers (Decimals)

What if you need to store a value with a decimal point, like an item's weight or a precise position? You need a floating-point number.

```c
#include <stdio.h>

int main() {
    // 'float' is for single-precision decimals
    float itemWeight = 2.5f; // The 'f' is good practice for floats
    
    // 'double' is for double-precision (more accurate) decimals
    double playerX = 10.75;
    
    printf("The potion weighs: %f kg\n", itemWeight);
    printf("Player X position: %f\n", playerX);
    
    // You can control the number of decimal places shown
    printf("The potion weighs: %.2f kg\n", itemWeight);

    return 0;
}
```

Key points:
- `float` and `double` store numbers with decimal points. `double` is more precise and generally a safer default choice unless you have specific memory or performance reasons to use `float`.
- Use `%f` in `printf` for both `float` and `double`.
- Appending `f` to a number (like `2.5f`) tells the compiler it's a `float`. This can prevent subtle bugs and warnings.

### Integer vs. Float Division
Watch what happens here:
```c
#include <stdio.h>

int main() {
    int a = 5;
    int b = 2;
    printf("Integer division 5 / 2 = %d\n", a / b); // Result is 2

    float c = 5.0f;
    float d = 2.0f;
    printf("Float division 5.0 / 2.0 = %f\n", c / d); // Result is 2.5

    return 0;
}
```
When C divides two integers, it throws away the remainder! To get a decimal result, at least one of the numbers must be a floating-point type.

### Boolean Values (True/False)

In games, we often need to track whether something is true or false:

```c
#include <stdio.h>
#include <stdbool.h>  // Need this for bool type

int main() {
    // Using bool type (modern C way)
    bool gameRunning = true;
    bool playerAlive = true;
    bool hasKey = false;
    
    // Display boolean values
    printf("Game running: %d\n", gameRunning);  // Shows 1 for true
    printf("Player alive: %d\n", playerAlive);  // Shows 1 for true
    printf("Has key: %d\n", hasKey);            // Shows 0 for false
    
    // Using booleans in conditions
    if (playerAlive) {
        printf("Player can continue!\n");
    }
    
    if (!hasKey) {  // ! means "not"
        printf("You need to find a key!\n");
    }
    
    return 0;
}
```

Key points:
- Include `<stdbool.h>` to use `bool`, `true`, and `false`
- `true` equals 1, `false` equals 0
- This is cleaner than using integers for true/false
- Use `!` to check for "not" (opposite)

### "Try This" Challenge 1

Create a program that declares variables for a monster's stats:
1. An `int` for its health, initialized to `75`.
2. A `char` for its symbol, initialized to `'g'` (for goblin).
3. A `float` for its attack speed, initialized to `1.5f`.
4. Print all three variables with descriptive text, like "Goblin ('g') has 75 HP and attacks every 1.5 seconds."

## Basic Math

Games involve lots of calculations. Let's learn basic math operations:

```c
#include <stdio.h>

int main() {
    int health = 100;
    int damage = 30;
    int healing = 20;
    
    // Subtraction
    health = health - damage;  // health is now 70
    printf("After taking damage: %d\n", health);
    
    // Addition
    health = health + healing; // health is now 90
    printf("After healing: %d\n", health);
    
    // Multiplication
    int doubleDamage = damage * 2;  // 60
    printf("Double damage would be: %d\n", doubleDamage);
    
    // Division
    int halfHealth = health / 2;    // 45
    printf("Half health is: %d\n", halfHealth);
    
    return 0;
}
```

### Shorthand Operators

C provides shortcuts for common operations:

```c
#include <stdio.h>

int main() {
    int score = 0;
    
    // Long way
    score = score + 10;
    
    // Short way (these are the same!)
    score += 10;    // Add 10 to score
    
    // Other shortcuts
    score -= 5;     // Subtract 5
    score *= 2;     // Multiply by 2
    score /= 3;     // Divide by 3
    
    // Special shortcuts for adding/subtracting 1
    score++;        // Add 1
    score--;        // Subtract 1
    
    printf("Final score: %d\n", score);
    
    return 0;
}
```

### "Try This" Challenge 2

Create a simple damage calculator:
1. Start with `100` health.
2. The player is hit by a goblin for `12` damage.
3. The player is then hit by a skeleton for `18` damage.
4. The player finds a small potion and heals for `20` health.
5. Use shorthand operators (`+=`, `-=`) for all calculations.
6. Print the player's health after each event.

## Making Decisions with If Statements

Games constantly make decisions. Let's learn how:

### Basic If Statement

```c
#include <stdio.h>

int main() {
    int playerHealth = 25;
    
    // Simple if
    if (playerHealth < 50) {
        printf("Warning: Low health!\n");
    }
    
    // Check for game over
    if (playerHealth <= 0) {
        printf("Game Over!\n");
    }
    
    return 0;
}
```

How it works:
- `if` checks if something is true
- Condition goes in parentheses `()`
- Code to run goes in braces `{}`
- Only runs if condition is true

### If-Else Statements

```c
#include <stdio.h>

int main() {
    int playerGold = 75;
    int swordCost = 100;
    
    if (playerGold >= swordCost) {
        printf("You can afford the sword!\n");
    } else {
        printf("Not enough gold. You need %d more.\n", 
               swordCost - playerGold);
    }
    
    return 0;
}
```

### Comparison Operators

```c
==  // Equal to (note: two equals signs!)
!=  // Not equal to
<   // Less than
>   // Greater than
<=  // Less than or equal to
>=  // Greater than or equal to
```

Common mistake: Using `=` instead of `==`
- `=` assigns a value
- `==` compares values

### Multiple Conditions

```c
#include <stdio.h>

int main() {
    int playerLevel = 5;
    int enemyLevel = 8;
    
    if (enemyLevel > playerLevel + 3) {
        printf("Enemy is too strong! Run!\n");
    } else if (enemyLevel > playerLevel) {
        printf("This will be challenging.\n");
    } else if (enemyLevel == playerLevel) {
        printf("Evenly matched!\n");
    } else {
        printf("You should win easily.\n");
    }
    
    return 0;
}
```

### "Try This" Challenge 3

Write a program that checks if a player can enter a special area.
1. Create an `int` for `playerLevel` and a `bool` for `hasMagicKey`.
2. The area requires the player to be at least level 10 **AND** have the magic key.
3. Use an `if` statement with the `&&` (AND) operator to check both conditions.
4. Print one message if they can enter, and a different one if they cannot.
5. **Bonus:** Change the requirement to be level 20 **OR** have the key. How does the `if` statement change?

## Comments - Notes in Your Code

Comments help explain your code:

```c
#include <stdio.h>

int main() {
    // This is a single-line comment
    // Use these for quick notes
    
    int health = 100;  // You can also put comments at the end of lines
    
    /*
     * This is a multi-line comment
     * Use these for longer explanations
     * Like explaining complex game logic
     */
    
    /* Comments are ignored by the compiler */
    // printf("This won't print!");  // Because it's commented out
    
    return 0;
}
```

Best practices:
- Comment WHY, not WHAT
- Bad: `int health = 100; // Set health to 100`
- Good: `int health = 100; // Players start at full health`

## Common Beginner Mistakes

1. **Forgetting semicolons**
   ```c
   int health = 100  // ERROR: Missing semicolon
   ```

2. **Wrong comparison operator**
   ```c
   if (health = 0)   // ERROR: This assigns 0 to health!
   if (health == 0)  // CORRECT: This checks if health equals 0
   ```

3. **Mismatched braces**
   ```c
   if (health < 50) {
       printf("Low health!");
   // ERROR: Missing closing brace
   ```

4. **Case sensitivity**
   ```c
   int Health = 100;
   printf("%d", health);  // ERROR: 'health' vs 'Health'
   ```

## Practice Project: Health Status Checker

Let's combine everything we've learned:

```c
#include <stdio.h>
#include <stdbool.h>

int main() {
    // Game state
    int playerHealth = 75;
    int maxHealth = 100;
    bool poisoned = true;
    
    // Display current health
    printf("=== HEALTH STATUS ===\n");
    printf("Health: %d/%d\n", playerHealth, maxHealth);
    
    // Check health status
    if (playerHealth <= 0) {
        printf("Status: DEAD\n");
        printf("Game Over!\n");
    } else if (playerHealth < 25) {
        printf("Status: CRITICAL\n");
        printf("Find healing immediately!\n");
    } else if (playerHealth < 50) {
        printf("Status: WOUNDED\n");
        printf("Be careful!\n");
    } else if (playerHealth < maxHealth) {
        printf("Status: HURT\n");
        printf("You've taken some damage.\n");
    } else {
        printf("Status: HEALTHY\n");
        printf("You're in perfect health!\n");
    }
    
    // Check for status effects
    if (poisoned) {
        printf("WARNING: You are poisoned!\n");
    }
    
    // Calculate health percentage
    // We multiply by 100 first to avoid integer division issues.
    // For example, (75 / 100) * 100 would be 0 * 100 = 0.
    // But (75 * 100) / 100 is 7500 / 100 = 75.
    int healthPercent = (playerHealth * 100) / maxHealth;
    printf("Health percentage: %d%%\n", healthPercent);
    
    return 0;
}
```

## Exercises

### Exercise 1: Gold Counter
Create a program that:
- Starts with 0 gold
- Finds 3 treasure chests with 25 gold each
- Buys a potion for 30 gold
- Displays gold after each action
- Checks if you can afford a sword (100 gold)

### Exercise 2: Level Checker
Create a program that:
- Has a player level (you choose)
- Checks if they can enter different areas:
  - Forest: Requires level 1+
  - Cave: Requires level 5+
  - Castle: Requires level 10+
- Prints which areas they can access

### Exercise 3: Damage Calculator
Create a program that:
- Has player attack power
- Has enemy defense
- Calculates damage (attack - defense)
- Makes sure damage is never negative
- Prints the result

## Summary

You've learned:
- How to write and run C programs
- Variables (int and char)
- Basic math operations
- If statements for decisions
- Comments for documentation
- Common mistakes to avoid

These fundamentals are the building blocks for everything else!

## Next Steps

In the next lesson, we'll learn about:
- Loops (repeating actions)
- More data types
- Simple input from the user
- Creating patterns and simple animations

Ready to continue? Head to [Lesson 3: C Basics Part 2](03-c-basics-part2.md)!

## Quick Reference

```c
// Variables
int number = 42;
char letter = 'A';

// Math
+ - * / %

// Comparisons
== != < > <= >=

// If statements
if (condition) {
    // code
} else {
    // code
}

// Comments
// Single line
/* Multi
   line */
```

Keep this handy as you practice!

## Commit Your Progress

Another lesson down! You've learned the absolute fundamentals of C. Let's save your progress.

```bash
git add .
git commit -m "Complete Lesson 2: C Basics Part 1"
```