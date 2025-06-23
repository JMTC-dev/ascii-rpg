# Lesson 13: Collision Detection - Making Your World Solid

Welcome back! So far, we've created a world and can move around in it. But have you noticed something? We can only walk into walls because we explicitly check for '#' characters. What about enemies? Items? Other players? In this lesson, we'll build a proper collision system that makes your game world feel real and interactive.

## The "Why": Making the World Feel Solid

A game world without collision detection is like a ghost world—you can walk through walls, enemies, and everything else. It doesn't feel real. Collision detection is the system that enforces the rules of the physical world, making it feel solid and interactive. It answers questions like:
- Can I walk here?
- Did my sword hit that goblin?
- Am I standing on a trap?
- Did I just pick up that potion?

## Understanding Collision Types

In our ASCII RPG, we need to handle several types of collisions:

1. **Solid Collisions**: Can't move through (walls, closed doors)
2. **Trigger Collisions**: Something happens when touched (items, switches)
3. **Entity Collisions**: Interactions with moving objects (enemies, NPCs)
4. **Layered Collisions**: Some things block some entities but not others

Let's start by understanding WHY we need each type:

### Solid Collisions
These create the physical boundaries of your world. Without them, level design is meaningless - players could skip straight to the end!

### Trigger Collisions
These create interactivity. When you step on a trap, pick up gold, or activate a switch, you're using trigger collisions.

### Entity Collisions
These handle interactions between moving objects. Can two enemies occupy the same space? What happens when the player bumps into an NPC?

## Building Our Collision System

Let's start with the foundation. We need to include several headers, and I'll explain why each is necessary:

```c
#include "raylib.h"      // For graphics and input (our game framework)
#include <stdbool.h>     // For boolean type (true/false) - clearer than using integers
#include <stdlib.h>      // For memory allocation (malloc/free) - we'll create dynamic lists
#include <string.h>      // For string operations - useful for comparing tile types

// Why do we define collision types as an enum?
// Enums give us named constants that are easier to understand than magic numbers
// They also help prevent errors - the compiler will warn if we misuse them
typedef enum {
    COLLISION_NONE = 0,      // Can walk through
    COLLISION_SOLID = 1,     // Blocks movement
    COLLISION_TRIGGER = 2,   // Triggers an event
    COLLISION_DAMAGE = 3,    // Causes damage
    COLLISION_SLOW = 4       // Slows movement
} CollisionType;

// This structure represents a collision rule
// Why use a structure? It groups related data together logically
typedef struct {
    char tile;               // Which tile this rule applies to
    CollisionType type;      // What kind of collision
    int damage;             // Damage if applicable (0 for none)
    float speedModifier;    // Speed change (1.0 = normal)
} CollisionRule;

// Why create a collision map separate from the visual map?
// This separation of concerns makes our code more flexible:
// - We can have multiple tiles look different but act the same
// - We can change collision behavior without changing graphics
// - We can have invisible collision zones
```

## The Collision Map Concept

Here's a key insight: your visual map (what players see) and your collision map (what blocks movement) don't have to be the same! This separation is powerful:

```c
// Let's create a system that separates visual from physical
typedef struct {
    char* visual;        // What the player sees
    char* collision;     // What actually blocks/triggers
    int width;
    int height;
} LayeredMap;

// Why use layers? Let me show you with an example:
// Visual:     ####....####
//            #..........#
//            #....@.....#
//            #..........#
//            ############
//
// Collision:  ####....####
//            #..........#
//            #..........#
//            #....XX....#  <- Invisible trap!
//            ############

// This function explains WHY we check collisions
bool CanMoveTo(LayeredMap* map, int x, int y, CollisionRule* rules, int ruleCount) {
    // First, check if we're within bounds
    // Why? Accessing memory outside our array causes crashes!
    if (x < 0 || x >= map->width || y < 0 || y >= map->height) {
        return false;  // Can't move outside the world
    }
    
    // Get the collision tile at this position
    char collisionTile = map->collision[y * map->width + x];
    
    // Now check our rules
    // Why loop through rules instead of hard-coding checks?
    // This makes our system extensible - we can add new tile types without changing this function
    for (int i = 0; i < ruleCount; i++) {
        if (rules[i].tile == collisionTile && rules[i].type == COLLISION_SOLID) {
            return false;  // This tile blocks movement
        }
    }
    
    return true;  // No blocking collision found
}
```

## Entity-to-Entity Collisions

