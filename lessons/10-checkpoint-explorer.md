# Lesson 10: Checkpoint Project - Simple ASCII Explorer

Congratulations! You've learned the fundamentals of C, how to render graphics with Raylib, and how to handle player movement. This is our first graphical checkpoint project, where we'll combine all those skills to build a complete, playable mini-game.

## Learning Goals

This checkpoint is designed to reinforce your understanding of:
- **Integration:** Combining separate modules (`player`, `world`, `main`) into a single, working graphical application.
- **Game Loop:** Implementing a complete game loop that handles input, updates game state, and draws the world and player.
- **State Management:** Tracking game state such as the player's position and inventory (keys).
- **Build Process:** Using a `Makefile` to compile a multi-file C project with an external library (Raylib).

## Step 1: Start Simple

Let's begin with the absolute minimum - a player on screen:

```c
// main.c - Version 1: Just show a player
#include "raylib.h"

int main() {
    // Create window
    InitWindow(800, 600, "ASCII Explorer");
    SetTargetFPS(60);
    
    // Player position
    int playerX = 10;
    int playerY = 7;
    
    // Game loop
    while (!WindowShouldClose()) {
        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Draw player at position
        DrawText("@", playerX * 30, playerY * 30, 30, WHITE);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
```

Compile and run:
```bash
gcc main.c -o game -lraylib
./game
```

Great! We see our player (@) on screen. Now let's make them move.

## Step 2: Adding Movement

Let's make our player walk around:

```c
// main.c - Version 2: Player can move
#include "raylib.h"

int main() {
    InitWindow(800, 600, "ASCII Explorer");
    SetTargetFPS(60);
    
    // Player data
    int playerX = 10;
    int playerY = 7;
    const int TILE_SIZE = 30;
    
    while (!WindowShouldClose()) {
        // Update - Handle input
        if (IsKeyPressed(KEY_W)) playerY--;
        if (IsKeyPressed(KEY_S)) playerY++;
        if (IsKeyPressed(KEY_A)) playerX--;
        if (IsKeyPressed(KEY_D)) playerX++;
        
        // Keep player on screen
        if (playerX < 0) playerX = 0;
        if (playerX > 25) playerX = 25;
        if (playerY < 0) playerY = 0;
        if (playerY > 19) playerY = 19;
        
        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Draw player
        DrawText("@", playerX * TILE_SIZE, playerY * TILE_SIZE, TILE_SIZE, WHITE);
        
        // Show instructions
        DrawText("WASD to move", 10, 10, 20, GRAY);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
```

Now we can move! But walking through empty space isn't very interesting. Let's add a world.

## Step 3: Creating a Simple World

Let's add walls and a floor:

```c
// main.c - Version 3: Add a world
#include "raylib.h"
#include <string.h>

int main() {
    InitWindow(800, 600, "ASCII Explorer");
    SetTargetFPS(60);
    
    // Player
    int playerX = 5;
    int playerY = 5;
    const int TILE_SIZE = 30;
    
    // Create a simple map
    const int MAP_WIDTH = 20;
    const int MAP_HEIGHT = 15;
    
    // Define map layout (# = wall, . = floor)
    const char* mapLayout[MAP_HEIGHT] = {
        "####################",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "####################"
    };
    
    // Copy map to working array
    char map[MAP_HEIGHT][MAP_WIDTH + 1];
    for (int y = 0; y < MAP_HEIGHT; y++) {
        strcpy(map[y], mapLayout[y]);
    }
    
    while (!WindowShouldClose()) {
        // Store old position
        int oldX = playerX;
        int oldY = playerY;
        
        // Handle input
        if (IsKeyPressed(KEY_W)) playerY--;
        if (IsKeyPressed(KEY_S)) playerY++;
        if (IsKeyPressed(KEY_A)) playerX--;
        if (IsKeyPressed(KEY_D)) playerX++;
        
        // Check collision with walls
        if (playerX < 0 || playerX >= MAP_WIDTH || 
            playerY < 0 || playerY >= MAP_HEIGHT ||
            map[playerY][playerX] == '#') {
            // Can't move there - restore position
            playerX = oldX;
            playerY = oldY;
        }
        
        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Draw world
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                char tile = map[y][x];
                Color color = (tile == '#') ? GRAY : DARKGRAY;
                
                DrawText(&tile, x * TILE_SIZE + 50, y * TILE_SIZE + 50, 
                        TILE_SIZE, color);
            }
        }
        
        // Draw player
        DrawText("@", playerX * TILE_SIZE + 50, playerY * TILE_SIZE + 50, 
                TILE_SIZE, WHITE);
        
        DrawText("ASCII Explorer", 10, 10, 30, WHITE);
        DrawText("WASD to move", 10, 45, 20, GRAY);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
```

