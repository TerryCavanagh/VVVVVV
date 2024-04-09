#ifndef TOUCH_H
#define TOUCH_H

#include <SDL.h>

#include <string>
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

    /* Map */
    TOUCH_BUTTON_MAP_MAP,
    TOUCH_BUTTON_MAP_CREW,
    TOUCH_BUTTON_MAP_STATS,
    TOUCH_BUTTON_MAP_QUIT,

    /* Map - Warp to ship */
    TOUCH_BUTTON_MAP_SHIP_WARP,

    /* Map - Quit buttons */
    TOUCH_BUTTON_MAP_QUIT_SAVE,
    TOUCH_BUTTON_MAP_QUIT_SAVEEXIT,
    TOUCH_BUTTON_MAP_QUIT_EXIT,

    /* Map - Back */
    TOUCH_BUTTON_MAP_BACK,

    /* Quit */
    TOUCH_BUTTON_QUIT_YES,
    TOUCH_BUTTON_QUIT_NO,

    NUM_TOUCH_BUTTONS
};

enum TouchButtonType
{
    TOUCH_BUTTON_TYPE_NONE,
    TOUCH_BUTTON_TYPE_MENU,
    TOUCH_BUTTON_TYPE_MENU_TOGGLE,
    TOUCH_BUTTON_TYPE_MAP
};

struct TouchButton
{
    int x;
    int y;
    int width;
    int height;
    bool down; // Whether the button is currently being pressed
    bool pressed; // Whether the button was pressed down and not dragged onto
    bool active; // Whether the button is currently active, i.e. visible and usable
    bool core; // Whether the button is a "core" button, one which always exists (but not necessarily active)
    bool ui; // Whether the button is on the UI layer or not
    int id; // The ID for the button, mainly used for menu buttons
    bool disabled; // Whether the button is disabled or not (gray and can't use), different from active
    bool checked; // If this is a checkbox, whether it's checked or not
    std::string text; // The text for the button, if it doesn't have an image
    SDL_Texture* image; // The image that gets displayed on the button, can be NULL
    SDL_FingerID fingerId;
    TouchButtonType type;
};

namespace touch
{
    extern std::vector<VVV_Finger> fingers;
    extern TouchButton buttons[NUM_TOUCH_BUTTONS];
    extern std::vector<TouchButton> dynamic_buttons;
    extern std::vector<TouchButton*> all_buttons;
    extern int scale;
    extern bool scroll;

    void refresh_buttons(void);
    void reset(void);
    void on_menu_create(void);
    void update_buttons(void);

    TouchButton create_button(int x, int y, int width, int height, std::string text);
    void register_button(TouchButton button);

    void create_menu_button(int x, int y, int width, int height, std::string text, int id);
    void create_menu_button(int x, int y, int width, int height, std::string text, int id, bool disabled);
    void create_toggle_button(int x, int y, int width, int height, std::string text, int id, bool checked);

    void remove_dynamic_buttons(void);

    void on_button_up(TouchButton* button);

    void init(void);
    void render_buttons(void);
    void render_buttons(int r, int g, int b);
    void render_buttons(int r, int g, int b, bool textbox_style);
    void render_buttons(int scale, bool ui, int r, int g, int b);
    void render(void);

    bool button_tapped(TouchButtonID button);
    bool touching_right(void);
    bool screen_down(void);
}

#endif /* TOUCH_H */
