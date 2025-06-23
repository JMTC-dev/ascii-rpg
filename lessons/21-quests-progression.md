# Lesson 21: Quests and Progression

Great RPGs aren't just about combat - they're about stories, goals, and the feeling of growing stronger. In this lesson, we'll create quest and progression systems that give players purpose and reward their efforts.

## The "Why": Quests as the Engine of Your Game

If combat is the core gameplay loop, quests are the engine that drives the player through that loop. A good quest system is the primary tool for storytelling and guiding the player through the world. It answers the player's most fundamental question: "What should I do next?"

Quests provide:
- **Direction & Motivation**: They give the player clear, achievable goals and a reason to pursue them (the reward).
- **Narrative Context**: They weave the story into the gameplay. "Kill 10 rats" is boring. "Kill 10 rats that are infesting the orphanage's basement" is a story.
- **Pacing and Structure**: They guide the player from one area to the next and provide a satisfying sense of progression.

## Understanding Quest Design

Before we code, let's think about what makes a good quest:

### Quest Types
1. **Main Quests**: Drive the story forward
2. **Side Quests**: Optional content for rewards
3. **Fetch Quests**: Collect X items
4. **Kill Quests**: Defeat Y enemies
5. **Escort Quests**: Protect someone
6. **Discovery Quests**: Find hidden locations

### Quest Structure
Every quest needs:
- Clear objectives
- Meaningful rewards
- Progress tracking
- Completion conditions

## Building a Quest System

Let's start with a flexible quest structure:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Quest states - where are we in this quest?
typedef enum {
    QUEST_NOT_STARTED,
    QUEST_ACTIVE,
    QUEST_COMPLETE,
    QUEST_TURNED_IN
} QuestState;

// Types of objectives
typedef enum {
    OBJECTIVE_KILL,      // Kill X enemies
    OBJECTIVE_COLLECT,   // Collect Y items
    OBJECTIVE_TALK,      // Talk to NPC
    OBJECTIVE_REACH,     // Go to location
    OBJECTIVE_SURVIVE    // Survive for time
} ObjectiveType;

// A single objective within a quest
typedef struct {
    ObjectiveType type;
    char description[128];
    int targetId;        // Enemy/item/NPC id
    int required;        // How many needed
    int current;         // How many done
    bool completed;
} QuestObjective;

// The quest structure
typedef struct {
    int id;
    char name[64];
    char description[256];
    
    QuestState state;
    
    // Objectives (can have multiple)
    QuestObjective* objectives;
    int objectiveCount;
    
    // Requirements to start
    int requiredLevel;
    int requiredQuestId;  // Must complete this first
    
    // Rewards
    int experienceReward;
    int goldReward;
    int itemReward;       // Item ID to give
    
    // Quest giver info
    int giverNPCId;
    int turnInNPCId;      // Might be different
} Quest;
```

### Why This Design?
- **Flexible**: Supports multiple objective types
- **Trackable**: Clear progress for each part
- **Chainable**: Quests can require other quests
- **Rewardable**: Multiple reward types

## Creating Quests

Let's make some example quests:

```c
// Create the main story quest
Quest* createMainQuest() {
    Quest* quest = malloc(sizeof(Quest));
    quest->id = 1;
    strcpy(quest->name, "The Hero's Journey");
    strcpy(quest->description, 
           "The village elder needs your help to save the town!");
    
    quest->state = QUEST_NOT_STARTED;
    quest->requiredLevel = 1;
    quest->requiredQuestId = 0;  // No prerequisite
    
    // Multiple objectives
    quest->objectiveCount = 3;
    quest->objectives = malloc(sizeof(QuestObjective) * 3);
    
    // Objective 1: Talk to elder
    quest->objectives[0] = (QuestObjective){
        .type = OBJECTIVE_TALK,
        .targetId = 100,  // Elder's NPC ID
        .required = 1,
        .current = 0,
        .completed = false
    };
    strcpy(quest->objectives[0].description, "Speak with Elder Aldric");
    
    // Objective 2: Kill goblins
    quest->objectives[1] = (QuestObjective){
        .type = OBJECTIVE_KILL,
        .targetId = 1,  // Goblin enemy ID
        .required = 5,
        .current = 0,
        .completed = false
    };
    strcpy(quest->objectives[1].description, "Defeat 5 goblins");
    
    // Objective 3: Return to elder
    quest->objectives[2] = (QuestObjective){
        .type = OBJECTIVE_TALK,
        .targetId = 100,
        .required = 1,
        .current = 0,
        .completed = false
    };
    strcpy(quest->objectives[2].description, "Return to Elder Aldric");
    
    // Rewards
    quest->experienceReward = 100;
    quest->goldReward = 50;
    quest->itemReward = 10;  // Health potion
    
    return quest;
}