Excellent! Now we have walls that block movement. Let's add things to collect.

## Step 4: Adding Interaction

Let's add keys and doors:

```c
// main.c - Version 4: Keys and doors
#include "raylib.h"
#include <string.h>
#include <stdio.h>

int main() {
    InitWindow(800, 600, "ASCII Explorer");
    SetTargetFPS(60);
    
    // Player
    int playerX = 2;
    int playerY = 2;
    int playerKeys = 0;
    const int TILE_SIZE = 30;
    
    // Map
    const int MAP_WIDTH = 20;
    const int MAP_HEIGHT = 15;
    
    // Map with items: k=key, D=door, K=golden key (goal)
    const char* mapLayout[MAP_HEIGHT] = {
        "####################",
        "#..................#",
        "#.@......k.........#",
        "#..................#",
        "#....####D####.....#",
        "#....#.......#.....#",
        "#....#...K...#.....#",
        "#....#.......#.....#",
        "#....#########.....#",
        "#..................#",
        "#..................#",
        "#........k.........#",
        "#..................#",
        "#..................#",
        "####################"
    };
    
    // Working map
    char map[MAP_HEIGHT][MAP_WIDTH + 1];
    for (int y = 0; y < MAP_HEIGHT; y++) {
        strcpy(map[y], mapLayout[y]);
        
        // Find and remove @ from map (just for player start pos)
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == '@') {
                playerX = x;
                playerY = y;
                map[y][x] = '.';
            }
        }
    }
    
    int gameWon = 0;
    
    while (!WindowShouldClose() && !gameWon) {
        // Save old position
        int oldX = playerX;
        int oldY = playerY;
        
        // Input
        if (IsKeyPressed(KEY_W)) playerY--;
        if (IsKeyPressed(KEY_S)) playerY++;
        if (IsKeyPressed(KEY_A)) playerX--;
        if (IsKeyPressed(KEY_D)) playerX++;
        
        // Check new position
        if (playerX >= 0 && playerX < MAP_WIDTH && 
            playerY >= 0 && playerY < MAP_HEIGHT) {
            
            char tile = map[playerY][playerX];
            
            // Handle different tiles
            switch(tile) {
                case '#':  // Wall
                    playerX = oldX;
                    playerY = oldY;
                    break;
                    
                case 'D':  // Door
                    if (playerKeys > 0) {
                        map[playerY][playerX] = '.';
                        playerKeys--;
                    } else {
                        playerX = oldX;
                        playerY = oldY;
                    }
                    break;
                    
                case 'k':  // Regular key
                    map[playerY][playerX] = '.';
                    playerKeys++;
                    break;
                    
                case 'K':  // Golden key - win!
                    gameWon = 1;
                    break;
            }
        } else {
            // Out of bounds
            playerX = oldX;
            playerY = oldY;
        }
        
        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Draw map
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                char tile = map[y][x];
                Color color = WHITE;
                
                switch(tile) {
                    case '#': color = GRAY; break;
                    case '.': color = DARKGRAY; break;
                    case 'k': color = GOLD; break;
                    case 'K': color = YELLOW; break;
                    case 'D': color = BROWN; break;
                }
                
                char str[2] = {tile, '\0'};
                DrawText(str, x * TILE_SIZE + 50, y * TILE_SIZE + 50, 
                        TILE_SIZE, color);
            }
        }
        
        // Draw player
        DrawText("@", playerX * TILE_SIZE + 50, playerY * TILE_SIZE + 50, 
                TILE_SIZE, WHITE);
        
        // UI
        DrawText("ASCII Explorer", 10, 10, 30, WHITE);
        char keyText[50];
        sprintf(keyText, "Keys: %d", playerKeys);
        DrawText(keyText, 10, 50, 20, GOLD);
        
        if (playerKeys == 0 && map[playerY][playerX] == 'D') {
            DrawText("You need a key!", 300, 50, 20, RED);
        }
        
        EndDrawing();
    }
    
    // Win screen
    if (gameWon) {
        while (!WindowShouldClose() && !IsKeyPressed(KEY_ENTER)) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("YOU WIN!", 250, 200, 60, GOLD);
            DrawText("You found the golden key!", 200, 280, 30, WHITE);
            DrawText("Press ENTER to exit", 220, 350, 20, GRAY);
            EndDrawing();
        }
    }
    
    CloseWindow();
    return 0;
}
```