Now let's handle collisions between moving objects. This is trickier because both objects might be trying to move!

```c
// First, we need a way to track all entities in our game
// Why use a linked list? Because the number of entities changes during gameplay
typedef struct Entity {
    int x, y;               // Position
    char symbol;            // Visual representation
    bool isSolid;           // Does it block other entities?
    bool isAlive;           // Is it active?
    int id;                 // Unique identifier
    struct Entity* next;    // Pointer to next entity in list
} Entity;

// Why give each entity an ID?
// IDs help us track specific entities across frames and prevent
// an entity from colliding with itself

// This function checks if a position is occupied by a solid entity
Entity* GetEntityAt(Entity* entityList, int x, int y, int excludeId) {
    Entity* current = entityList;
    
    // Walk through our linked list
    // Why use a linked list instead of an array?
    // - Easy to add/remove entities during gameplay
    // - No wasted memory on "empty" slots
    // - No arbitrary limit on entity count
    while (current != NULL) {
        // Check if this entity is at the position
        // Why exclude by ID? So entities don't collide with themselves!
        if (current->isAlive && 
            current->x == x && 
            current->y == y && 
            current->id != excludeId &&
            current->isSolid) {
            return current;  // Found a blocking entity
        }
        current = current->next;
    }
    
    return NULL;  // No entity at this position
}
```

## Implementing Different Collision Responses

Not all collisions should just stop movement. Let's create a flexible system:

```c
// This structure defines what happens during a collision
// Why use function pointers? They let us define different behaviors without
// massive if-else chains
typedef struct {
    void (*onCollide)(Entity* mover, Entity* target);  // Function to call
    const char* description;                           // What happens
} CollisionResponse;

// Example collision responses
// Notice how each function has the same signature - this is important!

void PushEntity(Entity* mover, Entity* target) {
    // Calculate push direction
    // Why calculate direction? So we push away from the mover
    int dx = target->x - mover->x;
    int dy = target->y - mover->y;
    
    // Simple push - move target one space away
    // In a real game, you'd check if the push destination is valid!
    target->x += dx;
    target->y += dy;
}

void DamageEntity(Entity* mover, Entity* target) {
    // This would reduce target's health
    // We'll implement this fully in the combat lesson
}

void SwapPositions(Entity* mover, Entity* target) {
    // Entities trade places
    // Why might we want this? Think puzzle games!
    int tempX = mover->x;
    int tempY = mover->y;
    mover->x = target->x;
    mover->y = target->y;
    target->x = tempX;
    target->y = tempY;
}
```

## Spatial Partitioning for Performance

As your game grows, checking every entity against every other entity becomes slow. Let's learn a technique called spatial partitioning:

```c
// Why do we need spatial partitioning?
// Imagine 100 entities. Checking each against all others = 10,000 checks!
// With spatial partitioning, we only check entities that are nearby.

#define GRID_SIZE 10  // Each cell is 10x10 tiles

typedef struct EntityNode {
    Entity* entity;
    struct EntityNode* next;
} EntityNode;

typedef struct {
    EntityNode*** grid;  // 2D array of entity lists
    int gridWidth;
    int gridHeight;
} SpatialGrid;

// Initialize our spatial grid
// Why use a grid? It's simple and works well for 2D games
SpatialGrid* CreateSpatialGrid(int worldWidth, int worldHeight) {
    SpatialGrid* sg = (SpatialGrid*)malloc(sizeof(SpatialGrid));
    
    // Calculate grid dimensions
    // Why divide by GRID_SIZE? We're grouping nearby entities
    sg->gridWidth = (worldWidth + GRID_SIZE - 1) / GRID_SIZE;
    sg->gridHeight = (worldHeight + GRID_SIZE - 1) / GRID_SIZE;
    
    // Allocate the grid
    // Why a 3D structure? It's a 2D array where each cell contains a list
    sg->grid = (EntityNode***)malloc(sg->gridHeight * sizeof(EntityNode**));
    for (int y = 0; y < sg->gridHeight; y++) {
        sg->grid[y] = (EntityNode**)malloc(sg->gridWidth * sizeof(EntityNode*));
        for (int x = 0; x < sg->gridWidth; x++) {
            sg->grid[y][x] = NULL;  // Empty list
        }
    }
    
    return sg;
}

// Get which grid cell an entity belongs to
void GetGridCell(int entityX, int entityY, int* gridX, int* gridY) {
    *gridX = entityX / GRID_SIZE;
    *gridY = entityY / GRID_SIZE;
}

// Now we can check collisions efficiently!
bool CheckNearbyCollisions(SpatialGrid* sg, Entity* entity, int newX, int newY) {
    int gridX, gridY;
    GetGridCell(newX, newY, &gridX, &gridY);
    
    // Only check the 9 cells around our position
    // Why 9? The cell we're in plus the 8 surrounding cells
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int checkX = gridX + dx;
            int checkY = gridY + dy;
            
            // Skip if outside grid
            if (checkX < 0 || checkX >= sg->gridWidth ||
                checkY < 0 || checkY >= sg->gridHeight) {
                continue;
            }
            
            // Check entities in this cell
            EntityNode* node = sg->grid[checkY][checkX];
            while (node != NULL) {
                if (node->entity->x == newX && 
                    node->entity->y == newY &&
                    node->entity->id != entity->id) {
                    return true;  // Collision found!
                }
                node = node->next;
            }
        }
    }
    
    return false;  // No collision
}
```

