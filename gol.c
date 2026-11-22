#include <stdbool.h>
#include <string.h>
bool cell[100][100];
bool nxt_gen_cell[100][100];

#define ALIVE true
#define DEAD false
#define HEIGHT 100
#define WIDTH 100

/* 
* Rules:
* if cell is alive, and has 2 or 3 live neighbours it lives in the next generation
* if cell is alive, and has less then 2 live neighbour it dies in the next generation
* if cell is alive, and had more then 3 live neighbours it also dies in the next generation
* is a cell is dead, and has exactly 3 live neighbours, it becomes alive in the next generation
*/

bool check_cell_next_state(int x, int y){
    bool present_state = cell[x][y];
    int live_neighbor = 0;

    if (x != 0  && y != 0)                  if (cell[x-1][y-1]) live_neighbor++;
    if (y != 0)                             if (cell  [x][y-1]) live_neighbor++;
    if (x != WIDTH - 1 && y != 0)           if (cell[x+1][y-1]) live_neighbor++;
    if (x != 0)                             if (cell[x-1][y]  ) live_neighbor++;
    if (x != WIDTH - 1)                     if (cell[x+1][y]  ) live_neighbor++;
    if (x != 0  && y != HEIGHT - 1)         if (cell[x-1][y+1]) live_neighbor++;
    if (y != HEIGHT - 1)                    if (cell  [x][y+1]) live_neighbor++;
    if (x != WIDTH - 1 && y != HEIGHT - 1)  if (cell[x+1][y+1]) live_neighbor++;

    if (present_state == ALIVE && (live_neighbor ==2 || live_neighbor == 3)) return ALIVE;
    if (present_state == DEAD && live_neighbor == 3) return ALIVE;

    return DEAD;
}


void update(){
    for (int i = 0; i < WIDTH; ++i){
        for (int j = 0; j < HEIGHT; ++j){
            nxt_gen_cell[i][j] = check_cell_next_state(i, j);
        }
    }

    memcpy(cell, nxt_gen_cell, sizeof cell);
}
