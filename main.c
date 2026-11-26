#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include "menu_ui.h"
#include "game_ui.h"

int SCREEN_WIDTH;
int SCREEN_HEIGHT;
double CELL_SIZE;
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

    SCREEN_WIDTH = (w == 0) ? 950 : w;
    SCREEN_HEIGHT = (h == 0) ? 750 : h;

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

    Game *gol = game_init(55, 40);
    int MENU_X = SCREEN_WIDTH - MENU_WIDTH;

    Text *status = create_text(&(SDL_Rect){MENU_X + 16, 16, 130, 40}, font, "Editing", &(SDL_Color){0, 0, 0, 225});
    Text *gen_count = create_text(&(SDL_Rect){MENU_X + 16, 54, 130, 40}, font, "Generation 0", &(SDL_Color){45, 45, 45, 225});
    Button *play_pause_btn = create_button(&(SDL_Rect){MENU_X + 16, 100, 130, 40}, font, "Play", play_pause, gol);
    Button *next_step_btn = create_button(&(SDL_Rect){MENU_X + 154, 100, 130, 40}, font, "Step", next_step, gol);
    Button *back_step_btn = create_button(&(SDL_Rect){MENU_X + 16, 150, 130, 40}, font, "Back", back_step, gol);
    Button *stop_btn = create_button(&(SDL_Rect){MENU_X + 154, 150, 130, 40}, font, "Stop", stop, gol);
    Button *clear_btn = create_button(&(SDL_Rect){MENU_X + 16, 200, 268, 40}, font, "Clear", clear_canvas, gol);
    Button *wrap_btn = create_button(&(SDL_Rect){MENU_X + 16, 440, 268, 40}, font, "Wrap edges: off", wrap, gol);

    Text *column_text = create_text(&(SDL_Rect){MENU_X + 16,(40 - TTF_FontHeight(font)) / 2 + 270, 120, 40}, font, "Columns", &(SDL_Color){0, 0, 0, 225});
    Text *row_text = create_text(&(SDL_Rect){MENU_X + 16,(40 - TTF_FontHeight(font)) / 2 + 320, 120, 40}, font, "Rows", &(SDL_Color){0, 0, 0, 225});
    Text *delay_text = create_text(&(SDL_Rect){MENU_X + 16,(40 - TTF_FontHeight(font)) / 2 + 370, 120, 40}, font, "Delay(ms)", &(SDL_Color){0, 0, 0, 225});

    char buffer[TEXTBOX_MAX];
    snprintf(buffer, sizeof(buffer), "%d", gol->now->cols);
    TextBox *columns = create_text_box(&(SDL_Rect){MENU_X + 164, 270, 120, 40}, font, buffer, 6 ,(40 - TTF_FontHeight(font)) / 2);
    snprintf(buffer, sizeof(buffer), "%d", gol->now->rows);
    TextBox *rows = create_text_box(&(SDL_Rect){MENU_X + 164, 320, 120, 40}, font, buffer, 6, (40 - TTF_FontHeight(font)) / 2);
    snprintf(buffer, sizeof(buffer), "%d", gol->delay_ms);
    TextBox *delay = create_text_box(&(SDL_Rect){MENU_X + 164, 370, 120, 40}, font, buffer, 6, (40 - TTF_FontHeight(font)) / 2);
    int _cols = 0;
    int _rows = 0;
    SDL_Event e;
    bool quit = false;
    while(quit == false){
        update_game_canvas(renderer, gol);
        status->render_update(renderer, status);
        gen_count->render_update(renderer, gen_count);
        play_pause_btn->render_update(renderer, play_pause_btn);
        next_step_btn->render_update(renderer, next_step_btn);
        back_step_btn->render_update(renderer, back_step_btn);
        stop_btn->render_update(renderer, stop_btn);
        clear_btn->render_update(renderer, clear_btn);
        wrap_btn->render_update(renderer, wrap_btn);

        column_text->render_update(renderer, column_text);
        row_text->render_update(renderer, row_text);
        delay_text->render_update(renderer, delay_text);
        columns->render_update(renderer, columns);
        rows->render_update(renderer, rows);
        delay->render_update(renderer, delay);
        SDL_RenderPresent(renderer);

        if ((SCREEN_WIDTH - MENU_WIDTH)/gol->now->cols < (SCREEN_HEIGHT)/gol->now->rows)
            CELL_SIZE = (SCREEN_WIDTH - MENU_WIDTH)/gol->now->cols;
        else    
            CELL_SIZE = (SCREEN_HEIGHT)/gol->now->rows;

        snprintf(gen_count->text, sizeof gen_count->text, "%s %d", "Generation", gol->history->count);
        if (!delay->focused && gol->mode != MODE_RUN) gol->delay_ms = atoi(delay->text);
        if (gol->mode == MODE_EDIT){
            mouse_update(gol);
            snprintf(play_pause_btn->label, sizeof play_pause_btn->label, "%s", "Play");
            snprintf(status->text, sizeof status->text, "%s", "Editing");

            if (_cols != atoi(columns->text) || _rows != atoi(rows->text)){
                if (!columns->focused && !rows->focused){
                    game_resize(gol, atoi(columns->text), atoi(rows->text));
                    _cols = atoi(columns->text);
                    _rows = atoi(rows->text);
                }
            }
            next_step_btn->enabled = true;
            back_step_btn->enabled = false;
            stop_btn->enabled = false;
            clear_btn->enabled = true;
            wrap_btn->enabled = true;
            columns->enabled = true;
            rows->enabled = true;
            delay->enabled = true;
        }
        else if (gol->mode == MODE_RUN){
            snprintf(play_pause_btn->label, sizeof play_pause_btn->label, "%s", "Pause");
            snprintf(status->text, sizeof status->text, "%s", "Running");
            Uint32 now = SDL_GetTicks();
            if (now - gol->last_step >= gol->delay_ms){
                gol->last_step = now;
                next_step(next_step_btn, gol);
            }
            next_step_btn->enabled = false;
            back_step_btn->enabled = false;
            stop_btn->enabled = true;
            clear_btn->enabled = false;
            wrap_btn->enabled = false;
            columns->enabled = false;
            rows->enabled = false;
            delay->enabled = false;
        }
        else if (gol->mode == MODE_PAUSED){
            snprintf(play_pause_btn->label, sizeof play_pause_btn->label, "%s", "Play");
            snprintf(status->text, sizeof status->text, "%s", "Paused");
            next_step_btn->enabled = true;
            if (gol->history->count == 0) back_step_btn->enabled = false;
            else back_step_btn->enabled = true;
            stop_btn->enabled = true;
            clear_btn->enabled = false;
            wrap_btn->enabled = false;
            columns->enabled = false;
            rows->enabled = false;
            delay->enabled = true;
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

            if (e.type == SDL_KEYDOWN &&
                e.key.keysym.sym == SDLK_RIGHT){
                next_step(NULL, gol);
            }
            if (e.type == SDL_KEYDOWN &&
                e.key.keysym.sym == SDLK_SPACE){
                play_pause(play_pause_btn, gol);
            }
            if (e.type == SDL_KEYDOWN &&
                e.key.keysym.sym == SDLK_LEFT){
                back_step(NULL, gol);
            }
        }
    }

    free(status);
    free(gen_count);
    free(play_pause_btn);
    free(next_step_btn);
    free(back_step_btn);
    free(stop_btn);
    free(clear_btn);
    free(wrap_btn);

    free(column_text);
    free(row_text);
    free(delay_text);
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
