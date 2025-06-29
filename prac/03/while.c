#include <stdio.h>

int main() {
  int countdown = 5;

  printf("Game starting in...\n");

  while (countdown > 0) {
    printf("%d...\n", countdown);
    countdown--;
  }

  printf("GO!\n");

  return 0;
}
