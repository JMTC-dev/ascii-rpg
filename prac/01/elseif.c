#include <stdio.h>

int main() {
  int playerLevel = 5;
  int enemyLevel = 8;

  if (enemyLevel > playerLevel + 3) {
    printf("Enemy is too strong! Run!\n");
  } else if (enemyLevel > playerLevel) {
    printf("This will be challenging.\n");
  } else if (enemyLevel == playerLevel) {
    printf("Evenly matched!\n");
  } else {
    printf("You should win easily.\n");
  }

  return 0;
}
