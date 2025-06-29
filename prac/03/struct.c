#include <stdio.h>

typedef struct {
  int health;
  int attackPower;
  char symbol;
  struct {
    int posX;
    int posY;
  } Position;
} Monster;

int main() {
  Monster goblin;

  goblin.health = 50;
  goblin.attackPower = 10;
  goblin.symbol = 'g';
  goblin.Position.posX = 5;
  goblin.Position.posY = 10;

  printf("A goblin ('%c') appears!\n", goblin.symbol);
  printf("It has %d HP and %d attack power.\n", goblin.health,
         goblin.attackPower);
  printf("It is sitting at X: %d, Y: %d\n", goblin.Position.posX,
         goblin.Position.posY);

  return 0;
}
