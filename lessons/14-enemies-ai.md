# Lesson 14: Enemies and AI - Bringing Your World to Life

Welcome to one of the most exciting parts of game development! Until now, your world has been static - walls don't move, items just sit there. But games become truly engaging when they push back, when they challenge us. That's where AI comes in.

## What is Game AI, Really?

When we say "AI" in games, we don't mean artificial consciousness or complex machine learning. Game AI is about creating the **illusion of intelligence**. It's a collection of rules and logic that makes an entity *appear* to be thinking and making decisions. Our goal is to make enemies behave in ways that are believable, challenging, and fun for the player.

Think about the ghosts in Pac-Man:
- Blinky (red) chases you directly
- Pinky (pink) tries to cut you off
- Inky (blue) is unpredictable
- Clyde (orange) sometimes chases, sometimes flees

Each ghost has just a few lines of code for behavior, but together they create complex, emergent gameplay. That's the magic we're after!

## Why Different AI Types Matter

Before we code, let's understand why games need various AI behaviors:

1. **Variety prevents predictability** - If all enemies act the same, players quickly get bored
2. **Different behaviors create puzzles** - Some enemies you fight, others you avoid
3. **AI teaches game mechanics** - Simple enemies introduce concepts, complex ones test mastery
4. **Personality through behavior** - A cowardly goblin feels different from a brave knight

## Building Our AI Foundation

Let's start with the includes and explain why each is necessary:

```c
#include "raylib.h"      // Graphics framework
#include <stdbool.h>     // For boolean values (cleaner than 0/1)
#include <stdlib.h>      // For rand() and memory allocation
#include <math.h>        // For distance calculations and pathfinding
#include <limits.h>      // For INT_MAX (used in pathfinding)

// Why use enums for AI states?
// Enums make our code self-documenting. "ENEMY_CHASING" is clearer than "state 3"
// They also prevent bugs - the compiler catches typos like "ENEMY_CHASNG"
typedef enum {
    AI_STATE_IDLE,       // Not doing anything
    AI_STATE_PATROL,     // Following a set path
    AI_STATE_CHASE,      // Pursuing the player
    AI_STATE_FLEE,       // Running away
    AI_STATE_ATTACK,     // In combat
    AI_STATE_SEARCH,     // Lost sight of player
    AI_STATE_DEAD        // No longer active
} AIState;

// Why separate AI type from state?
// An enemy's type (what it is) doesn't change, but its state (what it's doing) does
typedef enum {
    AI_TYPE_ZOMBIE,      // Slow, relentless
    AI_TYPE_GOBLIN,      // Fast, cowardly
    AI_TYPE_GUARD,       // Patrols, investigates
    AI_TYPE_ARCHER,      // Keeps distance
    AI_TYPE_BOSS         // Complex patterns
} AIType;

// Our enhanced enemy structure
// Notice how we're building on previous lessons!
typedef struct Enemy {
    // Basic properties (from previous lessons)
    int x, y;
    char symbol;
    Color color;
    int health;
    int maxHealth;
    
    // AI properties (new!)
    AIType type;
    AIState state;
    
    // Perception - how the enemy sees the world
    int sightRange;          // How far can it see?
    int hearingRange;        // How far can it hear?
    bool canSeePlayer;       // Currently sees player?
    int lastSeenPlayerX;     // Where was player last seen?
    int lastSeenPlayerY;
    
    // Movement
    float moveSpeed;         // Tiles per second
    float timeSinceMove;     // Movement timer
    int patrolIndex;         // Current patrol point
    
    // Combat
    int attackDamage;
    float attackCooldown;
    float timeSinceAttack;
    
    // Unique behaviors
    float courage;           // 0.0 = coward, 1.0 = fearless
    float aggression;        // How likely to attack vs flee
    
    struct Enemy* next;      // For linked list
} Enemy;

/* ===== CONCEPT DEEP DIVE: Linked Lists =====

Why `struct Enemy* next`? This is the core of a "linked list".

Imagine you have a chain of paper notes. Each note has some data written on it,
and at the bottom, it has an arrow pointing to the *next* note in the chain.
This is a linked list!

- The `Enemy` struct is a "note".
- The `next` pointer is the "arrow".

Why use this instead of a simple array `Enemy enemies[100]`?
- **Dynamic Size**: An array has a fixed size. What if you need 101 enemies? A linked list can grow and shrink as needed.
- **Easy Removal**: To remove an enemy from an array, you have to shift all the other enemies to fill the gap. With a linked list, you just change one "arrow" to point around the enemy you want to remove.

It's a fundamental data structure for managing collections of game objects where the count changes frequently.
========================================= */
```

