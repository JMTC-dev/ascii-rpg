# Lesson 8: ASCII Characters and Rendering - Drawing Your Game World

Now that you can create windows with Raylib, let's build a proper ASCII rendering system for our RPG. We'll create a flexible system that makes it easy to draw game worlds, characters, and UI elements.

## The Power of ASCII Art

ASCII art has a rich history in gaming. Before modern graphics, games like Rogue (1980) and NetHack used ASCII characters to represent entire worlds:

```
--------- 
|.......| 
|.....@.| 
|.......| 
|...>...| 
|.......| 
---------
```

Each character has meaning: `@` is the player, `>` is stairs down, `|` and `-` are walls. Simple, but effective!

## Understanding Our Rendering Approach

Before we dive into code, let's understand what we're building:

1. **Tile System**: Each location in our game world is a "tile" represented by a character
2. **Color Mapping**: Each tile type has a specific color to make it recognizable
3. **Flexible Rendering**: We can easily add new tile types without changing our core code

Think of it like creating a legend for a map - we decide that '#' means wall, '.' means floor, etc.

## Creating a Tile System

Let's start by defining what each ASCII character represents. We'll use `#define` to create named constants:

```c
#include "raylib.h"
#include <stdio.h>

// Why use #define?
// 1. Makes code readable: TILE_WALL is clearer than '#'
// 2. Easy to change: If we want walls to be '*' instead, we change it in one place
// 3. Prevents typos: The compiler will catch "TILE_WLAL" but not '#'

// Tile type definitions
#define TILE_FLOOR     '.'    // Empty space you can walk on
#define TILE_WALL      '#'    // Solid barrier
#define TILE_PLAYER    '@'    // That's you!
#define TILE_GOBLIN    'g'    // Basic enemy
#define TILE_ORC       'o'    // Stronger enemy
#define TILE_POTION    '!'    // Health restore item
#define TILE_GOLD      '$'    // Currency
#define TILE_DOOR      '+'    // Can be opened
#define TILE_STAIRS_DOWN '>'  // Go to next level
#define TILE_STAIRS_UP   '<'  // Return to previous level
#define TILE_WATER     '~'    // Impassable terrain
#define TILE_TREE      'T'    // Decoration/obstacle
#define TILE_CHEST     'C'    // Contains treasure

// Function to get the right color for each tile
// Why use a function? It keeps all color logic in one place
Color GetTileColor(char tile) {
    // A switch statement checks the tile character and returns appropriate color
    switch(tile) {
        case TILE_WALL:      
            return GRAY;  // Gray for stone walls
            
        case TILE_FLOOR:     
            return (Color){50, 50, 50, 255};  // Dark gray for floor
            // Note: (Color){R, G, B, A} creates a custom color
            // R,G,B = Red, Green, Blue (0-255)
            // A = Alpha (transparency), 255 = fully opaque
            
        case TILE_PLAYER:    
            return WHITE;  // Player stands out in white
            
        case TILE_GOBLIN:    
            return (Color){0, 255, 0, 255};   // Bright green for visibility
            
        case TILE_ORC:       
            return (Color){255, 128, 0, 255}; // Orange - more dangerous
            
        case TILE_POTION:    
            return RED;  // Red cross association with health
            
        case TILE_GOLD:      
            return GOLD;  // Raylib has predefined GOLD color
            
        case TILE_DOOR:      
            return BROWN;  // Wooden door
            
        case TILE_STAIRS_DOWN: 
        case TILE_STAIRS_UP:   
            return (Color){200, 200, 200, 255};  // Light gray
            
        case TILE_WATER:     
            return BLUE;  // Obviously!
            
        case TILE_TREE:      
            return (Color){0, 128, 0, 255};   // Dark green for foliage
            
        case TILE_CHEST:     
            return (Color){139, 69, 19, 255}; // Brown wood color
            
        default:             
            return WHITE;  // If we don't recognize the tile, use white
    }
}

// Function to get human-readable descriptions
// This will be useful for UI tooltips or help screens
const char* GetTileDescription(char tile) {
    switch(tile) {
        case TILE_WALL:      return "Stone Wall";
        case TILE_FLOOR:     return "Stone Floor";
        case TILE_PLAYER:    return "You";
        case TILE_GOBLIN:    return "Goblin";
        case TILE_ORC:       return "Orc";
        case TILE_POTION:    return "Health Potion";
        case TILE_GOLD:      return "Gold Coins";
        case TILE_DOOR:      return "Wooden Door";
        case TILE_WATER:     return "Water";
        case TILE_TREE:      return "Tree";
        case TILE_CHEST:     return "Treasure Chest";
        default:             return "Unknown";
    }
}
```

