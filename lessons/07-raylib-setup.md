# Lesson 7: Getting Started with Raylib

Time to bring your C knowledge to life with graphics! In this lesson, we'll install Raylib and create our first window. By the end, you'll have the foundation for your ASCII RPG.

## What is Raylib?

Raylib is a simple and easy-to-use library that handles:
- Creating windows
- Drawing text and shapes
- Getting keyboard/mouse input
- Playing sounds
- Much more!

It's perfect for beginners because it's straightforward and well-documented.

## Understanding Libraries in C

Before we use Raylib, let's understand what a library is:

```c
// Without a library, opening a window requires hundreds of lines:
// - Talk to the operating system
// - Request window creation
// - Handle different OS (Windows/Mac/Linux) differently
// - Manage memory for the window
// - Set up a drawing context
// ... and much more!

// With Raylib, it's just:
InitWindow(800, 600, "My Window");
```

A library is pre-written code that handles complex tasks for us. Think of it like using a microwave instead of building a fire to heat food - the complex work is hidden inside!

## Installing Raylib

### Option 1: Using Package Managers (Easiest)

**On Ubuntu/Debian Linux:**
```bash
sudo apt update
sudo apt install libraylib-dev
```

**On macOS (with Homebrew):**
```bash
brew install raylib
```

**On Windows (with MSYS2):**
```bash
pacman -S mingw-w64-x86_64-raylib
```

### Option 2: Download Pre-compiled Libraries

1. Go to https://github.com/raysan5/raylib/releases
2. Download the version for your system
3. Extract the files
4. Note the location - you'll need it for compiling

### Option 3: Build from Source

For the adventurous or if other methods don't work:
```bash
git clone https://github.com/raysan5/raylib.git
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install
```

## Your First Raylib Window

Let's create our first graphical program! We'll start simple and explain every single line.

Create a new file called `first_window.c`:

```c
#include "raylib.h"

int main() {
    // Initialize window
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "My First Raylib Window");
    
    // Set target FPS (frames per second)
    SetTargetFPS(60);
    
    // Main game loop
    while (!WindowShouldClose()) {  // Detect window close button or ESC key
        // Update
        // (We'll add game logic here later)
        
        // Draw
        BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Hello, Raylib!", 10, 10, 20, WHITE);
        EndDrawing();
    }
    
    // Clean up
    CloseWindow();
    
    return 0;
}
```

### Understanding Every Line

Let's break this down completely:

```c
#include "raylib.h"
```
This tells the compiler to include Raylib's functions. The quotes mean "look for this file in the current directory first, then in system directories."

```c
const int screenWidth = 800;
const int screenHeight = 600;
```
We define our window size. Why use `const`? It prevents us from accidentally changing these values later. Why 800x600? It's a common resolution that fits on most screens.

```c
InitWindow(screenWidth, screenHeight, "My First Raylib Window");
```
This creates the actual window. The function needs:
- Width in pixels (800)
- Height in pixels (600)  
- Title that appears in the window's title bar

```c
SetTargetFPS(60);
```
FPS = Frames Per Second. This tells Raylib we want our game to update 60 times per second. Why 60? It matches most monitor refresh rates and feels smooth to the human eye.

```c
while (!WindowShouldClose()) { // This function returns true if the user presses ESC or clicks the 'X' button
```
This is our game loop! It keeps running until:
- The user clicks the window's X button
- The user presses the ESC key
The `!` means "not", so this reads as "while the window should NOT close, keep going."

```c
BeginDrawing();
    // This is the "drawing zone". All drawing functions must be called after BeginDrawing().

    // First, we clear the screen to a single color. This is like erasing the whiteboard
    // before you draw the next frame. If you forget this, you'll see "smears" of all
    // previous frames.
    ClearBackground(BLACK);

    // Now we can draw our content for this frame.
    DrawText("Hello, Raylib!", 10, 10, 20, WHITE);

EndDrawing();
// This tells Raylib we're done drawing for this frame. It takes what we've drawn
// and displays it on the screen all at once.
```
Every frame, we must:
1. `BeginDrawing()` - Tell Raylib we're about to draw
2. `ClearBackground(BLACK)` - Erase everything from last frame with black
3. Draw our new content (text in this case)
4. `EndDrawing()` - Tell Raylib we're done, show it on screen

For `DrawText`, the parameters are:
- `"Hello, Raylib!"` - The text to display
- `10` - X position (pixels from left edge)
- `10` - Y position (pixels from top edge)
- `20` - Font size in pixels
- `WHITE` - Text color