## The Perception System - How Enemies See

Before an enemy can react, it needs to perceive the world. This is fundamental - without perception, AI is just random movement!

```c
// Calculate distance between two points
// Why Manhattan distance instead of Euclidean?
// In grid-based games, you can't move diagonally through walls,
// so straight-line distance can be misleading
int ManhattanDistance(int x1, int y1, int x2, int y2) {
    return abs(x2 - x1) + abs(y2 - y1);
}

// Check if enemy can see a position
// This is more complex than just distance - we need line of sight!
bool CanSeePosition(Enemy* enemy, int targetX, int targetY, char* map, int mapWidth) {
    int distance = ManhattanDistance(enemy->x, enemy->y, targetX, targetY);
    
    // First check: is it within sight range?
    if (distance > enemy->sightRange) {
        return false;
    }
    
    // Second check: is there a clear line of sight?
    // We'll use Bresenham's line algorithm (simple version)
    int x = enemy->x;
    int y = enemy->y;
    int dx = abs(targetX - x);
    int dy = abs(targetY - y);
    int sx = (x < targetX) ? 1 : -1;
    int sy = (y < targetY) ? 1 : -1;
    int err = dx - dy;
    
    while (x != targetX || y != targetY) {
        // Check if current position blocks sight
        if (map[y * mapWidth + x] == '#') {
            return false;  // Wall blocks vision
        }
        
        // Move along the line
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
    
    return true;  // Clear line of sight!
}

// Update what the enemy knows about the world
void UpdateEnemyPerception(Enemy* enemy, Player* player, char* map, int mapWidth) {
    bool couldSeePlayer = enemy->canSeePlayer;
    
    // Check if enemy can see player
    enemy->canSeePlayer = CanSeePosition(enemy, player->x, player->y, map, mapWidth);
    
    if (enemy->canSeePlayer) {
        // Update last known position
        enemy->lastSeenPlayerX = player->x;
        enemy->lastSeenPlayerY = player->y;
        
        // Alert! Transition from not seeing to seeing
        if (!couldSeePlayer) {
            // This is where you'd play an alert sound
            // or show an exclamation mark above the enemy
        }
    }
}
```

## State Machines - The Brain of AI

A state machine is the "brain" of our AI. It's a simple but powerful concept for organizing behavior. An entity can only be in **one state at a time** (e.g., `IDLE`, `CHASING`, `FLEEING`), and it transitions to other states based on events (like seeing the player).

This is a fantastic way to manage complexity. Instead of one giant `if/else` block, we can write a separate function for each state, making our code much cleaner and easier to debug.

```c
// Why use state machines?
// They make complex behavior manageable. Instead of one huge function
// with nested if statements, each state is self-contained.

void UpdateEnemyAI(Enemy* enemy, Player* player, char* map, int mapWidth, float deltaTime) {
    // First, update perception
    UpdateEnemyPerception(enemy, player, map, mapWidth);
    
    // Then, update based on current state
    switch (enemy->state) {
        case AI_STATE_IDLE:
            UpdateIdleState(enemy, player, deltaTime);
            break;
            
        case AI_STATE_PATROL:
            UpdatePatrolState(enemy, player, map, mapWidth, deltaTime);
            break;
            
        case AI_STATE_CHASE:
            UpdateChaseState(enemy, player, map, mapWidth, deltaTime);
            break;
            
        case AI_STATE_FLEE:
            UpdateFleeState(enemy, player, map, mapWidth, deltaTime);
            break;
            
        case AI_STATE_ATTACK:
            UpdateAttackState(enemy, player, deltaTime);
            break;
            
        case AI_STATE_SEARCH:
            UpdateSearchState(enemy, player, deltaTime);
            break;
    }
}

// Example: Idle State
// Notice how each state function is focused on one behavior
void UpdateIdleState(Enemy* enemy, Player* player, float deltaTime) {
    // In idle state, enemy just looks around
    
    // Can we see the player?
    if (enemy->canSeePlayer) {
        // Transition based on enemy type and personality
        float distanceToPlayer = ManhattanDistance(enemy->x, enemy->y, player->x, player->y);
        
        // Cowardly enemies flee if player is too close
        if (enemy->courage < 0.3f && distanceToPlayer < 5) {
            enemy->state = AI_STATE_FLEE;
            return;
        }
        
        // Aggressive enemies attack
        if (enemy->aggression > 0.7f) {
            enemy->state = AI_STATE_CHASE;
            return;
        }
        
        // Guards might just watch
        if (enemy->type == AI_TYPE_GUARD) {
            // Stay in idle but face the player
            // We'll implement facing directions later
        }
    }
    
    // Random chance to start patrolling
    if (rand() % 1000 < 5) {  // 0.5% chance per frame
        enemy->state = AI_STATE_PATROL;
    }
}
```

