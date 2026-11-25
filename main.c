#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include "menu_ui.h"
#include "game_ui.h"

int SCREEN_WIDTH = 950;
int SCREEN_HEIGHT = 750;
double CELL_SIZE = 15;
TTF_Font *font;


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

    font = TTF_OpenFont("/usr/share/fonts/noto/NotoSansMono-Light.ttf", 25);
    return 0;
}


int main(){
    int return_value = 0;
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    return_value = SDL_setup(&window, &renderer);
    if (return_value < 0) goto clean_up;

    Game *gol = game_init(40, 40);
    int MENU_X = SCREEN_WIDTH - MENU_WIDTH;
    Button *play_pause_btn = create_button(&(SDL_Rect){MENU_X + 16, 100, 130, 40}, font, "Pause", play_pause, gol);
    Button *next_step_btn = create_button(&(SDL_Rect){MENU_X + 154, 100, 130, 40}, font, "Step", next_step, gol);
    Button *back_step_btn = create_button(&(SDL_Rect){MENU_X + 16, 150, 130, 40}, font, "Back", NULL, NULL);
    Button *stop_btn = create_button(&(SDL_Rect){MENU_X + 154, 150, 130, 40}, font, "Stop", stop, gol);
    Button *clear_btn = create_button(&(SDL_Rect){MENU_X + 16, 200, 130, 40}, font, "Clear", clear_canvas, gol);
    Button *wrap_btn = create_button(&(SDL_Rect){MENU_X + 16, 440, 268, 40}, font, "Wrap edges: off", NULL, NULL);

    char buffer[TEXTBOX_MAX];
    snprintf(buffer, sizeof(buffer), "%d", gol->now->cols);
    TextBox *columns = create_text_box(&(SDL_Rect){MENU_X + 164, 270, 120, 40}, font, buffer, 4 ,(40 - TTF_FontHeight(font)) / 2);
    snprintf(buffer, sizeof(buffer), "%d", gol->now->rows);
    TextBox *rows = create_text_box(&(SDL_Rect){MENU_X + 164, 320, 120, 40}, font, buffer, 2, (40 - TTF_FontHeight(font)) / 2);
    snprintf(buffer, sizeof(buffer), "%d", gol->delay_ms);
    TextBox *delay = create_text_box(&(SDL_Rect){MENU_X + 164, 370, 120, 40}, font, buffer, 2, (40 - TTF_FontHeight(font)) / 2);

    SDL_Event e;
    bool quit = false;
    while(quit == false){
        update_game_canvas(renderer, gol);
        play_pause_btn->render_update(renderer, play_pause_btn);
        next_step_btn->render_update(renderer, next_step_btn);
        back_step_btn->render_update(renderer, back_step_btn);
        stop_btn->render_update(renderer, stop_btn);
        clear_btn->render_update(renderer, clear_btn);
        wrap_btn->render_update(renderer, wrap_btn);

        columns->render_update(renderer, columns);
        rows->render_update(renderer, rows);
        delay->render_update(renderer, delay);
        SDL_RenderPresent(renderer);

        if ((SCREEN_WIDTH - MENU_WIDTH)/gol->now->cols < (SCREEN_HEIGHT)/gol->now->rows)
            CELL_SIZE = (SCREEN_WIDTH - MENU_WIDTH)/gol->now->cols;
        else    
            CELL_SIZE = (SCREEN_HEIGHT)/gol->now->rows;

        mouse_update(gol);

        if (gol->mode == MODE_EDIT){
            
            next_step_btn->enabled = true;
            back_step_btn->enabled = true;
            stop_btn->enabled = false;
            wrap_btn->enabled = true;
            columns->enabled = true;
            rows->enabled = true;
            delay->enabled = true;
        }
        else if (gol->mode == MODE_RUN){
            next_step_btn->enabled = false;
            back_step_btn->enabled = false;
            stop_btn->enabled = true;
            wrap_btn->enabled = false;
            columns->enabled = false;
            rows->enabled = false;
            delay->enabled = false;
        }
        else if (gol->mode == MODE_PAUSED){
            next_step_btn->enabled = true;
            back_step_btn->enabled = true;
            stop_btn->enabled = true;
            wrap_btn->enabled = false;
            columns->enabled = false;
            rows->enabled = false;
            delay->enabled = false;
        }



        while (SDL_PollEvent(&e)){ 
            if (e.type == SDL_QUIT) quit = true;

            play_pause_btn->input_update(play_pause_btn, &e);
            next_step_btn->input_update(next_step_btn, &e);
            back_step_btn->input_update(back_step_btn, &e);
            stop_btn->input_update(stop_btn, &e);
            clear_btn->input_update(clear_btn, &e);
            wrap_btn->input_update(wrap_btn, &e);

            columns->input_update(columns, &e);
            rows->input_update(rows, &e);
            delay->input_update(delay, &e);

            if (e.type == SDL_KEYDOWN && (
                e.key.keysym.sym == SDLK_SPACE ||
                e.key.keysym.sym == SDLK_RIGHT)){
                next_step(gol);
            }
        }
        
    }

    free(play_pause_btn);
    free(next_step_btn);
    free(back_step_btn);
    free(stop_btn);
    free(clear_btn);
    free(wrap_btn);

    free(columns);
    free(rows);
    free(delay);
    game_end(gol);

clean_up:
    if (font) TTF_CloseFont(font);
    if (TTF_WasInit()) TTF_Quit();
    if (renderer != NULL) SDL_DestroyRenderer(renderer);
    if (window != NULL) SDL_DestroyWindow(window);
    
    SDL_Quit();
    return return_value;
}
