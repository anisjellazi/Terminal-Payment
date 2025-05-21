#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define VALID_CARD "1234567812345678"
#define VALID_PIN "1234"
#define LOG_FILE "transactions.log"

typedef struct {
    char card_number[20];
    char pin[10];
    float amount;
    char transaction_id[32];
    char timestamp[64];
} Transaction;

void generate_transaction_id(char *dest, size_t size) {
    snprintf(dest, size, "TXN-%ld", time(NULL));
    dest[size - 1] = '\0';
}

void get_current_timestamp(char *dest, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t != NULL) {
        strftime(dest, size, "%Y-%m-%d %H:%M:%S", t);
        dest[size - 1] = '\0';
    } else {
        strncpy(dest, "Unknown Time", size);
        dest[size - 1] = '\0';
    }
}

int read_card(char *buffer, size_t size) {
    printf("[TPE] Insert card (16-digit number): ");
    if (scanf("%19s", buffer) != 1) {
        return 0;
    }
    buffer[size - 1] = '\0';
    return strcmp(buffer, VALID_CARD) == 0;
}

int validate_pin(char *buffer, size_t size) {
    printf("[TPE] Enter PIN: ");
    if (scanf("%9s", buffer) != 1) {
        return 0;
    }
    buffer[size - 1] = '\0';
    return strcmp(buffer, VALID_PIN) == 0;
}

int read_amount(float *amount) {
    printf("[TPE] Enter amount: ");
    if (scanf("%f", amount) != 1 || *amount <= 0) {
        return 0;
    }
    return 1;
}

void log_transaction(const Transaction *txn) {
    FILE *f = fopen(LOG_FILE, "a");
    if (!f) {
        printf("[TPE] Warning: Failed to write log.\n");
        return;
    }
    fprintf(f, "%s | %s | Card: ****%s | Amount: %.2f\n",
            txn->timestamp,
            txn->transaction_id,
            &txn->card_number[12],
            txn->amount);
    fclose(f);
}

int main(void) {
    Transaction txn;

    if (!read_card(txn.card_number, sizeof(txn.card_number))) {
        printf("[TPE] Card read error or unrecognized card.\n");
        return 1;
    }

    if (!validate_pin(txn.pin, sizeof(txn.pin))) {
        printf("[TPE] Invalid PIN. Transaction declined.\n");
        return 1;
    }

    if (!read_amount(&txn.amount)) {
        printf("[TPE] Invalid amount.\n");
        return 1;
    }

    generate_transaction_id(txn.transaction_id, sizeof(txn.transaction_id));
    get_current_timestamp(txn.timestamp, sizeof(txn.timestamp));

    printf("[TPE] Transaction approved.\n");
    printf("[TPE] Transaction ID: %s\n", txn.transaction_id);
    printf("[TPE] Timestamp: %s\n", txn.timestamp);

    log_transaction(&txn);
    return 0;
}
