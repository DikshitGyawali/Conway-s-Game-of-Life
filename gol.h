#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H

#include <stdbool.h>

#define ALIVE true
#define DEAD false

#define HEIGHT 30
#define WIDTH 50

typedef struct {
    int cols;
    int rows;
    bool *cells; 
} Grid;

Grid *grid_create(int cols, int rows);
void  grid_free(Grid *g);
bool  cell_get(const Grid *g, int x, int y);
void  cell_toggle(Grid *g, int x, int y);
void  grid_clear(Grid *g);
void  grid_copy(Grid *dst, const Grid *src);
void  grid_step(const Grid *cur, Grid *next, bool wrap);
bool  grid_resize(Grid **g, int cols, int rows);


#endif
