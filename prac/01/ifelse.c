#include <stdio.h>

int main() {
  int playerGold = 75;
  int swordCost = 100;

  if (playerGold >= swordCost) {
    printf("You can afford the sword!\n");
  } else {
    printf("Not enough gold. You need %d more.\n", swordCost - playerGold);
  }

  return 0;
}
