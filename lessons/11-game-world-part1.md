# Lesson 11: Creating Game Worlds

Time to expand beyond single screens! In this lesson, we'll create larger game worlds with multiple rooms, implement camera systems, and even touch on procedural generation. Your RPG is about to get much bigger!

## The "Why": Hand-Crafted vs. Procedural Worlds

So far, our maps have been "hand-crafted"—we designed them character by character in our code. This is great for small, specific layouts. But what if you want a new dungeon every time you play?

This is where **procedural generation** comes in. We write an algorithm that creates the level *for* us. This gives our game:
- **Infinite Replayability**: No two playthroughs are the same.
- **Surprise and Discovery**: Even the developer doesn't know what the next map will look like.
- **Efficiency**: We can generate huge worlds from a small amount of code.

## Basic Map Structure

So far, we've been using fixed-size arrays for our maps. But what if we want different sized levels? Let's build a flexible map system that can handle any size.

### Understanding Dynamic Memory

Until now, our maps looked like this:
```c
char map[300];  // Fixed size: 20x15 = 300 tiles
```

This works, but it's limiting. We need dynamic memory allocation - requesting memory as needed.

### Building a Flexible Map System

First, let's understand what we need in a map:

```c
#include "raylib.h"
#include <stdlib.h>    // For malloc() and free()
#include <string.h>    // For strcpy() and memset()
#include <stdbool.h>

// Define what data represents a map
typedef struct {
    char* tiles;      // Pointer to tile array (dynamically sized)
    int width;        // Map width in tiles
    int height;       // Map height in tiles
    char* name;       // Map name (e.g., "Dungeon Level 1")
    int startX;       // Where player starts X
    int startY;       // Where player starts Y
} Map;
```

Now let's create functions to work with maps:

```c
// Create a new map with any size
Map* CreateMap(int width, int height, const char* name) {
    // Allocate memory for the Map structure itself
    Map* map = (Map*)malloc(sizeof(Map));
    
    // Store the dimensions
    map->width = width;
    map->height = height;
    
    // Allocate memory for all tiles
    // width * height = total number of tiles needed
    map->tiles = (char*)malloc(width * height * sizeof(char));
    
    // Allocate memory for the name and copy it
    // strlen(name) + 1 because we need space for '\0'
    map->name = (char*)malloc(strlen(name) + 1);
    strcpy(map->name, name);
    
    // Set default start position to center
    map->startX = width / 2;
    map->startY = height / 2;
    
    // Initialize all tiles as floor ('.')
    // memset fills memory with a specific value
    memset(map->tiles, '.', width * height);
    
    return map;  // Return pointer to our new map
}

// IMPORTANT: Free memory when done!
// Every malloc() needs a matching free()
void DestroyMap(Map* map) {
    if (map) {  // Check map isn't NULL
        free(map->tiles);   // Free tile array
        free(map->name);    // Free name string
        free(map);          // Free map structure
    }
}
```

### Safe Tile Access Functions

Direct array access can crash if we go out of bounds. Let's make it safe:

```c
// Safely get a tile at any position
char GetTile(Map* map, int x, int y) {
    // Check boundaries first
    if (x < 0 || x >= map->width || y < 0 || y >= map->height) {
        return '#';  // Treat out-of-bounds as walls
    }
    
    // Convert 2D position to 1D array index
    return map->tiles[y * map->width + x];
}

// Safely set a tile
void SetTile(Map* map, int x, int y, char tile) {
    // Only set if position is valid
    if (x >= 0 && x < map->width && y >= 0 && y < map->height) {
        map->tiles[y * map->width + x] = tile;
    }
    // If invalid position, silently do nothing
}
```

### Why This Design?

1. **Flexibility**: Create maps of any size
2. **Safety**: Boundary checking prevents crashes
3. **Encapsulation**: All map data stays together
4. **Memory Efficiency**: Only use what you need

### Using the Map System

```c
// Example usage
Map* dungeon = CreateMap(50, 50, "Dark Dungeon");
SetTile(dungeon, 10, 10, '#');  // Place a wall
char tile = GetTile(dungeon, 10, 10);  // Read it back

// Don't forget to clean up!
DestroyMap(dungeon);
```

## Room Generation

Now let's create interesting dungeons! We'll use procedural generation - creating content through algorithms rather than designing it by hand.