Great! We now have a complete game. But as it grows, we need better organization.

## Step 5: Organizing Our Code

Let's split our code into logical pieces. This is crucial for larger projects:

### Creating the Player Module

First, let's separate player-related code:

```c
// player.h - Player header file
#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int x;
    int y;
    char symbol;
    int keys;
} Player;

Player* createPlayer(int startX, int startY);
void movePlayer(Player* player, int dx, int dy);
void freePlayer(Player* player);

#endif
```

```c
// player.c - Player implementation
#include <stdlib.h>
#include "player.h"

Player* createPlayer(int startX, int startY) {
    Player* player = (Player*)malloc(sizeof(Player));
    player->x = startX;
    player->y = startY;
    player->symbol = '@';
    player->keys = 0;
    return player;
}

void movePlayer(Player* player, int dx, int dy) {
    player->x += dx;
    player->y += dy;
}

void freePlayer(Player* player) {
    free(player);
}
```

### Why Organize Like This?

1. **Easier to understand**: Each file has one purpose
2. **Easier to modify**: Change player code without touching world code
3. **Reusable**: Use these modules in other projects
4. **Team friendly**: Multiple people can work on different files

## Step 6: The World Module

Now let's organize world/map code:

```c
// world.h - World header
#ifndef WORLD_H
#define WORLD_H

#define WORLD_WIDTH 20
#define WORLD_HEIGHT 15

typedef struct {
    char tiles[WORLD_HEIGHT][WORLD_WIDTH];
    char* name;
} World;

World* createWorld(const char* name);
char getTile(World* world, int x, int y);
void setTile(World* world, int x, int y, char tile);
int isWalkable(char tile);
void freeWorld(World* world);

#endif
```

```c
// world.c - World implementation
#include <stdlib.h>
#include <string.h>
#include "world.h"

World* createWorld(const char* name) {
    World* world = (World*)malloc(sizeof(World));
    world->name = (char*)malloc(strlen(name) + 1);
    strcpy(world->name, name);
    
    // Initialize with empty tiles
    for (int y = 0; y < WORLD_HEIGHT; y++) {
        for (int x = 0; x < WORLD_WIDTH; x++) {
            world->tiles[y][x] = '.';
        }
    }
    
    return world;
}

char getTile(World* world, int x, int y) {
    if (x < 0 || x >= WORLD_WIDTH || y < 0 || y >= WORLD_HEIGHT) {
        return '#';  // Out of bounds = wall
    }
    return world->tiles[y][x];
}

void setTile(World* world, int x, int y, char tile) {
    if (x >= 0 && x < WORLD_WIDTH && y >= 0 && y < WORLD_HEIGHT) {
        world->tiles[y][x] = tile;
    }
}

int isWalkable(char tile) {
    return tile != '#' && tile != 'D';
}

void freeWorld(World* world) {
    free(world->name);
    free(world);
}
```

## Step 7: Building the Complete Game

Now let's put it all together in a clean, organized way:

