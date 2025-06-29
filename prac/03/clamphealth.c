#include <stdio.h>

int clamp(int value, int min, int max) {
  if (value < min) {
    value = min;
  } else if (value > max) {
    value = max;
  }

  return value;
}

int main() {
  int health = -10;
  printf("Health before clamp: %d\n", health);
  health = clamp(health, 0, 100);
  printf("Health after clamp: %d\n", health);
  health = 110;
  printf("Health above max health: %d\n", health);
  health = clamp(health, 0, 100);
  printf("Health clamped to max health: %d\n", health);

  return 0;
}
