# Lesson 18a: Saving and Loading Game Data – File I/O Fundamentals

In a real game players expect the ability to quit, come back later and continue right where they left off.  Implementing **save / load** teaches three very important C skills:

1. Standard **file‐I/O** (`fopen`, `fwrite`, `fread`, `fclose`)
2. Deciding **what** constitutes game state and how to serialize it
3. Defensive programming – checking errors and versioning your files

> You can finish this lesson in < 30 minutes if you already have your inventory system working.

---
## 1.  What Needs Saving?

Before writing any code list the _minimum_ information required to fully restore the game world.  For the ASCII-RPG this is usually:

* Player stats (position, HP, gold, EXP, etc.)
* Inventory contents
* Current dungeon level and tiles that have changed (open doors, looted chests…)
* Active quest progress and NPC flags

Create a simple C structure that groups these items so we have **one** thing to write to disk.

```c
// game_state.h
#ifndef GAME_STATE_H
#define GAME_STATE_H
#include <stdint.h>

#define MAX_INVENTORY 64
#define MAP_W 80
#define MAP_H 60

typedef struct {
    int id;
    int quantity;
} Item;

typedef struct {
    // --- player ---
    int16_t px, py;       // player position on map grid
    int16_t hp, hpMax;
    int16_t gold;

    // --- inventory ---
    Item inventory[MAX_INVENTORY];
    uint8_t inventoryCount;

    // --- world ---
    uint8_t currentLevel;
    char    tiles[MAP_W * MAP_H];

    // future-proofing: increase this when format changes
    uint8_t version;
} GameState;

#endif // GAME_STATE_H
```

### Why fixed-size arrays?
A binary save file is easiest when every structure has the _same_ byte length each time.

---
## 2.  Writing a Save File

```c
#include "game_state.h"
#include <stdio.h>
#include <string.h>

int SaveGame(const GameState* state, const char* path)
{
    FILE* fp = fopen(path, "wb");          // b = binary
    if (!fp) return 0;                      // could not open

    size_t written = fwrite(state, sizeof(GameState), 1, fp);
    fclose(fp);
    return written == 1;                    // 1 on success, 0 on failure
}
```

Important details:
* Use **binary** mode (`"wb"`) so the bytes are written unchanged on Windows.
* Always check the return value of `fwrite`.
* Store a `version` field inside the struct so you can migrate later.

---
## 3.  Loading a Save File

```c
int LoadGame(GameState* state, const char* path)
{
    FILE* fp = fopen(path, "rb");
    if (!fp) return 0;

    size_t read = fread(state, sizeof(GameState), 1, fp);
    fclose(fp);

    if (read != 1) return 0;               // corrupted / truncated file
    if (state->version != 1) return 0;     // wrong file format version
    return 1;
}
```

Tip: initialise the `GameState` with default values _before_ calling `LoadGame`; if the file is missing or broken you automatically start a new game.

---
## 4.  Hooking Into Your Game

1.  At program start try `LoadGame(&gState, "save.bin")`; fall back to defaults on failure.
2.  Add a _Save_ menu item that calls `SaveGame(&gState, "save.bin")`.
3.  Auto-save when the player enters the next dungeon level.

```c
if (IsKeyPressed(KEY_F5)) {
    if (SaveGame(&gState, "save.bin")) {
        TraceLog(LOG_INFO, "Game saved!");
    } else {
        TraceLog(LOG_ERROR, "Save failed!");
    }
}
```

---
## 5.  Exercise – JSON vs Binary

1. Rewrite `SaveGame` / `LoadGame` using the [cJSON](https://github.com/DaveGamble/cJSON) library to output a human-readable file.
2. Compare file sizes and load times.
3. Decide which format you prefer and why.

---
## 6.  Summary Checklist

- [x] Designed a `GameState` struct containing all necessary data
- [x] Implemented binary save / load functions
- [x] Added key-press or menu option to trigger saving
- [ ] **Challenge:** add an _auto-save every 60 seconds_ feature

Continue to **Lesson 19: NPCs & Dialogue** (file `18-npcs-dialogue.md`) when you are confident that saving and loading work. 