```c
// main.c - Complete game with organization
#include "raylib.h"
#include "player.h"
#include "world.h"
#include <stdio.h>
#include <string.h>

// Game constants
#define TILE_SIZE 30
#define OFFSET_X 50
#define OFFSET_Y 50

// Function to load our test level
void loadLevel(World* world) {
    const char* layout[WORLD_HEIGHT] = {
        "####################",
        "#..................#",
        "#..................#",
        "#.......k..........#",
        "#....####D####.....#",
        "#....#.......#.....#",
        "#....#...K...#.....#",
        "#....#.......#.....#",
        "#....#########.....#",
        "#..................#",
        "#..................#",
        "#........k.........#",
        "#..................#",
        "#..................#",
        "####################"
    };
    
    for (int y = 0; y < WORLD_HEIGHT; y++) {
        for (int x = 0; x < WORLD_WIDTH; x++) {
            world->tiles[y][x] = layout[y][x];
        }
    }
}

// Get color for each tile type
Color getTileColor(char tile) {
    switch(tile) {
        case '#': return GRAY;       // Wall
        case '.': return DARKGRAY;   // Floor
        case 'k': return GOLD;       // Key
        case 'K': return YELLOW;     // Golden key
        case 'D': return BROWN;      // Door
        case 'd': return DARKBROWN;  // Open door
        default: return WHITE;
    }
}

// Handle player trying to move
void handleMovement(Player* player, World* world, int dx, int dy) {
    int newX = player->x + dx;
    int newY = player->y + dy;
    char tile = getTile(world, newX, newY);
    
    // Check what's at the new position
    if (tile == '#') {
        return;  // Can't walk through walls
    }
    else if (tile == 'D') {
        // Door - need key
        if (player->keys > 0) {
            player->keys--;
            setTile(world, newX, newY, 'd');  // Open door
            movePlayer(player, dx, dy);
        }
    }
    else if (tile == 'k') {
        // Pick up key
        player->keys++;
        setTile(world, newX, newY, '.');
        movePlayer(player, dx, dy);
    }
    else {
        // Normal movement
        movePlayer(player, dx, dy);
    }
}

int main() {
    // Initialize
    InitWindow(800, 600, "ASCII Explorer - Complete");
    SetTargetFPS(60);
    
    // Create game objects
    Player* player = createPlayer(2, 2);
    World* world = createWorld("Test Dungeon");
    loadLevel(world);
    
    int gameWon = 0;
    
    // Game loop
    while (!WindowShouldClose() && !gameWon) {
        // Input
        if (IsKeyPressed(KEY_W)) handleMovement(player, world, 0, -1);
        if (IsKeyPressed(KEY_S)) handleMovement(player, world, 0, 1);
        if (IsKeyPressed(KEY_A)) handleMovement(player, world, -1, 0);
        if (IsKeyPressed(KEY_D)) handleMovement(player, world, 1, 0);
        
        // Check win condition
        if (getTile(world, player->x, player->y) == 'K') {
            gameWon = 1;
        }
        
        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Draw world
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            for (int x = 0; x < WORLD_WIDTH; x++) {
                char tile = world->tiles[y][x];
                Color color = getTileColor(tile);
                
                char str[2] = {tile, '\0'};
                DrawText(str, OFFSET_X + x * TILE_SIZE, 
                        OFFSET_Y + y * TILE_SIZE, TILE_SIZE, color);
            }
        }
        
        // Draw player
        DrawText("@", OFFSET_X + player->x * TILE_SIZE, 
                OFFSET_Y + player->y * TILE_SIZE, TILE_SIZE, WHITE);
        
        // UI
        DrawText("ASCII Explorer", 10, 10, 30, WHITE);
        
        char info[100];
        sprintf(info, "Keys: %d", player->keys);
        DrawText(info, 10, 45, 20, GOLD);
        
        DrawText("Find the golden key (K) to win!", 10, 570, 18, GRAY);
        
        EndDrawing();
    }
    
    // Win screen
    if (gameWon) {
        while (!WindowShouldClose() && !IsKeyPressed(KEY_ENTER)) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("YOU WIN!", 250, 200, 60, GOLD);
            DrawText("You found the golden key!", 180, 280, 30, WHITE);
            DrawText("Press ENTER to exit", 240, 350, 20, GRAY);
            EndDrawing();
        }
    }
    
    // Cleanup
    freePlayer(player);
    freeWorld(world);
    CloseWindow();
    
    return 0;
}
```

## Step 8: Creating a Makefile

As projects grow with multiple files, typing gcc commands gets tedious. A Makefile automates the build process!

### Understanding Makefiles

A Makefile is like a recipe that tells the computer how to build your program:

```makefile
# Makefile for ASCII Explorer
# Lines starting with # are comments

# Variables (like constants in C)
CC = gcc                    # The compiler to use
CFLAGS = -Wall -g          # Compiler flags: -Wall shows warnings, -g adds debug info
LIBS = -lraylib -lm        # Libraries to link: raylib and math

# List of object files we need
OBJS = main.o player.o world.o

# The first target is the default when you type 'make'
all: explorer

# Rule format:
# target: dependencies
#     commands (must start with TAB, not spaces!)

# How to build the final program
explorer: $(OBJS)
	$(CC) $(OBJS) -o explorer $(LIBS)

# How to build each object file
main.o: main.c player.h world.h
	$(CC) $(CFLAGS) -c main.c

player.o: player.c player.h
	$(CC) $(CFLAGS) -c player.c

world.o: world.c world.h
	$(CC) $(CFLAGS) -c world.c

# Utility targets
clean:
	rm -f *.o explorer

run: explorer
	./explorer
```

### How It Works

1. **Dependencies**: `main.o: main.c player.h world.h` means:
   - To build `main.o`, we need `main.c`, `player.h`, and `world.h`
   - If any of these change, `main.o` needs rebuilding