### Understanding Procedural Generation

Instead of manually designing each level, we'll teach the computer to build them for us. This gives us:
- Infinite variety
- Replayability  
- Surprises even for the developer!

### Building Blocks: Rooms

First, let's define what a room is:

```c
typedef struct {
    int x, y;           // Top-left corner position
    int width, height;  // Room dimensions
} Room;
```

Now let's carve out rooms in our map:

```c
// Create a rectangular room by setting tiles to floor
void CreateRoom(Map* map, Room room) {
    // Loop through all tiles in the room area
    for (int y = room.y; y < room.y + room.height; y++) {
        for (int x = room.x; x < room.x + room.width; x++) {
            SetTile(map, x, y, '.');  // Floor tile
        }
    }
}

// Create walls around the entire map edge
void CreateBorder(Map* map) {
    // Top and bottom walls
    for (int x = 0; x < map->width; x++) {
        SetTile(map, x, 0, '#');                    // Top edge
        SetTile(map, x, map->height - 1, '#');      // Bottom edge
    }
    
    // Left and right walls
    for (int y = 0; y < map->height; y++) {
        SetTile(map, 0, y, '#');                    // Left edge
        SetTile(map, map->width - 1, y, '#');       // Right edge
    }
}
```

### Connecting Rooms with Corridors

Rooms need connections or players get stuck! Let's dig corridors:

```c
// Connect two points with an L-shaped corridor
void CreateCorridor(Map* map, int x1, int y1, int x2, int y2) {
    // We'll use an L-shaped path: horizontal first, then vertical
    // This creates corridors with right angles (like in classic roguelikes)
    
    int x = x1;
    int y = y1;
    
    // Move horizontally toward target
    while (x != x2) {
        SetTile(map, x, y, '.');
        
        // Move one step closer to x2
        if (x2 > x) {
            x++;  // Move right
        } else {
            x--;  // Move left
        }
    }
    
    // Now move vertically to reach target
    while (y != y2) {
        SetTile(map, x, y, '.');
        
        // Move one step closer to y2
        if (y2 > y) {
            y++;  // Move down
        } else {
            y--;  // Move up
        }
    }
}
```

### The Dungeon Generation Algorithm

Now let's put it all together to generate a complete dungeon:

```c
// Generate a random dungeon with connected rooms
Map* GenerateDungeon(int width, int height, int roomCount) {
    // Create empty map
    Map* map = CreateMap(width, height, "Dungeon");
    
    // Step 1: Fill everything with walls
    memset(map->tiles, '#', width * height);
    
    // Step 2: Create array to store room data
    Room* rooms = (Room*)malloc(roomCount * sizeof(Room));
    
    // Step 3: Generate random rooms
    for (int i = 0; i < roomCount; i++) {
        // Random room size (5-14 wide, 5-12 tall)
        rooms[i].width = 5 + rand() % 10;
        rooms[i].height = 5 + rand() % 8;
        
        // Random position (with 1 tile margin from edges)
        rooms[i].x = 1 + rand() % (width - rooms[i].width - 2);
        rooms[i].y = 1 + rand() % (height - rooms[i].height - 2);
        
        // Carve out this room
        CreateRoom(map, rooms[i]);
    }
    
    // Step 4: Connect all rooms with corridors
    for (int i = 0; i < roomCount - 1; i++) {
        // Find center of current room
        int x1 = rooms[i].x + rooms[i].width / 2;
        int y1 = rooms[i].y + rooms[i].height / 2;
        
        // Find center of next room
        int x2 = rooms[i + 1].x + rooms[i + 1].width / 2;
        int y2 = rooms[i + 1].y + rooms[i + 1].height / 2;
        
        // Connect their centers
        CreateCorridor(map, x1, y1, x2, y2);
    }
    
    // Step 5: Set player start in first room center
    map->startX = rooms[0].x + rooms[0].width / 2;
    map->startY = rooms[0].y + rooms[0].height / 2;
    
    // Step 6: Populate rooms with content
    for (int i = 1; i < roomCount; i++) {  // Skip first room (player start)
        // Pick random position in room
        int x = rooms[i].x + rand() % rooms[i].width;
        int y = rooms[i].y + rand() % rooms[i].height;
        
        // Randomly place something (60% chance)
        int r = rand() % 100;
        if (r < 20) {
            SetTile(map, x, y, '!');      // 20% chance: Potion
        } else if (r < 40) {
            SetTile(map, x, y, '$');      // 20% chance: Gold
        } else if (r < 60) {
            SetTile(map, x, y, 'g');      // 20% chance: Goblin
        }
        // 40% chance: Nothing
    }
    
    // Step 7: Place exit stairs in the last room
    int lastRoom = roomCount - 1;
    SetTile(map, 
            rooms[lastRoom].x + rooms[lastRoom].width / 2,
            rooms[lastRoom].y + rooms[lastRoom].height / 2,
            '>');  // Stairs down
    
    // Clean up temporary room array
    free(rooms);
    
    return map;
}
```