## Collision Detection Shapes

While our game uses grid-based movement, understanding different collision shapes helps you think about game design:

```c
// Different collision shapes serve different purposes:

// 1. Point collision - simplest, used for projectiles
bool PointCollision(int x1, int y1, int x2, int y2) {
    return x1 == x2 && y1 == y2;
}

// 2. Rectangle collision - good for different sized entities
typedef struct {
    int x, y;
    int width, height;
} Rectangle;

bool RectangleCollision(Rectangle* r1, Rectangle* r2) {
    // Why this formula? We're checking if the rectangles DON'T overlap,
    // then inverting the result
    return !(r1->x + r1->width <= r2->x ||   // r1 is left of r2
             r2->x + r2->width <= r1->x ||   // r2 is left of r1
             r1->y + r1->height <= r2->y ||  // r1 is above r2
             r2->y + r2->height <= r1->y);   // r2 is above r1
}

// 3. Circle collision - good for area effects
bool CircleCollision(int x1, int y1, int radius1, int x2, int y2, int radius2) {
    // Calculate distance between centers
    int dx = x2 - x1;
    int dy = y2 - y1;
    int distanceSquared = dx * dx + dy * dy;
    
    // Why compare squared distances? It's faster than calculating square root!
    int radiusSum = radius1 + radius2;
    return distanceSquared <= radiusSum * radiusSum;
}
```

## Complete Collision System Example

Let's put it all together with a working example that demonstrates all our concepts:

