# Lesson 18: NPCs and Dialogue - Giving Your World a Voice

A world filled with only monsters and loot can feel lifeless. NPCs (Non-Player Characters) are the heart of an RPG world, providing story, services, and a reason for the player's adventure. In this lesson, we'll build a flexible system for creating characters and allowing the player to have conversations with them.

## Learning Goals

- Understand the role of NPCs in game design.
- Design a data structure to represent NPCs and their dialogue.
- Implement a stateful UI for displaying conversations (a dialogue box).
- Create a system for triggering and managing dialogue interactions.
- Write exercises that challenge you to expand the system.

---

## 1. The "Why": What Makes a Good NPC?

Before we write any code, let's think about the goal. Good NPCs:
- **Deliver Narrative:** They are your primary storytellers.
- **Provide Services:** They can be merchants, healers, or quest-givers.
- **Create Atmosphere:** A bustling town feels alive because of its inhabitants.
- **Offer Choices:** Dialogue can lead to different outcomes, making the player's decisions feel important.

Our system needs to be flexible enough to support all these roles.

---

## 2. The "What": Designing the Data Structures

We need to represent an NPC and the dialogue UI.

### The NPC Struct

An NPC is an entity, like the player or an enemy, but with a focus on interaction.

```c
// It's good practice to put this in a new file, like `npc.h`
#ifndef NPC_H
#define NPC_H

#include "raylib.h"

typedef struct NPC {
    int x;
    int y;
    char symbol;
    Color color;
    const char* name;
    
    // Dialogue Data
    const char** dialogueLines; // An array of strings for dialogue
    int lineCount;          // How many lines of dialogue this NPC has
    int currentLine;        // Which line they are currently on
    
    // In a larger game, you might add:
    // - Quest IDs they are associated with
    // - A pointer to their shop inventory
    // - A behavior state (e.g., wandering, standing still)
} NPC;

#endif
```
**Why `const char** dialogueLines`?** This is a pointer to an array of strings. It's a memory-efficient way to associate a list of text with an NPC without copying all the strings into the NPC struct itself.

### The DialogueBox Struct

We need a separate struct to manage the state of the UI. This is a key design principle: **separate game data (the NPC) from UI state (the dialogue box)**.

```c
// This could go in a `ui_dialogue.h` file
#include <stdbool.h>

typedef struct {
    bool isActive;           // Is the dialogue box currently on screen?
    const char* speakerName; // Whose name to display
    const char* currentText; // The full line of text to display
    
    // For the "typewriter" effect
    int charIndex;           // How many characters have been revealed
    float charTimer;         // Timer to control speed of reveal
} DialogueBox;
```

---

## 3. The "How": Implementing the System

Let's build the functions to make this work, step-by-step.

### Step 3.1: Starting a Conversation

This function will be called when the player interacts with an NPC. It configures the dialogue box and makes it active.

```c
// In `ui_dialogue.c`
void StartDialogue(DialogueBox* box, NPC* npc) {
    // Don't start if the NPC has nothing to say
    if (npc == NULL || npc->lineCount == 0) {
        return;
    }

    box->isActive = true;
    box->speakerName = npc->name;
    box->currentText = npc->dialogueLines[npc->currentLine];
    
    // Reset the typewriter effect
    box->charIndex = 0;
    box->charTimer = 0.0f;

    // Advance to the NPC's next line for the next time we talk to them.
    // The modulo operator (%) makes it wrap around to the start.
    npc->currentLine = (npc->currentLine + 1) % npc->lineCount;
}
```

### Step 3.2: Updating and Drawing the UI

The dialogue box needs to be updated every frame to create the typewriter effect and handle input.

