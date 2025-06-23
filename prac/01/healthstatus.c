#include <stdbool.h>
#include <stdio.h>

int main() {
  // Game State
  int playerHealth = 75;
  int maxHealth = 100;
  bool poisoned = true;

  // Display current health
  printf("=== Health Status ===\n");
  printf("Health: %d/%d\n", playerHealth, maxHealth);

  // Check health status
  if (playerHealth <= 0) {
    printf("Status: DEAD\n");
    printf("Game Over!\n");
  } else if (playerHealth < 25) {
    printf("Status: CRITICAL\n");
    printf("Find healing immeditately!\n");
  } else if (playerHealth < 50) {
    printf("Status: WOUNDED\n");
    printf("Be careful!\n");
  } else if (playerHealth < maxHealth) {
    printf("Status: HURT\n");
    printf("You've taken some damage.\n");
  } else {
    printf("Status: HEALTHY\n");
    printf("You're in perfect health!\n");
  }

  // Check for status effects
  if (poisoned) {
    printf("WARNING: You are poisoned!\n");
  }

  // Calculate health percentage
  // Multiply by 100 first to avoid integer division issues
  int healthPercent = (playerHealth * 100) / maxHealth;
  printf("Health percentage: %d%%\n", healthPercent);

  return 0;
}
