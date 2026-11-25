#include <SDL2/SDL.h>
#include "gol.h"
#include "game_ui.h"

void update_game_canvas(SDL_Renderer *renderer, Game *game){
    SDL_Rect full = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 0xFF);
    SDL_RenderFillRect(renderer, &full);

    Grid *grid = game->now;
    for (int i = 0; i < grid->rows; ++i){
        for (int j = 0; j < grid->cols; ++j){
            SDL_Rect cell_rect = {j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1};
            if (grid->cells[i * grid->cols + j]) SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
            else SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderFillRect(renderer, &cell_rect);
        }
    }
}

void mouse_update(Game *game){
    static int last_x = -1;
    static int last_y = -1;

    int x, y;
    Uint32 buttons = SDL_GetMouseState(&x, &y);

    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)){
        int cell_x = (int)((double)x / CELL_SIZE);
        int cell_y = (int)((double)y / CELL_SIZE);
        if (cell_x != last_x || cell_y != last_y){
            cell_toggle(game->now, cell_x, cell_y);
            last_x = cell_x;
            last_y = cell_y;
        }
    }
    else {
        last_x = -1;
        last_y = -1;
    }
}

Game *game_init(int cols, int rows){
    Game *game = (Game *)malloc(sizeof(Game));
    game->iterations = 0;
    game->now = grid_create(cols, rows);
    game->next = grid_create(cols, rows);
    game->mode = MODE_EDIT;
    game->delay_ms = 200;
    game->wrap = false;

    grid_clear(game->now);
    grid_clear(game->next);
    if ((SCREEN_WIDTH - MENU_WIDTH)/cols < (SCREEN_HEIGHT)/rows)
        CELL_SIZE = (SCREEN_WIDTH - MENU_WIDTH)/cols;
    else    
        CELL_SIZE = (SCREEN_HEIGHT)/rows;
    return game;
}

void game_end(Game *game){
    grid_free(game->now);
    grid_free(game->next);
    free(game);
}


void play_pause(void *game){ // play_pause_btn
    Game *gol = (Game *)game;
    if (gol->mode == MODE_RUN) gol->mode = MODE_PAUSED;
    else if (gol->mode == MODE_PAUSED) gol->mode = MODE_RUN;
}

void next_step(void *game){ // next_step_btn
    Game *gol = (Game *)game;
    if (gol->mode == MODE_EDIT){
        gol->mode = MODE_PAUSED;
        
    }
    grid_step(gol->now, gol->next, false);
    Grid *temp = gol->now;
    gol->now = gol->next;
    gol->next = temp;
}

void stop(void *game){ // stop_btn
    Game *gol = (Game *)game;

    if (gol->mode == MODE_RUN || gol->mode == MODE_PAUSED){
        grid_clear(gol->now); // temp history will be added
        gol->mode = MODE_EDIT;
    }
}

void clear_canvas(void *game){ // clear_btn
    Game *gol = (Game *)game;
    grid_clear(gol->now);
}

