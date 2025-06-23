# Lesson 20: Checkpoint Project - Mini RPG

It's time for our final checkpoint! Let's create a complete mini-RPG that combines all the systems you've learned. This project will help you understand how everything fits together before tackling the full game.

## Project Overview: "The Cursed Village"

You'll build a small but complete RPG with:
- A village hub with NPCs and shops
- A dangerous forest to explore
- Combat with multiple enemy types
- An inventory system with equipment
- A shop where you can buy and sell
- A simple quest to save the village
- Save/load functionality

## Learning Goals

This checkpoint demonstrates how to:
- Integrate multiple game systems
- Manage complex game state
- Create a satisfying game loop
- Balance gameplay mechanics
- Polish the player experience

## Step 1: Planning the Structure

Before we code, let's organize our project properly:

```
cursed-village/
├── main.c              # Main game loop
├── game.h              # Core game structures
├── player.h/c          # Player system
├── world.h/c           # World and maps
├── combat.h/c          # Combat system
├── inventory.h/c       # Inventory management
├── npc.h/c             # NPCs and dialogue
├── shop.h/c            # Shop system
├── quest.h/c           # Quest tracking
├── save.h/c            # Save/load system
└── Makefile
```

## Step 2: Core Game Structure

Let's start with the main game state:

```c
// game.h - Central game definitions
#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <stdbool.h>

// Game states - what's the player doing?
typedef enum {
    STATE_EXPLORING,
    STATE_TALKING,
    STATE_SHOPPING,
    STATE_COMBAT,
    STATE_INVENTORY,
    STATE_GAME_OVER,
    STATE_VICTORY
} GameState;

// Locations in our mini world
typedef enum {
    LOCATION_VILLAGE,
    LOCATION_FOREST,
    LOCATION_BOSS_LAIR
} Location;

// Central game structure
typedef struct Game {
    GameState state;
    Location currentLocation;
    
    // All our subsystems
    struct Player* player;
    struct World* world;
    struct NPCList* npcs;
    struct Quest* activeQuest;
    
    // Game progress
    bool forestUnlocked;
    bool bossDefeated;
    int turnCount;
    
    // UI state
    char* currentMessage;
    float messageTimer;
} Game;

// Core functions every game needs
Game* createGame();
void updateGame(Game* game, float deltaTime);
void renderGame(Game* game);
void cleanupGame(Game* game);

#endif
```

## Step 3: The Player System

Our player needs to feel powerful but vulnerable:

```c
// player.h
#ifndef PLAYER_H
#define PLAYER_H

#include "inventory.h"

typedef struct Player {
    // Position
    int x, y;
    
    // Stats
    int level;
    int experience;
    int experienceToNext;
    
    int health;
    int maxHealth;
    int attack;
    int defense;
    int gold;
    
    // Equipment slots
    struct Item* weapon;
    struct Item* armor;
    
    // Inventory
    struct Inventory* inventory;
    
    // Visual
    char symbol;
    Color color;
} Player;

Player* createPlayer(int startX, int startY);
void movePlayer(Player* player, int dx, int dy);
void levelUpPlayer(Player* player);
int calculateDamage(Player* player);
void healPlayer(Player* player, int amount);
void equipItem(Player* player, struct Item* item);

#endif
```

## Step 4: Building the Village

Let's create our hub area:

```c
// world.c - Village creation
World* createVillage() {
    World* world = malloc(sizeof(World));
    world->width = 25;
    world->height = 20;
    
    // Village layout
    const char* layout[] = {
        "#########################",
        "#.......#########.......#",
        "#.......#.......#.......#",
        "#.......#..INN..#.......#",
        "#.......#.......#.......#",
        "#.......####D####.......#",
        "#.......................#",
        "#.........@.............#",
        "#.......................#",
        "####D####.......####D####",
        "#.......#.......#.......#",
        "#.SHOP..#.......#.ELDER.#",
        "#.......#.......#.......#",
        "#.......#.......#.......#",
        "#########.......#########",
        "#.......................#",
        "#.......................#",
        "#........FOREST.........#",
        "#.........EXIT..........#",
        "#########################"
    };
    
    // Copy layout to world
    world->tiles = malloc(world->width * world->height);
    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            world->tiles[y * world->width + x] = layout[y][x];
        }
    }
    
    return world;
}
```