### How the Algorithm Works

1. **Fill with walls**: Start with solid rock
2. **Place rooms**: Randomly position rectangular rooms
3. **Connect rooms**: Dig corridors between room centers
4. **Add content**: Place items and enemies
5. **Set start/end**: Player starts in first room, exit in last

### Room Overlap?

You might notice rooms can overlap in this simple version. That's okay for now! Some solutions:
- Check for overlaps and regenerate
- Use this as a feature (merged rooms)
- Implement a more sophisticated algorithm later

### Try Different Parameters

Experiment with the generation:
- More rooms = more complex dungeon
- Larger rooms = more open spaces
- Smaller map = more cramped feeling

## Camera System

So far we've displayed entire small maps. But what if your dungeon is 100x100 tiles? We need a camera system to show only part of the map at a time, following the player.

### Understanding the Camera Concept

Think of the camera as a window that shows only a small part of your huge map. As the player moves, this window slides over the map to keep them in view.

### Building the Camera

First, let's define what a camera needs to track:

```c
typedef struct {
    int x, y;           // Top-left corner of view (in tiles)
    int viewWidth;      // How many tiles wide to show
    int viewHeight;     // How many tiles tall to show
    int mapWidth;       // Total map width (for bounds checking)
    int mapHeight;      // Total map height (for bounds checking)
} Camera;
```

Now let's create camera functions:

```c
// Initialize a new camera
Camera CreateCamera(int viewWidth, int viewHeight, int mapWidth, int mapHeight) {
    Camera cam;
    
    // Start at top-left corner
    cam.x = 0;
    cam.y = 0;
    
    // Set view size (how much we see at once)
    cam.viewWidth = viewWidth;    // e.g., 20 tiles wide
    cam.viewHeight = viewHeight;  // e.g., 15 tiles tall
    
    // Store map bounds for clamping
    cam.mapWidth = mapWidth;
    cam.mapHeight = mapHeight;
    
    return cam;
}
```

### Following the Player

The camera should center on the player when possible:

```c
// Center camera on a target position (usually the player)
void CenterCamera(Camera* cam, int targetX, int targetY) {
    // Calculate top-left corner to center view on target
    cam->x = targetX - cam->viewWidth / 2;
    cam->y = targetY - cam->viewHeight / 2;
    
    // But don't go outside the map! Clamp to valid range.
    
    // Don't go past left edge
    if (cam->x < 0) {
        cam->x = 0;
    }
    
    // Don't go past top edge
    if (cam->y < 0) {
        cam->y = 0;
    }
    
    // Don't go past right edge
    if (cam->x + cam->viewWidth > cam->mapWidth) {
        cam->x = cam->mapWidth - cam->viewWidth;
    }
    
    // Don't go past bottom edge
    if (cam->y + cam->viewHeight > cam->mapHeight) {
        cam->y = cam->mapHeight - cam->viewHeight;
    }
}
```

### Camera Utilities

Sometimes we need to check if something is visible:

```c
// Check if a position is currently visible
bool IsInView(Camera* cam, int x, int y) {
    return x >= cam->x && x < cam->x + cam->viewWidth &&
           y >= cam->y && y < cam->y + cam->viewHeight;
}

// This is useful for:
// - Only drawing visible entities
// - Only updating nearby enemies
// - Optimizing performance
```

### Drawing with the Camera

Here's how to draw only the visible portion of the map:

