# Lesson 9a (Optional): Advanced Movement and Game Feel

In the previous lesson, you built a solid, functional movement system. This lesson is optional, but highly recommended for those who want to make their game feel more responsive, polished, and professional. We'll explore advanced techniques that separate good games from great ones.

## 1. Diagonal Movement

Many games allow moving diagonally. Here's how to implement 8-directional movement:

```c
// First, let's expand our direction enum to include diagonals
typedef enum {
    DIR_NONE = 0,
    DIR_N, DIR_S, DIR_E, DIR_W,      // Cardinal directions
    DIR_NE, DIR_NW, DIR_SE, DIR_SW   // Diagonal directions
} Direction8;

// Get input for 8 directions
Direction8 GetInputDirection8() {
    // IsKeyDown() returns true while key is held (not just pressed)
    bool up = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    bool down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
    bool left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    bool right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
    
    // Check diagonal combinations first
    if (up && right) return DIR_NE;
    if (up && left) return DIR_NW;
    if (down && right) return DIR_SE;
    if (down && left) return DIR_SW;
    
    // Then check single directions
    if (up) return DIR_N;
    if (down) return DIR_S;
    if (right) return DIR_E;
    if (left) return DIR_W;
    
    return DIR_NONE;
}
```
**Important Note:** When implementing diagonal movement, you must also update your collision detection to handle it. A common approach is to check the horizontal and vertical components of the move separately to allow "sliding" along walls.

## 2. Movement with Timing

To control movement speed and make it independent of frame rate, we can use a timer.

```c
// Structure to track movement timing
typedef struct {
    float moveDelay;      // Time between moves (in seconds)
    float timeSinceMove;  // Time accumulator
} MovementTimer;

// In your game loop:
MovementTimer moveTimer = {0.15f, 0.0f}; // Move every 0.15 seconds

while (!WindowShouldClose()) {
    float deltaTime = GetFrameTime();
    moveTimer.timeSinceMove += deltaTime;

    if (moveTimer.timeSinceMove >= moveTimer.moveDelay) {
        // A move is allowed
        Direction inputDir = GetInputDirection(); // Using IsKeyDown for continuous movement
        if (inputDir != DIR_NONE) {
            TryMovePlayer(&player, inputDir, map, mapWidth, mapHeight);
            moveTimer.timeSinceMove = 0; // Reset timer
        }
    }
    // ... drawing code ...
}
```

## 3. Input Buffer System

Ever pressed a key slightly too early and the game ignored it? Input buffering fixes this by "remembering" your input for a short time.

```c
typedef struct {
    Direction bufferedInput;
    float bufferTime;
    float maxBufferTime;
} InputBuffer;

// In your game loop:
InputBuffer inputBuffer = {DIR_NONE, 0.0f, 0.2f}; // Buffer for 0.2s

// When getting input:
Direction input = GetInputDirection();
if (input != DIR_NONE) {
    BufferInput(&inputBuffer, input);
}

// When checking if a move is allowed:
if (moveTimer.timeSinceMove >= moveTimer.moveDelay) {
    if (inputBuffer.bufferedInput != DIR_NONE) {
        TryMovePlayer(&player, inputBuffer.bufferedInput, ...);
        inputBuffer.bufferedInput = DIR_NONE; // Consume buffered input
        moveTimer.timeSinceMove = 0;
    }
}

// At the end of the update, decay the buffer:
if (inputBuffer.bufferedInput != DIR_NONE) {
    inputBuffer.bufferTime -= deltaTime;
    if (inputBuffer.bufferTime <= 0) {
        inputBuffer.bufferedInput = DIR_NONE;
    }
}
```

## 4. Interaction System

Let's build a simple system for interacting with items on the map.

```c
// In your game loop, after a successful move:
char tileAtPlayer = GetTileAt(map, mapWidth, mapHeight, player.x, player.y);

if (tileAtPlayer == '!') { // Health Potion
    player.health += 25;
    if (player.health > player.maxHealth) player.health = player.maxHealth;
    SetTile(map, player.x, player.y, '.'); // Remove item from map
    // Show a message: "You picked up a potion!"
} else if (tileAtPlayer == '$') { // Gold
    player.gold += 10;
    SetTile(map, player.x, player.y, '.');
    // Show a message: "You found 10 gold!"
}
```
This is an "auto-pickup" system. You could also require a key press (like 'E') to interact with adjacent tiles.

These advanced techniques add a layer of polish that significantly improves the "feel" of your game. Experiment with them to see what works best for you!