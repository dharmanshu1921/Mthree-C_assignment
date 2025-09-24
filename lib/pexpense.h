#ifndef PEXPENSE_H
#define PEXPENSE_H

#include <time.h>

#define DESC_LEN 128
#define CATEGORY_LEN 32
#define DATA_FILE "pexpense.dat"
#define CSV_FILE  "pexpense_export.csv"

/* transaction type */
typedef enum { T_INCOME=1, T_EXPENSE=2 } TxType;

/* transaction record */
typedef struct {
    int id;
    TxType type;
    double amount;
    char category[CATEGORY_LEN];
    char desc[DESC_LEN];
    time_t ts; /* timestamp */
    int recurring_days; /* 0 = not recurring, else days between repeats */
} Transaction;

/* dynamic array container */
typedef struct {
    Transaction *items;
    size_t len;
    size_t cap;
    int next_id;
} TxList;

/* Budget structure */
typedef struct {
    double monthly_limit;
} Budget;

/* Library API */
void txlist_init(TxList *l);
void txlist_free(TxList *l);
int txlist_add(TxList *l, Transaction t);
int txlist_remove_by_id(TxList *l, int id);
Transaction* txlist_find(TxList *l, int id);
void txlist_sort_by_time(TxList *l, int descending);
void txlist_print_summary(TxList *l);
void txlist_export_csv(TxList *l, const char *filename);
int txlist_save(TxList *l, const char *filename);
int txlist_load(TxList *l, const char *filename);
void print_transaction(const Transaction *t);
void budget_init(Budget *b);
int budget_check_and_warn(Budget *b, TxList *l);

#endif /* PEXPENSE_H */
