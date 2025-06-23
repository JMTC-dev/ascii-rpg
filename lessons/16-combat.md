# Lesson 16: Combat System - Making Your Game Exciting

Combat brings your game to life! It's where players test their skills, make strategic choices, and feel the thrill of victory. Let's build a combat system step by step, starting simple and adding depth.

## The "Why": What Makes Combat Fun?

Combat is the core gameplay loop of most RPGs. Good combat is more than just trading blows; it's a puzzle for the player to solve. What makes it enjoyable?
- **Clear Feedback**: The player must immediately understand the result of their actions. Did I hit? How much damage? Did my opponent miss?
- **Meaningful Choices**: If "Attack" is always the best option, the combat is boring. Adding choices like "Defend" (reduce incoming damage) or "Special" (high risk, high reward) creates strategy.
- **Rhythm and Pacing**: Combat should have a flow. The player acts, the enemy reacts. This turn-based approach is clear and easy to understand.
- **A Touch of Randomness**: Purely deterministic combat is predictable. Adding a bit of randomness (damage ranges, miss chances) creates tension and excitement.

## Step 1: The Simplest Combat

Let's start with the absolute basics - hitting an enemy:

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));
    
    // Player and enemy
    int playerHealth = 100;
    int enemyHealth = 50;
    int playerDamage = 10;
    int enemyDamage = 5;
    
    printf("A goblin appears!\n");
    printf("Player: %d HP | Goblin: %d HP\n\n", playerHealth, enemyHealth);
    
    // Simple combat loop
    while (playerHealth > 0 && enemyHealth > 0) {
        // Player attacks
        printf("You attack! ");
        enemyHealth -= playerDamage;
        printf("Goblin takes %d damage.\n", playerDamage);
        
        if (enemyHealth <= 0) {
            printf("You defeated the goblin!\n");
            break;
        }
        
        // Enemy attacks
        printf("Goblin attacks! ");
        playerHealth -= enemyDamage;
        printf("You take %d damage.\n", enemyDamage);
        
        printf("\nPlayer: %d HP | Goblin: %d HP\n\n", playerHealth, enemyHealth);
    }
    
    if (playerHealth <= 0) {
        printf("You were defeated...\n");
    }
    
    return 0;
}
```

This works, but it's predictable. Every fight is the same! Let's add variety.

## Step 2: Adding Randomness

Combat needs uncertainty to be exciting:

```c
// Add random damage
int rollDamage(int baseDamage) {
    // Damage varies from 80% to 120%
    int min = baseDamage * 0.8;
    int max = baseDamage * 1.2;
    return min + rand() % (max - min + 1);
}

// Now combat is less predictable:
int damage = rollDamage(playerDamage);
enemyHealth -= damage;
printf("Goblin takes %d damage.\n", damage);
```

## Step 3: Hit and Miss System

Not every attack should hit:

```c
// Simple accuracy system
typedef struct {
    int health;
    int damage;
    int accuracy;  // Chance to hit (0-100)
} Fighter;

// Check if attack hits
int attackHits(int accuracy) {
    return (rand() % 100) < accuracy;
}

// In combat:
Fighter player = {100, 10, 80};  // 80% hit chance
Fighter goblin = {50, 5, 60};    // 60% hit chance

