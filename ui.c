#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include "ui.h"


void update_textbox_render(SDL_Renderer *renderer, TextBox *text_box){
    //box
    if (SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255) < 0){
        printf("DrawColor failed: %s\n", SDL_GetError());
        return;
    }
    if (SDL_RenderFillRect(renderer, &text_box->rect) < 0){
        printf("Rectangle fill render failed: %s\n", SDL_GetError());
        return;
    }
    Uint8 r, g, b, a;
    if (text_box->focused){ r = 0; g = 255; b = 0; a = 255; }
    else { r = 140; g = 140; b = 140; a = 255;}

    if (SDL_SetRenderDrawColor(renderer, r, g, b, a) < 0){
        printf("DrawColor failed: %s\n", SDL_GetError());
        return;
    }
    if (SDL_RenderDrawRect(renderer, &text_box->rect) < 0){
        printf("Rectangle outline render failed: %s\n", SDL_GetError());
        return;
    }

    //text
    SDL_Surface *s = TTF_RenderUTF8_Blended(text_box->font, text_box->text, (SDL_Color){0, 0, 0, 255});  // text becomes pixels
    if (s == NULL)  {
        printf("Text rendering failed: %s\n", TTF_GetError());
        return;
    }
    SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);                      // pixels the renderer can draw
    if (t == NULL) {
        printf("Text rendering failed: %s\n", SDL_GetError());
        return;
    }
    const SDL_Rect rect = {text_box->rect.x + text_box->pad_x, text_box->rect.y + text_box->pad_y, s->w, s->h};

    if (SDL_RenderCopy(renderer, t, NULL, &rect) < 0){
        printf("SLD_rendercopy failed: %s\n", SDL_GetError());
        return;
    }

    SDL_DestroyTexture(t);
    SDL_FreeSurface(s);
}

void update_textbox_input(TextBox *text_box, SDL_Event *e){

    if (e->type == SDL_MOUSEBUTTONDOWN){
        int x = e->button.x;
        int y = e->button.y;

        text_box->focused =
            x >= text_box->rect.x &&
            x < text_box->rect.x + text_box->rect.w &&
            y >= text_box->rect.y &&
            y < text_box->rect.y + text_box->rect.h;

        if (text_box->focused) SDL_StartTextInput();
        else SDL_StopTextInput();
    }

    if (!text_box->focused) return;

    if (e->type == SDL_TEXTINPUT){
        size_t len = strlen(text_box->text);
        size_t input_len = strlen(e->text.text);

        if (len + input_len < sizeof(text_box->text)){
            memcpy(text_box->text + len, e->text.text, input_len + 1);
            text_box->text[len+input_len] = '\0';
        }
    }

    if (e->type == SDL_KEYDOWN){
        if (e->key.keysym.sym == SDLK_BACKSPACE){
            size_t len = strlen(text_box->text);
            if (len > 0) text_box->text[len - 1] = '\0';
        }
        if (e->key.keysym.sym == SDLK_RETURN){
            text_box->focused = false;
        }
    }
}

TextBox *create_text_box(SDL_Rect *box, TTF_Font *f, int pad_x, int pad_y){
    if (pad_x < 0 || pad_y < 0 || pad_x >= box->w || pad_y >= box->h) return NULL;

    TextBox *text_box = (TextBox *)malloc(sizeof(TextBox));
    if (text_box == NULL) return NULL;

    memcpy(&text_box->rect, box, sizeof(SDL_Rect));
    text_box->font = f;
    text_box->text[0] = ':';
    text_box->text[1] = ')';
    text_box->text[2] = '\0';
    text_box->pad_x = pad_x;
    text_box->pad_y = pad_y;
    text_box->render_update = update_textbox_render;
    text_box->input_update = update_textbox_input;
    text_box->focused = false;
    return text_box;
}
