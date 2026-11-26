#ifndef GAME_UI_H
#define GAME_UI_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "gol.h"
#include "menu_ui.h"
#include "history.h"

typedef enum{
    MODE_EDIT,
    MODE_RUN,
    MODE_PAUSED
} Mode;

typedef struct{
    Grid *now;
    Grid *next;
    Mode mode;
    History *history;
    Uint32 delay_ms;
    Uint32 last_step;
    bool wrap;
} Game;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern double CELL_SIZE;
extern TTF_Font *font;


Game *game_init(int cols, int rows);
void game_resize(Game *game, int cols, int rows);
void game_end(Game *game);

void play_pause(Button *this, void *game);
void next_step(Button *this, void *game);
void back_step(Button *this, void *game);
void stop(Button *this, void *game);
void clear_canvas(Button *this, void *game);
void wrap(Button *this, void *game);

void update_game_canvas(SDL_Renderer *renderer, Game *game);
void mouse_update(Game *game);

#define MENU_WIDTH 300

#endif