if (attackHits(player.accuracy)) {
    int damage = rollDamage(player.damage);
    goblin.health -= damage;
    printf("Hit! Goblin takes %d damage.\n", damage);
} else {
    printf("Your attack missed!\n");
}
```

Now combat has tension - will your attack connect?

## Step 4: Building a Combat System

Let's organize our combat code properly:

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Fighter structure - everything needed for combat
typedef struct {
    char name[50];
    int health;
    int maxHealth;
    int damage;
    int accuracy;
    int defense;
} Fighter;

// Create a fighter
Fighter createFighter(const char* name, int health, int damage, int accuracy, int defense) {
    Fighter f;
    strcpy(f.name, name);
    f.health = health;
    f.maxHealth = health;
    f.damage = damage;
    f.accuracy = accuracy;
    f.defense = defense;
    return f;
}

// Attack result
typedef struct {
    int hit;       // Did it hit?
    int damage;    // How much damage?
    char message[100];
} AttackResult;

// Perform an attack
AttackResult attack(Fighter* attacker, Fighter* defender) {
    AttackResult result = {0, 0, ""};
    
    // Check if hit
    if (rand() % 100 < attacker->accuracy) {
        result.hit = 1;
        
        // Calculate damage
        int baseDamage = attacker->damage;
        int variance = baseDamage / 5;  // 20% variance
        result.damage = baseDamage - variance + rand() % (variance * 2 + 1);
        
        // Apply defense
        result.damage -= defender->defense;
        if (result.damage < 1) result.damage = 1;  // Always do at least 1
        
        // Apply damage
        defender->health -= result.damage;
        
        sprintf(result.message, "%s hits %s for %d damage!", 
                attacker->name, defender->name, result.damage);
    } else {
        sprintf(result.message, "%s misses!", attacker->name);
    }
    
    return result;
}

// Display fighter status
void showStatus(Fighter* f) {
    printf("%s: %d/%d HP\n", f->name, f->health, f->maxHealth);
}

// Simple combat encounter
void combat(Fighter* player, Fighter* enemy) {
    printf("\n=== COMBAT START ===\n");
    printf("%s encounters %s!\n\n", player->name, enemy->name);
    
    while (player->health > 0 && enemy->health > 0) {
        // Show status
        showStatus(player);
        showStatus(enemy);
        printf("\n");
        
        // Player turn
        printf("Your turn! Press Enter to attack...");
        getchar();
        
        AttackResult playerAttack = attack(player, enemy);
        printf("%s\n", playerAttack.message);
        
        if (enemy->health <= 0) {
            printf("\n%s is defeated!\n", enemy->name);
            break;
        }
        
        // Enemy turn
        printf("\n%s's turn...\n", enemy->name);
        AttackResult enemyAttack = attack(enemy, player);
        printf("%s\n", enemyAttack.message);
        
        if (player->health <= 0) {
            printf("\nYou have been defeated!\n");
            break;
        }
        
        printf("\n---\n");
    }
    
    printf("\n=== COMBAT END ===\n");
}

int main() {
    srand(time(NULL));
    
    // Create fighters
    Fighter player = createFighter("Hero", 100, 15, 85, 5);
    Fighter goblin = createFighter("Goblin", 40, 8, 70, 2);
    
    // Fight!
    combat(&player, &goblin);
    
    return 0;
}
```

## Step 5: Adding Combat Options

Real combat needs choices:

```c
// Combat actions
typedef enum {
    ACTION_ATTACK,
    ACTION_DEFEND,
    ACTION_SPECIAL
} CombatAction;

// Get player's choice
CombatAction getPlayerAction() {
    printf("Choose action:\n");
    printf("1. Attack\n");
    printf("2. Defend\n");
    printf("3. Special Attack\n");
    printf("Choice: ");
    
    int choice;
    scanf("%d", &choice);
    getchar();  // Clear newline
    
    switch(choice) {
        case 1: return ACTION_ATTACK;
        case 2: return ACTION_DEFEND;
        case 3: return ACTION_SPECIAL;
        default: return ACTION_ATTACK;
    }
}

// Handle different actions
void performAction(Fighter* attacker, Fighter* defender, CombatAction action) {
    switch(action) {
        case ACTION_ATTACK:
            {
                AttackResult result = attack(attacker, defender);
                printf("%s\n", result.message);
            }
            break;
            
        case ACTION_DEFEND:
            attacker->defense += 5;  // Temporary boost
            printf("%s defends! Defense increased.\n", attacker->name);
            break;
            
        case ACTION_SPECIAL:
            if (rand() % 100 < 50) {  // 50% chance
                int damage = attacker->damage * 2;
                defender->health -= damage;
                printf("%s uses special attack! %d damage!\n", 
                       attacker->name, damage);
            } else {
                printf("%s's special attack failed!\n", attacker->name);
            }
            break;
    }
}
```

## Step 6: Critical Hits and Status Effects

Let's add more excitement:

```c
// Enhanced attack with critical hits
AttackResult enhancedAttack(Fighter* attacker, Fighter* defender) {
    AttackResult result = {0, 0, ""};
    
    // Hit check
    if (rand() % 100 >= attacker->accuracy) {
        sprintf(result.message, "%s misses!", attacker->name);
        return result;
    }
    
    result.hit = 1;
    
    // Base damage
    int baseDamage = attacker->damage;
    int variance = baseDamage / 5;
    result.damage = baseDamage - variance + rand() % (variance * 2 + 1);
    
    // Critical hit chance (10%)
    if (rand() % 100 < 10) {
        result.damage *= 2;
        sprintf(result.message, "CRITICAL HIT! ");
    } else {
        result.message[0] = '\0';
    }
    
    // Apply defense
    result.damage -= defender->defense;
    if (result.damage < 1) result.damage = 1;
    
    // Apply damage
    defender->health -= result.damage;
    
    // Complete message
    char temp[100];
    sprintf(temp, "%s hits %s for %d damage!", 
            attacker->name, defender->name, result.damage);
    strcat(result.message, temp);
    
    return result;
}

// Simple status effects
typedef struct {
    int poisoned;      // Turns remaining
    int defending;     // Extra defense active?
    int powered_up;    // Attack boost active?
} StatusEffects;

// Apply status effects each turn
void applyStatusEffects(Fighter* f, StatusEffects* status) {
    if (status->poisoned > 0) {
        f->health -= 3;
        printf("%s takes 3 poison damage!\n", f->name);
        status->poisoned--;
    }
    
    if (status->defending) {
        f->defense += 5;
    }
    
    if (status->powered_up) {
        f->damage += 5;
    }
}
```

## Step 7: Visual Combat in Raylib

Now let's make combat visual:

```c
#include "raylib.h"

// Visual combat state
typedef struct {
    Fighter player;
    Fighter enemy;
    
    // Animation
    float animationTimer;
    int showingDamage;
    int lastDamage;
    Vector2 damagePos;
    
    // UI
    char combatLog[5][100];
    int logCount;
} CombatScene;

// Draw health bar
void drawHealthBar(int x, int y, int width, int height, 
                   int current, int max, Color color) {
    // Background
    DrawRectangle(x, y, width, height, DARKGRAY);
    
    // Health
    float percent = (float)current / max;
    int fillWidth = width * percent;
    DrawRectangle(x, y, fillWidth, height, color);
    
    // Border
    DrawRectangleLines(x, y, width, height, BLACK);
    
    // Text
    char text[20];
    sprintf(text, "%d/%d", current, max);
    DrawText(text, x + 5, y + 2, 20, WHITE);
}

// Draw combat scene
void drawCombat(CombatScene* scene) {
    // Draw fighters
    DrawText("@", 200, 300, 60, GREEN);  // Player
    DrawText("g", 500, 300, 60, RED);    // Enemy
    
    // Draw health bars
    drawHealthBar(150, 250, 150, 30, 
                  scene->player.health, scene->player.maxHealth, GREEN);
    drawHealthBar(450, 250, 150, 30, 
                  scene->enemy.health, scene->enemy.maxHealth, RED);
    
    // Draw names
    DrawText(scene->player.name, 150, 220, 20, WHITE);
    DrawText(scene->enemy.name, 450, 220, 20, WHITE);
    
    // Show damage numbers
    if (scene->showingDamage) {
        char dmgText[20];
        sprintf(dmgText, "-%d", scene->lastDamage);
        DrawText(dmgText, scene->damagePos.x, 
                scene->damagePos.y - scene->animationTimer * 50, 
                30, YELLOW);
    }
    
    // Combat log
    int logY = 400;
    for (int i = 0; i < scene->logCount; i++) {
        DrawText(scene->combatLog[i], 50, logY + i * 25, 20, WHITE);
    }
    
    // Action buttons
    DrawRectangle(100, 500, 100, 40, DARKGREEN);
    DrawText("Attack", 120, 510, 20, WHITE);
    
    DrawRectangle(220, 500, 100, 40, DARKBLUE);
    DrawText("Defend", 240, 510, 20, WHITE);
    
    DrawRectangle(340, 500, 100, 40, DARKPURPLE);
    DrawText("Special", 355, 510, 20, WHITE);
}

// Handle combat input
CombatAction getCombatInput() {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        
        // Check which button
        if (mouse.x >= 100 && mouse.x <= 200 && 
            mouse.y >= 500 && mouse.y <= 540) {
            return ACTION_ATTACK;
        }
        else if (mouse.x >= 220 && mouse.x <= 320 && 
                 mouse.y >= 500 && mouse.y <= 540) {
            return ACTION_DEFEND;
        }
        else if (mouse.x >= 340 && mouse.x <= 440 && 
                 mouse.y >= 500 && mouse.y <= 540) {
            return ACTION_SPECIAL;
        }
    }
    
    return -1;  // No action
}
```

