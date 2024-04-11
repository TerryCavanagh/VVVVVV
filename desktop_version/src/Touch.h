#ifndef TOUCH_H
#define TOUCH_H

#include <SDL.h>

#include <vector>

struct VVV_Finger
{
    float x;
    float y;
    bool pressed;
    bool on_button;
    SDL_FingerID id;
};

enum TouchButtonID
{
    /* General */
    TOUCH_BUTTON_LEFT,
    TOUCH_BUTTON_RIGHT,

    /* Gameplay */
    TOUCH_BUTTON_MAP,

    /* Menus */
    TOUCH_BUTTON_CANCEL,
    TOUCH_BUTTON_CONFIRM,

    NUM_TOUCH_BUTTONS
};

struct TouchButton
{
    int x;
    int y;
    int width;
    int height;
    bool down;
    bool active;
    SDL_Texture* image;
    SDL_FingerID fingerId;
};

namespace touch
{
    extern std::vector<VVV_Finger> fingers;
    extern TouchButton buttons[NUM_TOUCH_BUTTONS];
    extern float scale;

    void refresh_buttons(void);
    void reset(void);
    void update_buttons(void);

    void init(void);
    void render(void);

    bool button_tapped(TouchButtonID button);
    bool touching_right(void);
    bool screen_down(void);
}

#endif /* TOUCH_H */
