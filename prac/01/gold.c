#include <stdio.h>
#include <string.h>

int main() {
  int playerGold = 0;
  char treasureChestName[] = "Randuin's Treasure Chest";
  int treasureChestBounty = 25;
  char potionName[] = "Simple Potion";
  int potionGoldValue = 30;
  char swordName[] = "Steel Sword";
  int swordGoldValue = 100;
  char currencyName[] = "gold";

  printf("Player's Gold: %d %s\n", playerGold, currencyName);

  playerGold += treasureChestBounty;
  printf("You found %s! and inside was %d %s pieces. What a find!\n",
         treasureChestName, treasureChestBounty, currencyName);

  printf("Player's Gold: %d %s\n", playerGold, currencyName);

  playerGold += treasureChestBounty;
  strcpy(treasureChestName, "Vaylorn's Treasure Chest");
  printf("You found %s! and inside was %d %s pieces. What a find!\n",
         treasureChestName, treasureChestBounty, currencyName);

  printf("Player's Gold: %d %s\n", playerGold, currencyName);

  playerGold += treasureChestBounty;
  strcpy(treasureChestName, "Odin's Treasure Chest");
  printf("You found %s! and inside was %d %s pieces. What a find!\n",
         treasureChestName, treasureChestBounty, currencyName);

  printf("Player's Gold: %d %s\n", playerGold, currencyName);

  printf("You head to the shop and see the following items for sale:\n");
  printf("1. %s for %d %s\n", potionName, potionGoldValue, currencyName);
  printf("2. %s for %d %s\n", swordName, swordGoldValue, currencyName);

  if (playerGold >= potionGoldValue) {
    printf("You buy the %s for %d %s\n", potionName, potionGoldValue,
           currencyName);
  } else {
    printf("You do not have enough money to buy the %s!\nYou are missing "
           "%d %s\n",
           potionName, potionGoldValue - playerGold, currencyName);
  }

  printf("Player's Gold: %d %s\n", playerGold, currencyName);

  if (playerGold >= swordGoldValue) {
    printf("You buy the %s for %d %s\n", swordName, swordGoldValue,
           currencyName);
  } else {
    printf(
        "You do not have enough money to buy the %s!\nYou are missing %d %s\n",
        swordName, swordGoldValue - playerGold, currencyName);
  }

  printf("Player's Gold: %d %s\n", playerGold, currencyName);
}