## Step 8: Complete Combat Example

Here's everything working together:

```c
// combat_game.c - Complete combat system
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// [Previous structures and functions here]

int main() {
    InitWindow(800, 600, "RPG Combat");
    SetTargetFPS(60);
    srand(time(NULL));
    
    // Initialize combat
    CombatScene scene = {0};
    scene.player = createFighter("Hero", 100, 15, 85, 5);
    scene.enemy = createFighter("Goblin", 40, 8, 70, 2);
    
    strcpy(scene.combatLog[0], "Battle begins!");
    scene.logCount = 1;
    
    int playerTurn = 1;
    
    while (!WindowShouldClose()) {
        // Update
        if (scene.animationTimer > 0) {
            scene.animationTimer -= GetFrameTime();
            if (scene.animationTimer <= 0) {
                scene.showingDamage = 0;
            }
        }
        
        // Handle player turn
        if (playerTurn && scene.player.health > 0 && scene.enemy.health > 0) {
            CombatAction action = getCombatInput();
            
            if (action != -1) {
                // Clear old log if full
                if (scene.logCount >= 5) {
                    for (int i = 0; i < 4; i++) {
                        strcpy(scene.combatLog[i], scene.combatLog[i + 1]);
                    }
                    scene.logCount = 4;
                }
                
                // Perform action
                AttackResult result = {0};
                switch(action) {
                    case ACTION_ATTACK:
                        result = attack(&scene.player, &scene.enemy);
                        strcpy(scene.combatLog[scene.logCount++], result.message);
                        
                        if (result.hit) {
                            scene.showingDamage = 1;
                            scene.lastDamage = result.damage;
                            scene.damagePos = (Vector2){500, 280};
                            scene.animationTimer = 1.0f;
                        }
                        break;
                        
                    case ACTION_DEFEND:
                        scene.player.defense += 3;
                        sprintf(scene.combatLog[scene.logCount++], 
                                "%s defends!", scene.player.name);
                        break;
                        
                    case ACTION_SPECIAL:
                        // Special attack logic
                        break;
                }
                
                // Check victory
                if (scene.enemy.health <= 0) {
                    strcpy(scene.combatLog[scene.logCount++], "Victory!");
                }
                
                playerTurn = 0;
            }
        }
        
        // Enemy turn (automatic after delay)
        if (!playerTurn && scene.enemy.health > 0 && scene.player.health > 0) {
            static float enemyTimer = 1.0f;
            enemyTimer -= GetFrameTime();
            
            if (enemyTimer <= 0) {
                AttackResult result = attack(&scene.enemy, &scene.player);
                
                if (scene.logCount >= 5) {
                    for (int i = 0; i < 4; i++) {
                        strcpy(scene.combatLog[i], scene.combatLog[i + 1]);
                    }
                    scene.logCount = 4;
                }
                
                strcpy(scene.combatLog[scene.logCount++], result.message);
                
                if (result.hit) {
                    scene.showingDamage = 1;
                    scene.lastDamage = result.damage;
                    scene.damagePos = (Vector2){200, 280};
                    scene.animationTimer = 1.0f;
                }
                
                if (scene.player.health <= 0) {
                    strcpy(scene.combatLog[scene.logCount++], "Defeated...");
                }
                
                playerTurn = 1;
                enemyTimer = 1.0f;
            }
        }
        
        // Draw
        BeginDrawing();
        ClearBackground(DARKGRAY);
        
        DrawText("RPG COMBAT", 10, 10, 40, WHITE);
        drawCombat(&scene);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
```

## Combat Design Tips

### 1. Balance is Key
```c
// Rock-paper-scissors style
if (playerAction == ATTACK && enemyAction == DEFEND) {
    damage /= 2;  // Defending reduces damage
}
else if (playerAction == SPECIAL && enemyAction == ATTACK) {
    damage *= 1.5;  // Risky but rewarding
}
```

