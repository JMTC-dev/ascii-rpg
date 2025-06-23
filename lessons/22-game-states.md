# Lesson 22: Game States and Menus - Creating a Complete Experience

We've built all the pieces - combat, inventory, NPCs, quests. Now it's time to wrap them in a polished package. Game states and menus might seem like boring "glue code," but they're what transform a tech demo into a real game. They're the first thing players see and the framework that holds everything together.

## The "Why": The Framework of Your Game

Think of your game as a house. The systems we've built so far—combat, inventory, quests—are the rooms. But how do you get from one room to another? You need hallways, doors, and stairs. That's what a **game state manager** provides. It's the framework that connects all your systems into a cohesive whole.

A good state manager is crucial for:
- **Game Flow:** It controls the player's journey from the main menu, to the game, to the game over screen.
- **Clarity:** It prevents your main game loop from becoming a tangled mess of `if` statements. Instead of `if (inMenu) { ... } else if (inGame) { ... }`, we can simply `UpdateMenu()` or `UpdateGame()`.
- **Polish:** Smooth transitions, pause menus, and settings screens are all handled by the state manager. They are the difference between a tech demo and a real game.

## Understanding State Machines (Again!)

We used state machines for AI. Now we'll use them for the entire game:

```c
#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// Why use an enum for game states?
// - Type safety: Can't accidentally set state to invalid value
// - Clarity: STATE_MAIN_MENU is clearer than state = 2
// - Completeness: Compiler warns if we miss a case in switch
typedef enum {
    STATE_SPLASH,           // Studio/engine logo
    STATE_MAIN_MENU,        // Title screen
    STATE_GAME,             // Actually playing
    STATE_PAUSE,            // Game paused
    STATE_INVENTORY,        // Inventory screen (could be substates)
    STATE_DIALOGUE,         // In conversation
    STATE_SHOP,             // Shopping
    STATE_GAME_OVER,        // Player died
    STATE_VICTORY,          // Player won
    STATE_SETTINGS,         // Options menu
    STATE_LOAD_GAME,        // Load save screen
    STATE_SAVE_GAME,        // Save game screen
    STATE_QUIT              // Cleanup before exit
} GameState;

// Why track previous state?
// Some states need to return to where they came from (pause, inventory)
typedef struct {
    GameState current;
    GameState previous;
    float stateTime;        // How long in current state
    bool isTransitioning;   // Smooth transitions between states
    float transitionTime;
    float transitionDuration;
} StateManager;

// State manager functions
void InitStateManager(StateManager* sm, GameState initial) {
    sm->current = initial;
    sm->previous = initial;
    sm->stateTime = 0;
    sm->isTransitioning = false;
    sm->transitionTime = 0;
    sm->transitionDuration = 0.5f;  // Half second transitions
}

// Why separate state changes into request/update?
// Allows for cleanup, saving, and transitions
void ChangeState(StateManager* sm, GameState newState) {
    // Don't change if already transitioning
    if (sm->isTransitioning) return;
    
    sm->previous = sm->current;
    sm->current = newState;
    sm->stateTime = 0;
    sm->isTransitioning = true;
    sm->transitionTime = 0;
    
    // State exit logic
    OnStateExit(sm->previous);
    
    // State enter logic
    OnStateEnter(sm->current);
}

void UpdateStateManager(StateManager* sm, float deltaTime) {
    sm->stateTime += deltaTime;
    
    if (sm->isTransitioning) {
        sm->transitionTime += deltaTime;
        if (sm->transitionTime >= sm->transitionDuration) {
            sm->isTransitioning = false;
        }
    }
}
```

## Creating the Main Menu

The main menu is your game's front door. Let's make it inviting:

```c
// Menu item structure
typedef struct {
    char text[64];
    bool isEnabled;
    bool isVisible;
    void (*action)(void);   // Function to call when selected
} MenuItem;

// Main menu structure
typedef struct {
    MenuItem* items;
    int itemCount;
    int selectedIndex;
    
    // Visual settings
    char title[128];
    Color titleColor;
    Color selectedColor;
    Color normalColor;
    Color disabledColor;
    
    // Animation
    float titleBob;         // Title animation
    float selectionPulse;   // Selection highlight animation
} Menu;

// Create main menu
Menu* CreateMainMenu() {
    Menu* menu = (Menu*)malloc(sizeof(Menu));
    
    strcpy(menu->title, "ASCII RPG ADVENTURE");
    menu->titleColor = WHITE;
    menu->selectedColor = YELLOW;
    menu->normalColor = GRAY;
    menu->disabledColor = DARKGRAY;
    
    menu->itemCount = 5;
    menu->items = (MenuItem*)malloc(sizeof(MenuItem) * menu->itemCount);
    
    // Menu items
    strcpy(menu->items[0].text, "New Game");
    menu->items[0].isEnabled = true;
    menu->items[0].isVisible = true;
    menu->items[0].action = StartNewGame;
    
    strcpy(menu->items[1].text, "Continue");
    menu->items[1].isEnabled = SaveFileExists();  // Check for save
    menu->items[1].isVisible = true;
    menu->items[1].action = ContinueGame;
    
    strcpy(menu->items[2].text, "Settings");
    menu->items[2].isEnabled = true;
    menu->items[2].isVisible = true;
    menu->items[2].action = OpenSettings;
    
    strcpy(menu->items[3].text, "Credits");
    menu->items[3].isEnabled = true;
    menu->items[3].isVisible = true;
    menu->items[3].action = ShowCredits;
    
    strcpy(menu->items[4].text, "Quit");
    menu->items[4].isEnabled = true;
    menu->items[4].isVisible = true;
    menu->items[4].action = QuitGame;
    
    menu->selectedIndex = 0;
    menu->titleBob = 0;
    menu->selectionPulse = 0;
    
    return menu;
}

// Update main menu
void UpdateMainMenu(Menu* menu, StateManager* stateManager, float deltaTime) {
    // Animate title
    menu->titleBob += deltaTime * 2.0f;
    
    // Animate selection
    menu->selectionPulse += deltaTime * 4.0f;
    
    // Handle input
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        menu->selectedIndex--;
        // Skip disabled items
        while (menu->selectedIndex >= 0 && 
               !menu->items[menu->selectedIndex].isEnabled) {
            menu->selectedIndex--;
        }
        if (menu->selectedIndex < 0) {
            // Wrap to bottom
            menu->selectedIndex = menu->itemCount - 1;
            while (!menu->items[menu->selectedIndex].isEnabled) {
                menu->selectedIndex--;
            }
        }
    }
    
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        menu->selectedIndex++;
        // Skip disabled items
        while (menu->selectedIndex < menu->itemCount && 
               !menu->items[menu->selectedIndex].isEnabled) {
            menu->selectedIndex++;
        }
        if (menu->selectedIndex >= menu->itemCount) {
            // Wrap to top
            menu->selectedIndex = 0;
            while (!menu->items[menu->selectedIndex].isEnabled) {
                menu->selectedIndex++;
            }
        }
    }
    
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
        MenuItem* selected = &menu->items[menu->selectedIndex];
        if (selected->isEnabled && selected->action) {
            selected->action();
        }
    }
}

// Draw main menu with ASCII art
void DrawMainMenu(Menu* menu, int screenWidth, int screenHeight) {
    // Clear background
    DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);
    
    // Draw ASCII art title
    const char* asciiTitle[] = {
        " █████╗ ███████╗ ██████╗██╗██╗    ██████╗ ██████╗  ██████╗ ",
        "██╔══██╗██╔════╝██╔════╝██║██║    ██╔══██╗██╔══██╗██╔════╝ ",
        "███████║███████╗██║     ██║██║    ██████╔╝██████╔╝██║  ███╗",
        "██╔══██║╚════██║██║     ██║██║    ██╔══██╗██╔═══╝ ██║   ██║",
        "██║  ██║███████║╚██████╗██║██║    ██║  ██║██║     ╚██████╔╝",
        "╚═╝  ╚═╝╚══════╝ ╚═════╝╚═╝╚═╝    ╚═╝  ╚═╝╚═╝      ╚═════╝ "
    };
    
    // Draw title with bob effect
    int titleY = 50 + (int)(sin(menu->titleBob) * 5);
    for (int i = 0; i < 6; i++) {
        int titleX = (screenWidth - strlen(asciiTitle[0]) * 8) / 2;
        DrawText(asciiTitle[i], titleX, titleY + i * 20, 16, menu->titleColor);
    }
    
    // Draw menu items
    int menuY = 300;
    for (int i = 0; i < menu->itemCount; i++) {
        if (!menu->items[i].isVisible) continue;
        
        Color color = menu->normalColor;
        int fontSize = 20;
        
        if (i == menu->selectedIndex) {
            color = menu->selectedColor;
            fontSize = 24;  // Bigger when selected
            
            // Draw selection indicators with pulse
            int pulseOffset = (int)(sin(menu->selectionPulse) * 5);
            DrawText(">", 400 - pulseOffset, menuY, fontSize, color);
            DrawText("<", 600 + pulseOffset, menuY, fontSize, color);
        }
        
        if (!menu->items[i].isEnabled) {
            color = menu->disabledColor;
        }
        
        // Center text
        int textWidth = MeasureText(menu->items[i].text, fontSize);
        int textX = (screenWidth - textWidth) / 2;
        
        DrawText(menu->items[i].text, textX, menuY, fontSize, color);
        
        menuY += 40;
    }
    
    // Draw decorative border
    DrawRectangleLines(20, 20, screenWidth - 40, screenHeight - 40, DARKGRAY);
    
    // Version info
    DrawText("Version 1.0", 10, screenHeight - 30, 14, DARKGRAY);
    
    // Controls hint
    DrawText("↑↓ Navigate | ENTER Select", screenWidth - 250, screenHeight - 30, 14, DARKGRAY);
}
```