## Building a Render System

Now let's create a flexible rendering system. But first, let's understand why we need one:

### Why Create a Rendering System?

Instead of calling `DrawText()` with hardcoded positions and colors everywhere in our game loop, we'll create a centralized rendering system. This is a crucial concept in game architecture called **abstraction**. We are abstracting away the "how" of drawing, so the rest of our code can just say "what" to draw.

Our system will:
1.  Handle the math of converting grid positions (like `5, 10`) to screen positions (like `150, 300`).
2.  Automatically look up the correct color and symbol for a given tile type.
3.  Allow us to change the entire look of the game (font size, colors, etc.) by editing one central place.

### Understanding Structures (typedef struct)

Before we continue, let's understand this new C concept:

```c
// A structure groups related variables together
typedef struct {
    int cellWidth;      
    int cellHeight;     
    int fontSize;       
    int offsetX;        
    int offsetY;        
} RenderConfig;

// This creates a new type called "RenderConfig" that contains 5 integers
// It's like creating a custom variable that holds multiple values
```

Why use structures? Instead of passing 5 separate variables to every function, we can pass one structure that contains all 5. It's like putting related items in a labeled box.

### Our Rendering System

```c
// Rendering configuration structure
// This holds all settings for how we draw our game
typedef struct {
    int cellWidth;      // Width of each grid cell in pixels
    int cellHeight;     // Height of each grid cell in pixels
    int fontSize;       // Size of text characters
    int offsetX;        // Where to start drawing from left edge
    int offsetY;        // Where to start drawing from top edge
} RenderConfig;

// Function to create and initialize our render settings
RenderConfig InitRenderConfig(int screenWidth, int screenHeight) {
    RenderConfig config;
    
    // Set font size first - this determines everything else
    config.fontSize = 24;
    
    // Make cells square and sized to fit the font
    // Why? Square cells ensure '@' moves same distance horizontally and vertically
    config.cellWidth = config.fontSize;
    config.cellHeight = config.fontSize;
    
    // Position the game area on screen
    // offsetX = 50 means start drawing 50 pixels from left edge
    // This creates margins so game doesn't touch screen edges
    config.offsetX = 50;
    config.offsetY = 50;
    
    return config;
}

// Function to draw a single tile at a grid position
void DrawTile(char tile, int x, int y, RenderConfig* config) {
    // Convert grid position to screen position
    // If we want to draw at grid position (3, 2):
    // screenX = 50 + (3 * 24) = 50 + 72 = 122 pixels from left
    // screenY = 50 + (2 * 24) = 50 + 48 = 98 pixels from top
    int screenX = config->offsetX + (x * config->cellWidth);
    int screenY = config->offsetY + (y * config->cellHeight);
    
    // Get the appropriate color for this tile type
    Color color = GetTileColor(tile);
    
    // DrawText needs a string, but we have a char
    // We create a 2-character string: our character + null terminator
    char str[2] = {tile, '\0'};
    // Example: if tile = '@', str becomes "@\0" which is a valid C string
    
    // Finally, draw the character at the calculated position
    DrawText(str, screenX, screenY, config->fontSize, color);
}

// Function to draw an entire grid of tiles
void DrawTileGrid(char* grid, int width, int height, RenderConfig* config) {
    // Loop through every position in our grid
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Calculate which tile to draw
            // In a 1D array representing a 2D grid:
            // Position (x,y) is at index: y * width + x
            // Example: In a 5-wide grid, position (2,1) is at index 1*5+2 = 7
            char tile = grid[y * width + x];
            
            // Draw this tile
            DrawTile(tile, x, y, config);
        }
    }
}
```

### Understanding the Pointer (*) Symbol

You might have noticed `RenderConfig* config` in our functions. The `*` makes this a "pointer" - don't worry too much about this yet, but here's the basic idea:

- `RenderConfig config` = Pass a copy of the structure (uses more memory)
- `RenderConfig* config` = Pass the location of the structure (more efficient)
- When using a pointer, we access members with `->` instead of `.`

For now, just follow the pattern. We'll explore pointers more deeply later.

## Creating Visual Effects

Let's add some visual flair to make our ASCII game more appealing:

```c
// Animated water effect
char GetAnimatedWaterTile(float time) {
    int frame = ((int)(time * 2)) % 3;
    switch(frame) {
        case 0: return '~';
        case 1: return '≈';
        case 2: return '-';
        default: return '~';
    }
}

// Draw tile with background
void DrawTileWithBackground(char tile, int x, int y, RenderConfig* config, Color bgColor) {
    int screenX = config->offsetX + (x * config->cellWidth);
    int screenY = config->offsetY + (y * config->cellHeight);
    
    // Draw background
    DrawRectangle(screenX, screenY, config->cellWidth, config->cellHeight, bgColor);
    
    // Draw tile
    Color color = GetTileColor(tile);
    char str[2] = {tile, '\0'};
    DrawText(str, screenX + 2, screenY, config->fontSize, color);
}

// Highlight effect (for selection or hover)
void DrawTileHighlight(int x, int y, RenderConfig* config, Color highlightColor) {
    int screenX = config->offsetX + (x * config->cellWidth);
    int screenY = config->offsetY + (y * config->cellHeight);
    
    // Draw border
    DrawRectangleLines(screenX, screenY, config->cellWidth, config->cellHeight, highlightColor);
}
```

## Complete Rendering Example

Let's put everything together in a working example. I'll build it step by step so you can understand each part:

### Step 1: Basic Setup

First, let's include what we need and set up our window:

```c
#include "raylib.h"
#include <string.h>  // For strcpy() to copy strings

int main() {
    // Larger window to fit our game nicely
    const int screenWidth = 1024;
    const int screenHeight = 768;
    
    InitWindow(screenWidth, screenHeight, "ASCII Rendering System");
    SetTargetFPS(60);
    
    // Initialize our render configuration
    RenderConfig renderConfig = InitRenderConfig(screenWidth, screenHeight);
```

### Step 2: Creating a Sample Map

Now let's create a small game map. We'll use a string to define it:

```c
    // Define our map size
    const int mapWidth = 20;
    const int mapHeight = 15;
    
    // Create map as one long string
    // Each row is 20 characters, total 15 rows = 300 characters
    char map[300] = 
        "####################"  // Row 0: Top wall
        "#..................#"  // Row 1: Empty space with side walls
        "#...@......g.......#"  // Row 2: Player (@) and goblin (g)
        "#..................#"  // Row 3: Empty
        "#...########.......#"  // Row 4-8: Inner room
        "#...#......#.......#"
        "#...#......+.......#"  // + is a door
        "#...#......#.......#"
        "#...########.......#"
        "#..................#"  // Row 9: Empty
        "#.....!............#"  // Row 10: Health potion (!)
        "#...........$......#"  // Row 11: Gold ($)
        "#.......~~~~.......#"  // Row 12: Water (~)
        "#..................#"  // Row 13: Empty
        "####################"; // Row 14: Bottom wall
```

### Step 3: Adding Interactivity

Let's add a cursor so players can explore the map:

```c
    // Cursor position (starts at top-left)
    int cursorX = 0;
    int cursorY = 0;
    
    while (!WindowShouldClose()) {
        // Handle cursor movement
        if (IsKeyPressed(KEY_RIGHT) && cursorX < mapWidth - 1) {
            cursorX++;  // Move right (but not past edge)
        }
        if (IsKeyPressed(KEY_LEFT) && cursorX > 0) {
            cursorX--;  // Move left (but not past edge)
        }
        if (IsKeyPressed(KEY_DOWN) && cursorY < mapHeight - 1) {
            cursorY++;  // Move down
        }
        if (IsKeyPressed(KEY_UP) && cursorY > 0) {
            cursorY--;  // Move up
        }
```

### Step 4: Drawing Everything

Now let's render our map:

```c
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw the map using our rendering system
            DrawTileGrid(map, mapWidth, mapHeight, &renderConfig);
            
            // Draw cursor highlight (we'll create this function)
            DrawRectangleLines(
                renderConfig.offsetX + cursorX * renderConfig.cellWidth,
                renderConfig.offsetY + cursorY * renderConfig.cellHeight,
                renderConfig.cellWidth,
                renderConfig.cellHeight,
                YELLOW
            );
            
            // Draw title
            DrawText("ASCII RPG Renderer", 10, 10, 30, WHITE);
            
            // Show information about tile under cursor
            char hoveredTile = map[cursorY * mapWidth + cursorX];
            const char* tileDesc = GetTileDescription(hoveredTile);
            
            // sprintf creates formatted strings (like printf but to a string)
            char infoText[100];
            sprintf(infoText, "Cursor at (%d, %d): %s", cursorX, cursorY, tileDesc);
            DrawText(infoText, 10, screenHeight - 30, 20, WHITE);
            
            // Instructions
            DrawText("Use arrow keys to move cursor", screenWidth - 300, 10, 16, GRAY);
            DrawText("ESC to exit", screenWidth - 300, 30, 16, GRAY);
            
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
```