```c
CloseWindow();
```
When the loop ends, we must clean up. This closes the window and frees memory.

## Compiling with Raylib

The compile command depends on your system:

**Linux/macOS:**
```bash
gcc first_window.c -o first_window -lraylib -lm -lpthread -ldl
./first_window
```

**Windows (MinGW):**
```bash
gcc first_window.c -o first_window.exe -lraylib -lopengl32 -lgdi32 -lwinmm
first_window.exe
```

If Raylib isn't in standard locations, you might need:
```bash
gcc first_window.c -o first_window -I/path/to/raylib/include -L/path/to/raylib/lib -lraylib -lm
```

## Understanding the Code

Let's break down what each part does:

1. **Include Raylib**: `#include "raylib.h"` gives us access to all Raylib functions

2. **InitWindow**: Creates a window with specified width, height, and title

3. **SetTargetFPS**: Controls how fast the game runs (60 frames per second is smooth)

4. **Game Loop**: The `while (!WindowShouldClose())` loop runs until you close the window

5. **BeginDrawing/EndDrawing**: All drawing must happen between these

6. **ClearBackground**: Fills the screen with a color (erases previous frame)

7. **DrawText**: Draws text at position (x, y) with specified size and color

8. **CloseWindow**: Cleans up when done

## Drawing ASCII Characters

Let's modify our program to draw ASCII characters like in our game:

```c
#include "raylib.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "ASCII Drawing");
    SetTargetFPS(60);
    
    // Game variables
    int fontSize = 20;
    int spacing = fontSize;  // Space between characters
    
    while (!WindowShouldClose()) {
        // Draw
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw a wall
            DrawText("#", 100, 100, fontSize, GRAY);
            
            // Draw a player
            DrawText("@", 200, 100, fontSize, WHITE);
            
            // Draw an enemy
            DrawText("g", 300, 100, fontSize, RED);
            
            // Draw a potion
            DrawText("!", 400, 100, fontSize, GREEN);
            
            // Draw instructions
            DrawText("# = Wall, @ = Player, g = Goblin, ! = Potion", 
                     10, screenHeight - 30, 20, WHITE);
            
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
```

## Creating an ASCII Grid

Games often use grids. Let's build one step by step to understand how it works.

### Understanding Grid-Based Games

In ASCII games, we think of the screen as a grid of cells, like graph paper. Each cell can hold one character. This is different from pixel-based graphics where we can draw anywhere.

```
Column: 0  1  2  3  4
Row 0:  #  #  #  #  #
Row 1:  #  .  .  .  #
Row 2:  #  .  @  .  #
Row 3:  #  .  .  .  #
Row 4:  #  #  #  #  #
```

### Building Our Grid System

Let's create this grid programmatically:

```c
#include "raylib.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "ASCII Grid");
    SetTargetFPS(60);
    
    // Grid settings - let's understand each value
    const int gridWidth = 20;   // 20 cells wide
    const int gridHeight = 15;  // 15 cells tall
    const int cellSize = 30;    // Each cell is 30x30 pixels
    
    // Why these values?
    // 20 x 30 = 600 pixels wide
    // 15 x 30 = 450 pixels tall
    // This fits nicely in our 800x600 window with room for borders
    
    // Calculate starting position to center the grid
    // Let's break down this math:
    // screenWidth = 800
    // Grid total width = gridWidth * cellSize = 20 * 30 = 600
    // Extra space = 800 - 600 = 200
    // To center, put half on each side = 200 / 2 = 100
    int startX = (screenWidth - gridWidth * cellSize) / 2;
    int startY = (screenHeight - gridHeight * cellSize) / 2;
    
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw grid using nested loops
            // Outer loop: go through each row (y)
            // Inner loop: go through each column (x) in that row
            for (int y = 0; y < gridHeight; y++) {
                for (int x = 0; x < gridWidth; x++) {
                    // Calculate where to draw this cell
                    // For cell (3,2):
                    // drawX = 100 + 3 * 30 = 190 pixels from left
                    // drawY = 75 + 2 * 30 = 135 pixels from top
                    int drawX = startX + x * cellSize;
                    int drawY = startY + y * cellSize;
                    
                    // Decide what character to draw
                    // Check if we're on an edge
                    if (x == 0 ||                    // Left edge
                        x == gridWidth-1 ||          // Right edge
                        y == 0 ||                    // Top edge
                        y == gridHeight-1) {         // Bottom edge
                        // Draw wall character
                        DrawText("#", drawX, drawY, cellSize, GRAY);
                    } else {
                        // Draw floor character
                        DrawText(".", drawX, drawY, cellSize, DARKGRAY);
                    }
                }
            }
            
            // Draw player in center of grid
            // gridWidth/2 = 20/2 = 10 (center column)
            // gridHeight/2 = 15/2 = 7 (center row)
            int playerX = startX + (gridWidth/2) * cellSize;
            int playerY = startY + (gridHeight/2) * cellSize;
            DrawText("@", playerX, playerY, cellSize, WHITE);
            
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
```