## Movement AI - Making Enemies Navigate

Now let's make enemies actually move around the world intelligently:

```c
// Simple pathfinding - move towards target
// Why start simple? Most enemies don't need perfect pathfinding!
void MoveTowardsTarget(Enemy* enemy, int targetX, int targetY, char* map, int mapWidth) {
    int dx = targetX - enemy->x;
    int dy = targetY - enemy->y;
    
    // Try to move in the direction with greater distance first
    // This creates more natural movement than always trying X then Y
    if (abs(dx) > abs(dy)) {
        // Try horizontal first
        int moveX = (dx > 0) ? 1 : -1;
        if (CanMoveTo(map, mapWidth, enemy->x + moveX, enemy->y)) {
            enemy->x += moveX;
            return;
        }
        // If blocked, try vertical
        int moveY = (dy > 0) ? 1 : -1;
        if (CanMoveTo(map, mapWidth, enemy->x, enemy->y + moveY)) {
            enemy->y += moveY;
            return;
        }
    } else {
        // Try vertical first
        int moveY = (dy > 0) ? 1 : -1;
        if (CanMoveTo(map, mapWidth, enemy->x, enemy->y + moveY)) {
            enemy->y += moveY;
            return;
        }
        // If blocked, try horizontal
        int moveX = (dx > 0) ? 1 : -1;
        if (CanMoveTo(map, mapWidth, enemy->x + moveX, enemy->y)) {
            enemy->x += moveX;
            return;
        }
    }
}

// Chase state - actively pursue the player
void UpdateChaseState(Enemy* enemy, Player* player, char* map, int mapWidth, float deltaTime) {
    // Update movement timer
    enemy->timeSinceMove += deltaTime;
    
    // Can we still see the player?
    if (!enemy->canSeePlayer) {
        // Lost sight! Go to last known position
        enemy->state = AI_STATE_SEARCH;
        return;
    }
    
    // Are we close enough to attack?
    int distance = ManhattanDistance(enemy->x, enemy->y, player->x, player->y);
    if (distance <= 1) {  // Adjacent
        enemy->state = AI_STATE_ATTACK;
        return;
    }
    
    // Move towards player
    if (enemy->timeSinceMove >= 1.0f / enemy->moveSpeed) {
        MoveTowardsTarget(enemy, player->x, player->y, map, mapWidth);
        enemy->timeSinceMove = 0;
    }
}
```

## Smarter Movement - Basic Pathfinding

Our current enemies can get stuck behind walls. Let's make them smarter!

/* ===== CONCEPT DEEP DIVE: Pathfinding ===== 

Pathfinding is how we find a route from point A to point B, avoiding obstacles.
Think of it like GPS navigation - it finds the best route around buildings.

There are complex algorithms like A* (A-star) that find optimal paths, but
they're quite advanced. For now, we'll use a simpler approach that works
well for most games!

Advanced Note: If you're curious about A*, it's a fascinating algorithm that
uses heuristics (educated guesses) to efficiently find the shortest path.
It's used in many professional games, but requires understanding of:
- Priority queues
- Graph traversal
- Heuristic functions

For now, our simpler method will make enemies feel smart enough!
===== END CONCEPT DEEP DIVE ===== */

### Method 1: Wall Hugging (Simple but Effective)