## Step 5: Creating NPCs

Let's add life to our village:

```c
// npc.c - Village NPCs
void createVillageNPCs(Game* game) {
    // The Innkeeper - provides healing
    NPC* innkeeper = createNPC(10, 4, "Martha");
    innkeeper->symbol = 'I';
    innkeeper->dialogue = malloc(sizeof(char*) * 3);
    innkeeper->dialogue[0] = "Welcome to the Rusty Sword Inn!";
    innkeeper->dialogue[1] = "You look tired. Rest for 10 gold?";
    innkeeper->dialogue[2] = "Sleep well, brave adventurer.";
    innkeeper->dialogueCount = 3;
    
    // The Shopkeeper - sells items
    NPC* shopkeeper = createNPC(3, 11, "Gareth");
    shopkeeper->symbol = 'S';
    shopkeeper->hasShop = true;
    shopkeeper->dialogue = malloc(sizeof(char*) * 2);
    shopkeeper->dialogue[0] = "Gareth's Goods! Best prices in town!";
    shopkeeper->dialogue[1] = "What can I get for you today?";
    shopkeeper->dialogueCount = 2;
    
    // The Elder - gives the main quest
    NPC* elder = createNPC(18, 11, "Elder Aldric");
    elder->symbol = 'E';
    elder->dialogue = malloc(sizeof(char*) * 4);
    elder->dialogue[0] = "Please, you must help us!";
    elder->dialogue[1] = "A terrible beast lurks in the forest.";
    elder->dialogue[2] = "It has cursed our village!";
    elder->dialogue[3] = "Defeat it, and you'll be rewarded!";
    elder->dialogueCount = 4;
    elder->hasQuest = true;
    
    // Add to game
    addNPC(game->npcs, innkeeper);
    addNPC(game->npcs, shopkeeper);
    addNPC(game->npcs, elder);
}
```

## Step 6: The Shop System

A simple but complete shop:

```c
// shop.c
void initializeShop(NPC* shopkeeper) {
    Shop* shop = malloc(sizeof(Shop));
    shop->itemCount = 5;
    shop->items = malloc(sizeof(ShopItem) * 5);
    
    // Stock the shop
    shop->items[0] = (ShopItem){
        .name = "Health Potion",
        .description = "Restores 50 HP",
        .price = 50,
        .type = ITEM_CONSUMABLE,
        .value = 50
    };
    
    shop->items[1] = (ShopItem){
        .name = "Iron Sword",
        .description = "+5 Attack",
        .price = 100,
        .type = ITEM_WEAPON,
        .value = 5
    };
    
    shop->items[2] = (ShopItem){
        .name = "Leather Armor", 
        .description = "+3 Defense",
        .price = 80,
        .type = ITEM_ARMOR,
        .value = 3
    };
    
    shop->items[3] = (ShopItem){
        .name = "Antidote",
        .description = "Cures poison",
        .price = 30,
        .type = ITEM_CONSUMABLE,
        .value = 0
    };
    
    shop->items[4] = (ShopItem){
        .name = "Magic Scroll",
        .description = "Deals 100 damage",
        .price = 200,
        .type = ITEM_CONSUMABLE,
        .value = 100
    };
    
    shopkeeper->shop = shop;
}

// Shop interface
void runShop(Game* game, Shop* shop) {
    game->state = STATE_SHOPPING;
    int selectedItem = 0;
    
    while (game->state == STATE_SHOPPING) {
        // Handle input
        if (IsKeyPressed(KEY_UP)) {
            selectedItem = (selectedItem - 1 + shop->itemCount) % shop->itemCount;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            selectedItem = (selectedItem + 1) % shop->itemCount;
        }
        
        if (IsKeyPressed(KEY_ENTER)) {
            ShopItem* item = &shop->items[selectedItem];
            if (game->player->gold >= item->price) {
                // Purchase item
                game->player->gold -= item->price;
                addItemToInventory(game->player->inventory, 
                                 createItemFromShop(item));
                setGameMessage(game, "Purchase successful!");
            } else {
                setGameMessage(game, "Not enough gold!");
            }
        }
        
        if (IsKeyPressed(KEY_ESCAPE)) {
            game->state = STATE_EXPLORING;
        }
        
        // Render shop
        renderShopInterface(shop, selectedItem, game->player->gold);
    }
}
```