```c
#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// All our previous structures and functions...

// Define our collision rules
CollisionRule defaultRules[] = {
    {'#', COLLISION_SOLID, 0, 1.0f},      // Walls block movement
    {'~', COLLISION_SLOW, 0, 0.5f},       // Water slows movement
    {'^', COLLISION_DAMAGE, 10, 1.0f},    // Spikes cause damage
    {'+', COLLISION_SOLID, 0, 1.0f},      // Closed doors block
    {'=', COLLISION_TRIGGER, 0, 1.0f},    // Pressure plates trigger
};

// Complete collision check with all systems
typedef struct {
    bool canMove;
    CollisionType type;
    Entity* entityHit;
    char tileHit;
} CollisionResult;

CollisionResult CheckAllCollisions(LayeredMap* map, Entity* entityList, 
                                 Entity* mover, int newX, int newY) {
    CollisionResult result = {true, COLLISION_NONE, NULL, '.'};
    
    // Step 1: Check map boundaries
    // Why check this first? It's the fastest check and prevents crashes
    if (newX < 0 || newX >= map->width || newY < 0 || newY >= map->height) {
        result.canMove = false;
        result.type = COLLISION_SOLID;
        return result;
    }
    
    // Step 2: Check tile collisions
    char tile = map->collision[newY * map->width + newX];
    for (int i = 0; i < 5; i++) {  // 5 is our rule count
        if (defaultRules[i].tile == tile) {
            result.tileHit = tile;
            result.type = defaultRules[i].type;
            if (defaultRules[i].type == COLLISION_SOLID) {
                result.canMove = false;
                return result;
            }
        }
    }
    
    // Step 3: Check entity collisions
    Entity* hit = GetEntityAt(entityList, newX, newY, mover->id);
    if (hit != NULL && hit->isSolid) {
        result.canMove = false;
        result.type = COLLISION_SOLID;
        result.entityHit = hit;
    }
    
    return result;
}

// Main game demonstrating our collision system
int main() {
    const int screenWidth = 1024;
    const int screenHeight = 768;
    
    InitWindow(screenWidth, screenHeight, "Collision Detection System");
    SetTargetFPS(60);
    
    // Create our layered map
    LayeredMap map;
    map.width = 20;
    map.height = 15;
    
    // Visual map - what the player sees
    map.visual = (char*)malloc(map.width * map.height);
    strcpy(map.visual,
        "####################"
        "#..................#"
        "#..................#"
        "#...~~~~...........#"
        "#...~~~~...........#"
        "#..................#"
        "#......===.........#"  // Pressure plates
        "#..................#"
        "#...^^^^...........#"  // Spikes
        "#..................#"
        "#..................#"
        "#..................#"
        "#..................#"
        "#..................#"
        "####################"
    );
    
    // Collision map - usually same as visual, but can differ!
    map.collision = (char*)malloc(map.width * map.height);
    memcpy(map.collision, map.visual, map.width * map.height);
    
    // Add some invisible walls for demonstration
    // Why might we want invisible walls? Tutorial boundaries, puzzle mechanics, etc.
    for (int i = 0; i < 3; i++) {
        map.collision[10 * map.width + 10 + i] = '#';  // Invisible barrier
    }
    
    // Create entities
    Entity* player = (Entity*)malloc(sizeof(Entity));
    player->x = 5;
    player->y = 5;
    player->symbol = '@';
    player->isSolid = true;
    player->isAlive = true;
    player->id = 1;
    player->next = NULL;
    
    // Add an NPC
    Entity* npc = (Entity*)malloc(sizeof(Entity));
    npc->x = 10;
    npc->y = 5;
    npc->symbol = 'N';
    npc->isSolid = true;
    npc->isAlive = true;
    npc->id = 2;
    npc->next = NULL;
    
    player->next = npc;  // Link entities
    
    // Game state
    int cellSize = 30;
    int offsetX = 50;
    int offsetY = 50;
    char messageBuffer[100] = "Use WASD to move";
    float messageTimer = 0;
    
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        
        // Handle input
        int dx = 0, dy = 0;
        if (IsKeyPressed(KEY_W)) dy = -1;
        if (IsKeyPressed(KEY_S)) dy = 1;
        if (IsKeyPressed(KEY_A)) dx = -1;
        if (IsKeyPressed(KEY_D)) dx = 1;
        
        if (dx != 0 || dy != 0) {
            int newX = player->x + dx;
            int newY = player->y + dy;
            
            // Check collisions
            CollisionResult result = CheckAllCollisions(&map, player, player, newX, newY);
            
            if (result.canMove) {
                player->x = newX;
                player->y = newY;
                
                // Handle triggers
                switch (result.type) {
                    case COLLISION_TRIGGER:
                        sprintf(messageBuffer, "You stepped on a pressure plate!");
                        messageTimer = 3.0f;
                        break;
                    case COLLISION_DAMAGE:
                        sprintf(messageBuffer, "Ouch! You took damage from spikes!");
                        messageTimer = 3.0f;
                        break;
                    case COLLISION_SLOW:
                        sprintf(messageBuffer, "The water slows your movement");
                        messageTimer = 2.0f;
                        break;
                }
            } else {
                // Collision feedback
                if (result.entityHit != NULL) {
                    sprintf(messageBuffer, "You bumped into the NPC");
                } else if (result.tileHit == '#') {
                    sprintf(messageBuffer, "Solid wall blocks your path");
                } else {
                    sprintf(messageBuffer, "Something blocks your movement");
                }
                messageTimer = 2.0f;
            }
        }
        
        // Update message timer
        if (messageTimer > 0) {
            messageTimer -= deltaTime;
        }
        
        // Draw
        BeginDrawing();
            ClearBackground(BLACK);
            
            DrawText("Collision Detection Demo", 10, 10, 30, WHITE);
            
            // Draw map
            for (int y = 0; y < map.height; y++) {
                for (int x = 0; x < map.width; x++) {
                    char tile = map.visual[y * map.width + x];
                    Color color = WHITE;
                    
                    // Color based on collision type
                    for (int i = 0; i < 5; i++) {
                        if (defaultRules[i].tile == tile) {
                            switch (defaultRules[i].type) {
                                case COLLISION_SOLID: color = GRAY; break;
                                case COLLISION_DAMAGE: color = RED; break;
                                case COLLISION_SLOW: color = BLUE; break;
                                case COLLISION_TRIGGER: color = YELLOW; break;
                                default: color = DARKGRAY; break;
                            }
                        }
                    }
                    
                    char str[2] = {tile, '\0'};
                    DrawText(str, offsetX + x * cellSize, offsetY + y * cellSize,
                            cellSize, color);
                    
                    // Show invisible walls with a subtle indicator
                    if (map.collision[y * map.width + x] == '#' && 
                        map.visual[y * map.width + x] == '.') {
                        DrawRectangleLines(offsetX + x * cellSize, 
                                         offsetY + y * cellSize,
                                         cellSize, cellSize, 
                                         (Color){255, 255, 255, 50});
                    }
                }
            }
            
            // Draw entities
            Entity* current = player;
            while (current != NULL) {
                if (current->isAlive) {
                    char str[2] = {current->symbol, '\0'};
                    Color color = (current == player) ? WHITE : GREEN;
                    DrawText(str, offsetX + current->x * cellSize,
                            offsetY + current->y * cellSize, cellSize, color);
                }
                current = current->next;
            }
            
            // Draw message
            if (messageTimer > 0) {
                DrawText(messageBuffer, 10, screenHeight - 60, 20, YELLOW);
            }
            
            // Legend
            DrawText("# = Wall, ~ = Water (slow), ^ = Spikes (damage), = = Trigger", 
                    10, screenHeight - 30, 16, GRAY);
            
        EndDrawing();
    }
    
    // Cleanup
    free(map.visual);
    free(map.collision);
    free(player);
    free(npc);
    
    CloseWindow();
    return 0;
}
```

