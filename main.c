
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int main(){
    printf("SDL2 started\n");

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
        SDL_Quit();
        return -1;
    }

    SDL_Surface *screenSurface = SDL_GetWindowSurface(window);
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
    SDL_UpdateWindowSurface(window);

    SDL_Event e;
    bool quit = false;
    while( quit == false ){ 
        while (SDL_PollEvent(&e)){ if(e.type == SDL_QUIT) quit = true; }
    }


    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
