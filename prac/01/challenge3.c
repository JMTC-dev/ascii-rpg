#include <stdbool.h>
#include <stdio.h>

int main() {
  int playerLevel = 9;
  int requiredLevel = 10;
  bool hasMagicKey = false;

  if (playerLevel >= requiredLevel && hasMagicKey) {
    printf("The gates to Asgard unlock.\n");
    printf("Welcome to Asgard.\n");
  } else if (playerLevel >= requiredLevel && !hasMagicKey) {
    printf("Hmm, you are the correct level. You are just missing the magic "
           "key!\n");
  } else if (playerLevel < requiredLevel && hasMagicKey) {
    printf("You have the magic key. However, not the level. You need to be "
           "level %d.\nSo %d more %s for you.\n",
           requiredLevel, requiredLevel - playerLevel,
           requiredLevel - playerLevel == 1 ? "level" : "levels");
  } else {
    printf("You lack the magic key and the right levels. You need to be level "
           "%d.\nSo %d more %s for you.\n",
           requiredLevel, requiredLevel - playerLevel,
           requiredLevel - playerLevel == 1 ? "level" : "levels");
  }
}