## Why This Architecture?

Let's reflect on why we built our collision system this way:

1. **Separation of Concerns**: Visual and collision maps can differ, giving designers flexibility
2. **Data-Driven Design**: Collision rules are data, not hard-coded logic
3. **Extensibility**: New collision types require no changes to core logic
4. **Performance**: Spatial partitioning scales to large worlds
5. **Clarity**: Each function has one clear purpose

## Common Collision Bugs and Solutions

### Bug 1: Getting Stuck in Corners
**Why it happens**: When moving diagonally (e.g., up and right), you might try to move into a space that is diagonally adjacent but blocked by two walls forming a corner. A simple check of only the destination tile will fail.
**Solution**: A robust collision system checks the horizontal and vertical parts of a diagonal move separately. Try to move horizontally first; if it's a valid move, then try to move vertically. This creates a "sliding" effect along walls.

### Bug 2: Entities Overlapping on Spawn
**Why it happens**: When you create a new enemy or item, you might place it directly on top of another entity.
**Solution**: Always validate a spawn position before creating an entity there. Write a `IsPositionOccupied()` function and use it.

### Bug 3: Tunneling
**Why it happens**: If a fast-moving object (like an arrow) moves more than one tile per frame, it can pass completely through a thin wall without its position ever being *inside* the wall.
**Solution**: This requires more advanced collision detection, often called "sweep tests" or "continuous collision detection," where you check the entire path of movement, not just the start and end points. This is beyond the scope of this lesson but is an important concept in physics-based games.

## Practice Exercises

### "Try This" Challenges

1.  **Bouncy Slimes:** Create a new `CollisionResponse` function called `BounceEntity`. When a player bumps into a "Slime" entity, instead of stopping, the player should be knocked back one tile in the opposite direction. How would you implement this?

2.  **One-Way Doors:** Add a new tile type (e.g., `v` for a downward-facing one-way ledge). Modify your `CanMoveTo` function so that the player can move *onto* this tile, but if they try to move *from* this tile in the opposite direction (up), the move is blocked. This is great for creating paths where the player can't backtrack.

3.  **Area of Effect (AoE):** Use the `CircleCollision` function to create a "bomb" item. When the player uses it, check for all enemies within a certain radius (e.g., 3 tiles) of the bomb and apply damage to them. This shows how different collision shapes can be used for different game mechanics.

## Summary

You've learned:
- Why collision detection is fundamental to game feel
- How to separate visual and physical representations
- Different collision types and when to use them
- Performance optimization through spatial partitioning
- How to create flexible, extensible collision systems

Most importantly, you understand WHY we make these design decisions, not just how to implement them.

## Next Steps

In the next lesson, we'll bring your world to life with enemies and AI. We'll explore:
- Why AI makes games engaging
- Different AI techniques for different enemy types
- State machines for behavior
- Pathfinding basics

Your static world is about to become dangerous! Head to [Lesson 14: Enemies and AI](14-enemies-ai.md)!