This technique makes enemies follow walls when direct movement is blocked:

```c
// Simple but effective pathfinding
void MoveTowardsTargetSmart(Enemy* enemy, int targetX, int targetY, 
                           char* map, int mapWidth, int mapHeight) {
    int dx = 0;
    int dy = 0;
    
    // Determine direction to target
    if (targetX > enemy->x) dx = 1;
    else if (targetX < enemy->x) dx = -1;
    
    if (targetY > enemy->y) dy = 1;
    else if (targetY < enemy->y) dy = -1;
    
    // Try direct movement first
    if (dx != 0 && CanMoveTo(enemy->x + dx, enemy->y, map, mapWidth, mapHeight)) {
        enemy->x += dx;
        return;
    }
    if (dy != 0 && CanMoveTo(enemy->x, enemy->y + dy, map, mapWidth, mapHeight)) {
        enemy->y += dy;
        return;
    }
    
    // Direct path blocked - try moving around obstacle
    // This creates a "wall hugging" behavior
    if (dx != 0) {
        // Try moving vertically first
        if (CanMoveTo(enemy->x, enemy->y + 1, map, mapWidth, mapHeight)) {
            enemy->y += 1;
        } else if (CanMoveTo(enemy->x, enemy->y - 1, map, mapWidth, mapHeight)) {
            enemy->y -= 1;
        }
    } else if (dy != 0) {
        // Try moving horizontally first
        if (CanMoveTo(enemy->x + 1, enemy->y, map, mapWidth, mapHeight)) {
            enemy->x += 1;
        } else if (CanMoveTo(enemy->x - 1, enemy->y, map, mapWidth, mapHeight)) {
            enemy->x -= 1;
        }
    }
}
```

### Method 2: Breadcrumb System (More Intelligent)

For enemies that need to be smarter, we can use a "breadcrumb" system:

```c
// Breadcrumb pathfinding - simpler than A* but still effective
typedef struct {
    int x, y;
    int distance;
} Breadcrumb;

// Find next step towards target using breadcrumbs
void MoveWithBreadcrumbs(Enemy* enemy, int targetX, int targetY,
                        char* map, int mapWidth, int mapHeight) {
    // Create distance map
    int* distanceMap = calloc(mapWidth * mapHeight, sizeof(int));
    
    // Mark all cells as unvisited (-1)
    for (int i = 0; i < mapWidth * mapHeight; i++) {
        distanceMap[i] = -1;
    }
    
    // Start from target and work backwards
    distanceMap[targetY * mapWidth + targetX] = 0;
    
    // Simple flood-fill to calculate distances
    bool changed = true;
    int currentDistance = 0;
    
    while (changed) {
        changed = false;
        
        for (int y = 0; y < mapHeight; y++) {
            for (int x = 0; x < mapWidth; x++) {
                if (distanceMap[y * mapWidth + x] == currentDistance) {
                    // Check neighbors
                    int dx[] = {0, 0, -1, 1};
                    int dy[] = {-1, 1, 0, 0};
                    
                    for (int i = 0; i < 4; i++) {
                        int nx = x + dx[i];
                        int ny = y + dy[i];
                        
                        if (nx >= 0 && nx < mapWidth && 
                            ny >= 0 && ny < mapHeight &&
                            map[ny * mapWidth + nx] != '#' &&
                            distanceMap[ny * mapWidth + nx] == -1) {
                            
                            distanceMap[ny * mapWidth + nx] = currentDistance + 1;
                            changed = true;
                        }
                    }
                }
            }
        }
        currentDistance++;
    }
    
    // Now find best move from enemy position
    int bestX = enemy->x;
    int bestY = enemy->y;
    int bestDistance = INT_MAX;
    
    // Check all adjacent cells
    int dx[] = {0, 0, -1, 1};
    int dy[] = {-1, 1, 0, 0};
    
    for (int i = 0; i < 4; i++) {
        int nx = enemy->x + dx[i];
        int ny = enemy->y + dy[i];
        
        if (nx >= 0 && nx < mapWidth && 
            ny >= 0 && ny < mapHeight &&
            map[ny * mapWidth + nx] != '#') {
            
            int dist = distanceMap[ny * mapWidth + nx];
            if (dist >= 0 && dist < bestDistance) {
                bestDistance = dist;
                bestX = nx;
                bestY = ny;
            }
        }
    }
    
    // Move to best position
    enemy->x = bestX;
    enemy->y = bestY;
    
    free(distanceMap);
}
```