## Save System - Preserving Progress

A save system is essential for any RPG. Let's build a robust one:

```c
// Save file version for compatibility
#define SAVE_VERSION 1

// Save data structure
// Why separate structure? Makes it easy to add/remove saved data
typedef struct {
    int version;            // Save file version
    
    // Player data
    char playerName[64];
    int playerLevel;
    int playerExp;
    int playerHealth;
    int playerMaxHealth;
    int playerGold;
    int playerX;
    int playerY;
    int currentMapId;
    
    // Stats
    int enemiesKilled;
    int questsCompleted;
    int itemsCollected;
    float timePlayed;
    
    // Inventory
    int inventorySize;
    SavedItem* items;       // Simplified item data
    
    // Equipment
    int equippedWeaponId;
    int equippedArmorId;
    
    // Quest progress
    int activeQuestCount;
    SavedQuest* quests;     // Quest states
    
    // World state
    bool* chestsOpened;     // Which chests are opened
    bool* doorsUnlocked;    // Which doors are unlocked
    
    // Timestamp
    time_t saveTime;
} SaveData;

// Why use binary files?
// - Smaller size
// - Harder to modify (not foolproof, but deters casual cheating)
// - Faster to read/write

// Save game to file
bool SaveGame(const char* filename, GameWorld* world, Player* player) {
    FILE* file = fopen(filename, "wb");
    if (!file) return false;
    
    SaveData save = {0};
    save.version = SAVE_VERSION;
    
    // Populate save data
    strcpy(save.playerName, player->name);
    save.playerLevel = player->level;
    save.playerExp = player->experience;
    save.playerHealth = player->health;
    save.playerMaxHealth = player->maxHealth;
    save.playerGold = player->gold;
    save.playerX = player->x;
    save.playerY = player->y;
    save.currentMapId = world->currentMapId;
    
    save.enemiesKilled = player->stats.enemiesKilled;
    save.questsCompleted = player->stats.questsCompleted;
    save.itemsCollected = player->stats.itemsCollected;
    save.timePlayed = world->timePlayed;
    
    save.saveTime = time(NULL);
    
    // Write basic data
    fwrite(&save, sizeof(SaveData), 1, file);
    
    // Write inventory
    save.inventorySize = player->inventory->capacity;
    fwrite(&save.inventorySize, sizeof(int), 1, file);
    
    for (int i = 0; i < save.inventorySize; i++) {
        if (player->inventory->slots[i].item != NULL) {
            int itemId = player->inventory->slots[i].item->id;
            int quantity = player->inventory->slots[i].quantity;
            fwrite(&i, sizeof(int), 1, file);        // Slot index
            fwrite(&itemId, sizeof(int), 1, file);    // Item ID
            fwrite(&quantity, sizeof(int), 1, file);  // Quantity
        }
    }
    
    // Write -1 to indicate end of inventory
    int endMarker = -1;
    fwrite(&endMarker, sizeof(int), 1, file);
    
    // Write quest states
    fwrite(&player->journal->activeCount, sizeof(int), 1, file);
    for (int i = 0; i < player->journal->activeCount; i++) {
        Quest* quest = player->journal->activeQuests[i];
        fwrite(&quest->id, sizeof(int), 1, file);
        fwrite(&quest->isActive, sizeof(bool), 1, file);
        fwrite(&quest->isComplete, sizeof(bool), 1, file);
        
        // Write objective progress
        fwrite(&quest->objectiveCount, sizeof(int), 1, file);
        for (int j = 0; j < quest->objectiveCount; j++) {
            fwrite(&quest->objectives[j].currentCount, sizeof(int), 1, file);
            fwrite(&quest->objectives[j].isComplete, sizeof(bool), 1, file);
        }
    }
    
    fclose(file);
    return true;
}

// Load game from file
bool LoadGame(const char* filename, GameWorld* world, Player* player) {
    FILE* file = fopen(filename, "rb");
    if (!file) return false;
    
    SaveData save;
    fread(&save, sizeof(SaveData), 1, file);
    
    // Check version compatibility
    if (save.version != SAVE_VERSION) {
        fclose(file);
        return false;  // Incompatible save file
    }
    
    // Restore player data
    strcpy(player->name, save.playerName);
    player->level = save.playerLevel;
    player->experience = save.playerExp;
    player->health = save.playerHealth;
    player->maxHealth = save.playerMaxHealth;
    player->gold = save.playerGold;
    player->x = save.playerX;
    player->y = save.playerY;
    
    // Load the correct map
    if (world->currentMapId != save.currentMapId) {
        LoadMap(world, save.currentMapId);
    }
    
    // Restore stats
    player->stats.enemiesKilled = save.enemiesKilled;
    player->stats.questsCompleted = save.questsCompleted;
    player->stats.itemsCollected = save.itemsCollected;
    world->timePlayed = save.timePlayed;
    
    // Clear and restore inventory
    ClearInventory(player->inventory);
    
    int slotIndex;
    fread(&slotIndex, sizeof(int), 1, file);
    while (slotIndex != -1) {
        int itemId, quantity;
        fread(&itemId, sizeof(int), 1, file);
        fread(&quantity, sizeof(int), 1, file);
        
        // Find item in database and add to inventory
        Item* item = GetItemById(world->itemDatabase, itemId);
        if (item != NULL) {
            player->inventory->slots[slotIndex].item = item;
            player->inventory->slots[slotIndex].quantity = quantity;
        }
        
        fread(&slotIndex, sizeof(int), 1, file);
    }
    
    // Restore quest progress
    int activeQuestCount;
    fread(&activeQuestCount, sizeof(int), 1, file);
    
    for (int i = 0; i < activeQuestCount; i++) {
        int questId;
        bool isActive, isComplete;
        fread(&questId, sizeof(int), 1, file);
        fread(&isActive, sizeof(bool), 1, file);
        fread(&isComplete, sizeof(bool), 1, file);
        
        // Find quest and restore state
        Quest* quest = GetQuestById(world->questDatabase, questId);
        if (quest != NULL) {
            quest->isActive = isActive;
            quest->isComplete = isComplete;
            
            int objectiveCount;
            fread(&objectiveCount, sizeof(int), 1, file);
            for (int j = 0; j < objectiveCount && j < quest->objectiveCount; j++) {
                fread(&quest->objectives[j].currentCount, sizeof(int), 1, file);
                fread(&quest->objectives[j].isComplete, sizeof(bool), 1, file);
            }
            
            AddActiveQuest(player->journal, quest);
        }
    }
    
    fclose(file);
    return true;
}

// Save slot management
typedef struct {
    bool exists;
    char playerName[64];
    int playerLevel;
    float timePlayed;
    time_t saveTime;
    char locationName[64];
} SaveSlotInfo;

// Get save slot information for display
SaveSlotInfo GetSaveSlotInfo(int slot) {
    SaveSlotInfo info = {0};
    
    char filename[100];
    sprintf(filename, "save%d.dat", slot);
    
    FILE* file = fopen(filename, "rb");
    if (!file) {
        info.exists = false;
        return info;
    }
    
    SaveData save;
    fread(&save, sizeof(SaveData), 1, file);
    fclose(file);
    
    info.exists = true;
    strcpy(info.playerName, save.playerName);
    info.playerLevel = save.playerLevel;
    info.timePlayed = save.timePlayed;
    info.saveTime = save.saveTime;
    
    // Get location name from map ID
    GetMapName(save.currentMapId, info.locationName);
    
    return info;
}
```

