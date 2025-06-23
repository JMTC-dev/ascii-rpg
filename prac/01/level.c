#include <stdio.h>

int main() {
  int playerLevel = 4;

  char forestName[] = "Eldwin's Forest";
  int forestLevelRequirement = 1;
  char caveName[] = "Grotto's Cave";
  int caveLevelRequirement = 5;
  char castleName[] = "Arthur's Castle";
  int castleLevelRequirement = 10;

  if (playerLevel >= forestLevelRequirement) {
    printf("You are welcomed to %s!\n", forestName);
  } else if (playerLevel >= caveLevelRequirement) {
    printf("You are welcomed to %s!\n", caveName);
  } else if (playerLevel >= castleLevelRequirement) {
    printf("You are welcomed to %s!\n", castleName);
  } else {
    printf("You are not welcomed to any of the levelled areas, back to Archeon "
           "you go.\n");
  }

  return 0;
}