```c
// Draw the visible portion of the map
void DrawMapWithCamera(Map* map, Camera* cam, int cellSize, int offsetX, int offsetY) {
    // Only loop through visible tiles, not the entire map!
    for (int viewY = 0; viewY < cam->viewHeight; viewY++) {
        for (int viewX = 0; viewX < cam->viewWidth; viewX++) {
            // Convert view position to map position
            int mapX = cam->x + viewX;
            int mapY = cam->y + viewY;
            
            // Get the tile at this map position
            char tile = GetTile(map, mapX, mapY);
            
            // Choose color based on tile type
            Color color = WHITE;
            switch(tile) {
                case '#': color = GRAY; break;      // Walls
                case '.': color = DARKGRAY; break;  // Floor
                case '!': color = RED; break;       // Potion
                case '$': color = GOLD; break;      // Treasure
                case 'g': color = GREEN; break;     // Goblin
                case '>': color = WHITE; break;     // Stairs
                default: color = WHITE;
            }
            
            // Draw at screen position (not map position!)
            char str[2] = {tile, '\0'};
            int screenX = offsetX + viewX * cellSize;
            int screenY = offsetY + viewY * cellSize;
            DrawText(str, screenX, screenY, cellSize, color);
        }
    }
}
```

### Using the Camera System

In your game loop:

```c
// During initialization
Camera camera = CreateCamera(20, 15, map->width, map->height);

// When player moves
CenterCamera(&camera, player.x, player.y);

// When drawing
DrawMapWithCamera(map, &camera, 30, 50, 50);

// Draw player at center of view (if camera is centered on them)
int playerScreenX = (player.x - camera.x) * cellSize + offsetX;
int playerScreenY = (player.y - camera.y) * cellSize + offsetY;
DrawText("@", playerScreenX, playerScreenY, cellSize, WHITE);
```

### Camera Effects

You can create interesting effects:
- **Screen shake**: Add random offset when hit
- **Smooth scrolling**: Interpolate camera position
- **Look ahead**: Offset camera in movement direction
- **Room transitions**: Snap camera to room boundaries

## Multiple Levels

Let's add multiple dungeon levels:

```c
typedef struct {
    Map** levels;        // Array of level maps
    int levelCount;      // Number of levels
    int currentLevel;    // Current level index
} World;

// Create a multi-level world
World* CreateWorld(int levelCount) {
    World* world = (World*)malloc(sizeof(World));
    world->levels = (Map**)malloc(levelCount * sizeof(Map*));
    world->levelCount = levelCount;
    world->currentLevel = 0;
    
    // Generate each level
    for (int i = 0; i < levelCount; i++) {
        int size = 40 + i * 10;  // Levels get bigger
        int rooms = 5 + i * 2;   // More rooms per level
        world->levels[i] = GenerateDungeon(size, size, rooms);
        
        char levelName[50];
        sprintf(levelName, "Dungeon Level %d", i + 1);
        free(world->levels[i]->name);
        world->levels[i]->name = (char*)malloc(strlen(levelName) + 1);
        strcpy(world->levels[i]->name, levelName);
    }
    
    return world;
}

// Get current map
Map* GetCurrentMap(World* world) {
    return world->levels[world->currentLevel];
}

// Go to next level
void NextLevel(World* world, Player* player) {
    if (world->currentLevel < world->levelCount - 1) {
        world->currentLevel++;
        Map* newMap = GetCurrentMap(world);
        player->x = newMap->startX;
        player->y = newMap->startY;
    }
}
```

## Map Loading from Files

Save and load maps from text files:

```c
// Save map to file
void SaveMapToFile(Map* map, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) return;
    
    // Write header
    fprintf(file, "%d %d\n", map->width, map->height);
    fprintf(file, "%s\n", map->name);
    fprintf(file, "%d %d\n", map->startX, map->startY);
    
    // Write tiles
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            fputc(GetTile(map, x, y), file);
        }
        fputc('\n', file);
    }
    
    fclose(file);
}

// Load map from file
Map* LoadMapFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return NULL;
    
    int width, height;
    char name[100];
    int startX, startY;
    
    // Read header
    fscanf(file, "%d %d\n", &width, &height);
    fgets(name, sizeof(name), file);
    name[strlen(name)-1] = '\0';  // Remove newline
    fscanf(file, "%d %d\n", &startX, &startY);
    
    // Create map
    Map* map = CreateMap(width, height, name);
    map->startX = startX;
    map->startY = startY;
    
    // Read tiles
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            char tile = fgetc(file);
            SetTile(map, x, y, tile);
        }
        fgetc(file);  // Skip newline
    }
    
    fclose(file);
    return map;
}
```

