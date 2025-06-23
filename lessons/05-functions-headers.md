# Lesson 5: Functions, Headers, and Program Structure

As your programs grow beyond a hundred lines, putting everything in `main.c` becomes a nightmare. Let's learn how to structure larger programs professionally using functions and header files.

## Why Program Organization Matters

Imagine trying to find a single book in a library where all the books are thrown into one giant pile. That's what a single-file program feels like! Good organization is like having shelves, sections, and a catalog. It gives you:
- **Readability**: You can find what you're looking for quickly.
- **Maintainability**: Fixing a bug in one "module" is less likely to break another.
- **Reusability**: You can take a well-designed module (like `player.c`) and use it in another game.
- **Collaboration**: Different people can work on different files simultaneously.

## Deeper Dive into Functions

We've used simple functions, but there's more to learn.

### Function Prototypes

When functions call each other, order matters:

```c
#include <stdio.h>

// This won't compile - printMenu not defined yet!
/*
int main() {
    printMenu();  // ERROR!
    return 0;
}

void printMenu() {
    printf("Game Menu\n");
}
*/

// Solution 1: Define functions before using them
void printMenu() {
    printf("Game Menu\n");
}

int main() {
    printMenu();  // Now it works
    return 0;
}
```

Better solution - use prototypes:

```c
#include <stdio.h>

// Function prototypes (declarations)
void printMenu();
int calculateDamage(int attack, int defense);
void battleTurn(int *playerHP, int *enemyHP);

// Now main can come first!
int main() {
    printMenu();
    
    int damage = calculateDamage(25, 10);
    printf("Damage: %d\n", damage);
    
    return 0;
}

// Function definitions can come after
void printMenu() {
    printf("=== GAME MENU ===\n");
    printf("1. Start Game\n");
    printf("2. Load Game\n");
    printf("3. Quit\n");
}

int calculateDamage(int attack, int defense) {
    int damage = attack - defense;
    return (damage > 0) ? damage : 0;
}
```

### Functions Calling Functions

Functions can call other functions to build complex behavior:

```c
#include <stdio.h>

// Prototypes
void drawBorder();
void drawTitle();
void drawOptions();
void drawCompleteMenu();

int main() {
    drawCompleteMenu();
    return 0;
}

void drawBorder() {
    printf("********************\n");
}

void drawTitle() {
    printf("*   ASCII QUEST    *\n");
}

void drawOptions() {
    printf("* 1. New Game      *\n");
    printf("* 2. Continue      *\n");
    printf("* 3. Exit          *\n");
}

void drawCompleteMenu() {
    drawBorder();
    drawTitle();
    drawBorder();
    drawOptions();
    drawBorder();
}
```

### Static Variables in Functions

Sometimes functions need to remember values between calls:

```c
#include <stdio.h>

int getNextEnemyID() {
    static int nextID = 1;  // Initialized only once!
    return nextID++;        // Return current, then increment
}

void spawnEnemy() {
    int id = getNextEnemyID();
    printf("Spawned enemy #%d\n", id);
}

int main() {
    spawnEnemy();  // Spawned enemy #1
    spawnEnemy();  // Spawned enemy #2
    spawnEnemy();  // Spawned enemy #3
    
    return 0;
}
```

Without `static`, nextID would reset to 1 every time!

## Creating Header Files

Header files (.h) declare what's available, while source files (.c) contain the implementation.

### Your First Header File

Create `player.h`:
```c
// player.h
#ifndef PLAYER_H  // Header guard - prevents double inclusion
#define PLAYER_H

// Structure definition
typedef struct {
    char name[50];
    int health;
    int maxHealth;
    int gold;
    int level;
} Player;

// Function declarations
Player* createPlayer(const char *name);
void freePlayer(Player* player);
void healPlayer(Player *player, int amount);
void damagePlayer(Player *player, int damage);
void displayPlayerStats(const Player *player);
int isPlayerAlive(const Player *player);

#endif  // PLAYER_H
```