## Settings and Configuration

Let players customize their experience:

```c
// Game settings
typedef struct {
    // Audio
    float masterVolume;
    float musicVolume;
    float sfxVolume;
    
    // Display
    bool fullscreen;
    int windowScale;        // 1x, 2x, 3x for pixel games
    bool vsync;
    
    // Gameplay
    float textSpeed;        // For dialogue
    bool autoSave;
    int autoSaveInterval;   // Minutes
    
    // Controls
    int keyUp;
    int keyDown;
    int keyLeft;
    int keyRight;
    int keyInteract;
    int keyInventory;
    int keyPause;
} GameSettings;

// Load settings from file
void LoadSettings(GameSettings* settings) {
    FILE* file = fopen("settings.cfg", "r");
    if (!file) {
        // Use defaults
        settings->masterVolume = 1.0f;
        settings->musicVolume = 0.8f;
        settings->sfxVolume = 1.0f;
        settings->fullscreen = false;
        settings->windowScale = 2;
        settings->vsync = true;
        settings->textSpeed = 1.0f;
        settings->autoSave = true;
        settings->autoSaveInterval = 5;
        
        // Default controls
        settings->keyUp = KEY_W;
        settings->keyDown = KEY_S;
        settings->keyLeft = KEY_A;
        settings->keyRight = KEY_D;
        settings->keyInteract = KEY_E;
        settings->keyInventory = KEY_I;
        settings->keyPause = KEY_ESCAPE;
        return;
    }
    
    // Read settings
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char key[64];
        char value[64];
        
        if (sscanf(line, "%s = %s", key, value) == 2) {
            if (strcmp(key, "masterVolume") == 0) {
                settings->masterVolume = atof(value);
            } else if (strcmp(key, "musicVolume") == 0) {
                settings->musicVolume = atof(value);
            }
            // ... parse other settings
        }
    }
    
    fclose(file);
}

// Settings menu
typedef struct {
    const char* name;
    SettingType type;
    void* value;            // Pointer to actual setting
    float minValue;         // For sliders
    float maxValue;
    float step;
    const char** options;   // For choice settings
    int optionCount;
} SettingItem;

// Draw settings menu
void DrawSettingsMenu(SettingItem* items, int itemCount, int selectedIndex) {
    DrawText("SETTINGS", 100, 50, 30, WHITE);
    
    int y = 150;
    for (int i = 0; i < itemCount; i++) {
        Color color = (i == selectedIndex) ? YELLOW : GRAY;
        
        // Draw setting name
        DrawText(items[i].name, 100, y, 20, color);
        
        // Draw value
        switch (items[i].type) {
            case SETTING_SLIDER:
                {
                    float value = *(float*)items[i].value;
                    float percent = (value - items[i].minValue) / 
                                  (items[i].maxValue - items[i].minValue);
                    
                    // Draw slider bar
                    DrawRectangle(400, y + 5, 200, 10, DARKGRAY);
                    DrawRectangle(400, y + 5, 200 * percent, 10, color);
                    
                    // Draw value
                    char valueText[32];
                    sprintf(valueText, "%.0f%%", value * 100);
                    DrawText(valueText, 620, y, 20, color);
                }
                break;
                
            case SETTING_TOGGLE:
                {
                    bool value = *(bool*)items[i].value;
                    const char* text = value ? "ON" : "OFF";
                    Color toggleColor = value ? GREEN : RED;
                    DrawText(text, 500, y, 20, toggleColor);
                }
                break;
                
            case SETTING_CHOICE:
                {
                    int value = *(int*)items[i].value;
                    DrawText(items[i].options[value], 500, y, 20, color);
                }
                break;
        }
        
        y += 40;
    }
    
    DrawText("← → Adjust | ↑ ↓ Navigate | ESC Back", 100, 600, 16, DARKGRAY);
}
```