### When to Use Each Method

1. **Direct Movement**: Fast enemies, open areas
2. **Wall Hugging**: Maze-like levels, medium intelligence
3. **Breadcrumb**: Important enemies, when path quality matters
4. **A* (Advanced)**: Boss enemies, when optimal paths are crucial

For most enemies in our game, wall hugging provides a good balance of 
intelligence and performance!

## Personality-Driven AI

Let's make each enemy feel unique by giving them personality traits:

```c
// Create enemy with personality
Enemy* CreateEnemy(int x, int y, AIType type) {
    Enemy* enemy = (Enemy*)malloc(sizeof(Enemy));
    enemy->x = x;
    enemy->y = y;
    enemy->state = AI_STATE_IDLE;
    enemy->type = type;
    enemy->timeSinceMove = 0;
    enemy->timeSinceAttack = 0;
    enemy->canSeePlayer = false;
    enemy->next = NULL;
    
    // Set type-specific properties
    // Notice how each type has a different "feel"
    switch (type) {
        case AI_TYPE_ZOMBIE:
            enemy->symbol = 'z';
            enemy->color = GREEN;
            enemy->health = 30;
            enemy->maxHealth = 30;
            enemy->sightRange = 5;
            enemy->hearingRange = 3;
            enemy->moveSpeed = 0.5f;     // Slow
            enemy->attackDamage = 10;
            enemy->courage = 1.0f;       // Fearless
            enemy->aggression = 1.0f;    // Always attacks
            break;
            
        case AI_TYPE_GOBLIN:
            enemy->symbol = 'g';
            enemy->color = DARKGREEN;
            enemy->health = 15;
            enemy->maxHealth = 15;
            enemy->sightRange = 7;
            enemy->hearingRange = 5;
            enemy->moveSpeed = 2.0f;     // Fast
            enemy->attackDamage = 5;
            enemy->courage = 0.2f;       // Cowardly
            enemy->aggression = 0.4f;    // Prefers to flee
            break;
            
        case AI_TYPE_GUARD:
            enemy->symbol = 'G';
            enemy->color = BLUE;
            enemy->health = 50;
            enemy->maxHealth = 50;
            enemy->sightRange = 8;
            enemy->hearingRange = 6;
            enemy->moveSpeed = 1.0f;     // Normal
            enemy->attackDamage = 15;
            enemy->courage = 0.8f;       // Brave
            enemy->aggression = 0.6f;    // Defensive
            break;
            
        case AI_TYPE_ARCHER:
            enemy->symbol = 'a';
            enemy->color = BROWN;
            enemy->health = 20;
            enemy->maxHealth = 20;
            enemy->sightRange = 12;      // Excellent vision
            enemy->hearingRange = 4;
            enemy->moveSpeed = 1.5f;
            enemy->attackDamage = 8;
            enemy->courage = 0.5f;       // Cautious
            enemy->aggression = 0.7f;    // Aggressive at range
            break;
    }
    
    // Add some randomness to personality
    // Why? It makes each enemy slightly unique
    enemy->courage += (rand() % 20 - 10) / 100.0f;      // ±0.1
    enemy->aggression += (rand() % 20 - 10) / 100.0f;   // ±0.1
    
    // Clamp values
    if (enemy->courage < 0) enemy->courage = 0;
    if (enemy->courage > 1) enemy->courage = 1;
    if (enemy->aggression < 0) enemy->aggression = 0;
    if (enemy->aggression > 1) enemy->aggression = 1;
    
    return enemy;
}
```

## Group AI - Coordinated Behaviors

Real intelligence emerges when enemies work together:

```c
// Enemy communication system
typedef struct {
    int alertX, alertY;      // Position of alert
    float alertTime;         // When alert was raised
    int alerterId;           // Which enemy raised it
} AlertSystem;

// When an enemy spots the player, it alerts nearby allies
void RaiseAlert(Enemy* alerter, Enemy* allEnemies, AlertSystem* alerts) {
    // Create alert at enemy's position
    alerts->alertX = alerter->x;
    alerts->alertY = alerter->y;
    alerts->alertTime = 0;  // Just happened
    alerts->alerterId = alerter->id;
    
    // Alert nearby enemies
    Enemy* current = allEnemies;
    while (current != NULL) {
        if (current != alerter && current->state == AI_STATE_IDLE) {
            int distance = ManhattanDistance(current->x, current->y, alerter->x, alerter->y);
            
            // Within hearing range?
            if (distance <= current->hearingRange) {
                // Guards investigate alerts
                if (current->type == AI_TYPE_GUARD) {
                    current->state = AI_STATE_SEARCH;
                    current->lastSeenPlayerX = alerts->alertX;
                    current->lastSeenPlayerY = alerts->alertY;
                }
                // Others might flee or prepare
                else if (current->courage < 0.5f) {
                    current->state = AI_STATE_FLEE;
                }
            }
        }
        current = current->next;
    }
}

// Flanking behavior - surround the player
void UpdateFlankingBehavior(Enemy* enemy, Enemy* allEnemies, Player* player) {
    // Count allies already near player
    int alliesNorth = 0, alliesSouth = 0, alliesEast = 0, alliesWest = 0;
    
    Enemy* current = allEnemies;
    while (current != NULL) {
        if (current != enemy && current->state == AI_STATE_CHASE) {
            int dx = current->x - player->x;
            int dy = current->y - player->y;
            
            if (abs(dx) > abs(dy)) {
                if (dx > 0) alliesEast++;
                else alliesWest++;
            } else {
                if (dy > 0) alliesSouth++;
                else alliesNorth++;
            }
        }
        current = current->next;
    }
    
    // Choose least crowded direction
    // This naturally creates flanking behavior!
    int targetX = player->x;
    int targetY = player->y;
    
    int minAllies = alliesNorth;
    targetY = player->y - 2;  // Approach from north
    
    if (alliesSouth < minAllies) {
        minAllies = alliesSouth;
        targetY = player->y + 2;  // Approach from south
    }
    if (alliesEast < minAllies) {
        minAllies = alliesEast;
        targetX = player->x + 2;  // Approach from east
        targetY = player->y;
    }
    if (alliesWest < minAllies) {
        targetX = player->x - 2;  // Approach from west
        targetY = player->y;
    }
    
    // Now move towards flanking position
    MoveTowardsTarget(enemy, targetX, targetY, map, mapWidth);
}
```

## Complete AI Example

Let's put it all together in a working demonstration:

```c
#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

// Include all our previous structures and functions...

int main() {
    const int screenWidth = 1024;
    const int screenHeight = 768;
    
    InitWindow(screenWidth, screenHeight, "AI System Demo");
    SetTargetFPS(60);
    srand(time(NULL));
    
    // Create map
    const int mapWidth = 30;
    const int mapHeight = 20;
    char map[600];
    
    // Simple room layout
    memset(map, '#', mapWidth * mapHeight);
    // Carve out rooms
    for (int y = 1; y < mapHeight - 1; y++) {
        for (int x = 1; x < mapWidth - 1; x++) {
            if ((x < 10 || x > 20) || (y < 7 || y > 13)) {
                map[y * mapWidth + x] = '.';
            }
        }
    }
    
    // Create corridors
    for (int x = 10; x <= 20; x++) {
        map[10 * mapWidth + x] = '.';
    }
    
    // Create player
    Player player = {5, 10, '@', WHITE, 100, 100};
    
    // Create enemies with different types
    Enemy* enemies = CreateEnemy(25, 5, AI_TYPE_ZOMBIE);
    enemies->next = CreateEnemy(25, 15, AI_TYPE_GOBLIN);
    enemies->next->next = CreateEnemy(15, 10, AI_TYPE_GUARD);
    enemies->next->next->next = CreateEnemy(5, 18, AI_TYPE_ARCHER);
    
    // Alert system
    AlertSystem alerts = {0, 0, -1.0f, -1};
    
    // UI
    int cellSize = 25;
    int offsetX = 50;
    int offsetY = 100;
    char infoText[200];
    
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        
        // Player input
        int dx = 0, dy = 0;
        if (IsKeyPressed(KEY_W)) dy = -1;
        if (IsKeyPressed(KEY_S)) dy = 1;
        if (IsKeyPressed(KEY_A)) dx = -1;
        if (IsKeyPressed(KEY_D)) dx = 1;
        
        if (dx != 0 || dy != 0) {
            int newX = player.x + dx;
            int newY = player.y + dy;
            
            if (map[newY * mapWidth + newX] != '#') {
                // Check enemy collision
                bool blocked = false;
                Enemy* current = enemies;
                while (current != NULL) {
                    if (current->x == newX && current->y == newY) {
                        blocked = true;
                        break;
                    }
                    current = current->next;
                }
                
                if (!blocked) {
                    player.x = newX;
                    player.y = newY;
                }
            }
        }
        
        // Update all enemies
        Enemy* current = enemies;
        bool anyoneSeesPlayer = false;
        while (current != NULL) {
            if (current->state != AI_STATE_DEAD) {
                UpdateEnemyAI(current, &player, map, mapWidth, deltaTime);
                
                // Check if anyone can see player
                if (current->canSeePlayer) {
                    anyoneSeesPlayer = true;
                    
                    // Raise alert if just spotted
                    if (alerts.alertTime < 0) {
                        RaiseAlert(current, enemies, &alerts);
                    }
                }
            }
            current = current->next;
        }
        
        // Update alert timer
        if (alerts.alertTime >= 0) {
            alerts.alertTime += deltaTime;
            if (alerts.alertTime > 5.0f) {  // Alert lasts 5 seconds
                alerts.alertTime = -1.0f;
            }
        }
        
        // Draw
        BeginDrawing();
            ClearBackground(BLACK);
            
            DrawText("Enemy AI Demonstration", 10, 10, 30, WHITE);
            DrawText("WASD to move - Watch how different enemies react!", 10, 45, 20, GRAY);
            
            // Draw map
            for (int y = 0; y < mapHeight; y++) {
                for (int x = 0; x < mapWidth; x++) {
                    char tile = map[y * mapWidth + x];
                    Color color = (tile == '#') ? GRAY : DARKGRAY;
                    
                    // Show alert area
                    if (alerts.alertTime >= 0 && alerts.alertTime < 1.0f) {
                        int dist = ManhattanDistance(x, y, alerts.alertX, alerts.alertY);
                        if (dist <= 8) {
                            color = (Color){100, 50, 50, 255};  // Red tint
                        }
                    }
                    
                    char str[2] = {tile, '\0'};
                    DrawText(str, offsetX + x * cellSize, offsetY + y * cellSize,
                            cellSize, color);
                }
            }
            
            // Draw enemies with state indicators
            current = enemies;
            while (current != NULL) {
                if (current->state != AI_STATE_DEAD) {
                    char str[2] = {current->symbol, '\0'};
                    DrawText(str, offsetX + current->x * cellSize,
                            offsetY + current->y * cellSize, cellSize, current->color);
                    
                    // Draw state indicator
                    const char* stateSymbol = "";
                    Color stateColor = WHITE;
                    switch (current->state) {
                        case AI_STATE_IDLE: stateSymbol = "?"; stateColor = GRAY; break;
                        case AI_STATE_PATROL: stateSymbol = "..."; stateColor = BLUE; break;
                        case AI_STATE_CHASE: stateSymbol = "!"; stateColor = RED; break;
                        case AI_STATE_FLEE: stateSymbol = "~"; stateColor = YELLOW; break;
                        case AI_STATE_SEARCH: stateSymbol = "?"; stateColor = ORANGE; break;
                        case AI_STATE_ATTACK: stateSymbol = "*"; stateColor = RED; break;
                    }
                    DrawText(stateSymbol, 
                            offsetX + current->x * cellSize + cellSize,
                            offsetY + current->y * cellSize - 10,
                            16, stateColor);
                    
                    // Draw vision range when chasing
                    if (current->canSeePlayer) {
                        DrawCircleLines(offsetX + current->x * cellSize + cellSize/2,
                                      offsetY + current->y * cellSize + cellSize/2,
                                      current->sightRange * cellSize, 
                                      (Color){255, 0, 0, 50});
                    }
                }
                current = current->next;
            }
            
            // Draw player
            DrawText("@", offsetX + player.x * cellSize,
                    offsetY + player.y * cellSize, cellSize, WHITE);
            
            // Enemy type legend
            DrawText("Enemy Types:", screenWidth - 200, 100, 16, WHITE);
            DrawText("z - Zombie (slow, fearless)", screenWidth - 200, 120, 14, GREEN);
            DrawText("g - Goblin (fast, cowardly)", screenWidth - 200, 140, 14, DARKGREEN);
            DrawText("G - Guard (patrol, investigate)", screenWidth - 200, 160, 14, BLUE);
            DrawText("a - Archer (long range)", screenWidth - 200, 180, 14, BROWN);
            
            // State legend
            DrawText("States:", screenWidth - 200, 220, 16, WHITE);
            DrawText("? - Idle/Searching", screenWidth - 200, 240, 14, GRAY);
            DrawText("! - Chasing", screenWidth - 200, 260, 14, RED);
            DrawText("~ - Fleeing", screenWidth - 200, 280, 14, YELLOW);
            
            // Alert status
            if (anyoneSeesPlayer) {
                DrawText("DETECTED!", 10, 70, 20, RED);
            }
            
        EndDrawing();
    }
    
    // Cleanup
    while (enemies != NULL) {
        Enemy* next = enemies->next;
        free(enemies);
        enemies = next;
    }
    
    CloseWindow();
    return 0;
}
```

