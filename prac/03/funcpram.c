#include <stdio.h>

void spawnEnemy(int health, int level) {
  printf("Spawning level %d enemy with %d HP.\n", level, health);
}

int main() {
  spawnEnemy(100, 5);
  spawnEnemy(25, 1);
  return 0;
}