## Game Flow Integration

Now let's tie everything together with proper state management:

```c
// Complete game structure with state management
typedef struct {
    StateManager stateManager;
    
    // Game systems
    GameWorld* world;
    Player* player;
    
    // UI systems
    Menu* mainMenu;
    Menu* pauseMenu;
    DialogueBox* dialogueBox;
    
    // Settings
    GameSettings settings;
    
    // Timing
    float totalTime;
    float autoSaveTimer;
    
    // Flags
    bool shouldQuit;
    bool hasUnsavedChanges;
} Game;

// Main game loop with state management
void RunGame() {
    Game game = {0};
    InitStateManager(&game.stateManager, STATE_SPLASH);
    
    // Initialize Raylib
    InitWindow(800, 600, "ASCII RPG");
    SetTargetFPS(60);
    
    // Load settings
    LoadSettings(&game.settings);
    ApplySettings(&game.settings);
    
    // Create menus
    game.mainMenu = CreateMainMenu();
    game.pauseMenu = CreatePauseMenu();
    
    // Main game loop
    while (!WindowShouldClose() && !game.shouldQuit) {
        float deltaTime = GetFrameTime();
        UpdateStateManager(&game.stateManager, deltaTime);
        
        // Update based on current state
        switch (game.stateManager.current) {
            case STATE_SPLASH:
                UpdateSplashScreen(&game);
                break;
                
            case STATE_MAIN_MENU:
                UpdateMainMenu(game.mainMenu, &game.stateManager, deltaTime);
                break;
                
            case STATE_GAME:
                UpdateGame(&game, deltaTime);
                break;
                
            case STATE_PAUSE:
                UpdatePauseMenu(game.pauseMenu, &game);
                break;
                
            case STATE_INVENTORY:
                UpdateInventoryScreen(&game);
                break;
                
            case STATE_DIALOGUE:
                UpdateDialogue(game.dialogueBox, deltaTime);
                break;
                
            case STATE_SHOP:
                UpdateShop(&game);
                break;
                
            case STATE_GAME_OVER:
                UpdateGameOverScreen(&game);
                break;
                
            case STATE_VICTORY:
                UpdateVictoryScreen(&game);
                break;
                
            case STATE_SETTINGS:
                UpdateSettingsMenu(&game);
                break;
                
            case STATE_SAVE_GAME:
                UpdateSaveScreen(&game);
                break;
                
            case STATE_LOAD_GAME:
                UpdateLoadScreen(&game);
                break;
        }
        
        // Draw based on current state
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Draw previous state during transitions
        if (game.stateManager.isTransitioning) {
            float alpha = game.stateManager.transitionTime / 
                         game.stateManager.transitionDuration;
            
            // Fade effect
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), 
                         Fade(BLACK, alpha));
        }
        
        switch (game.stateManager.current) {
            case STATE_SPLASH:
                DrawSplashScreen(&game);
                break;
                
            case STATE_MAIN_MENU:
                DrawMainMenu(game.mainMenu, GetScreenWidth(), GetScreenHeight());
                break;
                
            case STATE_GAME:
                DrawGame(&game);
                break;
                
            case STATE_PAUSE:
                DrawGame(&game);  // Draw game in background
                DrawPauseOverlay(&game);
                break;
                
            // ... other states
        }
        
        EndDrawing();
    }
    
    // Cleanup
    if (game.hasUnsavedChanges) {
        // Auto-save before quit
        QuickSave(&game);
    }
    
    SaveSettings(&game.settings);
    FreeGameResources(&game);
    CloseWindow();
}

// State-specific update functions
void UpdateGame(Game* game, float deltaTime) {
    // Auto-save timer
    if (game->settings.autoSave) {
        game->autoSaveTimer += deltaTime;
        if (game->autoSaveTimer >= game->settings.autoSaveInterval * 60.0f) {
            QuickSave(game);
            game->autoSaveTimer = 0;
            ShowNotification("Auto-saved", 2.0f);
        }
    }
    
    // Check for pause
    if (IsKeyPressed(game->settings.keyPause)) {
        ChangeState(&game->stateManager, STATE_PAUSE);
        return;
    }
    
    // Check for inventory
    if (IsKeyPressed(game->settings.keyInventory)) {
        ChangeState(&game->stateManager, STATE_INVENTORY);
        return;
    }
    
    // Normal game update
    UpdatePlayer(game->player, game->world, deltaTime);
    UpdateEnemies(game->world->enemies, game->player, deltaTime);
    UpdateNPCs(game->world->npcs, game->player, deltaTime);
    
    // Check win/lose conditions
    if (game->player->health <= 0) {
        ChangeState(&game->stateManager, STATE_GAME_OVER);
    } else if (CheckVictoryCondition(game)) {
        ChangeState(&game->stateManager, STATE_VICTORY);
    }
}

// Pause menu handling
void UpdatePauseMenu(Menu* pauseMenu, Game* game) {
    UpdateMenu(pauseMenu, GetFrameTime());
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        // Return to game
        ChangeState(&game->stateManager, STATE_GAME);
    }
    
    // Handle pause menu selections
    if (IsKeyPressed(KEY_ENTER)) {
        switch (pauseMenu->selectedIndex) {
            case 0:  // Resume
                ChangeState(&game->stateManager, STATE_GAME);
                break;
            case 1:  // Save Game
                ChangeState(&game->stateManager, STATE_SAVE_GAME);
                break;
            case 2:  // Settings
                ChangeState(&game->stateManager, STATE_SETTINGS);
                break;
            case 3:  // Main Menu
                if (game->hasUnsavedChanges) {
                    ShowConfirmDialog("Unsaved changes will be lost. Continue?",
                                    GoToMainMenu, NULL);
                } else {
                    ChangeState(&game->stateManager, STATE_MAIN_MENU);
                }
                break;
        }
    }
}
```

