# Lesson 9: Player Movement and Input

Time to bring your game to life! In this lesson, we'll implement player movement, handle input properly, and create a responsive control system. By the end, you'll have a player character that moves smoothly through your ASCII world.

## Understanding Game Input

Good input handling is crucial for game feel. Players expect:
- **Immediate response** to their actions
- **Smooth movement** without delays
- **Clear feedback** for their inputs
- **Intuitive controls** that feel natural

## Why Input Handling is Tricky

Before we code, let's understand the challenges:

1. **Keyboard Repeat**: When you hold a key, the OS sends repeated key presses. This can make movement too fast or jerky.
2. **Multiple Keys**: What if the player presses Up and Right at the same time?
3. **Frame Rate**: On a fast computer, checking input 60 times per second might move the player too quickly.
4. **Validation**: We need to check if moves are legal (not walking through walls).

We'll solve these problems step by step.

## Step 1: The Player Data Structure

Before we can move something, we need a "something" to move. In our game, this is the player. We'll use a `struct` to group all the data that defines our player in one neat package.

```c
#include "raylib.h"
#include <stdbool.h>  // This gives us 'bool', 'true', and 'false' keywords

// Define what data represents a player
typedef struct {
    // Position in the game grid
    int x;          // Column (0 = leftmost)
    int y;          // Row (0 = topmost)
    
    // Visual representation
    char symbol;    // What character represents the player
    Color color;    // What color to draw the player
    
    // Game stats
    int health;     // Current health points
    int maxHealth;  // Maximum health points
    int gold;       // Currency collected
} Player;

// Function to create a new player
// Why a function? It ensures we always initialize all values properly
Player CreatePlayer(int startX, int startY) {
    Player player;
    
    // Set starting position
    player.x = startX;
    player.y = startY;
    
    // Set appearance
    player.symbol = '@';  // Traditional roguelike player symbol
    player.color = WHITE; // White stands out against most backgrounds
    
    // Set initial stats
    player.health = 100;
    player.maxHealth = 100;
    player.gold = 0;  // Start with no money
    
    return player;  // Return the initialized player
}
```

### Why Use a Struct for the Player?

- **Organization**: All player-related data (`x`, `y`, `health`, `gold`, etc.) is bundled together. This is much cleaner than having dozens of separate variables like `playerX`, `playerY`, `playerHealth`.
- **Portability**: We can pass a single `Player*` pointer to functions instead of 7 or 8 separate arguments. This makes our function calls much cleaner.
- **Scalability**: It's easy to add new properties later (e.g., `int mana;`, `int experience;`) without changing every function that uses the player.

## Step 2: A Simple Movement System

Now let's implement basic 4-directional movement. We'll build this by answering three questions:
1.  How do we represent directions in our code?
2.  How do we get input from the player?
3.  How do we change the player's coordinates based on that input?

### Step 1: Defining Directions

First, let's create a clear way to represent directions:

```c
// Movement directions using an enum
// Why an enum? It's clearer than using numbers (0, 1, 2, 3, 4)
typedef enum {
    DIR_NONE = 0,   // No movement
    DIR_UP,         // Move up (decrease Y)
    DIR_DOWN,       // Move down (increase Y)
    DIR_LEFT,       // Move left (decrease X)
    DIR_RIGHT       // Move right (increase X)
} Direction;

// Note: In computer graphics, Y often increases downward!
// (0,0) is usually the top-left corner
```

### Step 2: Reading Player Input

Now let's check which keys the player is pressing:

```c
// Function to get current input direction
Direction GetInputDirection() {
    // IsKeyPressed() returns true only on the frame the key is first pressed
    // This prevents ultra-fast movement from key repeat
    
    // Check each direction - we support both WASD and arrow keys
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        return DIR_UP;
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        return DIR_DOWN;
    }
    if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)) {
        return DIR_LEFT;
    }
    if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) {
        return DIR_RIGHT;
    }
    
    // No movement keys pressed
    return DIR_NONE;
}

// Why check in this order? It doesn't really matter, but being consistent
// helps when debugging. If multiple keys are pressed, the first one wins.
```

### Step 3: Moving the Player

Now let's update the player's position based on the direction:

```c
// Move player based on direction
// Note the * in Player* - this means we're modifying the original player,
// not a copy. Without *, changes wouldn't affect the original!
void MovePlayer(Player* player, Direction dir) {
    switch(dir) {
        case DIR_UP:    
            player->y--;  // Move up = decrease Y coordinate
            break;
            
        case DIR_DOWN:  
            player->y++;  // Move down = increase Y coordinate
            break;
            
        case DIR_LEFT:  
            player->x--;  // Move left = decrease X coordinate
            break;
            
        case DIR_RIGHT: 
            player->x++;  // Move right = increase X coordinate
            break;
            
        case DIR_NONE:  
            // Do nothing - no movement requested
            break;
    }
}

> **Problem:** This function is too trusting! It doesn't check if the move is valid. If we used this function as-is, the player could walk right off the screen or through walls. We'll fix this in the next step.
```

## Step 3: Collision Detection

A game world doesn't feel real until it has rules. The most basic rule is that you can't walk through solid objects. This is called **collision detection**.

### Understanding the Problem

When the player tries to move, we need to check:
1. Are they trying to move outside the map boundaries?
2. Is there a wall or other obstacle at the destination?
3. Is another entity (enemy, NPC) blocking the way?

Let's solve these one at a time.

### Checking Valid Positions

```c
// Function to check if a position is valid for movement
bool IsValidPosition(char* map, int width, int height, int x, int y) {
    // Step 1: Check if position is within map boundaries
    if (x < 0) return false;          // Too far left
    if (x >= width) return false;     // Too far right  
    if (y < 0) return false;          // Too far up
    if (y >= height) return false;    // Too far down
    
    // Step 2: Check what's at this position in the map
    // Remember: map is a 1D array representing a 2D grid
    // Position (x,y) is at index: y * width + x
    int index = y * width + x;
    char tile = map[index];
    
    // Step 3: Determine if this tile can be walked on
    if (tile == '#') return false;    // Walls block movement
    if (tile == '~') return false;    // Water blocks movement (for now)
    
    // All other tiles are walkable
    return true;
}
```

### Improved Movement Function

Now let's create a movement function that checks before moving:

```c
// Try to move player in a direction (with validation)
void TryMovePlayer(Player* player, Direction dir, char* map, int mapWidth, int mapHeight) {
    // First, figure out where the player wants to go
    int newX = player->x;  // Start with current position
    int newY = player->y;
    
    // Calculate new position based on direction
    switch(dir) {
        case DIR_UP:    newY--; break;  // Up means Y decreases
        case DIR_DOWN:  newY++; break;  // Down means Y increases
        case DIR_LEFT:  newX--; break;  // Left means X decreases
        case DIR_RIGHT: newX++; break;  // Right means X increases
        case DIR_NONE:  return;         // No movement, so we're done
    }
    
    // Check if the new position is valid
    if (IsValidPosition(map, mapWidth, mapHeight, newX, newY)) {
        // Move is valid! Update player position
        player->x = newX;
        player->y = newY;
    }
    // If not valid, simply don't move (player bumps into obstacle)
}
```

## Complete Movement Example

Let's put it all together in a working program. I'll explain each section as we build it:

```c
#include "raylib.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

// Include all our previous type definitions and functions here
// (Direction enum, Player struct, movement functions, etc.)

int main() {
    // Window Setup
    // We create a window large enough to show our map comfortably
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "Player Movement");
    SetTargetFPS(60);  // Consistent 60 FPS for smooth movement
    
    // Map Creation
    // Let's create a simple test map with some obstacles
    const int mapWidth = 20;
    const int mapHeight = 15;
    char map[300];  // 20 * 15 = 300 tiles total
    
    // This string represents our map layout
    // # = walls (blocking)
    // . = floor (walkable)
    // The inner rooms test our collision detection
    strcpy(map, 
        "####################"  // Top wall
        "#..................#"  // Open space
        "#..................#"
        "#...####...####....#"  // Two small rooms
        "#...#  #...#  #....#"  // Note: spaces inside rooms
        "#...#  #...#  #....#"  // Player can't enter these
        "#...#  #...#  #....#"
        "#...####...####....#"  // Bottom of rooms
        "#..................#"  // More open space
        "#..................#"
        "#..................#"
        "#..................#"
        "#..................#"
        "#..................#"
        "####################"  // Bottom wall
    );
    
    // Player Creation
    // Start the player at position (5, 5) - safely away from walls
    Player player = CreatePlayer(5, 5);
    
    // Rendering Configuration
    int cellSize = 30;     // Each tile is 30x30 pixels
    int offsetX = 50;      // Start drawing 50 pixels from left
    int offsetY = 50;      // Start drawing 50 pixels from top
    
    // Main Game Loop
    while (!WindowShouldClose()) {
        // === UPDATE PHASE ===
        
        // Check for player input
        Direction inputDir = GetInputDirection();
        
        // Only try to move if a direction was pressed
        if (inputDir != DIR_NONE) {
            // This function validates the move before applying it
            TryMovePlayer(&player, inputDir, map, mapWidth, mapHeight);
        }
        
        // === DRAW PHASE ===
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw the map first (so player appears on top)
            for (int y = 0; y < mapHeight; y++) {
                for (int x = 0; x < mapWidth; x++) {
                    // Get the tile at this position
                    char tile = map[y * mapWidth + x];
                    
                    // Choose color based on tile type
                    Color color;
                    if (tile == '#') {
                        color = GRAY;      // Walls are gray
                    } else {
                        color = DARKGRAY;  // Floor is dark gray
                    }
                    
                    // Convert char to string for DrawText
                    char str[2] = {tile, '\0'};
                    
                    // Calculate screen position and draw
                    int screenX = offsetX + x * cellSize;
                    int screenY = offsetY + y * cellSize;
                    DrawText(str, screenX, screenY, cellSize, color);
                }
            }
            
            // Draw player on top of the map
            char playerStr[2] = {player.symbol, '\0'};
            int playerScreenX = offsetX + player.x * cellSize;
            int playerScreenY = offsetY + player.y * cellSize;
            DrawText(playerStr, playerScreenX, playerScreenY, 
                    cellSize, player.color);
            
            // Draw UI information
            DrawText("Use WASD or Arrow Keys to move", 10, 10, 20, WHITE);
            
            // Show current position (helpful for debugging)
            char posText[50];
            sprintf(posText, "Position: (%d, %d)", player.x, player.y);
            DrawText(posText, 10, screenHeight - 30, 20, WHITE);
            
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
```

### Understanding the Complete Program

This example demonstrates several important concepts:

1. **Game Loop Structure**: Update first, then draw
2. **Collision Detection**: Player can't walk through walls
3. **Clean Rendering**: Map draws first, player on top
4. **User Feedback**: Shows controls and position

### What Happens When You Run It

1. A window opens showing a map with walls around the edge
2. The player (@) starts at position (5, 5)
3. You can move with WASD or arrow keys
4. The player stops when hitting walls (#)
5. Position updates in real-time at the bottom

### "Try This" Challenges

1.  **Traps!:** Add a new tile type, `^`, to the map. In `IsValidPosition`, make it so this tile is *walkable* (returns `true`). But in your main game loop, after a successful move, check if the player is now standing on a `^` tile. If so, print a message like "You stepped on a trap and took 5 damage!" and decrease their health.

2.  **Locked Doors:** Add a `D` tile to the map. In `IsValidPosition`, make this tile block movement (return `false`). Now, add a `bool hasKey;` field to your `Player` struct. Modify `TryMovePlayer` so that if the target tile is a `D`, it checks if `player->hasKey` is `true`. If it is, allow the move and maybe change the 'D' tile to a '.' floor tile to "open" it.

3.  **Diagonal Movement (Simple):** How would you add diagonal movement using the `IsKeyDown` function for multiple key presses at once? You don't need to implement the "sliding" logic yet, just see if you can get the player to move diagonally into open space.

### Common Issues and Solutions

**Player moves too fast?**
- We're using IsKeyPressed() which only triggers once per press
- For continuous movement, we'd need a movement timer

**Player gets stuck?**
- Check your collision detection logic
- Make sure walls are marked as '#'

**Can't see the whole map?**
- Adjust cellSize to make tiles smaller
- Or implement a camera system (covered later)


## Summary

You've learned:
- How to represent a player with a `struct`
- How to handle keyboard input for movement
- How to implement grid-based collision detection

Your player can now explore the game world safely without walking through walls!

## Next Steps

You have a player that can move around a static world. Now it's time to put your skills to the test in the first checkpoint project! You'll combine everything you've learned so far to build a small, complete graphical game.

*(Optionally, for those who want to explore more advanced movement concepts like timers, animation, and input buffering, you can check out the supplementary lesson: `09a-advanced-movement.md`)*

Ready for your first checkpoint? Head to [Lesson 10: Checkpoint Project - Simple ASCII Explorer](10-checkpoint-explorer.md)!