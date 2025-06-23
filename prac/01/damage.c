#include <stdio.h>

int main() {
  int playerAttack = 10;
  int playerHealth = 100;
  int enemyDefense = 11;
  int enemyHealth = 100;
  int damage = playerAttack - enemyDefense;

  printf("=== BATTLE START ===\n");
  printf("Player's Health: %d\n", playerHealth);
  printf("Player's Attack: %d\n", playerAttack);
  printf("Enemy's Health: %d\n", enemyHealth);
  printf("Enemy's Defense: %d\n\n", enemyDefense);

  if (damage > 0) {
    printf("You did %d damage to the enemy!\n\n", damage);
    enemyHealth -= damage;
  } else {
    printf("You don't do enough damage to defeat the enemy defense!\n\n");
  }

  printf("Player's Health: %d\n", playerHealth);
  printf("Player's Attack: %d\n", playerAttack);
  printf("Enemy's Health: %d\n", enemyHealth);
  printf("Enemy's Defense: %d\n", enemyDefense);

  return 0;
}
