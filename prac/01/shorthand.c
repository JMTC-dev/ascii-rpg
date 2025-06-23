#include <stdio.h>

int main() {
  int score = 0;

  score = score + 10;

  score += 10;

  score -= 5;
  score *= 2;
  score /= 3;

  score++;
  score--;

  printf("Final score: %d\n", score);

  return 0;
}
