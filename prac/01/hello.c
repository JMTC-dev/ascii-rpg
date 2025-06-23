#include <stdbool.h>
#include <stdio.h>
int main() {
  int playerHealth;
  playerHealth = 100;
  int playerGold = 50;
  char playerSymbol = '@';
  char wallSymbol = '#';
  char floorSymbol = '.';
  float itemWeight = 2.5f;
  double playerX = 10.75;
  bool gameRunning = true;
  bool playerAlive = true;
  bool hasKey = false;

  printf("The potion weighs %f kg\n", itemWeight);
  printf("Player X position: %f\n", playerX);
  printf("The potion weighs: %.2f kg\n", itemWeight);

  printf("The player looks like: %c\n", playerSymbol);
  printf("Walls look like: %c\n", wallSymbol);
  printf("Floor looks like %c\n", floorSymbol);

  printf("Health: %d\n", playerHealth);
  printf("Gold: %d\n", playerGold);

  // Integer vs. Float Division
  int a = 5;
  int b = 2;
  printf("Integer division %d / %d = %d\n", a, b, a / b);

  float c = 5.0f;
  float d = 2.0f;
  printf("Float division %f / %f = %f\n", c, d, c / d);

  // Boolean Values (True/False)
  printf("Game running: %d\n", gameRunning);
  printf("Player alive: %d\n", playerAlive);
  printf("Has key: %d\n", hasKey);

  if (playerAlive) {
    printf("Player can continue!\n");
  }

  if (!hasKey) {
    printf("You need to find a key!\n");
  }

  return 0;
}