## Polish and Game Feel

The difference between good and great games is in the details:

```c
// Transition effects
typedef enum {
    TRANSITION_NONE,
    TRANSITION_FADE,
    TRANSITION_SLIDE_LEFT,
    TRANSITION_SLIDE_RIGHT,
    TRANSITION_PIXELATE,
    TRANSITION_CROSSFADE
} TransitionType;

// Draw transition effect
void DrawTransition(TransitionType type, float progress) {
    switch (type) {
        case TRANSITION_FADE:
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                         Fade(BLACK, progress));
            break;
            
        case TRANSITION_SLIDE_LEFT:
            DrawRectangle(GetScreenWidth() * (1 - progress), 0,
                         GetScreenWidth(), GetScreenHeight(), BLACK);
            break;
            
        case TRANSITION_PIXELATE:
            // Mosaic effect
            int pixelSize = 1 + (int)(progress * 32);
            for (int y = 0; y < GetScreenHeight(); y += pixelSize) {
                for (int x = 0; x < GetScreenWidth(); x += pixelSize) {
                    if ((x / pixelSize + y / pixelSize) % 2 == 0) {
                        DrawRectangle(x, y, pixelSize, pixelSize, BLACK);
                    }
                }
            }
            break;
    }
}

// Notification system
typedef struct Notification {
    char text[128];
    float lifetime;
    float maxLifetime;
    Color color;
    struct Notification* next;
} Notification;

// Show notification to player
void ShowNotification(const char* text, float duration, Color color) {
    Notification* notif = (Notification*)malloc(sizeof(Notification));
    strcpy(notif->text, text);
    notif->lifetime = 0;
    notif->maxLifetime = duration;
    notif->color = color;
    
    // Add to notification queue
    // ...
}

// Achievement system
typedef struct {
    int id;
    char name[64];
    char description[128];
    bool isUnlocked;
    time_t unlockTime;
    char iconChar;
} Achievement;

// Check achievement conditions
void CheckAchievements(Game* game) {
    // First blood
    if (game->player->stats.enemiesKilled == 1) {
        UnlockAchievement(game, ACHIEVEMENT_FIRST_BLOOD);
    }
    
    // Hoarder
    if (game->player->inventory->totalValue >= 10000) {
        UnlockAchievement(game, ACHIEVEMENT_HOARDER);
    }
    
    // Speed runner
    if (game->totalTime < 600 && CheckVictoryCondition(game)) {
        UnlockAchievement(game, ACHIEVEMENT_SPEEDRUN);
    }
}
```

