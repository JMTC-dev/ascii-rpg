# Lesson 15: Checkpoint Project - Dungeon Crawler

This is our second major checkpoint project. We will combine everything you've learned since the last checkpoint: procedural world generation, collision detection, and basic enemy AI.

The goal is to build a small, playable dungeon crawler that demonstrates a clean, modular project structure.

## Learning Goals

This checkpoint reinforces:
- **Integration**: Combining multiple complex systems (world, player, enemies, rendering).
- **Modular Design**: Structuring the project with header files, implementation files, and a `Makefile`.
- **Dynamic Memory**: Using `malloc` and `free` to manage game objects.
- **Game Loop**: Creating a complete loop that handles input, updates state, and renders the scene.

## Step 1: Project Structure

As with our previous projects, good organization is key. We will structure our files logically. You will create:
- `world.h` / `world.c`: For map data and procedural generation.
- `entity.h` / `entity.c`: A unified module for both player and enemies.
- `main.c`: The entry point and main game loop.
- `Makefile`: To compile everything.

## Step 2: The Code

Now, populate your files with the following code. This combines the procedural generation from Lesson 11 with the entity and collision concepts from Lessons 13 and 14.

### `entity.h`
```c
#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"

typedef enum {
    ET_PLAYER,
    ET_ENEMY
} EntityType;

typedef struct Entity {
    int x;
    int y;
    char symbol;
    Color color;
    EntityType type;
    int moveTimer;
    struct Entity* next;
} Entity;

Entity* CreatePlayer(int x, int y);
Entity* CreateEnemy(int x, int y);
void FreeEntities(Entity* head);

#endif
```

### `entity.c`
```c
#include "entity.h"
#include <stdlib.h>

static Entity* createEntity(int x, int y, char symbol, Color color, EntityType type) {
    Entity* entity = (Entity*)malloc(sizeof(Entity));
    if (entity != NULL) {
        entity->x = x;
        entity->y = y;
        entity->symbol = symbol;
        entity->color = color;
        entity->type = type;
        entity->moveTimer = 0;
        entity->next = NULL;
    }
    return entity;
}

Entity* CreatePlayer(int x, int y) {
    return createEntity(x, y, '@', WHITE, ET_PLAYER);
}

Entity* CreateEnemy(int x, int y) {
    return createEntity(x, y, 'g', RED, ET_ENEMY);
}

void FreeEntities(Entity* head) {
    Entity* current = head;
    while (current != NULL) {
        Entity* next = current->next;
        free(current);
        current = next;
    }
}
```

### `world.h`
```c
#ifndef WORLD_H
#define WORLD_H

#include "entity.h"

typedef struct {
    char* tiles;
    int width;
    int height;
} World;

World* CreateWorld(int width, int height, int numRooms, int* playerStartX, int* playerStartY);
void FreeWorld(World* world);
int CanWalk(World* world, Entity* allEntities, int x, int y);

#endif
```

### `world.c`
*(This is a simplified version of the generator from Lesson 11)*
```c
#include "world.h"
#include <stdlib.h>
#include <string.h>

// Internal helper functions for generation
static void CreateRoom(World* world, int x, int y, int w, int h) {
    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            if (j > 0 && j < world->width -1 && i > 0 && i < world->height -1)
            world->tiles[i * world->width + j] = '.';
        }
    }
}

static void CreateHCorridor(World* world, int x1, int x2, int y) {
    for (int x = (x1 < x2 ? x1 : x2); x <= (x1 > x2 ? x1 : x2); x++) {
        world->tiles[y * world->width + x] = '.';
    }
}

static void CreateVCorridor(World* world, int y1, int y2, int x) {
    for (int y = (y1 < y2 ? y1 : y2); y <= (y1 > y2 ? y1 : y2); y++) {
        world->tiles[y * world->width + x] = '.';
    }
}

World* CreateWorld(int width, int height, int numRooms, int* playerStartX, int* playerStartY) {
    World* world = (World*)malloc(sizeof(World));
    if (world == NULL) return NULL;

    world->width = width;
    world->height = height;
    world->tiles = (char*)malloc(width * height);
    if (world->tiles == NULL) {
        free(world);
        return NULL;
    }

    memset(world->tiles, '#', width * height);

    int* rooms = (int*)malloc(sizeof(int) * numRooms * 4);
    int lastX, lastY;

    for (int i = 0; i < numRooms; i++) {
        int w = 5 + rand() % 6;
        int h = 5 + rand() % 6;
        int x = 1 + rand() % (width - w - 2);
        int y = 1 + rand() % (height - h - 2);
        CreateRoom(world, x, y, w, h);

        int centerX = x + w / 2;
        int centerY = y + h / 2;

        if (i == 0) {
            *playerStartX = centerX;
            *playerStartY = centerY;
        } else {
            if (rand() % 2 == 0) {
                CreateHCorridor(world, lastX, centerX, lastY);
                CreateVCorridor(world, lastY, centerY, centerX);
            } else {
                CreateVCorridor(world, lastY, centerY, lastX);
                CreateHCorridor(world, lastX, centerX, centerY);
            }
        }
        lastX = centerX;
        lastY = centerY;
    }
    
    world->tiles[lastY * width + lastX] = '>';
    free(rooms);
    return world;
}

void FreeWorld(World* world) {
    if (world != NULL) {
        free(world->tiles);
        free(world);
    }
}

int CanWalk(World* world, Entity* allEntities, int x, int y) {
    if (x < 0 || x >= world->width || y < 0 || y >= world->height || world->tiles[y * world->width + x] == '#') {
        return 0;
    }
    Entity* current = allEntities;
    while (current != NULL) {
        if (current->x == x && current->y == y) return 0;
        current = current->next;
    }
    return 1;
}
```