// Create a side quest
Quest* createCollectionQuest() {
    Quest* quest = malloc(sizeof(Quest));
    quest->id = 2;
    strcpy(quest->name, "Herb Gathering");
    strcpy(quest->description, 
           "The healer needs medicinal herbs from the forest.");
    
    quest->state = QUEST_NOT_STARTED;
    quest->requiredLevel = 2;
    quest->requiredQuestId = 0;
    
    quest->objectiveCount = 1;
    quest->objectives = malloc(sizeof(QuestObjective));
    
    quest->objectives[0] = (QuestObjective){
        .type = OBJECTIVE_COLLECT,
        .targetId = 5,  // Herb item ID
        .required = 10,
        .current = 0,
        .completed = false
    };
    strcpy(quest->objectives[0].description, "Collect 10 Healing Herbs");
    
    quest->experienceReward = 50;
    quest->goldReward = 30;
    quest->itemReward = 11;  // Mana potion
    
    return quest;
}
```

## The Quest Manager

Now let's manage active quests:

```c
typedef struct {
    Quest** quests;        // All quests in the game
    int questCount;
    
    Quest** activeQuests;  // Player's active quests
    int activeCount;
    int maxActive;         // Limit active quests
    
    // Track completed quests
    int* completedIds;
    int completedCount;
} QuestManager;

// Initialize quest system
QuestManager* createQuestManager() {
    QuestManager* qm = malloc(sizeof(QuestManager));
    
    qm->maxActive = 5;  // Can have 5 quests at once
    qm->activeQuests = malloc(sizeof(Quest*) * qm->maxActive);
    qm->activeCount = 0;
    
    qm->questCount = 0;
    qm->quests = NULL;
    
    qm->completedIds = malloc(sizeof(int) * 100);  // Track up to 100
    qm->completedCount = 0;
    
    return qm;
}

// Check if player can start a quest
bool canStartQuest(QuestManager* qm, Quest* quest, int playerLevel) {
    // Check level requirement
    if (playerLevel < quest->requiredLevel) {
        return false;
    }
    
    // Check prerequisite quest
    if (quest->requiredQuestId > 0) {
        bool foundPrereq = false;
        for (int i = 0; i < qm->completedCount; i++) {
            if (qm->completedIds[i] == quest->requiredQuestId) {
                foundPrereq = true;
                break;
            }
        }
        if (!foundPrereq) return false;
    }
    
    // Check if already active or completed
    for (int i = 0; i < qm->activeCount; i++) {
        if (qm->activeQuests[i]->id == quest->id) {
            return false;  // Already have it
        }
    }
    
    // Check if room for more quests
    if (qm->activeCount >= qm->maxActive) {
        return false;
    }
    
    return true;
}

// Start a quest
void startQuest(QuestManager* qm, Quest* quest) {
    if (qm->activeCount < qm->maxActive) {
        quest->state = QUEST_ACTIVE;
        qm->activeQuests[qm->activeCount++] = quest;
        
        printf("Quest Started: %s\n", quest->name);
        printf("%s\n", quest->description);
    }
}
```

## Tracking Quest Progress

The key to good quests is tracking progress:

```c
// Update quest progress when something happens
void updateQuestProgress(QuestManager* qm, ObjectiveType type, int targetId, int amount) {
    // Check all active quests
    for (int i = 0; i < qm->activeCount; i++) {
        Quest* quest = qm->activeQuests[i];
        
        // Check all objectives in this quest
        for (int j = 0; j < quest->objectiveCount; j++) {
            QuestObjective* obj = &quest->objectives[j];
            
            // Does this objective match what just happened?
            if (obj->type == type && obj->targetId == targetId && !obj->completed) {
                obj->current += amount;
                
                // Check if objective complete
                if (obj->current >= obj->required) {
                    obj->current = obj->required;
                    obj->completed = true;
                    printf("Objective Complete: %s\n", obj->description);
                    
                    // Check if whole quest complete
                    if (isQuestComplete(quest)) {
                        quest->state = QUEST_COMPLETE;
                        printf("Quest Complete: %s\n", quest->name);
                    }
                }
                
                // Show progress
                printf("Progress: %s (%d/%d)\n", 
                       obj->description, obj->current, obj->required);
            }
        }
    }
}

// Check if all objectives done
bool isQuestComplete(Quest* quest) {
    for (int i = 0; i < quest->objectiveCount; i++) {
        if (!quest->objectives[i].completed) {
            return false;
        }
    }
    return true;
}

