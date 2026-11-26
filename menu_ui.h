#ifndef UI_H
#define UI_H

#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#define TEXTBOX_MAX 8

typedef struct TextBox{
    SDL_Rect rect;
    TTF_Font *font;
    char text[TEXTBOX_MAX];
    int pad_x, pad_y;
    void (*render_update)(SDL_Renderer *, struct TextBox *);
    void (*input_update)(struct TextBox *, SDL_Event *);
    bool focused;
    bool enabled;
} TextBox;


TextBox *create_text_box(SDL_Rect *box, TTF_Font *f, const char *ini_text, int pad_x, int pad_y);

#define BUTTON_LABEL_MAX 16

typedef struct Button{
    SDL_Rect rect;
    TTF_Font *font;
    char label[BUTTON_LABEL_MAX];
    bool hovered, pressed, enabled;
    void (*on_click)(struct Button *this, void *user_data);  // called once per click, on mouse release
    void *user_data; // handed to on_click (can be NULL)
    void (*render_update)(SDL_Renderer *, struct Button *);
    void (*input_update)(struct Button *, SDL_Event *);
} Button;


Button *create_button(SDL_Rect *box, TTF_Font *f, const char *label, void (*on_click)(Button *,void *), void *user_data);

#define TEXT_MAX 256

typedef struct Text{
    SDL_Rect rect;
    TTF_Font *font;
    SDL_Color color;
    char text[TEXT_MAX];
    void (*render_update)(SDL_Renderer *, struct Text *);
} Text;


Text *create_text(SDL_Rect *box, TTF_Font *f, const char *string, SDL_Color *color);



#endif
