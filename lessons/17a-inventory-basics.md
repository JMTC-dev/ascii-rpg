# Lesson 17a: Inventory Basics – Items & Core Concepts

Loot is one of the easiest ways to make progress visible.  In this first inventory chapter we'll set up *what* an item is.  Next chapter we'll worry about where to put those items.

You'll leave with:
• A lightweight `Item` struct that covers weapons, potions and keys.  
• A clear idea of how bit-flags let one thing belong to several categories.  
• A tiny fixed-size item database we can grow later.

---
## 1. The "Why": What Makes Loot Fun?
Loot is the engine of progression in most RPGs. It's the tangible reward for overcoming challenges. A good item system provides:
- **Visible Progression:** Finding a "+2 Sword" feels much more concrete than just seeing a stat number go up.
- **Meaningful Choices:** Should I use the fast dagger or the slow, heavy axe? Should I sell this rare item or save it for a special occasion?
- **Player Expression:** The gear a player chooses reflects their preferred playstyle.
- **A Reason to Explore:** The desire to find what's in the next chest is a powerful motivator.

---
## 2 Common inventory models
Pattern | Games that use it | Good | Not so good
------- | ---------------- | ---- | ------------
Grid    | Diablo, Resident Evil | Feels tactile, mini-gamey | UI heavy
Slot    | Pokémon, JRPGs       | Simple to teach | Hard caps can feel arbitrary
Weight  | Skyrim               | Realistic           | Math overhead

We'll start with the **slot** model because it's simple, clear, and easy to represent in our ASCII interface.

---
## 3 The `Item` type
Every item needs at least a name, a symbol, and some way to tell what kind of thing it is.  Here's a first pass:

```c
#include <stdbool.h>
#include "raylib.h"   // only for Color

// Why <stdbool.h>?  C is older than the 'bool' keyword.  This header adds:
//      bool   -> alias for _Bool
//      true   -> 1
//      false  -> 0
// Makes conditions read like modern languages.

// We don't use `bool` in this first snippet, but the next lesson adds fields like
// `bool equipped` and helper functions that return `true`/`false`. Including it
// here avoids forgetting later.

// One item can be more than one category, so we store categories as BITS
typedef enum {
    ITEM_WEAPON      = 1 << 0,
    ITEM_ARMOR       = 1 << 1,
    ITEM_CONSUMABLE  = 1 << 2,
    ITEM_KEY         = 1 << 3,
    ITEM_MAGICAL     = 1 << 4
} ItemType;

// Rarity is a single choice – no bit-flags needed
typedef enum { RARITY_COMMON, RARITY_RARE, RARITY_EPIC } ItemRarity;

typedef struct {
    int        id;      // unique number for save files
    char       name[32];
    char       symbol;  // what shows up in the map – '!' for potions, '/' for swords
    Color      color;   // colour in ASCII renderer

    ItemType   type;    // weapon? consumable? maybe both → bit-flags
    ItemRarity rarity;  // controls drop rates and colour tint

    int        value;   // shop price in gold
    
    // Equipment stats
    int        damageBonus;
    int        defenseBonus;
} Item;
```

### Bit-flags in practice
Numbers are just bits.  Setting multiple bits lets one item belong to more than one category.

```
ITEM_WEAPON   00001
ITEM_MAGICAL  10000
Combined      10001   // magic sword
```

Test a flag with `&`:
```c
if (item.type & ITEM_CONSUMABLE) {
    // potions go here
}
```

**How does `1 << n` work?**  Each left-shift moves the single `1` bit one place to the left, giving us powers of two:

Shift | Binary        | Decimal
----- | ------------- | -------
0     | `0000 0001`   | 1
1     | `0000 0010`   | 2
2     | `0000 0100`   | 4
3     | `0000 1000`   | 8
4     | `0001 0000`   | 16

Because every flag owns a unique bit we can combine them with the **OR** operator and still separate them later.  Example:  `ITEM_WEAPON | ITEM_MAGICAL` → `0001 | 10000` = `10001` (decimal 17).

---
## 4 A micro item database
We only need a fixed array for now:
```c
#define MAX_ITEMS 32
static Item g_items[MAX_ITEMS];
static int  g_itemCount = 0;

void addItem(Item it) {
    if (g_itemCount < MAX_ITEMS)
        g_items[g_itemCount++] = it;
}
```
Example use:
```c
addItem((Item){ .id=1,
                .name="Health Potion",
                .symbol='!',
                .color=RED,
                .type=ITEM_CONSUMABLE,
                .rarity=RARITY_COMMON,
                .value=25 });
```
If we ever need more than 32 items we'll swap in a dynamic array or vector.

*Why not malloc a bigger array right away?*  Two reasons:
1. A fixed array keeps the code short while we focus on concepts.
2. You won't hit 32 unique items until later lessons.

When we outgrow this, we'll switch to a **dynamic array** (sometimes called a *vector*):
- Allocate memory with `malloc` for, say, 64 items.
- If that fills up, call `realloc` to grow to 128, 256, …
- Store the current `capacity` alongside the `count` so you know when to grow.

We'll walk through that upgrade in Lesson 18 when the number of dialogue lines explodes.

---
## 5. "Try This" Challenges

1.  **Define More Items:** Using the `addItem` function, define the following items and add them to the database:
    *   An `Iron Sword` (`id=2`, symbol `'/'`, type `ITEM_WEAPON`, rarity `RARITY_COMMON`, value `50`, `damageBonus` `5`).
    *   A `Steel Shield` (`id=3`, symbol `']'`, type `ITEM_ARMOR`, rarity `RARITY_UNCOMMON`, value `120`, `defenseBonus` `8`).
    *   A `Goblin Key` (`id=4`, symbol `'k'`, type `ITEM_KEY`, rarity `RARITY_COMMON`, value `0`).

2.  **Filter by Type:** Write a function `void printItemsOfType(ItemType type)` that loops through the global `g_items` array and prints the name of every item that has the specified `type` flag. Test it by printing all `ITEM_WEAPON` items.
    *   *Hint:* You'll use the `&` bitwise operator: `if (g_items[i].type & type) { ... }`.

3.  **Find by ID:** Write a function `const Item* findItemByID(int id)` that searches the database and returns a *pointer* to the item with the matching ID. If no item is found, it should return `NULL`. Test it in `main` by finding the "Iron Sword" and printing its stats.

---
Commit when you're happy:
```bash
git add lessons/17a-inventory-basics.md
git commit -m "Rewrite 17a with clearer walkthrough"
```

Head to [Lesson 17b: Inventory Containers](17b-inventory-containers.md) to actually store items in the player's bag.