// Example: Player kills an enemy
void onEnemyKilled(QuestManager* qm, int enemyId) {
    updateQuestProgress(qm, OBJECTIVE_KILL, enemyId, 1);
}

// Example: Player picks up item
void onItemCollected(QuestManager* qm, int itemId) {
    updateQuestProgress(qm, OBJECTIVE_COLLECT, itemId, 1);
}

// Example: Player talks to NPC
void onNPCTalked(QuestManager* qm, int npcId) {
    updateQuestProgress(qm, OBJECTIVE_TALK, npcId, 1);
}
```

## Quest UI

Players need to see their quests:

```c
// Draw quest log
void drawQuestLog(QuestManager* qm, int x, int y) {
    DrawText("QUEST LOG", x, y, 24, WHITE);
    y += 30;
    
    if (qm->activeCount == 0) {
        DrawText("No active quests", x, y, 18, GRAY);
        return;
    }
    
    // Show each active quest
    for (int i = 0; i < qm->activeCount; i++) {
        Quest* quest = qm->activeQuests[i];
        
        // Quest name (different color if complete)
        Color nameColor = quest->state == QUEST_COMPLETE ? GREEN : WHITE;
        DrawText(quest->name, x, y, 20, nameColor);
        y += 25;
        
        // Show objectives
        for (int j = 0; j < quest->objectiveCount; j++) {
            QuestObjective* obj = &quest->objectives[j];
            
            // Checkbox
            char checkbox = obj->completed ? 'X' : ' ';
            DrawText(TextFormat("[%c]", checkbox), x + 10, y, 16, WHITE);
            
            // Objective text
            Color objColor = obj->completed ? GREEN : GRAY;
            DrawText(obj->description, x + 40, y, 16, objColor);
            
            // Progress (if applicable)
            if (obj->type == OBJECTIVE_KILL || obj->type == OBJECTIVE_COLLECT) {
                DrawText(TextFormat("(%d/%d)", obj->current, obj->required), 
                        x + 300, y, 16, objColor);
            }
            
            y += 20;
        }
        
        y += 10;  // Space between quests
    }
}

// Draw quest tracker (minimal UI during gameplay)
void drawQuestTracker(QuestManager* qm) {
    int x = GetScreenWidth() - 250;
    int y = 100;
    
    // Only show current objective for active quests
    for (int i = 0; i < qm->activeCount && i < 3; i++) {  // Max 3 shown
        Quest* quest = qm->activeQuests[i];
        if (quest->state != QUEST_ACTIVE) continue;
        
        // Find current objective
        for (int j = 0; j < quest->objectiveCount; j++) {
            if (!quest->objectives[j].completed) {
                DrawText(quest->objectives[j].description, x, y, 14, WHITE);
                
                if (quest->objectives[j].type == OBJECTIVE_KILL || 
                    quest->objectives[j].type == OBJECTIVE_COLLECT) {
                    DrawText(TextFormat("%d/%d", 
                            quest->objectives[j].current,
                            quest->objectives[j].required), 
                            x, y + 15, 12, GRAY);
                }
                
                y += 40;
                break;  // Only show first incomplete objective
            }
        }
    }
}
```

## Quest Rewards

Completing quests should feel rewarding:

```c
// Turn in completed quest
void turnInQuest(QuestManager* qm, Quest* quest, Player* player) {
    if (quest->state != QUEST_COMPLETE) {
        return;  // Not ready to turn in
    }
    
    // Give rewards
    printf("\nQuest Completed: %s\n", quest->name);
    printf("Rewards:\n");
    
    if (quest->experienceReward > 0) {
        player->experience += quest->experienceReward;
        printf("  +%d Experience\n", quest->experienceReward);
        
        // Check for level up
        checkLevelUp(player);
    }
    
    if (quest->goldReward > 0) {
        player->gold += quest->goldReward;
        printf("  +%d Gold\n", quest->goldReward);
    }
    
    if (quest->itemReward > 0) {
        Item* reward = createItem(quest->itemReward);
        addToInventory(player->inventory, reward);
        printf("  Received: %s\n", reward->name);
    }
    
    // Mark as turned in
    quest->state = QUEST_TURNED_IN;
    
    // Add to completed list
    qm->completedIds[qm->completedCount++] = quest->id;
    
    // Remove from active quests
    removeActiveQuest(qm, quest);
    
    // Check if this unlocks new quests
    checkNewQuests(qm, player);
}
```

## Character Progression

Quests tie into character growth:

```c
// Player progression system
typedef struct {
    int level;
    int experience;
    int experienceToNext;
    
    // Base stats
    int strength;
    int agility;
    int intelligence;
    
    // Derived stats
    int maxHealth;
    int maxMana;
    int attackPower;
    int defense;
    
    // Skill points to spend
    int unspentPoints;
} PlayerStats;