## Complete World Example

Here's everything working together:

```c
#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

// Include all previous structures and functions...

int main() {
    const int screenWidth = 1024;
    const int screenHeight = 768;
    
    InitWindow(screenWidth, screenHeight, "Game World System");
    SetTargetFPS(60);
    
    // Seed random number generator
    srand(time(NULL));
    
    // Create world with 3 levels
    World* world = CreateWorld(3);
    Map* currentMap = GetCurrentMap(world);
    
    // Create player at start position
    Player player = CreatePlayer(currentMap->startX, currentMap->startY);
    
    // Setup camera (20x15 viewport)
    Camera camera = CreateCamera(20, 15, currentMap->width, currentMap->height);
    CenterCamera(&camera, player.x, player.y);
    
    // Rendering setup
    int cellSize = 30;
    int mapOffsetX = 50;
    int mapOffsetY = 80;
    
    // Mini-map settings
    int miniMapSize = 3;
    int miniMapX = screenWidth - 200;
    int miniMapY = 50;
    
    while (!WindowShouldClose()) {
        // Get current map
        currentMap = GetCurrentMap(world);
        
        // Handle input
        Direction inputDir = GetInputDirection();
        if (inputDir != DIR_NONE) {
            int oldX = player.x;
            int oldY = player.y;
            
            TryMovePlayer(&player, inputDir, currentMap->tiles, 
                         currentMap->width, currentMap->height);
            
            // Center camera on player
            CenterCamera(&camera, player.x, player.y);
            
            // Check for level transition
            char currentTile = GetTile(currentMap, player.x, player.y);
            if (currentTile == '>') {
                NextLevel(world, &player);
                currentMap = GetCurrentMap(world);
                camera.mapWidth = currentMap->width;
                camera.mapHeight = currentMap->height;
                CenterCamera(&camera, player.x, player.y);
            }
            
            // Auto-pickup items
            if (currentTile == '!') {
                player.health += 25;
                if (player.health > player.maxHealth) {
                    player.health = player.maxHealth;
                }
                SetTile(currentMap, player.x, player.y, '.');
            } else if (currentTile == '$') {
                player.gold += 10;
                SetTile(currentMap, player.x, player.y, '.');
            }
        }
        
        // Draw
        BeginDrawing();
            ClearBackground(BLACK);
            
            // Draw level name
            DrawText(currentMap->name, 10, 10, 30, WHITE);
            
            // Draw main view
            DrawMapWithCamera(currentMap, &camera, cellSize, mapOffsetX, mapOffsetY);
            
            // Draw player (centered in viewport)
            int playerScreenX = (player.x - camera.x) * cellSize + mapOffsetX;
            int playerScreenY = (player.y - camera.y) * cellSize + mapOffsetY;
            DrawText("@", playerScreenX, playerScreenY, cellSize, WHITE);
            
            // Draw mini-map
            DrawText("Map:", miniMapX, miniMapY - 25, 20, WHITE);
            for (int y = 0; y < currentMap->height; y++) {
                for (int x = 0; x < currentMap->width; x++) {
                    char tile = GetTile(currentMap, x, y);
                    Color color = BLACK;
                    
                    if (tile == '#') color = GRAY;
                    else if (tile == '.') color = (Color){40, 40, 40, 255};
                    else if (tile == '>') color = YELLOW;
                    
                    // Highlight player position
                    if (x == player.x && y == player.y) color = WHITE;
                    
                    DrawRectangle(miniMapX + x * miniMapSize, 
                                 miniMapY + y * miniMapSize, 
                                 miniMapSize, miniMapSize, color);
                }
            }
            
            // Draw stats
            char stats[100];
            sprintf(stats, "HP: %d/%d | Gold: %d | Level: %d/%d", 
                    player.health, player.maxHealth, player.gold,
                    world->currentLevel + 1, world->levelCount);
            DrawText(stats, 10, 45, 20, WHITE);
            
            // Instructions
            DrawText("WASD/Arrows: Move | Find '>' to go deeper", 10, screenHeight - 30, 18, GRAY);
            
        EndDrawing();
    }
    
    // Cleanup
    for (int i = 0; i < world->levelCount; i++) {
        DestroyMap(world->levels[i]);
    }
    free(world->levels);
    free(world);
    
    CloseWindow();
    return 0;
}
```

