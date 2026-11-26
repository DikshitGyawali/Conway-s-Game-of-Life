#ifndef HISTORY_H
#define HISTORY_H

typedef struct {
    bool *data;       // all slots in one block of memory; one slot = cols * rows cells
    int capacity;     // how many generations can be remembered
    int count;        // how many are stored right now (0 to capacity)
    int cell_count;   // cols * rows, the size of one slot
} History;

#define MAX_HISTORY_CAPACITY 5000

History *history_init(int capacity ,int cols, int rows);
bool history_push(History *h, const Grid *g);
bool history_pop(History *h, Grid *g);
void history_clear(History *h);
History *history_resize(History *h, int cols, int rows);
void history_free(History *h);

#endif