### 2. Provide Feedback
```c
// Visual feedback for different results
if (critical) {
    DrawText("CRITICAL!", x, y, 40, YELLOW);
    // Screen shake effect
}
else if (missed) {
    DrawText("MISS", x, y, 30, GRAY);
}
```

### 3. Make Choices Matter
```c
// Each action has trade-offs
ACTION_POWER:   damage * 2, accuracy - 20
ACTION_QUICK:   damage * 0.5, accuracy + 20, attacks twice
ACTION_DEFEND:  damage * 0, defense + 10 next turn
```

## Practice Exercises

1. **Add Magic System**: Create spells that cost MP
2. **Implement Combos**: Certain action sequences do bonus damage
3. **Create Boss Patterns**: Enemies that change tactics
4. **Add Equipment Effects**: Weapons that modify combat stats

## Common Pitfalls

1. **Too Much RNG**: Some randomness is good, pure luck is bad
2. **Unclear Feedback**: Players must understand what happened
3. **No Strategy**: Every fight shouldn't be "mash attack"
4. **Unbalanced Numbers**: Test your damage formulas!

## Mini Integration: Combat in Your Game

Here's how to integrate this combat system with your existing game:

```c
// game_integration.c - Adding combat to your explorer game

typedef struct {
    // Your existing game state
    Player* player;
    Map* currentMap;
    Enemy* enemies;
    
    // New combat state
    CombatScene* activeCombat;
    bool inCombat;
} Game;

// Trigger combat when player touches enemy
void CheckEnemyCollision(Game* game) {
    Enemy* enemy = GetEnemyAt(game->enemies, game->player->x, game->player->y);
    
    if (enemy && !game->inCombat) {
        // Start combat!
        game->activeCombat = malloc(sizeof(CombatScene));
        initCombat(game->activeCombat, game->player, enemy);
        game->inCombat = true;
    }
}

// Update your main game loop
void UpdateGame(Game* game) {
    if (game->inCombat) {
        // Combat mode
        updateCombat(game->activeCombat);
        
        // Check if combat ended
        if (game->activeCombat->state == COMBAT_VICTORY) {
            // Remove defeated enemy
            RemoveEnemy(game->enemies, game->activeCombat->enemy);
            free(game->activeCombat);
            game->inCombat = false;
            
            // Reward player
            game->player->experience += 10;
            game->player->gold += 5;
        }
        else if (game->activeCombat->state == COMBAT_DEFEAT) {
            // Handle player death
            RespawnPlayer(game->player);
            free(game->activeCombat);
            game->inCombat = false;
        }
    } else {
        // Normal exploration mode
        UpdatePlayer(game->player);
        UpdateEnemies(game->enemies);
        CheckEnemyCollision(game);
    }
}

// Draw the appropriate screen
void DrawGame(Game* game) {
    if (game->inCombat) {
        drawCombat(game->activeCombat);
    } else {
        DrawMap(game->currentMap);
        DrawPlayer(game->player);
        DrawEnemies(game->enemies);
    }
}
```

### Quick Integration Checklist
- [ ] Add combat state to your game structure
- [ ] Detect player-enemy collision
- [ ] Switch between exploration and combat modes
- [ ] Handle combat results (victory/defeat)
- [ ] Update player stats after combat
- [ ] Clean up combat memory properly

## Summary

You've learned to create:
- Basic attack systems
- Hit/miss mechanics
- Damage calculations
- Visual combat feedback
- Strategic combat choices
- Status effects

Good combat makes players think, react, and feel powerful when they succeed!

## Next Steps

With combat working, let's add items and inventory to give players more options!

Continue to [Lesson 17a: Inventory Basics](17a-inventory-basics.md)!

## Quick Reference

```c
// Combat flow
1. Player chooses action
2. Calculate if hit
3. Calculate damage
4. Apply effects
5. Show feedback
6. Check victory/defeat
7. Enemy turn

// Damage formula
damage = baseDamage * modifiers - defense
if (damage < 1) damage = 1;  // Always do some damage

// Hit chance
hitChance = accuracy - evasion;
if (rand() % 100 < hitChance) // Hit!
```

Remember: Start simple, add complexity gradually!