## Advanced World Features

### 1. Biomes and Themes

```c
typedef enum {
    BIOME_DUNGEON,
    BIOME_FOREST,
    BIOME_CAVE,
    BIOME_CASTLE
} BiomeType;

typedef struct {
    BiomeType type;
    char wallTile;
    char floorTile;
    Color wallColor;
    Color floorColor;
    const char* name;
} Biome;

Biome biomes[] = {
    {BIOME_DUNGEON, '#', '.', GRAY, DARKGRAY, "Stone Dungeon"},
    {BIOME_FOREST, 'T', ',', BROWN, GREEN, "Dark Forest"},
    {BIOME_CAVE, '*', '.', DARKBROWN, DARKGRAY, "Deep Cave"},
    {BIOME_CASTLE, '#', '.', LIGHTGRAY, GRAY, "Ancient Castle"}
};
```

### 2. Persistent World State

```c
typedef struct {
    bool visited;
    bool itemsCollected[100];  // Track collected items
    bool enemiesDefeated[100]; // Track defeated enemies
} MapState;

typedef struct {
    MapState* mapStates;
    int stateCount;
} WorldState;
```

### 3. Special Rooms

```c
void CreateTreasureRoom(Map* map, Room room) {
    CreateRoom(map, room);
    
    // Add treasure in center
    int centerX = room.x + room.width / 2;
    int centerY = room.y + room.height / 2;
    
    SetTile(map, centerX, centerY, 'C');     // Chest
    SetTile(map, centerX-1, centerY, '$');   // Gold
    SetTile(map, centerX+1, centerY, '$');   // Gold
    SetTile(map, centerX, centerY-1, '!');   // Potion
}

void CreateBossRoom(Map* map, Room room) {
    CreateRoom(map, room);
    
    // Add boss
    int centerX = room.x + room.width / 2;
    int centerY = room.y + room.height / 2;
    SetTile(map, centerX, centerY, 'B');     // Boss
    
    // Add pillars
    SetTile(map, room.x + 2, room.y + 2, '#');
    SetTile(map, room.x + room.width - 3, room.y + 2, '#');
    SetTile(map, room.x + 2, room.y + room.height - 3, '#');
    SetTile(map, room.x + room.width - 3, room.y + room.height - 3, '#');
}
```

## "Try This" Challenges

1.  **Better Corridors:** Our current corridor generator is simple but can create strange paths. Can you modify `CreateCorridor` to sometimes dig horizontally first, and sometimes vertically first? (Hint: `if (rand() % 2 == 0)`). This will make your dungeons look more varied.

2.  **Camera Zoom:** Add a feature where the player can zoom the camera in and out using the mouse wheel.
    *   *Hint:* Raylib has a function `GetMouseWheelMove()`. You can add its result to a `zoom` variable in your `Camera` struct. Then, use `camera.zoom` in your `BeginMode2D()` call. Make sure to clamp the zoom level so it doesn't get too small or too large!

3.  **Themed Levels:** Modify the `GenerateDungeon` function to take a `BiomeType` enum (e.g., `BIOME_CAVE`, `BIOME_FOREST`). Based on the biome, change the characters used for walls and floors (e.g., a cave might use `*` for walls, while a forest uses `T` for trees). This is the first step toward creating visually distinct areas in your game.

## Map Design Tips

1. **Loops**: Allow multiple paths to destinations
2. **Landmarks**: Create memorable locations
3. **Secrets**: Hide optional areas with rewards
4. **Progression**: Gate areas by keys or abilities
5. **Variety**: Mix room sizes and shapes

## Summary

You've learned:
- Dynamic map creation and management
- Procedural dungeon generation
- Camera systems for large worlds
- Multiple level handling
- Map saving and loading
- Advanced world features

Your game now has explorable worlds!

## Next Steps

In the next lesson, we'll implement collision detection to make the world feel solid and interactive. We'll handle:
- Wall collisions
- Entity collisions
- Trigger zones
- Physics basics

Ready for solid gameplay? Head to [Lesson 13: Collision Detection](13-collision.md)!