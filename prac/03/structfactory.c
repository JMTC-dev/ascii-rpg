#include <stdio.h>

typedef struct {
  int healAmount;
  int goldCost;
} Potion;

int main() {
  Potion smallPotion = {20, 50};
  Potion largePotion = {50, 120};

  printf("Potions for Sale:\n");
  printf("Small Potion: Heals %d, Costs %d gold.\n", smallPotion.healAmount,
         smallPotion.goldCost);
  printf("Large Potion: Heals %d, Costs %d gold.\n", largePotion.healAmount,
         largePotion.goldCost);

  return 0;
}
