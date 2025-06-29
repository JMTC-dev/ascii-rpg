#include <stdio.h>

typedef struct {
  int health;
  int maxHealth;
} Monster;

Monster healMonster(Monster m, int amount) {
  m.health += amount;
  if (m.health > m.maxHealth) {
    m.health = m.maxHealth;
  }
  return m;
}

int main() {
  Monster goblin = {50, 100};
  printf("Goblin has %d HP.\n", goblin.health);

  goblin = healMonster(goblin, 30);

  printf("Goblin now has %d HP.\n", goblin.health);
}
