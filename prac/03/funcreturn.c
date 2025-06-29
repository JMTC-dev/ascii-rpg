#include <stdio.h>

int calculateAttackDamage(int strength) {
  int damage = strength * 2;
  return damage;
}

int main() {
  int playerStrength = 15;
  int damageDealt = calculateAttackDamage(playerStrength);

  printf("You attack for %d damage!\n", damageDealt);

  return 0;
}