Create `player.c`:
```c
// player.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For malloc and free
#include "player.h"  // Include our header

Player* createPlayer(const char *name) {
    Player* newPlayer = (Player*)malloc(sizeof(Player));
    if (newPlayer == NULL) {
        // In a real game, handle memory allocation failure
        return NULL;
    }
    // Use strncpy for safer string copying to prevent buffer overflows.
    strncpy(newPlayer->name, name, sizeof(newPlayer->name) - 1);
    newPlayer->name[sizeof(newPlayer->name) - 1] = '\0'; // Ensure null-termination
    
    newPlayer->health = 100;
    newPlayer->maxHealth = 100;
    newPlayer->gold = 0;
    newPlayer->level = 1;
    return newPlayer;
}

void freePlayer(Player* player) {
    free(player);
}

void healPlayer(Player *player, int amount) {
    player->health += amount;
    if (player->health > player->maxHealth) {
        player->health = player->maxHealth;
    }
    printf("%s healed for %d HP!\n", player->name, amount);
}

void damagePlayer(Player *player, int damage) {
    player->health -= damage;
    if (player->health < 0) {
        player->health = 0;
    }
    printf("%s took %d damage!\n", player->name, damage);
}

void displayPlayerStats(const Player *player) {
    printf("\n=== %s ===\n", player->name);
    printf("Level: %d\n", player->level);
    printf("Health: %d/%d\n", player->health, player->maxHealth);
    printf("Gold: %d\n", player->gold);
    printf("\n");
}

int isPlayerAlive(const Player *player) {
    return player->health > 0;
}
```

Use in `main.c`:
```c
// main.c
#include <stdio.h>
#include "player.h"  // Include our player header

int main() {
    Player* hero = createPlayer("Adventurer");
    
    displayPlayerStats(hero);
    
    damagePlayer(hero, 30);
    healPlayer(hero, 20);
    
    displayPlayerStats(hero);
    
    if (isPlayerAlive(hero)) {
        printf("The adventure continues!\n");
    }
    
    freePlayer(hero); // Always free memory you allocate!
    return 0;
}
```

### Compiling Multiple Files

```bash
gcc -c player.c -o player.o    # Compile player.c to object file
gcc -c main.c -o main.o        # Compile main.c to object file  
gcc player.o main.o -o game    # Link them together
./game                         # Run the game
```

Or all at once:
```bash
gcc player.c main.c -o game
```

## Header Guards Explained

Header guards prevent problems when a header is included multiple times:

```c
// Without guards - BAD!
// player.h
typedef struct {
    int health;
} Player;

// If included twice, you get:
// "error: redefinition of 'struct Player'"
```

```c
// With guards - GOOD!
// player.h
#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int health;
} Player;

#endif

// First include: PLAYER_H not defined, so code is included
// Second include: PLAYER_H is defined, so code is skipped
```

## Organizing a Game Project

Here's a good structure for game projects:

```
mygame/
├── main.c          # Main program
├── player.h        # Player header
├── player.c        # Player implementation
├── enemy.h         # Enemy header
├── enemy.c         # Enemy implementation
├── world.h         # World/map header
├── world.c         # World implementation
└── Makefile        # Build instructions (optional)
```

### Example: Enemy Module

Create `enemy.h`:
```c
// enemy.h
#ifndef ENEMY_H
#define ENEMY_H

typedef struct {
    char name[50];
    int health;
    int attack;
    int goldReward;
} Enemy;

Enemy createGoblin();
Enemy createOrc();
void displayEnemy(Enemy *enemy);
int isEnemyAlive(Enemy *enemy);

#endif
```

Create `enemy.c`:
```c
// enemy.c
#include <stdio.h>
#include <string.h>
#include "enemy.h"

Enemy createGoblin() {
    Enemy goblin;
    strncpy(goblin.name, "Goblin", sizeof(goblin.name) - 1);
    goblin.name[sizeof(goblin.name) - 1] = '\0';
    goblin.health = 30;
    goblin.attack = 10;
    goblin.goldReward = 20;
    return goblin;
}

Enemy createOrc() {
    Enemy orc;
    strncpy(orc.name, "Orc", sizeof(orc.name) - 1);
    orc.name[sizeof(orc.name) - 1] = '\0';
    orc.health = 50;
    orc.attack = 15;
    orc.goldReward = 40;
    return orc;
}

void displayEnemy(Enemy *enemy) {
    printf("%s - HP: %d, ATK: %d\n", 
           enemy->name, enemy->health, enemy->attack);
}

int isEnemyAlive(Enemy *enemy) {
    return enemy->health > 0;
}
```

## Common Patterns

### Constants in Headers

```c
// config.h
#ifndef CONFIG_H
#define CONFIG_H

// Game constants
#define MAX_PLAYER_HEALTH 100
#define STARTING_GOLD 50
#define MAX_INVENTORY_SIZE 20

// Screen dimensions
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

// Gameplay values
#define POTION_HEAL_AMOUNT 25
#define CRITICAL_HIT_CHANCE 0.1

#endif
```

### Shared Types

```c
// types.h
#ifndef TYPES_H
#define TYPES_H

// Common types used across modules
typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    int width;
    int height;
} Size;

typedef enum {
    ITEM_POTION,
    ITEM_SWORD,
    ITEM_SHIELD,
    ITEM_KEY
} ItemType;

#endif
```

