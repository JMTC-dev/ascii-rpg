# Lesson 6: Checkpoint Project - Your First Text Game

This is our first checkpoint project! It's time to combine everything you've learned so far—variables, loops, functions, structs, and multi-file projects—into a complete, playable game. We'll build a text-based dungeon crawler, which is a perfect way to solidify your understanding of C before we add the complexity of graphics.

## Learning Goals

- **Integration:** Combine multiple modules (`player`, `room`, `main`) into a single working program.
- **Application:** Apply your knowledge of loops, structs, and functions to a real game problem.
- **Project Management:** Use a `Makefile` to compile a project with multiple `.c` and `.h` files.
- **Game Logic:** Implement a complete game loop with input, state changes, and win/loss conditions.

## Step 1: Project Structure

First, let's set up our files. This project will be organized into modules, just like we learned in Lesson 5.

Create the following files:
- `player.h`
- `player.c`
- `room.h`
- `room.c`
- `main.c`
- `Makefile`

## Step 2: The Player Module

This module will handle the player's state.

**`player.h`**
```c
#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int health;
    int gold;
    int currentRoom;
} Player;

Player* createPlayer(int health, int gold, int startRoom);
void freePlayer(Player* player);

#endif
```

**`player.c`**
```c
#include "player.h"
#include <stdlib.h>

Player* createPlayer(int health, int gold, int startRoom) {
    Player* player = (Player*)malloc(sizeof(Player));
    if (player != NULL) {
        player->health = health;
        player->gold = gold;
        player->currentRoom = startRoom;
    }
    return player;
}

void freePlayer(Player* player) {
    free(player);
}
```

## Step 3: The Room Module

This module will define the structure of our dungeon.

**`room.h`**
```c
#ifndef ROOM_H
#define ROOM_H

#define MAX_ROOMS 4

typedef struct {
    const char* description;
    int hasGold;
    int goldAmount;
    int northExit;
    int southExit;
    int eastExit;
    int westExit;
    int hasMonster;
    int monsterHealth;
} Room;

void setupDungeon(Room rooms[]);

#endif
```

**`room.c`**
```c
#include "room.h"

void setupDungeon(Room rooms[]) {
    // Room 0: Entrance
    rooms[0].description = "The dungeon entrance. Paths lead north and east.";
    rooms[0].hasGold = 0;
    rooms[0].goldAmount = 0;
    rooms[0].northExit = 1;
    rooms[0].southExit = -1;
    rooms[0].eastExit = 2;
    rooms[0].westExit = -1;
    rooms[0].hasMonster = 0;

    // Room 1: Guard room
    rooms[1].description = "An old guard room. A goblin stands here.";
    rooms[1].hasGold = 1;
    rooms[1].goldAmount = 20;
    rooms[1].northExit = -1;
    rooms[1].southExit = 0;
    rooms[1].eastExit = -1;
    rooms[1].westExit = -1;
    rooms[1].hasMonster = 1;
    rooms[1].monsterHealth = 30;

    // Room 2: Treasury
    rooms[2].description = "The treasury! Paths lead west and north.";
    rooms[2].hasGold = 1;
    rooms[2].goldAmount = 50;
    rooms[2].northExit = 3;
    rooms[2].southExit = -1;
    rooms[2].eastExit = -1;
    rooms[2].westExit = 0;
    rooms[2].hasMonster = 0;

    // Room 3: Exit
    rooms[3].description = "The exit! Sunlight streams in!";
    rooms[3].hasGold = 0;
    rooms[3].goldAmount = 0;
    rooms[3].northExit = -1;
    rooms[3].southExit = 2;
    rooms[3].eastExit = -1;
    rooms[3].westExit = -1;
    rooms[3].hasMonster = 0;
}
```

## Step 4: The Main Game (`main.c`)

This file will contain the main game loop and tie our modules together.

