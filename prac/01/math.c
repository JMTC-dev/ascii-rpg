#include <stdio.h>

int main() {
  int health = 100;
  int damage = 30;
  int healing = 20;

  // Subtraction
  health = health - damage;
  printf("After taking damage: %d\n", health);

  // Addition
  health = health + healing;
  printf("After healing: %d\n", health);

  // Multiplication
  int doubleDamage = damage * 2;
  printf("Double damage would be: %d\n", doubleDamage);

  // Division
  int halfHealth = health / 2;
  printf("Half health is: %d\n", halfHealth);

  return 0;
}