## Why These AI Techniques?

Let's reflect on our design choices:

1. **State Machines** - Clear, debuggable behavior that's easy to extend
2. **Perception Systems** - Enemies feel aware, not omniscient
3. **Personality Traits** - Variety from the same code base
4. **Group Behaviors** - Emergent complexity from simple rules
5. **Performance Considerations** - Spatial partitioning for many enemies

## Common AI Pitfalls and Solutions

### Pitfall 1: Perfect AI
**Problem**: Enemies that never miss or always know where the player is
**Solution**: Add perception limits, reaction times, and occasional "mistakes"

### Pitfall 2: Predictable Patterns
**Problem**: Players memorize and exploit AI behavior
**Solution**: Add randomness, multiple strategies, and adaptive behavior

### Pitfall 3: Unfair Difficulty
**Problem**: AI that's frustrating rather than challenging
**Solution**: Telegraph attacks, provide counterplay options, tune carefully

## Practice Exercises

### "Try This" Challenges

1.  **Patrol Routes:** The `Enemy` struct has fields for `patrolX`, `patrolY`, and `patrolCount`. Implement the `UpdatePatrolState` function. The enemy should move towards its current `patrolIndex` point. When it arrives, it should increment the `patrolIndex` (looping back to 0 if it reaches the end) and then transition back to the `AI_STATE_IDLE` for a few seconds before continuing its patrol. What happens if it sees the player while on patrol?

2.  **Cowardly Goblins:** In `UpdateIdleState` and `UpdateChaseState`, use the `courage` personality trait. If a goblin's `courage` is low (e.g., < 0.3) and the player gets too close, make the goblin transition to the `AI_STATE_FLEE`. You'll need to implement the `UpdateFleeState` function, which should make the goblin move *away* from the player.

3.  **Smarter Searching:** The current `UpdateChaseState` transitions to `AI_STATE_SEARCH` when it loses sight of the player. Implement the `UpdateSearchState` function. It should make the enemy move towards the player's `lastSeenPlayerX` and `lastSeenPlayerY`. If it reaches that spot and still can't see the player, it should transition back to `AI_STATE_IDLE` or `AI_STATE_PATROL`. This creates a more believable "search" behavior.

## Summary

You've learned:
- What game AI really is and why it matters
- How to create perception systems so enemies "see"
- State machines for organizing complex behaviors
- Pathfinding for intelligent movement
- Personality systems for variety
- Group behaviors for emergent gameplay

Your enemies are now living, breathing challenges that will make your game engaging!

## Next Steps

In the next lesson, we'll implement combat systems so all this chasing and fleeing has consequences. We'll cover:
- Health and damage systems
- Different attack types
- Combat feedback and game feel
- Balancing combat difficulty

Ready to add some action? Head to [Lesson 15: Checkpoint Project - Dungeon Crawler](15-checkpoint-dungeon.md)!