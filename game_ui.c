#include <SDL2/SDL.h>
#include "gol.h"
#include "game_ui.h"
#include "history.h"

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
    if (game == NULL) { printf("allocation failed for Game data\n"); return NULL; }
    game->history = history_init(1000, cols, rows);
    if (game->history == NULL) { printf("allocation failed for Game data\n");free(game); return NULL; }
    game->now = grid_create(cols, rows);
    game->next = grid_create(cols, rows);
    game->mode = MODE_EDIT;
    game->delay_ms = 200;
    game->last_step = SDL_GetTicks();
    game->wrap = false;

    grid_clear(game->now);
    grid_clear(game->next);
    if ((SCREEN_WIDTH - MENU_WIDTH)/cols < (SCREEN_HEIGHT)/rows)
        CELL_SIZE = (SCREEN_WIDTH - MENU_WIDTH)/cols;
    else    
        CELL_SIZE = (SCREEN_HEIGHT)/rows;
    return game;
}

void game_resize(Game *game, int cols, int rows){
    grid_resize(&game->now, cols, rows);
    grid_resize(&game->next, cols, rows);
    game->history = history_resize(game->history, cols, rows);
}

void game_end(Game *game){
    grid_free(game->now);
    grid_free(game->next);
    history_free(game->history);
    free(game);
}


void play_pause(Button *this, void *game){ // play_pause_btn
    Game *gol = (Game *)game;
    static Button *play_pause_btn;
    if (this != NULL) play_pause_btn = this;
    else this = play_pause_btn;

    if (gol->mode == MODE_RUN){
        gol->mode = MODE_PAUSED;
    }
    else if (gol->mode == MODE_PAUSED || gol->mode == MODE_EDIT) {
        gol->mode = MODE_RUN;
    }
    gol->last_step = SDL_GetTicks();
}

void next_step(Button *this, void *game){ // next_step_btn
    (void)this;
    Game *gol = (Game *)game;
    if (gol->mode == MODE_EDIT){
        gol->mode = MODE_PAUSED;
    }
    history_push(gol->history, gol->now);
    grid_step(gol->now, gol->next, gol->wrap);
    Grid *temp = gol->now;
    gol->now = gol->next;
    gol->next = temp;
}

void back_step(Button *this, void *game){ // back_step_btn
    (void)this;
    Game *gol = (Game *)game;
    if (gol->mode == MODE_EDIT || 
        gol->mode == MODE_RUN || 
       (gol->mode == MODE_PAUSED && gol->history->count == 0)) return;
    history_pop(gol->history, gol->now);
}

void stop(Button *this, void *game){ // stop_btn
    (void)this;
    Game *gol = (Game *)game;

    if (gol->mode == MODE_RUN || gol->mode == MODE_PAUSED){
        gol->mode = MODE_EDIT;
        memcpy(gol->now->cells, gol->history->data, gol->history->cell_count);
        history_clear(gol->history);
    }
}

void clear_canvas(Button *this, void *game){ // clear_btn
    (void)this;
    Game *gol = (Game *)game;
    grid_clear(gol->now);
}

void wrap(Button *this, void *game){ // wrap_btn
    Game *gol = (Game *)game;
    if (gol->wrap) {
        gol->wrap = false;
        snprintf(this->label, sizeof this->label, "%s", "Wrap edges: off");
    }
    else{
        gol->wrap = true;
        snprintf(this->label, sizeof this->label, "%s", "Wrap edges: on");
    }
    history_clear(gol->history);
}