### `main.c`
```c
#include "raylib.h"
#include "world.h"
#include "entity.h"
#include <stdlib.h>
#include <time.h>

#define TILE_SIZE 25
#define VIEW_WIDTH 25
#define VIEW_HEIGHT 20

int main() {
    InitWindow(800, 600, "Dungeon Crawler");
    SetTargetFPS(60);
    srand(time(NULL));

    int pX, pY;
    World* world = CreateWorld(50, 40, 10, &pX, &pY);
    
    Entity* player = CreatePlayer(pX, pY);
    Entity* enemies = CreateEnemy(pX + 5, pY + 5);
    enemies->next = CreateEnemy(pX - 5, pY - 5);
    player->next = enemies; // Master list for collision checks

    Camera2D camera = { 0 };
    camera.target = (Vector2){ player->x * TILE_SIZE, player->y * TILE_SIZE };
    camera.offset = (Vector2){ 400, 300 };
    camera.zoom = 1.0f;

    while (!WindowShouldClose()) {
        // Player Movement
        int dx = 0, dy = 0;
        if (IsKeyPressed(KEY_W)) dy = -1;
        if (IsKeyPressed(KEY_S)) dy = 1;
        if (IsKeyPressed(KEY_A)) dx = -1;
        if (IsKeyPressed(KEY_D)) dx = 1;

        if ((dx != 0 || dy != 0) && CanWalk(world, player, player->x + dx, player->y + dy)) {
            player->x += dx;
            player->y += dy;
        }

        // Enemy Movement (simple random walk)
        Entity* current = enemies;
        while(current != NULL) {
            current->moveTimer++;
            if (current->moveTimer > 30) {
                int ex = 0, ey = 0;
                int dir = rand() % 4;
                if (dir == 0) ex = 1; else if (dir == 1) ex = -1;
                else if (dir == 2) ey = 1; else ey = -1;
                if (CanWalk(world, player, current->x + ex, current->y + ey)) {
                    current->x += ex;
                    current->y += ey;
                }
                current->moveTimer = 0;
            }
            current = current->next;
        }

        camera.target = (Vector2){ player->x * TILE_SIZE, player->y * TILE_SIZE };

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);

        // Draw World
        for (int y = 0; y < world->height; y++) {
            for (int x = 0; x < world->width; x++) {
                char tile = world->tiles[y * world->width + x];
                Color color = (tile == '#') ? GRAY : DARKGRAY;
                if (tile == '>') color = YELLOW;
                DrawText(&tile, x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, color);
            }
        }

        // Draw Entities
        current = player;
        while(current != NULL) {
            DrawText(&current->symbol, current->x * TILE_SIZE, current->y * TILE_SIZE, TILE_SIZE, current->color);
            current = current->next;
        }

        EndMode2D();
        DrawText("Find the exit (>)", 10, 10, 20, WHITE);
        EndDrawing();
    }

    FreeWorld(world);
    FreeEntities(player);
    CloseWindow();
    return 0;
}
```

### `Makefile`
```makefile
CC = gcc
CFLAGS = -Wall -g -std=c99
LIBS = -lraylib -lm
TARGET = dungeon

all: $(TARGET)

$(TARGET): main.o world.o entity.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o world.o entity.o $(LIBS)

main.o: main.c world.h entity.h
	$(CC) $(CFLAGS) -c main.c

world.o: world.c world.h entity.h
	$(CC) $(CFLAGS) -c world.c

entity.o: entity.c entity.h
	$(CC) $(CFLAGS) -c entity.c

clean:
	rm -f *.o $(TARGET)
```

## Step 3: Compile and Run
Save all these files in the same directory. Open a terminal in that directory and run:
```bash
make
./dungeon
```
You now have a working dungeon crawler that uses a modular structure!

## Summary
This checkpoint demonstrates your mastery of:
- ✓ Dynamic memory allocation for worlds and entities
- ✓ Camera systems for large maps
- ✓ Collision detection between entities and the world
- ✓ Basic AI movement
- ✓ Modular code organization with a `Makefile`

This foundation prepares you for the advanced systems coming next.

## Next Steps

You've successfully integrated several complex systems into a playable game prototype. This is a huge milestone! You're now ready to add the core gameplay mechanic that makes RPGs exciting: combat.

Continue to [Lesson 16: Combat System](16-combat.md) when you're ready!

## Complete Code

The full working example is about 200 lines - much more manageable than our earlier attempts! You built it step by step, understanding each part.

Remember: Good games aren't about code length, but about fun gameplay!