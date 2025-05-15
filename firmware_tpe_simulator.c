/*
 * firmware_tpe_simulator.c
 * Simulates a basic TPE firmware in C (no hardware required)
 */
#include <stdio.h>
#include <string.h>

#define VALID_CARD "1234567812345678"
#define VALID_PIN "1234"

int main() {
    char card_input[20];
    char pin_input[10];
    float amount = 0;

    printf("[TPE] Insert card (16-digit number): ");
    scanf("%19s", card_input);

    if (strcmp(card_input, VALID_CARD) != 0) {
        printf("[TPE] Card read error or unrecognized card.\n");
        return 1;
    }

    printf("[TPE] Enter PIN: ");
    scanf("%9s", pin_input);

    if (strcmp(pin_input, VALID_PIN) != 0) {
        printf("[TPE] Invalid PIN. Transaction declined.\n");
        return 1;
    }

    printf("[TPE] Enter amount: ");
    scanf("%f", &amount);

    if (amount <= 0) {
        printf("[TPE] Invalid amount.\n");
        return 1;
    }

    printf("[TPE] Transaction of %.2f approved.\n", amount);
    return 0;
}
