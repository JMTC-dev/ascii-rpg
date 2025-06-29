#include <stdio.h>

int main() {
  int damageRolls[] = {8, 12, 5, 15, 9};

  int size = sizeof(damageRolls) / sizeof(damageRolls[0]);
  int damage = 0;
  for (int i = 0; i < size; i++) {
    damage += damageRolls[i];
  }

  printf("Sum of damage rolls %d\n", damage);

  return 0;
}
