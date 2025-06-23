# Lesson 17b: Putting Items Somewhere – Inventory Containers

In 17a we defined what an `Item` is.  Now we need a **place** to hold them: a bag, a hot-bar, maybe equipment slots.  We'll keep the data structures small so you understand every line.

By the end you will have:
• A `Slot` struct that can be empty or hold an `Item` pointer and a stack count.  
• A fixed-size `Inventory` with add / remove helpers.  
• A quick ASCII render you can drop into the game loop.

---
## 1. The "Slot": An Atom of Inventory

An inventory is made of slots. A slot is a container that can hold an item. It needs to know two things:
1.  *What* item it holds.
2.  *How many* of that item it holds.

A slot can also be empty.

```c
#include "item.h"   // where the Item struct lives

#define MAX_STACK  99   // hard limit for consumables

typedef struct {
    const Item *item;  // NULL means empty
    int         qty;   // 1 for equipment, >1 for potions, 0 if empty
} Slot;
```
**Why a `const Item*` pointer?** This is a crucial design choice. We are not copying the entire `Item` object into the slot. Instead, we are just storing a *pointer* (a memory address) to the single, master copy of that item in our item database. This is much more memory-efficient, especially if you have thousands of items. The `const` keyword is a promise that we won't try to change the item's template through this slot.

---
## 2 Inventory structure
We'll start with ten slots—five quick-bar slots and five backpack slots:

```c
#define INV_SIZE 10

typedef struct {
    Slot slots[INV_SIZE];
} Inventory;
```
Initialize everything to empty:
```c
void inv_init(Inventory *inv) {
    for (int i = 0; i < INV_SIZE; ++i) {
        inv->slots[i].item = NULL;
        inv->slots[i].qty  = 0;
    }
}
```
---
## 3 Adding an item
Rules we'll use:
1. Try to stack onto an existing stack (same item and stackable).  
2. Otherwise find the first empty slot.  
3. Return `true` if something was added, `false` if the bag is full.

```c
bool inv_add(Inventory *inv, const Item *it, int amount) {
    // 1. stacking
    for (int i = 0; i < INV_SIZE; ++i) {
        Slot *s = &inv->slots[i];
        if (s->item == it && s->qty < MAX_STACK) {
            int space = MAX_STACK - s->qty;
            int add   = (amount < space) ? amount : space;
            s->qty   += add;
            amount   -= add;
            if (amount == 0) return true;
        }
    }
    // 2. empty slot
    for (int i = 0; i < INV_SIZE; ++i) {
        Slot *s = &inv->slots[i];
        if (s->item == NULL) {
            s->item = it;
            s->qty  = amount > MAX_STACK ? MAX_STACK : amount;
            amount -= s->qty;
            if (amount == 0) return true;
        }
    }
    return false; // still items left – bag full
}
```

Removing is the same idea in reverse—left as an exercise.

---
## 4 Simple ASCII render
```c
void inv_draw(const Inventory *inv, int startX, int startY) {
    for (int i = 0; i < INV_SIZE; ++i) {
        const Slot *s = &inv->slots[i];
        int x = startX + (i % 5) * 4;   // 5 columns
        int y = startY + (i / 5) * 2;   // 2 rows
        DrawText("[ ]", x, y, 20, LIGHTGRAY);
        if (s->item) {
            // TextFormat works like printf, but returns a string for Raylib functions
            DrawText(TextFormat("%c", s->item->symbol), x + 8, y, 20, s->item->color);
            if (s->qty > 1) DrawText(TextFormat("%d", s->qty), x + 20, y + 10, 10, WHITE);
        }
    }
}
```
Drop `inv_draw(&playerInv, 10, 10);` inside your `BeginDrawing/EndDrawing` block to see the bag.

---
## 5. "Try This" Challenges

1.  **Implement `inv_remove`:** Write the `bool inv_remove(Inventory* inv, const Item* it, int amount)` function. It should search the inventory for the specified item and remove the given amount. It should be smart enough to remove from multiple stacks if necessary. Return `true` if the full amount was removed, and `false` otherwise.

2.  **Auto-pickup:** In your main game loop, when the player moves to a new tile, check if there's an item on that tile. If there is, try to add it to the player's inventory using `inv_add`. If `inv_add` returns `true`, remove the item from the map. If it returns `false`, print a message like "Inventory full!".

3.  **Hotbar:** The first 5 slots of our inventory can be treated as a hotbar. In your game, if the player presses the '1' key, it should try to use the item in slot 0. '2' should use the item in slot 1, and so on. You'll need to write a `useItem(const Item* it)` function that checks the item's type and performs an action (e.g., prints "Used Health Potion!").

---
## Commit
```bash
git add lessons/17b-inventory-containers.md
git commit -m "Add Lesson 17b: inventory containers and slots"
```
Now the player has pockets; next up is [Lesson 17c: Equipping Gear](17c-equipment.md) (weapon/armor stats) and prettier UI.