### Key Concepts Explained

**Nested Loops**: We use two `for` loops (one inside the other) because we need to visit every cell in a 2D grid. The outer loop (`for y...`) iterates through the rows, and the inner loop (`for x...`) iterates through each column within that row. This is a fundamental pattern for working with any 2D data.

**Grid to Screen Conversion**: This is one of the most important concepts in 2D games. We have a "game world" coordinate system (the grid, e.g., tile `(5, 3)`) and a "screen" coordinate system (pixels, e.g., `(150, 90)`). The formula `screenPos = startPos + gridPos * cellSize` is how we translate from one to the other. Mastering this translation is key to all rendering.

**Why cellSize = 30?**: This makes our characters readable but not huge. Too small and text is hard to read, too large and we can't fit much on screen.

## Handling Basic Input

Let's make it interactive:

```c
#include "raylib.h"
#include <stdio.h>

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "Input Test");
    SetTargetFPS(60);
    
    // Variables
    char lastKey = ' ';
    int mouseX = 0;
    int mouseY = 0;
    
    while (!WindowShouldClose()) {
        // Update
        // Check for key presses
        if (IsKeyPressed(KEY_W)) lastKey = 'W';
        if (IsKeyPressed(KEY_A)) lastKey = 'A';
        if (IsKeyPressed(KEY_S)) lastKey = 'S';
        if (IsKeyPressed(KEY_D)) lastKey = 'D';
        if (IsKeyPressed(KEY_SPACE)) lastKey = ' ';
        
        // Get mouse position
        mouseX = GetMouseX();
        mouseY = GetMouseY();
        
        // Draw
        BeginDrawing();
            ClearBackground(BLACK);
            
            DrawText("Press WASD or SPACE keys", 10, 10, 20, WHITE);
            
            // Show last key pressed
            char keyText[50];
            sprintf(keyText, "Last key: %c", lastKey);
            DrawText(keyText, 10, 40, 20, YELLOW);
            
            // Show mouse position
            char mouseText[50];
            sprintf(mouseText, "Mouse: (%d, %d)", mouseX, mouseY);
            DrawText(mouseText, 10, 70, 20, GREEN);
            
            // Draw something at mouse position
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                DrawText("*", mouseX, mouseY, 30, RED);
            }
            
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
```

## Using a Monospace Font

For ASCII games, monospace fonts (where all characters have the same width) work best:

```c
#include "raylib.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "Monospace Font");
    SetTargetFPS(60);
    
    // Load a monospace font (if you have one)
    // Font monoFont = LoadFont("resources/monospace.ttf");
    
    // Or use the default font with consistent spacing
    int fontSize = 20;
    int charWidth = MeasureText("@", fontSize);  // All chars same width in monospace
    
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw aligned characters
            for (int i = 0; i < 10; i++) {
                DrawText("#", 100 + i * charWidth, 100, fontSize, GRAY);
                DrawText(".", 100 + i * charWidth, 100 + fontSize, fontSize, DARKGRAY);
                DrawText("#", 100 + i * charWidth, 100 + fontSize * 2, fontSize, GRAY);
            }
            
            DrawText("Monospace alignment is important for ASCII games!", 10, 300, 20, WHITE);
            
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
```

## Colors in Raylib

Raylib provides many predefined colors:

```c
#include "raylib.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "Raylib Colors");
    SetTargetFPS(60);
    
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Common colors for games
            DrawText("@", 100, 100, 30, WHITE);        // Player
            DrawText("#", 150, 100, 30, GRAY);         // Wall
            DrawText("g", 200, 100, 30, RED);          // Enemy
            DrawText("!", 250, 100, 30, GREEN);        // Health
            DrawText("$", 300, 100, 30, GOLD);         // Treasure
            DrawText("~", 350, 100, 30, BLUE);         // Water
            DrawText("^", 400, 100, 30, BROWN);        // Mountain
            DrawText("*", 450, 100, 30, YELLOW);       // Magic
            
            // Custom color (RGBA)
            Color customPurple = {128, 0, 128, 255};   // R, G, B, Alpha
            DrawText("&", 500, 100, 30, customPurple);
            
            DrawText("Different colors for different game elements", 10, 200, 20, WHITE);
            
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
```

