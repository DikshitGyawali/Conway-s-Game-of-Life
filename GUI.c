#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include "gol.h"
#include "ui.h"

int SCREEN_WIDTH = 950;
int SCREEN_HEIGHT = 750;
double CELL_SIZE = 15;
TTF_Font *font;

void update_game_canvas(SDL_Renderer *renderer, Grid *grid){
    SDL_Rect full = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 0xFF);
    SDL_RenderFillRect(renderer, &full);
    
    for (int i = 0; i < grid->rows; ++i){
        for (int j = 0; j < grid->cols; ++j){
            SDL_Rect cell_rect = {j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE - 1, CELL_SIZE - 1};
            if (grid->cells[i * grid->cols + j]) SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
            else SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderFillRect(renderer, &cell_rect);
        }
    }
}

int SDL_setup(SDL_Window **window, SDL_Renderer **renderer){
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return -1;
    }

    *window = SDL_CreateWindow(
        "Conway's Game of Life",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP
    );
    if (*window == NULL){
        printf("Window creation failed: %s\n", SDL_GetError());
        return -1;
    }
    int w, h;
    SDL_GetWindowSize(*window, &w, &h);

    SCREEN_WIDTH = (w == 0) ? SCREEN_WIDTH : w;
    SCREEN_HEIGHT = (h == 0) ? SCREEN_HEIGHT : h;

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (*renderer == NULL){
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return -1;
    }

    if (TTF_Init() < 0) {
        printf("ui_init: TTF_Init failed: %s\n", TTF_GetError());
        return -1;
    }

    font = TTF_OpenFont("/usr/share/fonts/noto/NotoSansMono-Light.ttf", 16);
    return 0;
}

void mouse_update(Grid *g){
    static int last_x = -1;
    static int last_y = -1;

    int x, y;
    Uint32 buttons = SDL_GetMouseState(&x, &y);

    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)){
        int cell_x = (int)((double)x / CELL_SIZE);
        int cell_y = (int)((double)y / CELL_SIZE);
        if (cell_x != last_x || cell_y != last_y){
            cell_toggle(g, cell_x, cell_y);
            last_x = cell_x;
            last_y = cell_y;
        }
    }
    else {
        last_x = -1;
        last_y = -1;
    }
}

int main(){
    int return_value = 0;
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    Grid *grid = NULL;
    Grid *grid_next = NULL;
    return_value = SDL_setup(&window, &renderer);
    if (return_value < 0) goto clean_up;

    grid = grid_create(40, 30);
    grid_clear(grid);
    grid_next = grid_create(40, 30);
    grid_clear(grid_next);
    Grid *now = grid;
    Grid *next = grid_next;
    CELL_SIZE = (double)SCREEN_HEIGHT/(double)now->rows;

    TextBox *demo = create_text_box(&(SDL_Rect){CELL_SIZE * now->cols + 20, 20, 50, 30}, font, 5, 2);
    if (demo == NULL) printf("error creating text_box\n");

    SDL_Event e;
    bool quit = false;
    while(quit == false){

        CELL_SIZE = (double)SCREEN_HEIGHT/(double)now->rows;

        update_game_canvas(renderer, now);
        demo->render_update(renderer, demo);
        SDL_RenderPresent(renderer);

        mouse_update(now);

        while (SDL_PollEvent(&e)){ 
            if (e.type == SDL_QUIT) quit = true;

            demo->input_update(demo, &e);

            if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_SPACE || e.key.keysym.sym == SDLK_RIGHT)) {
                grid_step(now, next, false);
                Grid *temp = now;
                now = next;
                next = temp;
            }
        }
        
    }
    free(demo);

clean_up:
    if (font) TTF_CloseFont(font);
    if (TTF_WasInit()) TTF_Quit();
    if (renderer != NULL) SDL_DestroyRenderer(renderer);
    if (window != NULL) SDL_DestroyWindow(window);
    if (grid != NULL) grid_free(grid);
    if (grid_next != NULL) grid_free(grid_next);
    SDL_Quit();
    return return_value;
}
