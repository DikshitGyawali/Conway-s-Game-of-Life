#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include "menu_ui.h"


void update_textbox_render(SDL_Renderer *renderer, TextBox *text_box){
    //box
    Uint8 r = 255, g = 255, b = 255, a = 255;
    if (!text_box->enabled){
        r = 240; g = 241; b = 243; a = 255;
    }
    if (SDL_SetRenderDrawColor(renderer, r, g, b, a) < 0 ||
        SDL_RenderFillRect(renderer, &text_box->rect) < 0){
        printf("Rectangle fill render failed: %s\n", SDL_GetError());
        return;
    }

    
    if (text_box->focused && text_box->enabled){ r = 0; g = 255; b = 0; a = 255; }
    else { r = 140; g = 140; b = 140; a = 255;}

    if (SDL_SetRenderDrawColor(renderer, r, g, b, a) < 0 ||
        SDL_RenderDrawRect(renderer, &text_box->rect) < 0){
        printf("Rectangle outline render failed: %s\n", SDL_GetError());
        return;
    }

    if (text_box->text[0] == '\0') return;

    //text
    r = 0; g = 0; b = 0; a = 255;
    if(!text_box->enabled) { r = 164; g = 168; b = 175, a = 255; }
    SDL_Surface *s = TTF_RenderUTF8_Blended(text_box->font, text_box->text, (SDL_Color){r, g, b, a});  // text becomes pixels
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

    if(!text_box->enabled) text_box->focused = false;

    if (e->type == SDL_MOUSEBUTTONDOWN){
        int x = e->button.x;
        int y = e->button.y;

        text_box->focused =
            x >= text_box->rect.x &&
            x < text_box->rect.x + text_box->rect.w &&
            y >= text_box->rect.y &&
            y < text_box->rect.y + text_box->rect.h;
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
        if (e->key.keysym.sym == SDLK_RETURN || e->key.keysym.sym == SDLK_ESCAPE){
            text_box->focused = false;
        }
    }
}

TextBox *create_text_box(SDL_Rect *box, TTF_Font *f, const char *ini_text, int pad_x, int pad_y){
    if (pad_x < 0 || pad_y < 0 || pad_x >= box->w || pad_y >= box->h) return NULL;

    TextBox *text_box = (TextBox *)malloc(sizeof(TextBox));
    if (text_box == NULL) return NULL;

    memcpy(&text_box->rect, box, sizeof(SDL_Rect));
    text_box->font = f;
    snprintf(text_box->text, sizeof text_box->text, "%s", ini_text);
    text_box->pad_x = pad_x;
    text_box->pad_y = pad_y;
    text_box->render_update = update_textbox_render;
    text_box->input_update = update_textbox_input;
    text_box->focused = false;
    text_box->enabled = true;
    return text_box;
}



void update_button_render(SDL_Renderer *renderer, Button *button){
    //box
    SDL_Color face = {255, 255, 255, 255};
    SDL_Color border = {140, 140, 140, 255};
    SDL_Color text = {0, 0, 0, 255};

    if (!button->enabled){
        face = (SDL_Color){240, 241, 243, 255};
        border = (SDL_Color){201, 204, 209, 255};
        text = (SDL_Color){164, 168, 175, 255};
    }
    else if (button->pressed && button->hovered) face = (SDL_Color){218, 223, 232, 255};
    else if (button->hovered) face = (SDL_Color){243, 246, 250, 255};

    if (SDL_SetRenderDrawColor(renderer, face.r, face.g, face.b, face.a) < 0 ||
        SDL_RenderFillRect(renderer, &button->rect) < 0){
        printf("Button fill failed: %s\n", SDL_GetError());
        return;
    }
    if (SDL_SetRenderDrawColor(renderer, border.r, border.g, border.b, border.a) < 0 ||
        SDL_RenderDrawRect(renderer, &button->rect) < 0){
        printf("Button outline failed: %s\n", SDL_GetError());
        return;
    }

    if (button->label[0] == '\0') return;
    //label
    SDL_Surface *s = TTF_RenderUTF8_Blended(button->font, button->label, text);
    if (s == NULL){
        printf("Text rendering failed: %s\n", TTF_GetError());
        return;
    }
    SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
    if (t == NULL){
        printf("Text rendering failed: %s\n", SDL_GetError());
        SDL_FreeSurface(s);
        return;
    }
    const SDL_Rect rect = {
        button->rect.x + (button->rect.w - s->w) / 2,
        button->rect.y + (button->rect.h - s->h) / 2,
        s->w, s->h
    };
    if (SDL_RenderCopy(renderer, t, NULL, &rect) < 0){
        printf("SDL_RenderCopy failed: %s\n", SDL_GetError());
    }

    SDL_DestroyTexture(t);
    SDL_FreeSurface(s);
}

void update_button_input(Button *button, SDL_Event *e){
    if (e->type == SDL_MOUSEMOTION){
        SDL_Point p = { e->motion.x, e->motion.y };
        button->hovered = SDL_PointInRect(&p, &button->rect);
    }
    if (e->type == SDL_WINDOWEVENT && e->window.event == SDL_WINDOWEVENT_LEAVE){
        button->hovered = false;  // the mouse left the window
    }

    if (!button->enabled){
        button->pressed = false;
        return;
    }

    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT){
        SDL_Point p = { e->button.x, e->button.y };
        button->pressed = SDL_PointInRect(&p, &button->rect);
    }

    if (e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_LEFT){
        SDL_Point p = { e->button.x, e->button.y };
        bool inside = SDL_PointInRect(&p, &button->rect);
        if (button->on_click != NULL && button->pressed && inside) button->on_click(button , button->user_data);
        button->pressed = false;
    }
}

Button *create_button(SDL_Rect *box, TTF_Font *f, const char *label, void (*on_click)(Button *,void *), void *user_data){
    Button *button = (Button *)malloc(sizeof(Button));
    if (button == NULL) return NULL;

    button->rect = *box;
    button->font = f;
    snprintf(button->label, sizeof button->label, "%s", label);
    button->hovered = false;
    button->pressed = false;
    button->enabled = true;
    button->on_click = on_click;
    button->user_data = user_data;
    button->render_update = update_button_render;
    button->input_update = update_button_input;

    SDL_StartTextInput();

    return button;
}


void update_text_render(SDL_Renderer *renderer, Text *text){
    SDL_Surface *s = TTF_RenderUTF8_Blended(text->font, text->text, text->color);
    if (s == NULL){
        printf("Text rendering failed: %s\n", TTF_GetError());
        return;
    }
    SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
    if (t == NULL){
        printf("Text rendering failed: %s\n", SDL_GetError());
        return;
    }
    const SDL_Rect rect = {text->rect.x, text->rect.y, s->w, s->h};

    if (SDL_RenderCopy(renderer, t, NULL, &rect) < 0){
        printf("SLD_rendercopy failed: %s\n", SDL_GetError());
        return;
    }

    SDL_DestroyTexture(t);
    SDL_FreeSurface(s);
}

Text *create_text(SDL_Rect *box, TTF_Font *f, const char *string, SDL_Color *color){
    Text *text = (Text *)malloc(sizeof(Text));
    if (text == NULL) return NULL;

    text->rect = *box;
    text->color = *color;
    text->font = f;
    snprintf(text->text, sizeof text->text, "%s", string);
    text->render_update = update_text_render;
    return text;
}