2. **Variables**: 
   - `$(CC)` gets replaced with `gcc`
   - `$(OBJS)` gets replaced with `main.o player.o world.o`

3. **Commands**:
   - Must start with a TAB character (not spaces!)
   - Run the actual compilation commands

### Using Make

```bash
make          # Builds the default target (explorer)
make clean    # Removes built files
make run      # Builds and runs the game
```

Make is smart - it only rebuilds what changed! If you edit `player.c`, it only recompiles `player.o` and relinks, not everything.

### Common Makefile Issues

1. **"Missing separator"**: Use TAB, not spaces, before commands
2. **"No rule to make target"**: Check file names match exactly
3. **Undefined reference**: Add missing libraries to LIBS

## What You've Learned

By building this project step by step, you've demonstrated a solid understanding of the fundamentals of C game development. You've practiced:

1.  **Incremental Development**: We started with just a player on screen and gradually added a world, collision, and interaction, one feature at a time. This is how all large software is built.
2.  **Game Architecture**: You successfully separated the concepts of the `Player`, the `World`, and the main game loop into different modules. This is the foundation of clean, maintainable code.
3.  **State Management**: You tracked the player's position and key count, and the state of the world's tiles (doors opening, keys disappearing).
4.  **Build Systems**: You used a `Makefile` to automate the compilation of a project with multiple files and an external dependency (Raylib), a crucial skill for any C programmer.

## "Try This" Challenges

Now that you have a working game, make it your own!

1.  **More Treasure:** Add a new collectible to the map, like a gold coin (`$`). Make it so the player can pick them up. Add a `playerGold` variable to the `Player` struct and display the current gold count on the UI.
2.  **A Second Key Type:** Add a new door type (e.g., `d`) and a new key type (e.g., `j`). The player should need the correct key for the correct door. This will require you to expand the `Player` struct and the collision logic.
3.  **Sound Effects:** Raylib makes sound easy!
    a. In `main`, call `InitAudioDevice();` after `InitWindow();`.
    b. Load a sound with `Sound fxWobble = LoadSound("weird.wav");` (you'll need to find a `.wav` file).
    c. Play it when the player moves with `PlaySound(fxWobble);`.
    d. Don't forget to `UnloadSound(fxWobble);` and `CloseAudioDevice();` during cleanup!
4.  **A "Bumping" Animation:** When the player tries to move into a wall, they currently just stop. Can you make it more interesting? When a collision occurs, briefly change the player's color to `RED` for a fraction of a second to give better visual feedback. You'll need a timer variable to control how long the color stays red.

### Example: Adding Treasures

```c
// In player struct
int treasures;

// In tile handling
case 'T':  // Treasure
    player->treasures++;
    setTile(world, newX, newY, '.');
    movePlayer(player, dx, dy);
    break;

// In UI
sprintf(info, "Keys: %d | Treasures: %d", player->keys, player->treasures);
```

## Common Issues and Solutions

**Problem**: Undefined reference errors
**Solution**: Make sure all .c files are compiled and linked

**Problem**: Player walks through walls
**Solution**: Check collision before moving, not after

**Problem**: Keys don't work on doors
**Solution**: Ensure you're checking the correct tile position

## Project Structure Benefits

Your organized project:
```
ascii-explorer/
├── main.c      # Game loop and main logic
├── player.c    # Player behavior
├── player.h    # Player interface
├── world.c     # World/map logic
├── world.h     # World interface
└── Makefile    # Build instructions
```

This structure:
- Makes code easier to understand
- Allows independent testing of modules
- Prepares you for larger projects
- Follows professional standards

## Summary

Congratulations! You've built a complete, well-organized game. You started with a single character on screen and built up to a full game with:
- Modular code organization
- Collision detection
- Item collection
- Win conditions
- Clean architecture

This checkpoint proves you're ready for more advanced topics!

## Next Steps

You're prepared for procedural generation, advanced rendering, and complex game systems. The next lessons will build on this foundation.

Continue to [Lesson 11: Creating Game Worlds](11-game-world-part1.md) when you're ready!

## Quick Reference

```c
// Module pattern
// header.h - Interface (what others can use)
// implementation.c - Code (how it works)

// Game loop pattern
while (running) {
    HandleInput();
    UpdateGameState();
    DrawEverything();
}

// Collision pattern
if (canMoveTo(newX, newY)) {
    player->x = newX;
    player->y = newY;
}
```

Remember: Professional games are built incrementally with good organization!