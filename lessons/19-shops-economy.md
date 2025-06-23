# Lesson 19: Shops and Economy

Every RPG needs a way for players to spend their hard-earned gold! Let's create a shop system that feels rewarding and teaches important game economy concepts.

## The "Why": The Role of Shops and Economy

A game's economy is the system that governs how players earn and spend resources. A well-designed economy is crucial for player motivation. Shops are the primary interface for this system. They serve several key purposes:
- **Provide a Goal:** Earning gold is meaningless unless there's something desirable to spend it on. Shops provide that goal.
- **Drive Gameplay Loops:** The core loop of "Defeat Enemies -> Get Gold -> Buy Better Gear -> Defeat Stronger Enemies" is a powerful motivator.
- **Offer Strategic Choices:** Can I afford the powerful sword now, or should I buy several cheaper potions? These decisions make the game engaging.
- **Control Pacing:** Shops act as safe havens and natural breaks between exploration and combat.

## Starting Simple: A Basic Shop

Let's build our shop system step by step.

### Step 1: What Can We Buy?

First, let's define shop items:

```c
typedef struct {
    char* name;
    char symbol;
    int price;
    int value;  // How much it helps (damage for weapons, healing for potions)
    char* description;
} ShopItem;

// Create some items to sell
ShopItem createPotion() {
    ShopItem item;
    item.name = "Health Potion";
    item.symbol = '!';
    item.price = 50;
    item.value = 25;  // Heals 25 HP
    item.description = "Restores 25 health points";
    return item;
}

ShopItem createSword() {
    ShopItem item;
    item.name = "Iron Sword";
    item.symbol = '/';
    item.price = 100;
    item.value = 10;  // +10 attack
    item.description = "A sturdy iron blade (+10 ATK)";
    return item;
}
```

### Understanding the Design:
- Each item has a **price** (what it costs)
- Each item has a **value** (what it does)
- Descriptions help players decide what to buy

## Step 2: The Shop Interface

Now let's create a simple text-based shop:

```c
void displayShop(ShopItem* items, int itemCount, int playerGold) {
    printf("\n=== WELCOME TO THE SHOP ===\n");
    printf("Your gold: %d\n\n", playerGold);
    
    for (int i = 0; i < itemCount; i++) {
        printf("%d. %s - %d gold\n", i + 1, items[i].name, items[i].price);
        printf("   %s\n", items[i].description);
        
        // Show if player can afford it
        if (playerGold >= items[i].price) {
            printf("   [Can afford]\n");
        } else {
            printf("   [Too expensive - need %d more gold]\n", 
                   items[i].price - playerGold);
        }
        printf("\n");
    }
}
```

### Why This Works:
- Shows all available items
- Clearly displays prices
- Tells player if they can afford each item
- Provides helpful descriptions

## Step 3: Making Purchases

Let's add the ability to buy items:

```c
// Simple purchase system
int makePurchase(ShopItem* item, int* playerGold) {
    if (*playerGold >= item->price) {
        *playerGold -= item->price;
        printf("You bought %s!\n", item->name);
        return 1;  // Success
    } else {
        printf("Not enough gold! You need %d more.\n", 
               item->price - *playerGold);
        return 0;  // Failed
    }
}

// Example usage
int main() {
    int playerGold = 75;
    ShopItem potion = createPotion();
    ShopItem sword = createSword();
    
    ShopItem shopItems[2] = {potion, sword};
    
    displayShop(shopItems, 2, playerGold);
    
    // Try to buy something
    printf("Attempting to buy potion...\n");
    if (makePurchase(&shopItems[0], &playerGold)) {
        printf("Gold remaining: %d\n", playerGold);
    }
    
    return 0;
}
```

## Step 4: Integrating with Inventory

Shops need to work with your inventory system:

```c
typedef struct {
    ShopItem* items;
    int* quantities;
    int capacity;
    int count;
} Inventory;

// Add item to inventory after purchase
void addToInventory(Inventory* inv, ShopItem item) {
    // Check if we already have this item
    for (int i = 0; i < inv->count; i++) {
        if (strcmp(inv->items[i].name, item.name) == 0) {
            inv->quantities[i]++;
            return;
        }
    }
    
    // New item - add if space available
    if (inv->count < inv->capacity) {
        inv->items[inv->count] = item;
        inv->quantities[inv->count] = 1;
        inv->count++;
    } else {
        printf("Inventory full!\n");
    }
}
```

## Step 5: Shop in Raylib