```c
// In `ui_dialogue.c`

void UpdateDialogueBox(DialogueBox* box) {
    if (!box->isActive) return;

    // If player presses key, either finish the line or close the box
    if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_SPACE)) {
        // Is the text still typing out?
        if (box->charIndex < strlen(box->currentText)) {
            // If so, instantly reveal the whole line
            box->charIndex = strlen(box->currentText);
        } else {
            // If text is already finished, close the box
            box->isActive = false;
        }
        return; // Stop further processing this frame
    }

    // Typewriter effect logic
    if (box->charIndex < strlen(box->currentText)) {
        box->charTimer += GetFrameTime();
        if (box->charTimer >= 0.04f) { // Adjust this value for faster/slower text
            box->charIndex++;
            box->charTimer = 0.0f;
        }
    }
}

void DrawDialogueBox(const DialogueBox* box) {
    if (!box->isActive) return;

    // A dark, semi-transparent box at the bottom of the screen
    int boxX = 50;
    int boxY = GetScreenHeight() - 150;
    int boxWidth = GetScreenWidth() - 100;
    int boxHeight = 100;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(BLACK, 0.8f));
    DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, WHITE);

    // Speaker's Name
    DrawText(box->speakerName, boxX + 20, boxY + 15, 20, YELLOW);

    // The dialogue text with typewriter effect
    // We create a temporary substring to draw only the revealed characters
    char textToShow[1024];
    strncpy(textToShow, box->currentText, box->charIndex);
    textToShow[box->charIndex] = '\0'; // IMPORTANT: Always null-terminate a C string!

    DrawText(textToShow, boxX + 20, boxY + 50, 18, WHITE);
}
```

### Step 3.3: Triggering the Interaction

In your main game loop, you need to check if the player is trying to talk to a nearby NPC.

```c
// In your main game update function

// Only process game world input if no UI is active
if (!dialogueBox.isActive) {
    // ... handle player movement ...

    // Check for interaction
    if (IsKeyPressed(KEY_E)) {
        // Find the NPC the player is facing or is next to
        NPC* targetNPC = FindAdjacentNPC(player, all_npcs); 
        if (targetNPC != NULL) {
            StartDialogue(&dialogueBox, targetNPC);
        }
    }
}

// Update UI systems AFTER game world logic
UpdateDialogueBox(&dialogueBox);

// In your main drawing section
BeginDrawing();
    // ... draw world, player, etc. ...
    
    // Draw UI elements last so they appear on top
    DrawDialogueBox(&dialogueBox);
EndDrawing();
```
The `FindAdjacentNPC()` function is something you would write. It would loop through all NPCs and check if any are at a position adjacent to the player.

---

## 4. Exercises and Critical Thinking

Simply copying code doesn't teach you how to solve problems. Try to implement these features yourself to truly understand the system.

1.  **Interaction Indicator:** How would you draw a "!" or a speech bubble above an NPC's head when the player is close enough to talk to them?
    *   *Hint:* You'll need to check the distance between the player and the NPC every frame in your main update loop.

2.  **Branching Dialogue:** Modify the `DialogueBox` and `NPC` structs to support choices. For example: "Do you accept the quest? (Y/N)".
    *   *Hint:* The `DialogueBox` will need an array of strings for the choices and an integer to track the `selectedChoice`. You'll need to handle `KEY_UP`/`KEY_DOWN` input when the dialogue is finished typing.

3.  **Contextual Dialogue:** Change the `StartDialogue` logic so that an NPC says one thing the first time you meet them, and something else on subsequent conversations.
    *   *Hint:* Add a `bool hasBeenMet;` flag to your `NPC` struct.

4.  **Preventing Movement:** When the dialogue box is active, the player shouldn't be able to walk around. How would you implement this?
    *   *Hint:* Look at the `if (!dialogueBox.isActive)` block in the "Triggering the Interaction" example. All your player movement logic should be inside that block.

---

## Next Steps

With NPCs who can talk, the next logical step is to give them a purpose. In the next lesson, we'll create a shop system so you can buy and sell items, turning your NPCs into merchants.

Continue to [Lesson 19: Shops and Economy](19-shops-economy.md).