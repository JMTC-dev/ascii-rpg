#include <stdio.h>

int main() {
  int playerHealth = 25;

  if (playerHealth < 50) {
    printf("Warning: Low health!\n");
  }

  if (playerHealth <= 0) {
    printf("Game Over!\n");
  }

  return 0;
}