## Your First Mini ASCII Scene

Let's combine everything into a small scene:

```c
#include "raylib.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "ASCII Scene");
    SetTargetFPS(60);
    
    // Scene data
    const char* scene[10] = {
        "##########",
        "#........#",
        "#...@....#",
        "#........#",
        "#.g....!.#",
        "#........#",
        "#...##...#",
        "#...##...#",
        "#........#",
        "##########"
    };
    
    int cellSize = 30;
    int startX = 200;
    int startY = 100;
    
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw the scene
            for (int y = 0; y < 10; y++) {
                for (int x = 0; x < 10; x++) {
                    char tile = scene[y][x];
                    Color color = WHITE;
                    
                    // Choose color based on tile
                    switch(tile) {
                        case '#': color = GRAY; break;
                        case '@': color = WHITE; break;
                        case 'g': color = RED; break;
                        case '!': color = GREEN; break;
                        case '.': color = DARKGRAY; break;
                    }
                    
                    // Convert char to string for DrawText
                    char str[2] = {tile, '\0'};
                    DrawText(str, startX + x * cellSize, startY + y * cellSize, 
                            cellSize, color);
                }
            }
            
            DrawText("A simple ASCII scene", 10, 450, 20, WHITE);
            DrawText("@ = You, g = Goblin, ! = Potion, # = Wall", 10, 480, 20, WHITE);
            
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
```

## Common Raylib Functions for ASCII Games

Here are the main functions you'll use:

```c
// Window
InitWindow(width, height, "title");
WindowShouldClose();  // Returns true when X clicked or ESC pressed
CloseWindow();

// Drawing
BeginDrawing();
EndDrawing();
ClearBackground(color);
DrawText("text", x, y, fontSize, color);

// Input
IsKeyPressed(key);    // Was key just pressed?
IsKeyDown(key);       // Is key being held?
IsMouseButtonPressed(button);
GetMouseX();
GetMouseY();

// Timing
SetTargetFPS(60);
GetFrameTime();       // Time since last frame
```

## Troubleshooting Common Issues

1. **"Cannot find -lraylib"**: Raylib isn't installed or isn't in the library path
   - Solution: Check installation, use -L flag to specify library location

2. **Window opens and immediately closes**: You might have an error in your code
   - Solution: Run from terminal to see error messages

3. **Text looks blurry**: The font size might be too small or large
   - Solution: Experiment with different font sizes

4. **Compilation errors**: Missing includes or typos
   - Solution: Check that raylib.h is included and accessible

## Practice Projects

### "Try This" Challenge 1: Your Own Scene
Modify the "Mini ASCII Scene" example. Change the `scene` array to design your own small level. Add a new tile type (e.g., a key 'k' or a treasure chest 'C') and give it a unique color in the `switch` statement.

### "Try This" Challenge 2: Interactive Drawing
Modify the "Input Test" example. Instead of just drawing a `*` at the mouse position, make it so clicking the left mouse button draws a blue circle (`DrawCircle`) and clicking the right mouse button draws a red square (`DrawRectangle`).

### "Try This" Challenge 3: Grid Pattern
Modify the "ASCII Grid" example. Instead of drawing walls only on the border, can you create a checkerboard pattern of `.` and `#` tiles?
*Hint: `(x + y) % 2 == 0` will be true for every other tile.*

## Summary

You've learned:
- How to install and set up Raylib
- Creating a window and game loop
- Drawing text and ASCII characters
- Handling basic input
- Using colors effectively
- Creating ASCII scenes

Raylib makes game development accessible while teaching fundamental concepts!

## Next Steps

In the next lesson, we'll dive deeper into ASCII rendering and create a proper game display system. We'll learn about:
- Efficient grid rendering
- Character-based graphics
- Creating game boards
- Managing screen space

Before moving on:
1. Make sure Raylib is properly installed
2. Run all the example programs
3. Try the practice projects
4. Experiment with different colors and layouts

Ready to create proper ASCII graphics? Head to [Lesson 8: ASCII Characters and Rendering](08-ascii-rendering.md)!

## Quick Compile Reference

Save this for easy compiling:

```bash
# Linux/Mac
gcc game.c -o game -lraylib -lm -lpthread -ldl

# Windows
gcc game.c -o game.exe -lraylib -lopengl32 -lgdi32 -lwinmm

# With debugging
gcc -g game.c -o game -lraylib -lm

# With warnings
gcc -Wall game.c -o game -lraylib -lm
```