```c
#include <stdio.h>
#include <stdlib.h>
#include "player.h"
#include "room.h"

void showRoom(const Room* room, const Player* player) {
    printf("\n%s\n", room->description);
    if (room->hasGold) {
        printf("You see %d gold coins!\n", room->goldAmount);
    }
    if (room->hasMonster) {
        printf("A goblin with %d HP blocks your path!\n", room->monsterHealth);
    }
    printf("Health: %d | Gold: %d\n", player->health, player->gold);
}

int main() {
    Player* player = createPlayer(100, 0, 0);
    Room dungeon[MAX_ROOMS];
    setupDungeon(dungeon);

    printf("=== DUNGEON ESCAPE ===\n");

    int gameRunning = 1;
    while (gameRunning) {
        Room* currentRoom = &dungeon[player->currentRoom];
        showRoom(currentRoom, player);

        if (player->currentRoom == 3) {
            printf("\nYou found the exit! YOU WIN!\n");
            break;
        }

        printf("\nCommands: (n)orth, (s)outh, (e)ast, (w)est, (t)ake, (f)ight, (q)uit: ");
        char command;
        scanf(" %c", &command);

        switch(command) {
            case 'n':
                if (currentRoom->northExit != -1) player->currentRoom = currentRoom->northExit;
                else printf("You can't go that way.\n");
                break;
            // ... (implement other directions: s, e, w) ...
            case 't':
                if (currentRoom->hasGold) {
                    player->gold += currentRoom->goldAmount;
                    currentRoom->hasGold = 0;
                    printf("You take the gold!\n");
                } else {
                    printf("No gold here.\n");
                }
                break;
            case 'f':
                if (currentRoom->hasMonster) {
                    printf("You fight the goblin!\n");
                    currentRoom->monsterHealth -= 20; // Simple combat
                    if (currentRoom->monsterHealth <= 0) {
                        printf("You defeated the goblin!\n");
                        currentRoom->hasMonster = 0;
                    }
                } else {
                    printf("Nothing to fight.\n");
                }
                break;
            case 'q':
                gameRunning = 0;
                break;
            default:
                printf("Unknown command.\n");
        }
    }

    printf("\nThanks for playing!\n");
    freePlayer(player);
    return 0;
}
```
*(Note: For brevity, not all direction commands are implemented in the snippet above, but you should add them.)*

## Step 5: The Makefile

To compile our multi-file project, we need a `Makefile`.

```makefile
# Makefile for Dungeon Escape
CC = gcc
CFLAGS = -Wall -g
TARGET = game

all: $(TARGET)

$(TARGET): main.o player.o room.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o player.o room.o

main.o: main.c player.h room.h
	$(CC) $(CFLAGS) -c main.c

player.o: player.c player.h
	$(CC) $(CFLAGS) -c player.c

room.o: room.c room.h
	$(CC) $(CFLAGS) -c room.c

clean:
	rm -f *.o $(TARGET)
```

## Step 6: Compile and Run

Now, open your terminal in the project directory and simply type:
```bash
make
```
This will compile all the files and create an executable named `game`. To play, run:
```bash
./game
```

## What You've Learned

By building this game, you've practiced the most important skill in software development: **integration**. You took separate, logical modules and combined them into a single, functioning application.

Specifically, you've demonstrated your understanding of:
- **Modular Design**: Separating code into logical `player` and `room` modules.
- **Header Files**: Declaring public interfaces (`.h`) for your modules while keeping the implementation private (`.c`).
- **Multi-file Compilation**: Using a `Makefile` to manage dependencies and build a project from multiple source files.
- **Dynamic Memory**: Using `malloc` and `free` to manage the `Player`'s lifecycle.
- **Game Loop**: Creating an interactive experience that takes user input, updates the game state, and displays the result.

This structured approach is how professional programmers build large, complex applications. Mastering it now will make the rest of this tutorial much easier.

## "Try This" Challenges

Now that you have a working game, try to expand it!

1.  **Flesh out the `main.c` `switch` statement:** Implement the `s` (south), `e` (east), and `w` (west) movement commands.
2.  **Improve Combat:** The current combat is very simple (`monsterHealth -= 20`). Can you make it so the player also takes damage? What happens if the player's health reaches 0?
3.  **Add More Rooms:** Modify `MAX_ROOMS` in `room.h` and add more rooms to the `setupDungeon` function in `room.c`. Can you create a simple maze?
4.  **Add More Items:** Add a `hasPotion` field to the `Room` struct and a corresponding `(p)otion` command for the player to use it and restore health.

## Next Steps

You're ready to add graphics! In the next lesson, we'll introduce Raylib and start making visual games.

Continue to [Lesson 7: Getting Started with Raylib](07-raylib-setup.md) when you're ready!