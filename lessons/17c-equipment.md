# Lesson 17c: Equipping Gear – Turning Items Into Stats

Our inventory can now store items, but wielding a sword or wearing armor needs a separate system: **equipment slots** that affect the player's numbers.

## The "Why": Separating "What You Have" from "What You Use"

An inventory holds everything you're carrying. An **equipment** system manages what you're actively *using*. This separation is crucial because equipped items directly modify your character's stats, like damage and defense.

By the end of this lesson, you will have:
- An `Equipment` struct with dedicated slots for a weapon and armor.
- A system to calculate a player's final stats based on their equipped gear.
- `equip` and `unequip` functions to manage gear.

---
## 1 Equipment slots
We'll start with just two slots – weapon and armor – to keep the code short.  Expand later if you need helmets, rings, etc.

```c
#include "item.h"

typedef struct {
    const Item *weapon;  // NULL = empty
    const Item *armor;
} Equipment;
```

---
## 2 Player stats refresher
Assume your `Player` struct already has base values:
```c
typedef struct {
    int baseHP;
    int baseDamage;

    Equipment gear;   // new
} Player;
```
We'll compute *effective* stats on the fly. This is a much better design than changing the player's base stats directly. It means that when you unequip an item, its bonus is automatically removed without any extra work.

```c
int player_damage(const Player *p) {
    int dmg = p->baseDamage;
    if (p->gear.weapon) dmg += p->gear.weapon->damageBonus;
    return dmg;
}
```

---
## 3 Equip / unequip helpers
Rules: only items of the right category go in the right slot.

```c
bool equip(Player *p, const Item *it) {
    if (it->type & ITEM_WEAPON) {
        p->gear.weapon = it;
        return true;
    }
    if (it->type & ITEM_ARMOR) {
        p->gear.armor = it;
        return true;
    }
    return false; // not equipable
}

void unequip_weapon(Player *p) { p->gear.weapon = NULL; }
```
In a real project you'd push removed items back into the inventory; exercise below.

---
## 4 Quick text display
```c
void equipment_draw(const Equipment *e, int x, int y) {
    DrawText("Weapon:", x, y, 20, WHITE);
    DrawText(e->weapon ? e->weapon->name : "--", x + 100, y, 20, LIGHTGRAY);

    DrawText("Armor:",  x, y+20, 20, WHITE);
    DrawText(e->armor ? e->armor->name  : "--", x + 100, y+20, 20, LIGHTGRAY);
}
```

---
## 5 Wire it up in the game loop (pseudo)
```c
if (IsKeyPressed(KEY_E)) {
    int idx = selectedInventorySlot();
    const Item *it = inv.slots[idx].item;
    if (it) equip(&player, it);
}
```
Pick any binding scheme that suits your project.

---
## 6. "Try This" Challenges

1.  **Full Stat Calculation:** Create a `player_defense(const Player* p)` function that works like the `player_damage` example. It should add the `defenseBonus` from the equipped armor to the player's base defense.

2.  **Unequip to Inventory:** The current `unequip_weapon` function just makes the item disappear! Modify it to `bool unequip_weapon(Player* p, Inventory* inv)`. It should take the weapon from the equipment slot and try to add it back to the player's main inventory using the `inv_add` function from the previous lesson. It should return `true` on success and `false` if there's no room in the inventory.

3.  **Smart Equip:** The current `equip` function overwrites whatever is in the slot. Modify it to `bool equip(Player* p, Inventory* inv, const Item* newItem)`. Before equipping the `newItem`, it should check if a weapon is already equipped. If so, it should try to `unequip` the old weapon first (moving it to the inventory). If the unequip fails (because the inventory is full), the new item cannot be equipped, and the function should return `false`.

---
## Commit
```bash
git add lessons/17c-equipment.md
git commit -m "Add Lesson 17c: equipment system"
```
Next, we'll populate the world with characters to talk to in [Lesson 18: NPCs and Dialogue](18-npcs-dialogue.md).