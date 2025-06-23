#include <stdio.h>

int main() {
  int playerHealth = 100;
  int goblinDamage = 12;
  int skeletonDamage = 18;
  int potionHeal = 20;

  printf("Player's Health: %d\n", playerHealth);
  // Goblin hits player for 12 damage
  playerHealth -= goblinDamage;
  printf("Goblin hits player for %d damage\n", goblinDamage);
  printf("Player's Health: %d\n", playerHealth);

  // Skeleton hits player for 18 damage
  playerHealth -= skeletonDamage;
  printf("Skeleton hits player for %d damage\n", skeletonDamage);
  printf("Player's Health: %d\n", playerHealth);

  // Player finds potion and heals for 20 health
  playerHealth += potionHeal;
  printf("Player finds & drinks small potion and heals for %d health\n",
         potionHeal);
  printf("Player's Health: %d\n", playerHealth);

  return 0;
}
