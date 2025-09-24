#include "pexpense.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define INITIAL_CAP 16

/* internal helper */
static int ensure_cap(TxList *l, size_t need) {
    if (l->cap >= need) return 0;
    size_t newcap = l->cap ? l->cap*2 : INITIAL_CAP;
    while (newcap < need) newcap *= 2;
    Transaction *tmp = realloc(l->items, newcap * sizeof(Transaction));
    if (!tmp) return -1;
    l->items = tmp;
    l->cap = newcap;
    return 0;
}

void txlist_init(TxList *l) {
    l->items = NULL;
    l->len = 0;
    l->cap = 0;
    l->next_id = 1;
}

void txlist_free(TxList *l) {
    if (l->items) free(l->items);
    l->items = NULL;
    l->len = l->cap = 0;
}

/* add returns new id on success, -1 on failure */
int txlist_add(TxList *l, Transaction t) {
    if (ensure_cap(l, l->len + 1) != 0) return -1;
    t.id = l->next_id++;
    l->items[l->len++] = t;
    return t.id;
}

int txlist_remove_by_id(TxList *l, int id) {
    size_t i;
    for (i = 0; i < l->len; ++i) {
        if (l->items[i].id == id) break;
    }
    if (i == l->len) return 0; /* not found */
    memmove(&l->items[i], &l->items[i+1], (l->len - i - 1) * sizeof(Transaction));
    l->len--;
    return 1;
}

Transaction* txlist_find(TxList *l, int id) {
    for (size_t i = 0; i < l->len; ++i) {
        if (l->items[i].id == id) return &l->items[i];
    }
    return NULL;
}

/* sorting by timestamp using qsort */
static int cmp_time_asc(const void *a, const void *b) {
    const Transaction *ta = a;
    const Transaction *tb = b;
    if (ta->ts < tb->ts) return -1;
    if (ta->ts > tb->ts) return 1;
    return 0;
}
static int cmp_time_desc(const void *a, const void *b) { return -cmp_time_asc(a,b); }

void txlist_sort_by_time(TxList *l, int descending) {
    if (l->len < 2) return;
    qsort(l->items, l->len, sizeof(Transaction), descending ? cmp_time_desc : cmp_time_asc);
}

void print_transaction(const Transaction *t) {
    char buf[64];
    struct tm lt;
    localtime_r(&t->ts, &lt);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &lt);
    printf("[%d] %s | %s | %.2f | %s\n",
           t->id,
           buf,
           (t->type == T_INCOME ? "INCOME" : "EXPENSE"),
           t->amount,
           t->desc);
}

void txlist_print_summary(TxList *l) {
    double income = 0.0, expense = 0.0;
    for (size_t i = 0; i < l->len; ++i) {
        if (l->items[i].type == T_INCOME) income += l->items[i].amount;
        else expense += l->items[i].amount;
    }
    printf("Transactions: %zu | Total Income: %.2f | Total Expense: %.2f | Net: %.2f\n",
           l->len, income, expense, income - expense);
}

void txlist_export_csv(TxList *l, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) { perror("fopen"); return; }
    fprintf(f, "id,timestamp,type,amount,category,description\n");
    for (size_t i = 0; i < l->len; ++i) {
        char buf[64]; struct tm lt;
        localtime_r(&l->items[i].ts, &lt);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &lt);
        fprintf(f, "%d,%s,%d,%.2f,%s,%s\n",
                l->items[i].id, buf, (int)l->items[i].type,
                l->items[i].amount, l->items[i].category, l->items[i].desc);
    }
    fclose(f);
    printf("Exported %zu rows to %s\n", l->len, filename);
}

/* binary save/load: simple format: next_id, len, then raw transactions */
int txlist_save(TxList *l, const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (!f) return -1;
    if (fwrite(&l->next_id, sizeof(int), 1, f) != 1) { fclose(f); return -1; }
    if (fwrite(&l->len, sizeof(size_t), 1, f) != 1) { fclose(f); return -1; }
    if (l->len > 0) {
        if (fwrite(l->items, sizeof(Transaction), l->len, f) != l->len) { fclose(f); return -1; }
    }
    fclose(f);
    return 0;
}

int txlist_load(TxList *l, const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) return -1;
    int nextid = 1;
    size_t len = 0;
    if (fread(&nextid, sizeof(int), 1, f) != 1) { fclose(f); return -1; }
    if (fread(&len, sizeof(size_t), 1, f) != 1) { fclose(f); return -1; }
    if (ensure_cap(l, len) != 0) { fclose(f); return -1; }
    if (len > 0) {
        if (fread(l->items, sizeof(Transaction), len, f) != len) { fclose(f); return -1; }
    }
    l->len = len;
    l->next_id = nextid;
    fclose(f);
    return 0;
}

/* Budget helpers */
void budget_init(Budget *b) { b->monthly_limit = 0.0; }

/* Check this month's expenses and warn if over budget.
   Simplified: compares total expenses to monthly_limit. */
int budget_check_and_warn(Budget *b, TxList *l) {
    if (b->monthly_limit <= 0.0) return 0;
    time_t now = time(NULL);
    struct tm t;
    localtime_r(&now, &t);
    int year = t.tm_year, mon = t.tm_mon;
    double month_exp = 0.0;
    for (size_t i = 0; i < l->len; ++i) {
        struct tm it; localtime_r(&l->items[i].ts, &it);
        if (it.tm_year == year && it.tm_mon == mon && l->items[i].type == T_EXPENSE) {
            month_exp += l->items[i].amount;
        }
    }
    if (month_exp > b->monthly_limit) {
        printf("WARNING: This month's expense %.2f exceeds budget %.2f\n", month_exp, b->monthly_limit);
        return 1;
    } else {
        printf("This month's expense: %.2f / Budget: %.2f\n", month_exp, b->monthly_limit);
        return 0;
    }
}