Let's create a visual shop for our ASCII RPG:

```c
typedef struct {
    ShopItem* items;
    int itemCount;
    int selectedItem;
    bool isOpen;
} Shop;

void drawShop(Shop* shop, int playerGold) {
    if (!shop->isOpen) return;
    
    // Draw shop window
    int shopX = 100;
    int shopY = 50;
    int shopWidth = 600;
    int shopHeight = 500;
    
    // Background
    DrawRectangle(shopX, shopY, shopWidth, shopHeight, DARKGRAY);
    DrawRectangleLines(shopX, shopY, shopWidth, shopHeight, WHITE);
    
    // Title
    DrawText("SHOP", shopX + 250, shopY + 20, 40, WHITE);
    
    // Player gold
    char goldText[50];
    sprintf(goldText, "Your Gold: %d", playerGold);
    DrawText(goldText, shopX + 20, shopY + 70, 20, GOLD);
    
    // Items
    for (int i = 0; i < shop->itemCount; i++) {
        int itemY = shopY + 120 + (i * 80);
        Color textColor = WHITE;
        
        // Highlight selected item
        if (i == shop->selectedItem) {
            DrawRectangle(shopX + 10, itemY - 5, shopWidth - 20, 70, DARKGREEN);
        }
        
        // Can afford check
        if (playerGold < shop->items[i].price) {
            textColor = GRAY;
        }
        
        // Item info
        char itemText[100];
        sprintf(itemText, "%c %s - %d gold", 
                shop->items[i].symbol, 
                shop->items[i].name, 
                shop->items[i].price);
        DrawText(itemText, shopX + 20, itemY, 24, textColor);
        
        // Description
        DrawText(shop->items[i].description, shopX + 20, itemY + 30, 18, textColor);
    }
    
    // Instructions
    DrawText("UP/DOWN: Select | ENTER: Buy | ESC: Leave", 
             shopX + 20, shopY + shopHeight - 40, 18, WHITE);
}

void updateShop(Shop* shop, int* playerGold, Inventory* playerInv) {
    if (!shop->isOpen) return;
    
    // Navigation
    if (IsKeyPressed(KEY_UP)) {
        shop->selectedItem--;
        if (shop->selectedItem < 0) {
            shop->selectedItem = shop->itemCount - 1;
        }
    }
    if (IsKeyPressed(KEY_DOWN)) {
        shop->selectedItem++;
        if (shop->selectedItem >= shop->itemCount) {
            shop->selectedItem = 0;
        }
    }
    
    // Purchase
    if (IsKeyPressed(KEY_ENTER)) {
        ShopItem* item = &shop->items[shop->selectedItem];
        if (*playerGold >= item->price) {
            *playerGold -= item->price;
            addToInventory(playerInv, *item);
            // Could play purchase sound here
        }
    }
    
    // Exit shop
    if (IsKeyPressed(KEY_ESCAPE)) {
        shop->isOpen = false;
    }
}
```

## Game Economy Basics

Good game economies balance several factors:

### 1. Income Sources
```c
// Different ways to earn gold
enum IncomeSource {
    INCOME_ENEMY_DROP,      // Defeating enemies
    INCOME_TREASURE_CHEST,  // Finding chests
    INCOME_QUEST_REWARD,    // Completing quests
    INCOME_ITEM_SALE       // Selling items back
};

// Example drop system
int getEnemyGoldDrop(char enemyType) {
    switch(enemyType) {
        case 'g': return 5 + rand() % 6;   // Goblin: 5-10 gold
        case 'o': return 10 + rand() % 11; // Orc: 10-20 gold
        case 'D': return 50 + rand() % 51; // Dragon: 50-100 gold
        default: return 0;
    }
}
```

### 2. Price Scaling
```c
// Prices should match player progression
int getItemPrice(int itemLevel, int basePrice) {
    return basePrice * (1 + itemLevel * 0.5);
    // Level 0: 100 gold
    // Level 1: 150 gold
    // Level 2: 200 gold
}
```

### 3. Money Sinks
Things for players to spend excess gold on:
```c
typedef enum {
    PURCHASE_CONSUMABLE,  // One-time use
    PURCHASE_EQUIPMENT,   // Permanent upgrade
    PURCHASE_COSMETIC,    // Visual only
    PURCHASE_SERVICE      // Inn rest, fast travel
} PurchaseType;
```

## Special Shop Types