## Step 7: The Forest and Combat

Create a dangerous area to explore:

```c
// world.c - Forest generation
World* createForest() {
    World* world = malloc(sizeof(World));
    world->width = 30;
    world->height = 30;
    
    // Fill with trees
    world->tiles = malloc(world->width * world->height);
    for (int i = 0; i < world->width * world->height; i++) {
        world->tiles[i] = 'T';  // Tree
    }
    
    // Create paths
    createForestPaths(world);
    
    // Add clearings
    for (int i = 0; i < 5; i++) {
        int cx = 5 + rand() % 20;
        int cy = 5 + rand() % 20;
        createClearing(world, cx, cy, 3);
    }
    
    // Place enemies
    placeForestEnemies(world);
    
    // Place the boss lair entrance
    world->tiles[15 * world->width + 15] = '>';
    
    return world;
}

// combat.c - Battle system
void startCombat(Game* game, Enemy* enemy) {
    game->state = STATE_COMBAT;
    
    // Combat loop
    while (game->state == STATE_COMBAT && 
           game->player->health > 0 && 
           enemy->health > 0) {
        
        // Player turn
        renderCombatScreen(game, enemy);
        
        if (IsKeyPressed(KEY_A)) {
            // Attack
            int damage = calculatePlayerDamage(game->player);
            enemy->health -= damage;
            
            char msg[100];
            sprintf(msg, "You deal %d damage!", damage);
            setGameMessage(game, msg);
        }
        else if (IsKeyPressed(KEY_I)) {
            // Use item
            openCombatInventory(game);
        }
        else if (IsKeyPressed(KEY_R)) {
            // Try to run
            if (rand() % 100 < 50) {
                game->state = STATE_EXPLORING;
                setGameMessage(game, "You escaped!");
                break;
            } else {
                setGameMessage(game, "Can't escape!");
            }
        }
        
        // Enemy turn
        if (enemy->health > 0) {
            int damage = enemy->attack - game->player->defense;
            if (damage < 1) damage = 1;
            game->player->health -= damage;
            
            char msg[100];
            sprintf(msg, "%s attacks for %d damage!", enemy->name, damage);
            setGameMessage(game, msg);
        }
    }
    
    // Combat results
    if (enemy->health <= 0) {
        // Victory!
        game->player->experience += enemy->expReward;
        game->player->gold += enemy->goldReward;
        
        // Check for level up
        if (game->player->experience >= game->player->experienceToNext) {
            levelUpPlayer(game->player);
        }
        
        game->state = STATE_EXPLORING;
    }
    else if (game->player->health <= 0) {
        game->state = STATE_GAME_OVER;
    }
}
```

## Step 8: The Quest System

A simple but engaging quest:

```c
// quest.c
Quest* createMainQuest() {
    Quest* quest = malloc(sizeof(Quest));
    quest->name = "Save the Village";
    quest->description = "Defeat the Shadow Beast in the forest";
    quest->stage = 0;  // Not started
    quest->maxStages = 3;
    
    // Quest stages
    quest->stages = malloc(sizeof(char*) * 3);
    quest->stages[0] = "Talk to Elder Aldric";
    quest->stages[1] = "Find the Shadow Beast in the forest";
    quest->stages[2] = "Defeat the Shadow Beast";
    
    quest->rewards.gold = 500;
    quest->rewards.experience = 200;
    quest->rewards.itemName = "Hero's Medallion";
    
    return quest;
}

// Update quest progress
void updateQuest(Game* game) {
    Quest* quest = game->activeQuest;
    
    switch(quest->stage) {
        case 0:  // Need to talk to elder
            if (game->npcs->elder->talkedTo) {
                quest->stage = 1;
                game->forestUnlocked = true;
                setGameMessage(game, "Quest Updated: Find the Shadow Beast!");
            }
            break;
            
        case 1:  // Need to find boss
            if (game->currentLocation == LOCATION_BOSS_LAIR) {
                quest->stage = 2;
                setGameMessage(game, "Quest Updated: Defeat the Shadow Beast!");
            }
            break;
            
        case 2:  // Need to defeat boss
            if (game->bossDefeated) {
                quest->stage = 3;  // Complete!
                giveQuestRewards(game, quest);
                game->state = STATE_VICTORY;
            }
            break;
    }
}
```