### Complete Working Program

Here's everything together with all our helper functions:

```c
#include "raylib.h"
#include <string.h>
#include <stdio.h>   // For sprintf()

// Include all our tile definitions and functions from earlier
// (GetTileColor, GetTileDescription, etc.)

// Add this helper function for cursor highlighting
void DrawTileHighlight(int x, int y, RenderConfig* config, Color color) {
    int screenX = config->offsetX + x * config->cellWidth;
    int screenY = config->offsetY + y * config->cellHeight;
    DrawRectangleLines(screenX, screenY, config->cellWidth, config->cellHeight, color);
}

int main() {
    // ... (all the code from steps 1-4)
}
```

### What You've Learned

1. **Modular Design**: We separated rendering logic from game logic
2. **Data Representation**: Our map is just characters, but renders as a colorful world
3. **Grid Systems**: Converting between grid coordinates and screen pixels
4. **User Interface**: Showing helpful information to the player

### "Try This" Challenges

1.  **New Tile Type:** Add a new tile to the system. Pick a character (e.g., `C` for Chest), add a new `#define` for it, and give it a color and description in the `GetTileColor` and `GetTileDescription` functions. Then, add your new tile to the `map` string in the main example to see it appear!

2.  **Flashing Cursor:** In the main example, make the yellow cursor highlight fade in and out.
    *   *Hint:* Use `GetTime()` from Raylib to get a constantly changing value. The `sin()` function from `<math.h>` is great for creating smooth oscillations. `float alpha = (sin(GetTime() * 5.0f) + 1.0f) / 2.0f;` will give you a value that smoothly cycles between 0.0 and 1.0. You can use this with `Fade(YELLOW, alpha)` to change the color's transparency.

3.  **Health-Based Player Color:** Modify the `DrawTile` function. If the tile being drawn is the `TILE_PLAYER`, have it check a `playerHealth` variable. If the health is below 50, draw the player in `ORANGE`. If it's below 25, draw them in `RED`. Otherwise, draw them in `WHITE`. This provides instant visual feedback to the player.

## Advanced ASCII Techniques

### 1. Double-Width Characters

Some ASCII art uses character combinations:

```c
void DrawDoubleWall(int x, int y, RenderConfig* config) {
    int screenX = config->offsetX + (x * config->cellWidth);
    int screenY = config->offsetY + (y * config->cellHeight);
    DrawText("██", screenX, screenY, config->fontSize, GRAY);
}
```

### 2. Box Drawing Characters

Use special characters for better walls:

```c
// Box drawing characters
#define BOX_HORIZONTAL   '─'
#define BOX_VERTICAL     '│'
#define BOX_TOP_LEFT     '┌'
#define BOX_TOP_RIGHT    '┐'
#define BOX_BOTTOM_LEFT  '└'
#define BOX_BOTTOM_RIGHT '┘'
#define BOX_CROSS        '┼'
```

### 3. Layered Rendering

Draw background and foreground separately:

```c
typedef struct {
    char background[300];  // Floor, walls, etc.
    char foreground[300];  // Entities, items, etc.
} LayeredMap;

void DrawLayeredMap(LayeredMap* map, int width, int height, RenderConfig* config) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            
            // Draw background
            if (map->background[index] != ' ') {
                DrawTile(map->background[index], x, y, config);
            }
            
            // Draw foreground
            if (map->foreground[index] != ' ') {
                DrawTile(map->foreground[index], x, y, config);
            }
        }
    }
}
```

## Creating a Mini-Map

ASCII games often include mini-maps:

```c
void DrawMiniMap(char* map, int mapWidth, int mapHeight, int screenX, int screenY) {
    int miniCellSize = 4;
    
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            char tile = map[y * mapWidth + x];
            Color color;
            
            // Simplified colors for mini-map
            switch(tile) {
                case TILE_WALL: color = GRAY; break;
                case TILE_PLAYER: color = WHITE; break;
                case TILE_WATER: color = BLUE; break;
                default: color = (Color){20, 20, 20, 255}; break;
            }
            
            DrawRectangle(screenX + x * miniCellSize, 
                         screenY + y * miniCellSize, 
                         miniCellSize, miniCellSize, color);
        }
    }
    
    // Draw border
    DrawRectangleLines(screenX - 1, screenY - 1, 
                      mapWidth * miniCellSize + 2, 
                      mapHeight * miniCellSize + 2, WHITE);
}
```