## "Try This" Challenges

1.  **Math Utility Module:**
    a. Create `math_utils.h` and `math_utils.c`.
    b. In the header, declare three functions: `int min(int a, int b)`, `int max(int a, int b)`, and `int clamp(int value, int min, int max)`.
    c. In the `.c` file, implement these functions.
    d. In `main.c`, `#include "math_utils.h"` and use your new functions to solve a simple problem, like keeping a player's health clamped between 0 and 100.

2.  **Dice Roller Module:**
    a. Create `dice.h` and `dice.c`.
    b. In the header, declare a function `int roll_dice(int num_dice, int num_sides)`.
    c. In the `.c` file, implement the function. It should simulate rolling `num_dice` dice, each with `num_sides` sides, and return the sum. Remember to include `<stdlib.h>` for `rand()`.
    d. In `main.c`, use your dice roller to simulate a player's attack roll (e.g., `roll_dice(2, 6)` for 2d6 damage).

3.  **Project Structure:** Take the `player` and `enemy` modules from this lesson and create a `Makefile` that compiles `main.c`, `player.c`, and `enemy.c` into a single executable named `rpg`.

## Common Mistakes

1. **Forgetting header guards**
   ```c
   // BAD - Will cause redefinition errors
   // types.h
   typedef struct { int x; } Point;
   ```

2. **Including .c files**
   ```c
   // NEVER DO THIS
   #include "player.c"  // NO!
   
   // Always include .h files
   #include "player.h"  // YES!
   ```

3. **Circular dependencies**
   ```c
   // player.h includes enemy.h
   // enemy.h includes player.h
   // This creates a loop!
   ```

## Complete Example: Simple RPG Structure

Let's create a mini RPG with proper organization:

`item.h`:
```c
#ifndef ITEM_H
#define ITEM_H

// Forward-declare the Player struct to avoid circular dependencies.
// This tells the compiler "a struct named Player exists" without needing
// its full definition, which is useful if item.h and player.h include each other.
struct Player;

typedef struct {
    char name[30];
    int healAmount;
    // This is a function pointer, an advanced topic.
    // It lets us assign different functions to different items.
    void (*use)(struct Player* player);
} Item;

Item createPotion();

#endif
```

`battle.h`:
```c
#ifndef BATTLE_H
#define BATTLE_H

#include "player.h"
#include "enemy.h"

typedef struct {
    Player *player;
    Enemy *enemy;
    int turn;
} Battle;

Battle* startBattle(Player *player, Enemy *enemy);
void executeTurn(Battle *battle);
int isBattleOver(Battle *battle);
void endBattle(Battle *battle);

#endif
```

## Building Larger Programs

As programs grow, use a Makefile:

```makefile
# Makefile
CC = gcc
CFLAGS = -Wall -g
OBJECTS = main.o player.o enemy.o battle.o item.o

game: $(OBJECTS)
	$(CC) $(OBJECTS) -o game

main.o: main.c player.h enemy.h battle.h
	$(CC) $(CFLAGS) -c main.c

player.o: player.c player.h
	$(CC) $(CFLAGS) -c player.c

enemy.o: enemy.c enemy.h
	$(CC) $(CFLAGS) -c enemy.c

battle.o: battle.c battle.h player.h enemy.h
	$(CC) $(CFLAGS) -c battle.c

item.o: item.c item.h
	$(CC) $(CFLAGS) -c item.c

clean:
	rm -f *.o game
```

Then just type `make` to build!

## Commit Your Progress

You're now organizing code like a pro! Separating files and using headers is a fundamental skill for building any large-scale application. Let's save this milestone.

```bash
git add .
git commit -m "Complete Lesson 5: Functions and Headers"
```

## Summary

You've learned:
- Function prototypes for better organization
- Static variables in functions
- Creating and using header files
- Header guards to prevent redefinition
- Organizing projects into modules
- Compiling multi-file programs

Good organization makes programming much more manageable!

## Next Steps

Now we'll use everything we've learned to build a complete text-based game before moving to graphics!

Ready to create your first real game? Head to [Lesson 6: Building Your First Text Game](06-text-game.md)!

## Quick Reference

```c
// Header guard pattern
#ifndef MODULENAME_H
#define MODULENAME_H

// Declarations here

#endif

// Function prototype
returnType functionName(parameterTypes);

// Static variable
static int counter = 0;

// Compile multiple files
gcc file1.c file2.c -o program
```

Organization is a superpower in programming - use it!