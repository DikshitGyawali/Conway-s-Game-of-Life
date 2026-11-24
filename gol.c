#include "gol.h"
#include <stdlib.h>
#include <string.h>

Grid *grid_create(int cols, int rows){
    bool *cells = (bool *)calloc(cols * rows, sizeof (bool));
    if (cells == NULL) return NULL; 
    Grid *grid = (Grid *)malloc(sizeof(Grid));
    if (grid == NULL) { free(cells) ; return NULL; }
    grid->cells = cells;
    grid->cols = cols;
    grid->rows = rows;
    return grid;
}

void grid_free(Grid *g){
    free(g->cells);
    free(g);
}

bool cell_get(const Grid *g , int x, int y){
    if (x < 0 || y < 0 || y >= g->rows || x >= g->cols) return false;
    return g->cells[y * g->cols + x];
}

void cell_toggle(Grid *g, int x, int y){
    if (x < 0 || y < 0 || y >= g->rows || x >= g->cols) return;
    g->cells[y * g->cols + x] = !g->cells[y * g->cols + x];
}

void  grid_clear(Grid *g){
    memset(g->cells, 0, g->cols * g->rows);
}

void grid_copy(Grid *dst, const Grid *src){
    memcpy(dst->cells, src->cells, dst->cols * dst->rows);
}

static bool check_cell_next_state(const Grid *g, int x, int y, bool wrap){
    bool present_state = g->cells[y * g->cols + x];
    int live_neighbor = 0;    

    for (int i = -1; i <= 1; ++i){
        for (int j = -1; j <= 1; ++j){
            if (i == 0 && j == 0) continue;

            if (wrap){
                if (cell_get(g, (x + j + g->cols) % g->cols, (y + i + g->rows) % g->rows)) 
                    live_neighbor++; 
            }
            else {
                if (cell_get(g, x + j, y + i))
                    live_neighbor++;
            }
        }
    }

    if (present_state == ALIVE && (live_neighbor == 2 || live_neighbor == 3)) return ALIVE;
    if (present_state == DEAD && live_neighbor == 3) return ALIVE;

    return DEAD;
}

void grid_step(const Grid *cur, Grid *next, bool wrap){
    for (int i = 0; i < cur->rows; ++i){
        for (int j = 0; j < cur->cols; ++j){
            next->cells[i * next->cols + j] = check_cell_next_state(cur, j, i, wrap);
        }
    }
}

bool grid_resize(Grid **g, int cols, int rows){

    Grid *new_grid = grid_create(cols, rows);
    if (new_grid == NULL) return false;

    int low_rows = ((*g)->rows > rows) ? rows : (*g)->rows;
    int low_cols = ((*g)->cols > cols) ? cols : (*g)->cols;

    for (int i = 0; i < low_rows; ++i){
        for (int j = 0; j < low_cols; ++j){
            new_grid->cells[i * low_cols + j] = (*g)->cells[i * low_cols + j];
        }
    }
    free((*g)->cells);
    free(*g);
    *g = new_grid;
    return true;
}
