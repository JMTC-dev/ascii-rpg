#include <stdio.h>
int main() {
  int health = 75;
  char name[] = "Goblin";
  char symbol = 'g';
  float attackSpeed = 1.5f;

  printf("%s ('%c') has %d HP and attacks every %.1f seconds.\n", name, symbol,
         health, attackSpeed);

  return 0;
}
