#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gol.h"
#include "history.h"


History *history_init(int capacity ,int cols, int rows){
    History *h = malloc(sizeof(History));
    if (h == NULL ){printf("failed memory allocation for history\n"); return NULL;}
    capacity = (capacity < MAX_HISTORY_CAPACITY) ? capacity : MAX_HISTORY_CAPACITY;
    h->data = malloc(cols * rows * capacity);
    if (h->data == NULL) {printf("failed memory allocation for history\n");free(h) ; return NULL;}

    h->capacity = capacity;
    h->count = 0;
    h->cell_count = cols * rows;
    return h;
}

bool history_push(History *h, const Grid *g){
    if (h->cell_count != g->cols * g->rows) { printf("Grid g should have same number of cell as History h->cell_count"); return false; }
    if (h->capacity == h->count){ printf("MAX capacity reached\n"); return false; }
    void *dst = h->data +  h->count * h->cell_count;
    memcpy(dst, g->cells, h->cell_count);
    h->count++;
    return true;
}

bool history_pop(History *h, Grid *g){
    if (h->cell_count != g->cols * g->rows) { printf("Grid g should have same number of cell as History h->cell_count"); return false; }
    if (h->count == 0) return false;
    void *src = h->data + (h->count - 1)* h->cell_count;
    memcpy(g->cells, src, h->cell_count);
    h->count--;
    return true;
}

void history_clear(History *h){
    h->count = 0;
}

void history_free(History *h){
    free(h->data);
    free(h);
}

History *history_resize(History *h, int cols, int rows){
    int capacity = h->capacity;
    history_free(h);
    return history_init(capacity, cols, rows);
}