## Complete Example - Putting It All Together

Here's how everything works in harmony:

```c
// Game states callbacks
void OnStateEnter(GameState state) {
    switch (state) {
        case STATE_SPLASH:
            LoadSplashResources();
            break;
            
        case STATE_MAIN_MENU:
            LoadMenuMusic();
            UpdateMainMenuState();  // Check for saves
            break;
            
        case STATE_GAME:
            if (!IsGameLoaded()) {
                GenerateNewWorld();
                CreatePlayer();
            }
            StartGameMusic();
            break;
            
        case STATE_PAUSE:
            PauseGameMusic();
            break;
            
        case STATE_GAME_OVER:
            PlayGameOverSound();
            break;
            
        case STATE_VICTORY:
            PlayVictoryMusic();
            CalculateFinalScore();
            break;
    }
}

void OnStateExit(GameState state) {
    switch (state) {
        case STATE_SPLASH:
            UnloadSplashResources();
            break;
            
        case STATE_GAME:
            // Quick save current progress
            game->hasUnsavedChanges = true;
            break;
            
        case STATE_SHOP:
            // Close any open transactions
            CloseCurrentShop();
            break;
    }
}

// Complete main function
int main() {
    // Initialize game
    RunGame();
    return 0;
}
```

## Why This Architecture Matters

