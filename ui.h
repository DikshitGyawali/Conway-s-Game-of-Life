#ifndef UI_H
#define UI_H

#include <SDL2/SDL_ttf.h>

#define TEXTBOX_MAX 8

typedef struct TextBox{
    SDL_Rect rect;
    TTF_Font *font;
    char text[TEXTBOX_MAX];
    int pad_x, pad_y;
    void (*render_update)(SDL_Renderer *, struct TextBox *);
    void (*input_update)(struct TextBox *, SDL_Event *);
    bool focused;
} TextBox;


TextBox *create_text_box(SDL_Rect *box, TTF_Font *f, int pad_x, int pad_y);


#endif
