
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int main(){
    printf("SDL2 started\n");
    int return_value = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Conway's Game of Life",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (window == NULL){
        printf("Window creation failed: %s\n", SDL_GetError());
        return_value = -1;
        goto clean_up;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL){
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return_value = -1;
        goto clean_up;
    }

    SDL_Rect cell = {SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &cell);
    
    SDL_RenderPresent(renderer);


    SDL_Event e;
    bool quit = false;
    while( quit == false ){ 
        while (SDL_PollEvent(&e)){ if(e.type == SDL_QUIT) quit = true; }
    }


clean_up:
    if (window != NULL) SDL_DestroyWindow(window);
    if (renderer != NULL) SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return return_value;
}