// Calculate experience needed for next level
int getExperienceRequired(int level) {
    // Exponential curve: 100, 300, 600, 1000...
    return 100 * level * (level + 1) / 2;
}

// Level up!
void levelUp(Player* player) {
    player->stats.level++;
    
    // Increase base stats
    player->stats.strength += 2;
    player->stats.agility += 2;
    player->stats.intelligence += 2;
    
    // Update derived stats
    player->stats.maxHealth = 100 + (player->stats.strength * 10);
    player->stats.maxMana = 50 + (player->stats.intelligence * 5);
    player->stats.attackPower = 10 + (player->stats.strength * 2);
    player->stats.defense = 5 + (player->stats.agility);
    
    // Give skill points
    player->stats.unspentPoints += 3;
    
    // Full heal on level up
    player->currentHealth = player->stats.maxHealth;
    player->currentMana = player->stats.maxMana;
    
    // Calculate next level requirement
    player->stats.experienceToNext = getExperienceRequired(player->stats.level);
    
    // Celebration!
    printf("LEVEL UP! You are now level %d!\n", player->stats.level);
    PlaySound(levelUpSound);  // Assuming you have sound
}

// Check if player should level up
void checkLevelUp(Player* player) {
    while (player->stats.experience >= player->stats.experienceToNext) {
        player->stats.experience -= player->stats.experienceToNext;
        levelUp(player);
    }
}
```

## Quest Chains

Some quests lead to others:

```c
// Create a quest chain
void createQuestChain() {
    // Quest 1: Introduction
    Quest* quest1 = malloc(sizeof(Quest));
    quest1->id = 10;
    strcpy(quest1->name, "New Arrival");
    strcpy(quest1->description, "Introduce yourself to the town");
    // ... objectives ...
    
    // Quest 2: Requires quest 1
    Quest* quest2 = malloc(sizeof(Quest));
    quest2->id = 11;
    quest2->requiredQuestId = 10;  // Must complete quest 1 first
    strcpy(quest2->name, "Proving Yourself");
    strcpy(quest2->description, "Show the town you can help");
    // ... objectives ...
    
    // Quest 3: The real challenge
    Quest* quest3 = malloc(sizeof(Quest));
    quest3->id = 12;
    quest3->requiredQuestId = 11;
    strcpy(quest3->name, "Hero of the Town");
    strcpy(quest3->description, "Save the town from great danger");
    // ... objectives ...
}
```

## Dynamic Quests

Make quests feel alive:

```c
// Daily quests that reset
typedef struct {
    Quest* quest;
    int dayLastCompleted;
} DailyQuest;

// Random quest generation
Quest* generateRandomQuest(int playerLevel) {
    Quest* quest = malloc(sizeof(Quest));
    quest->id = 1000 + rand() % 1000;  // Random ID
    
    // Pick random type
    int type = rand() % 3;
    switch(type) {
        case 0:  // Hunt quest
            sprintf(quest->name, "Bounty: %s", getRandomEnemyName());
            quest->objectives[0].type = OBJECTIVE_KILL;
            quest->objectives[0].required = 3 + rand() % 5;
            quest->goldReward = 20 * quest->objectives[0].required;
            break;
            
        case 1:  // Gather quest  
            sprintf(quest->name, "Gathering: %s", getRandomItemName());
            quest->objectives[0].type = OBJECTIVE_COLLECT;
            quest->objectives[0].required = 5 + rand() % 10;
            quest->goldReward = 10 * quest->objectives[0].required;
            break;
            
        case 2:  // Delivery quest
            sprintf(quest->name, "Delivery to %s", getRandomNPCName());
            quest->objectives[0].type = OBJECTIVE_TALK;
            quest->objectives[0].required = 1;
            quest->goldReward = 50 + rand() % 50;
            break;
    }
    
    quest->experienceReward = quest->goldReward * 2;
    quest->requiredLevel = playerLevel - 1;  // Slightly below player
    
    return quest;
}
```

## Quest Design Best Practices

### 1. Clear Objectives
```c
// Bad: Vague objective
"Help the village"