Let's reflect on our design:

1. **State Management**: Clean transitions, no spaghetti code
2. **Save System**: Players can stop and resume anytime
3. **Settings**: Accessibility and personal preference
4. **Polish**: Transitions, notifications, achievements
5. **Robustness**: Handles edge cases, confirms destructive actions

## Best Practices

### Menu Design
- **Clear hierarchy**: Most important options first
- **Visual feedback**: Show what's selected, what's disabled
- **Consistent controls**: Same keys do same things everywhere
- **Quick access**: Common actions should be easy

### Save System Design
- **Multiple slots**: Let players experiment
- **Auto-save**: Prevent lost progress
- **Save anywhere**: Respect player's time
- **Clear information**: Show what will be saved

### State Management
- **One state at a time**: Avoid complex state stacks
- **Clean transitions**: Enter/exit callbacks
- **Preserve context**: Remember where player came from
- **Handle interruptions**: What if player quits during transition?

## Common Pitfalls

### Pitfall 1: Forgetting Edge Cases
**Problem**: What happens if player saves during combat?
**Solution**: Define clear save points or handle all states

### Pitfall 2: Lost Progress
**Problem**: Player loses hours of progress
**Solution**: Auto-save, multiple saves, save confirmations

### Pitfall 3: Confusing Menus
**Problem**: Players can't find options
**Solution**: Standard layouts, clear labeling, help text

## Practice Exercises

### "Try This" Challenges

1.  **Animated Transitions:** The current `DrawTransition` for `TRANSITION_FADE` is simple. Can you make it more interesting? Try making it a "wipe" where a black rectangle slides across the screen.
    *   *Hint:* For a slide-from-left transition, the rectangle's width would be `GetScreenWidth() * progress`. For a slide-from-top, the height would be `GetScreenHeight() * progress`.

2.  **Confirmation Dialogs:** The `UpdatePauseMenu` function has a placeholder for a confirmation dialog. Implement it! When the player tries to quit to the main menu with unsaved changes, you should change to a new `STATE_CONFIRM_QUIT`. This state should draw the game screen dimmed, with a text box in the middle asking "Are you sure? (Y/N)". Pressing 'Y' would change state to `MAIN_MENU`, and 'N' would change it back to `PAUSE`.

3.  **Key Rebinding:** This is a classic settings menu feature. In the `UpdateSettingsMenu` function, if the player presses ENTER on a control setting, you could enter a "waiting for key" state. The next key the player presses becomes the new binding for that action.
    *   *Hint:* Raylib's `GetKeyPressed()` function returns the keycode of *any* key pressed on that frame, which is perfect for this.

## Summary

You've learned:
- State machine architecture for game flow
- Creating polished menus with personality
- Robust save/load systems
- Settings and configuration
- Polish techniques that make games feel professional

Your game now has the framework of a complete, polished experience!

## Next Steps

In our final lesson, we'll put EVERYTHING together into a complete game! We'll:
- Combine all systems
- Add final polish
- Create content
- Package for distribution

Ready to see your complete game? Head to [Lesson 23: Bringing It All Together - Your Complete ASCII RPG](23-complete-game.md)!