### 1. The Traveling Merchant
```c
typedef struct {
    Shop baseShop;
    int daysUntilMove;
    bool hasRareItem;
} TravelingMerchant;

// Appears randomly with special items
void updateTravelingMerchant(TravelingMerchant* merchant, int currentDay) {
    merchant->daysUntilMove--;
    if (merchant->daysUntilMove <= 0) {
        // Move to new location
        merchant->daysUntilMove = 3 + rand() % 5;
        // Chance for rare item
        merchant->hasRareItem = (rand() % 100) < 20;
    }
}
```

### 2. The Blacksmith
```c
// Upgrade existing items instead of buying new ones
typedef struct {
    int upgradeLevel;
    int upgradeCost;
} BlacksmithService;

int calculateUpgradeCost(int currentLevel) {
    return 50 * (currentLevel + 1) * (currentLevel + 1);
    // Level 0->1: 50 gold
    // Level 1->2: 200 gold
    // Level 2->3: 450 gold
}
```

## Balancing Your Economy

### Testing Questions:
1. Can players afford their first upgrade after 30 minutes?
2. Do players have meaningful choices about what to buy?
3. Is there always something to save up for?
4. Are consumables worth their price?

### Common Problems and Solutions:

**Problem**: Players get too rich too fast
**Solution**: Add more expensive items or services

**Problem**: Everything is too expensive
**Solution**: Increase gold drops or add more income sources

**Problem**: Nothing worth buying
**Solution**: Make items more impactful or add variety

## Complete Shop Example

Here's a simple but complete shop system:

```c
// shopkeeper.c - A friendly shop NPC
typedef struct {
    int x, y;
    char symbol;
    Shop shop;
    char* greeting;
} Shopkeeper;

Shopkeeper createShopkeeper(int x, int y) {
    Shopkeeper keeper;
    keeper.x = x;
    keeper.y = y;
    keeper.symbol = '$';
    keeper.greeting = "Welcome, adventurer! Care to see my wares?";
    
    // Stock the shop
    keeper.shop.itemCount = 3;
    keeper.shop.items = malloc(sizeof(ShopItem) * 3);
    keeper.shop.items[0] = createPotion();
    keeper.shop.items[1] = createSword();
    keeper.shop.items[2] = createArmor();  // You'd implement this
    keeper.shop.selectedItem = 0;
    keeper.shop.isOpen = false;
    
    return keeper;
}

// Check if player is near shopkeeper
void checkShopInteraction(Player* player, Shopkeeper* keeper) {
    int dx = abs(player->x - keeper->x);
    int dy = abs(player->y - keeper->y);
    
    // Adjacent to shopkeeper?
    if (dx <= 1 && dy <= 1) {
        DrawText("Press E to shop", 10, 550, 20, WHITE);
        
        if (IsKeyPressed(KEY_E)) {
            keeper->shop.isOpen = true;
        }
    }
}
```

## "Try This" Challenges

1.  **Buyback System:** Implement a "Sell" tab in the shop UI. When the player is in sell mode, it should display their inventory. When they select an item, it should be sold to the shop for 50% of its `price`, adding gold to the player and removing the item from their inventory.

2.  **Limited Stock:** Modify the `ShopItem` struct to include a `quantity` field. When an item is purchased, decrease its quantity. If the quantity reaches zero, the item should still be displayed but be grayed out and unpurchasable. This creates scarcity and makes item choices more meaningful.

3.  **Reputation Pricing:** Add a `reputation` variable to your player struct (from -100 to 100). In the `updateShop` function, modify the item prices based on reputation. High reputation could give a 10% discount, while low reputation could add a 20% markup. This makes the player's actions outside of shopping affect the economy.

## Summary

You've learned to create:
- Item definitions with prices and effects
- Shop interfaces (text and graphical)
- Purchase systems with inventory integration
- Game economy balancing
- Special shop variations

A good shop system makes collecting gold exciting and provides meaningful player choices!

## Next Steps

With shops complete, let's create our final checkpoint project that combines everything!

Continue to [Lesson 20: Checkpoint - Mini RPG](20-checkpoint-mini-rpg.md)!

## Quick Reference

```c
// Basic shop flow
1. Display items with prices
2. Check player gold
3. Handle selection
4. Process purchase
5. Update inventory
6. Deduct gold

// Economy balance
- Early game: 5-10 gold per enemy
- Mid game: 20-50 gold per enemy  
- Late game: 100+ gold per enemy
- Consumables: 20-30% of enemy drops
- Equipment: 10-20 enemies worth of gold
```

Remember: Good economies give players interesting choices!