#include <stdio.h>

int main() {
  int enemyHealth[5] = {50, 50, 50, 50, 50};

  printf("Enemy Status:\n");
  for (int i = 0; i < 5; i++) {
    printf("Enemy %d: %d HP\n", i + 1, enemyHealth[i]);
  }

  return 0;
}
