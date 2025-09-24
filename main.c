/*
 * SmartExpense 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lib/pexpense.h"

#define BUF 256

/* ANSI escape codes for colors */
#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[1;36m"
#define BOLD    "\033[1m"

/* Struct for category summary */
typedef struct {
    char name[50];   // category name
    double total;    // total amount
} CategorySummary;

static void banner(void) {
    printf(CYAN "\n=====================================\n" RESET);
    printf(BOLD "         💸 SmartExpense 💸\n" RESET);
    printf(CYAN "   Personal Finance Manager (INR)\n" RESET);
    printf("=====================================\n");
}

static void greet_user(char *username) {
    printf(YELLOW "Welcome! Please enter your name: " RESET);
    if (fgets(username, BUF, stdin)) {
        username[strcspn(username, "\n")] = 0;
        printf(GREEN "\nHello, %s! Let's manage your finances 💰\n" RESET, username);
    }
}

static void add_tx(TxList *L) {
    Transaction t = {0};
    char line[BUF];

    printf(YELLOW "Type (1=Income, 2=Expense): " RESET);
    if (!fgets(line, sizeof(line), stdin)) return;
    t.type = (TxType)atoi(line);

    printf(YELLOW "Amount (₹): " RESET);
    if (!fgets(line, sizeof(line), stdin)) return;
    t.amount = atof(line);

    printf(YELLOW "Category: " RESET);
    if (!fgets(t.category, sizeof(t.category), stdin)) return;
    t.category[strcspn(t.category, "\n")] = 0;

    printf(YELLOW "Short description: " RESET);
    if (!fgets(t.desc, sizeof(t.desc), stdin)) return;
    t.desc[strcspn(t.desc, "\n")] = 0;

    t.ts = time(NULL);

    int id = txlist_add(L, t);
    if (id < 0) puts(RED "Failed to add transaction." RESET);
    else printf(GREEN "✔ Added transaction [ID=%d]\n" RESET, id);
}

static void list_tx(TxList *L) {
    if (L->len == 0) { puts(RED "No transactions yet." RESET); return; }
    txlist_sort_by_time(L, 0);

    printf(BOLD CYAN "\n%-5s %-20s %-10s %-12s %-15s %-20s\n" RESET,
           "ID", "Date/Time", "Type", "Amount(₹)", "Category", "Description");
    printf("-------------------------------------------------------------------------------\n");

    for (size_t i = 0; i < L->len; ++i) {
        char buf[64];
        struct tm *lt_ptr = localtime(&L->items[i].ts);
        if (!lt_ptr) continue;
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", lt_ptr);

        printf("%-5d %-20s %-10s ₹%-11.2f %-15s %-20s\n",
               L->items[i].id,
               buf,
               (L->items[i].type == T_INCOME ? GREEN "INCOME" RESET : RED "EXPENSE" RESET),
               L->items[i].amount,
               L->items[i].category,
               L->items[i].desc);
    }
}

static void delete_tx(TxList *L) {
    char buf[BUF];
    printf(YELLOW "Enter ID to delete: " RESET);
    if (!fgets(buf, sizeof(buf), stdin)) return;
    int id = atoi(buf);
    if (txlist_remove_by_id(L, id))
        printf(GREEN "✔ Deleted transaction [ID=%d]\n" RESET, id);
    else
        printf(RED "ID %d not found.\n" RESET, id);
}

static void export_csv(TxList *L) {
    txlist_export_csv(L, CSV_FILE);
}

static void set_budget(Budget *B) {
    char line[BUF];
    printf(YELLOW "Enter monthly budget (₹, 0 to disable): " RESET);
    if (!fgets(line, sizeof(line), stdin)) return;
    B->monthly_limit = atof(line);
    printf(GREEN "✔ Budget set to ₹%.2f\n" RESET, B->monthly_limit);
}

static void print_summary(TxList *L) {
    double total_income = 0, total_expense = 0;
    CategorySummary cats[50]; int cat_count = 0;

    for (size_t i = 0; i < L->len; i++) {
        if (L->items[i].type == T_INCOME) total_income += L->items[i].amount;
        else total_expense += L->items[i].amount;

        // Category aggregation
        int found = 0;
        for (int c = 0; c < cat_count; c++) {
            if (strcmp(cats[c].name, L->items[i].category) == 0) {
                cats[c].total += L->items[i].amount;
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(cats[cat_count].name, L->items[i].category);
            cats[cat_count].total = L->items[i].amount;
            cat_count++;
        }
    }

    double net = total_income - total_expense;
    double avg_expense = L->len > 0 ? total_expense / L->len : 0;

    printf(BOLD CYAN "\n====== Monthly Summary ======\n" RESET);
    printf(GREEN "Total Income: ₹%.2f\n" RESET, total_income);
    printf(RED "Total Expense: ₹%.2f\n" RESET, total_expense);
    printf(BOLD "%sNet Balance: ₹%.2f%s\n", net>=0?GREEN:RED, net, RESET);
    printf(YELLOW "Average Expense per transaction: ₹%.2f\n" RESET, avg_expense);

    printf(BOLD CYAN "\nCategory-wise Breakdown:\n" RESET);
    for (int i = 0; i < cat_count; i++) {
        printf("%-15s : ₹%-10.2f\n", cats[i].name, cats[i].total);
    }

    printf("=============================\n");
}

static void save_and_exit(TxList *L) {
    if (txlist_save(L, DATA_FILE) != 0) {
        perror("save");
    } else {
        printf(GREEN "✔ Data saved to %s\n" RESET, DATA_FILE);
    }
    txlist_free(L);
    printf(CYAN "\nGoodbye! Stay on top of your finances 💰\n" RESET);
    exit(0);
}

static void load_on_start(TxList *L, const char *filename) {
    if (txlist_load(L, filename) == 0) {
        printf(GREEN "✔ Loaded %zu transactions.\n" RESET, L->len);
    } else {
        printf(YELLOW "No existing data (starting fresh).\n" RESET);
    }
}

int main(void) {
    TxList L;
    Budget B;
    char username[BUF];

    txlist_init(&L);
    budget_init(&B);
    banner();
    greet_user(username);

    char datafile[BUF];
    snprintf(datafile, sizeof(datafile), "%s_data.bin", username);
    load_on_start(&L, datafile);

    char cmd[BUF];
    while (1) {
        printf(BOLD CYAN "\n=== MENU ===\n" RESET);
        puts("1) Add transaction");
        puts("2) List transactions");
        puts("3) Delete transaction");
        puts("4) Summary");
        puts("5) Export CSV");
        puts("6) Set monthly budget");
        puts("7) Check budget");
        puts("8) Save & Exit");
        printf(YELLOW "Choose> " RESET);

        if (!fgets(cmd, sizeof(cmd), stdin)) break;
        switch (atoi(cmd)) {
            case 1: add_tx(&L); break;
            case 2: list_tx(&L); break;
            case 3: delete_tx(&L); break;
            case 4: print_summary(&L); break;
            case 5: export_csv(&L); break;
            case 6: set_budget(&B); break;
            case 7: budget_check_and_warn(&B, &L); break;
            case 8: save_and_exit(&L); break;
            default: puts(RED "Unknown option, try again." RESET); break;
        }
    }
    txlist_free(&L);
    return 0;
}