// Good: Specific objective
"Defeat 10 goblins threatening the village"
```

### 2. Meaningful Rewards
```c
// Scale rewards with effort
int calculateReward(Quest* quest) {
    int totalEffort = 0;
    
    for (int i = 0; i < quest->objectiveCount; i++) {
        QuestObjective* obj = &quest->objectives[i];
        
        switch(obj->type) {
            case OBJECTIVE_KILL:
                totalEffort += obj->required * 10;
                break;
            case OBJECTIVE_COLLECT:
                totalEffort += obj->required * 5;
                break;
            case OBJECTIVE_TALK:
                totalEffort += 20;
                break;
        }
    }
    
    quest->experienceReward = totalEffort;
    quest->goldReward = totalEffort / 2;
}
```

### 3. Story Integration
```c
// Quests that change the world
void onQuestComplete(Quest* quest, GameWorld* world) {
    switch(quest->id) {
        case QUEST_SAVE_VILLAGE:
            // Village is now safe
            world->villageState = VILLAGE_SAFE;
            removeAllGoblins(world);
            spawnFriendlyGuards(world);
            break;
            
        case QUEST_OPEN_BRIDGE:
            // New area accessible
            world->bridgeOpen = true;
            setTile(world, bridgeX, bridgeY, TILE_BRIDGE);
            break;
    }
}
```

## Complete Quest Example

Here's a full quest implementation:

```c
// questgiver.c - NPC that gives quests
void handleQuestGiver(NPC* npc, Player* player, QuestManager* qm) {
    // Find quests this NPC gives
    Quest* availableQuest = NULL;
    
    for (int i = 0; i < qm->questCount; i++) {
        Quest* quest = qm->quests[i];
        if (quest->giverNPCId == npc->id && 
            quest->state == QUEST_NOT_STARTED &&
            canStartQuest(qm, quest, player->level)) {
            availableQuest = quest;
            break;
        }
    }
    
    // Check if turning in quest
    Quest* completableQuest = NULL;
    for (int i = 0; i < qm->activeCount; i++) {
        Quest* quest = qm->activeQuests[i];
        if (quest->turnInNPCId == npc->id &&
            quest->state == QUEST_COMPLETE) {
            completableQuest = quest;
            break;
        }
    }
    
    // Show appropriate dialogue
    if (completableQuest) {
        showDialogue(npc, "Excellent work! Here's your reward.");
        turnInQuest(qm, completableQuest, player);
    }
    else if (availableQuest) {
        showDialogue(npc, availableQuest->description);
        
        if (showYesNoPrompt("Accept quest?")) {
            startQuest(qm, availableQuest);
        }
    }
    else {
        showDialogue(npc, "Come back when you're stronger!");
    }
}
```

## Practice Exercises

### "Try This" Challenges

1.  **Branching Quest Path:** Create a quest with a `OBJECTIVE_KILL` and an `OBJECTIVE_TALK` objective. The player can either kill the "Bandit Leader" OR talk to the "Shady Informant" to complete the quest. How would you modify the `isQuestComplete` function to handle this "A or B" logic?
    *   *Hint:* You could add a `requiredObjectives` field to the `Quest` struct. For this quest, `objectiveCount` would be 2, but `requiredObjectives` would be 1.

2.  **Dynamic Quest Description:** Modify the `drawQuestLog` function. When it displays an objective's description, it should dynamically insert the current progress. Instead of just "Defeat 5 goblins", it should display "Defeat goblins (2/5)".
    *   *Hint:* You'll need to use `TextFormat()` or `sprintf()` to build the string before drawing it.

3.  **Failed Quests:** Add a `timeLimit` (in seconds) to the `Quest` struct. In your main game loop, for each active quest, you'll need to count down the timer. If a quest's timer reaches zero before it's complete, it should be moved to a new `QUEST_FAILED` state, and the player should be notified.

## Summary

You've learned to create:
- Flexible quest structures
- Progress tracking systems
- Quest UI elements
- Reward systems
- Character progression
- Dynamic quest generation

Good quests give players purpose and make progression meaningful!

## Final Project

You now have all the tools to create a complete ASCII RPG! The next lesson will guide you through assembling everything into a polished game.

Continue to [Lesson 22: Game States and Menus](22-game-states.md)!

## Quick Reference

```c
// Quest flow
1. NPC offers quest
2. Player accepts (adds to active)
3. Player completes objectives
4. Progress updates automatically
5. Quest marked complete
6. Player returns to NPC
7. Receives rewards
8. Quest marked turned in

// Progression formula
Experience = BaseAmount * QuestLevel * DifficultyMultiplier
NextLevel = CurrentLevel * 100 * (CurrentLevel + 1) / 2
```

Remember: Great quests tell stories while teaching game mechanics!