## Performance Optimization

For larger maps, optimize rendering:

```c
typedef struct {
    int viewportWidth;
    int viewportHeight;
    int cameraX;
    int cameraY;
} Camera;

void DrawMapWithCamera(char* map, int mapWidth, int mapHeight, 
                      Camera* camera, RenderConfig* config) {
    // Only draw visible tiles
    int startX = camera->cameraX;
    int startY = camera->cameraY;
    int endX = startX + camera->viewportWidth;
    int endY = startY + camera->viewportHeight;
    
    // Clamp to map bounds
    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;
    if (endX > mapWidth) endX = mapWidth;
    if (endY > mapHeight) endY = mapHeight;
    
    // Draw only visible portion
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            char tile = map[y * mapWidth + x];
            // Draw relative to camera
            DrawTile(tile, x - startX, y - startY, config);
        }
    }
}
```

## ASCII Particle Effects

Add visual feedback for actions:

```c
typedef struct {
    float x, y;
    float vx, vy;
    char symbol;
    Color color;
    float lifetime;
} Particle;

void UpdateParticle(Particle* p, float deltaTime) {
    p->x += p->vx * deltaTime;
    p->y += p->vy * deltaTime;
    p->lifetime -= deltaTime;
    
    // Fade out
    p->color.a = (unsigned char)(255 * (p->lifetime / 1.0f));
}

void DrawParticle(Particle* p, RenderConfig* config) {
    if (p->lifetime > 0) {
        int screenX = config->offsetX + (int)(p->x * config->cellWidth);
        int screenY = config->offsetY + (int)(p->y * config->cellHeight);
        char str[2] = {p->symbol, '\0'};
        DrawText(str, screenX, screenY, config->fontSize, p->color);
    }
}
```

## Practice Exercises

### Exercise 1: Custom Tileset
Create your own set of ASCII tiles for different terrain types (grass, stone, sand, etc.)

### Exercise 2: Lighting System
Implement a simple lighting system where tiles get darker the farther they are from the player

### Exercise 3: ASCII Animation
Create animated tiles for fire (using ^, *, and ') that cycle through frames

## Common Rendering Patterns

```c
// Fog of war
void DrawWithFogOfWar(char* map, bool* visible, int width, int height, RenderConfig* config) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            if (visible[index]) {
                DrawTile(map[index], x, y, config);
            } else {
                DrawTile(' ', x, y, config);  // Or a dimmed version
            }
        }
    }
}

// Health bar using ASCII
void DrawHealthBar(int current, int max, int x, int y) {
    int barLength = 10;
    int filled = (current * barLength) / max;
    
    DrawText("[", x, y, 20, WHITE);
    for (int i = 0; i < barLength; i++) {
        if (i < filled) {
            DrawText("=", x + 20 + i * 15, y, 20, RED);
        } else {
            DrawText("-", x + 20 + i * 15, y, 20, DARKGRAY);
        }
    }
    DrawText("]", x + 20 + barLength * 15, y, 20, WHITE);
}
```

## Summary

You've learned:
- Creating a tile system with meaningful ASCII characters
- Building a flexible rendering system
- Adding visual effects and animations
- Optimizing rendering with cameras
- Creating UI elements with ASCII

Your ASCII renderer is now capable of displaying complex game worlds!

## Next Steps

In the next lesson, we'll make our game interactive by adding player movement and input handling. You'll learn to:
- Handle keyboard input properly
- Move the player character
- Implement smooth movement
- Add input feedback

Ready to make things move? Head to [Lesson 9: Player Movement and Input](09-player-movement.md)!

## ASCII Reference Sheet

Save this for quick reference:
```
Player/NPCs:    @ = Player, g = Goblin, o = Orc, k = Knight
Terrain:        # = Wall, . = Floor, ~ = Water, ^ = Mountain
Items:          ! = Potion, $ = Gold, / = Sword, ] = Armor
Dungeon:        + = Door, > = Stairs down, < = Stairs up
Nature:         T = Tree, " = Grass, * = Flower
Special:        % = Food, ? = Scroll, = = Chest
```