## Step 9: Save System

Let players continue their adventure:

```c
// save.c
void saveGame(Game* game) {
    FILE* file = fopen("savegame.dat", "wb");
    if (!file) return;
    
    // Save version for compatibility
    int version = 1;
    fwrite(&version, sizeof(int), 1, file);
    
    // Save player data
    fwrite(&game->player->level, sizeof(int), 1, file);
    fwrite(&game->player->experience, sizeof(int), 1, file);
    fwrite(&game->player->health, sizeof(int), 1, file);
    fwrite(&game->player->maxHealth, sizeof(int), 1, file);
    fwrite(&game->player->gold, sizeof(int), 1, file);
    fwrite(&game->player->x, sizeof(int), 1, file);
    fwrite(&game->player->y, sizeof(int), 1, file);
    
    // Save game progress
    fwrite(&game->currentLocation, sizeof(int), 1, file);
    fwrite(&game->forestUnlocked, sizeof(bool), 1, file);
    fwrite(&game->bossDefeated, sizeof(bool), 1, file);
    fwrite(&game->activeQuest->stage, sizeof(int), 1, file);
    
    // Save inventory
    saveInventory(file, game->player->inventory);
    
    fclose(file);
    setGameMessage(game, "Game saved!");
}

Game* loadGame() {
    FILE* file = fopen("savegame.dat", "rb");
    if (!file) return NULL;
    
    Game* game = createGame();
    
    // Check version
    int version;
    fread(&version, sizeof(int), 1, file);
    if (version != 1) {
        fclose(file);
        return NULL;  // Incompatible save
    }
    
    // Load player data
    fread(&game->player->level, sizeof(int), 1, file);
    fread(&game->player->experience, sizeof(int), 1, file);
    fread(&game->player->health, sizeof(int), 1, file);
    fread(&game->player->maxHealth, sizeof(int), 1, file);
    fread(&game->player->gold, sizeof(int), 1, file);
    fread(&game->player->x, sizeof(int), 1, file);
    fread(&game->player->y, sizeof(int), 1, file);
    
    // Load game progress
    fread(&game->currentLocation, sizeof(int), 1, file);
    fread(&game->forestUnlocked, sizeof(bool), 1, file);
    fread(&game->bossDefeated, sizeof(bool), 1, file);
    fread(&game->activeQuest->stage, sizeof(int), 1, file);
    
    // Load inventory
    loadInventory(file, game->player->inventory);
    
    fclose(file);
    return game;
}
```

## Step 10: The Main Game Loop

Bringing it all together:

```c
// main.c
int main() {
    InitWindow(800, 600, "The Cursed Village - Mini RPG");
    SetTargetFPS(60);
    
    // Main menu
    Game* game = NULL;
    bool startNewGame = showMainMenu();  // Returns true for new, false for load
    
    if (startNewGame) {
        game = createGame();
        showIntroduction();  // "The village needs a hero..."
    } else {
        game = loadGame();
        if (!game) {
            game = createGame();  // Fallback if load fails
        }
    }
    
    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        
        // Update based on state
        switch(game->state) {
            case STATE_EXPLORING:
                handleExploration(game);
                break;
                
            case STATE_TALKING:
                handleDialogue(game);
                break;
                
            case STATE_SHOPPING:
                // Handled by shop system
                break;
                
            case STATE_COMBAT:
                // Handled by combat system
                break;
                
            case STATE_INVENTORY:
                handleInventory(game);
                break;
                
            case STATE_GAME_OVER:
                handleGameOver(game);
                break;
                
            case STATE_VICTORY:
                handleVictory(game);
                break;
        }
        
        // Update subsystems
        updateQuest(game);
        updateMessages(game, deltaTime);
        
        // Render
        BeginDrawing();
        ClearBackground(BLACK);
        
        renderGame(game);
        
        EndDrawing();
    }
    
    // Cleanup
    cleanupGame(game);
    CloseWindow();
    
    return 0;
}

// Handle exploration movement and interaction
void handleExploration(Game* game) {
    // Movement
    int dx = 0, dy = 0;
    if (IsKeyPressed(KEY_W)) dy = -1;
    if (IsKeyPressed(KEY_S)) dy = 1;
    if (IsKeyPressed(KEY_A)) dx = -1;
    if (IsKeyPressed(KEY_D)) dx = 1;
    
    if (dx != 0 || dy != 0) {
        int newX = game->player->x + dx;
        int newY = game->player->y + dy;
        
        // Check for collision
        char tile = getTileAt(game->world, newX, newY);
        
        if (tile == '#' || tile == 'T') {
            // Can't walk through walls or trees
            return;
        }
        
        // Check for NPCs
        NPC* npc = getNPCAt(game->npcs, newX, newY);
        if (npc) {
            startDialogue(game, npc);
            return;
        }
        
        // Check for location transitions
        if (game->currentLocation == LOCATION_VILLAGE && 
            newY >= game->world->height - 1) {
            // Exit to forest
            if (game->forestUnlocked) {
                transitionToForest(game);
            } else {
                setGameMessage(game, "The forest is too dangerous right now!");
            }
            return;
        }
        
        // Check for enemies (in forest)
        if (game->currentLocation == LOCATION_FOREST) {
            Enemy* enemy = getEnemyAt(game->world->enemies, newX, newY);
            if (enemy) {
                startCombat(game, enemy);
                return;
            }
        }
        
        // Move player
        game->player->x = newX;
        game->player->y = newY;
        
        // Check for items on ground
        checkForPickups(game);
    }
    
    // Other controls
    if (IsKeyPressed(KEY_I)) {
        game->state = STATE_INVENTORY;
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        showPauseMenu(game);
    }
}
```

## Balancing Your Game

A mini-RPG needs careful balance:

### Player Progression
```c
// Balanced leveling curve
int getExperienceNeeded(int level) {
    return 50 * level * level;  // 50, 200, 450, 800...
}

// Stat growth per level
void levelUpPlayer(Player* player) {
    player->level++;
    player->maxHealth += 10;
    player->health = player->maxHealth;  // Full heal on level
    player->attack += 2;
    player->defense += 1;
    player->experienceToNext = getExperienceNeeded(player->level);
}
```

### Enemy Difficulty
```c
// Forest enemies scale with player
Enemy* createForestEnemy(int playerLevel) {
    Enemy* enemy = malloc(sizeof(Enemy));
    
    int type = rand() % 3;
    switch(type) {
        case 0:  // Wolf
            enemy->name = "Forest Wolf";
            enemy->symbol = 'w';
            enemy->health = 20 + playerLevel * 5;
            enemy->attack = 5 + playerLevel;
            enemy->expReward = 10 + playerLevel * 5;
            enemy->goldReward = 5 + rand() % 10;
            break;
            
        case 1:  // Bandit
            enemy->name = "Bandit";
            enemy->symbol = 'b';
            enemy->health = 30 + playerLevel * 7;
            enemy->attack = 7 + playerLevel;
            enemy->expReward = 15 + playerLevel * 7;
            enemy->goldReward = 10 + rand() % 20;
            break;
            
        case 2:  // Shadow Creature
            enemy->name = "Shadow Minion";
            enemy->symbol = 's';
            enemy->health = 25 + playerLevel * 6;
            enemy->attack = 8 + playerLevel * 2;
            enemy->expReward = 20 + playerLevel * 10;
            enemy->goldReward = 15 + rand() % 15;
            break;
    }
    
    return enemy;
}
```

## Polish and Game Feel

Small touches that make a big difference:

### Visual Feedback
```c
// Flash effect when hit
void renderDamageFlash(int x, int y) {
    static float flashTimer = 0;
    flashTimer += GetFrameTime();
    
    if (flashTimer < 0.1f) {
        DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, 
                     TILE_SIZE, TILE_SIZE, 
                     Fade(RED, 0.5f));
    } else if (flashTimer > 0.2f) {
        flashTimer = 0;
    }
}

// Floating damage numbers
typedef struct {
    int x, y;
    int damage;
    float timer;
} DamageNumber;

void updateDamageNumbers(DamageNumber* numbers, int count, float deltaTime) {
    for (int i = 0; i < count; i++) {
        if (numbers[i].timer > 0) {
            numbers[i].timer -= deltaTime;
            numbers[i].y -= deltaTime * 50;  // Float up
        }
    }
}
```

### Sound Effects
```c
// Add audio feedback
void initializeSounds(GameSounds* sounds) {
    InitAudioDevice();
    sounds->attack = LoadSound("assets/sword.wav");
    sounds->hurt = LoadSound("assets/hurt.wav");
    sounds->levelUp = LoadSound("assets/levelup.wav");
    sounds->purchase = LoadSound("assets/coin.wav");
    sounds->victory = LoadSound("assets/victory.wav");
}
```

## Testing Your Game

Create a checklist to ensure quality:

1. **Can you complete the main quest?**
2. **Is combat challenging but fair?**
3. **Do shops provide meaningful choices?**
4. **Does leveling feel rewarding?**
5. **Can you save and load properly?**
6. **Are there any soft-locks?**
7. **Is the difficulty curve smooth?**

## Common Issues and Solutions

### Problem: Players get lost
```c
// Add a simple map
void renderMiniMap(Game* game) {
    int mapX = GetScreenWidth() - 110;
    int mapY = 10;
    
    DrawRectangle(mapX, mapY, 100, 100, Fade(BLACK, 0.7f));
    DrawRectangleLines(mapX, mapY, 100, 100, WHITE);
    
    // Show player position
    int px = (game->player->x * 100) / game->world->width;
    int py = (game->player->y * 100) / game->world->height;
    DrawCircle(mapX + px, mapY + py, 3, WHITE);
}
```

### Problem: Combat feels repetitive
```c
// Add variety with skills
typedef struct {
    char* name;
    int cooldown;
    int currentCooldown;
    void (*effect)(Game* game, Enemy* target);
} Skill;

Skill* createPowerStrike() {
    Skill* skill = malloc(sizeof(Skill));
    skill->name = "Power Strike";
    skill->cooldown = 3;
    skill->effect = powerStrikeEffect;
    return skill;
}
```

## Your Complete Mini-RPG

You've now built:
- ✅ A living village with NPCs
- ✅ A shop system with economy
- ✅ Combat with strategy
- ✅ Character progression
- ✅ A quest with narrative
- ✅ Save/load functionality
- ✅ Polish and game feel

## Practice Challenges

1. **Add a Second Quest**: Create a side quest from another NPC
2. **Implement Magic**: Add spells to combat
3. **Create a Dungeon**: Add an underground area with puzzles
4. **Add Crafting**: Combine items to make better ones
5. **Boss Phases**: Make the boss fight change as it loses health

## Reflection Questions

1. What makes your game fun to play?
2. How could you expand this into a larger game?
3. What systems work well together?
4. What would you change about the balance?

## Summary

Congratulations! You've created a complete mini-RPG that combines:
- Multiple interconnected systems
- Meaningful player progression
- Engaging gameplay loop
- Professional polish

This project proves you can create real games with the skills you've learned!

## Next Steps

Ready for the final lesson? You'll learn about quests and progression systems!

Continue to [Lesson 21: Quests and Progression](21-quests-progression.md)!

## Quick Reference

```c
// Core game loop pattern
while (gameRunning) {
    HandleInput();
    UpdateGameState();
    UpdateSystems();
    CheckWinConditions();
    Render();
}

// State management
switch(gameState) {
    case EXPLORING: ...
    case COMBAT: ...
    case SHOPPING: ...
}

// System integration
Player -> Inventory -> Items
World -> NPCs -> Quests
Combat -> Experience -> Leveling
```

Remember: A great game is more than the sum of its parts!