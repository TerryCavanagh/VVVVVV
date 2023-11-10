#ifndef KEYPOLL_H
#define KEYPOLL_H

#include <map> // FIXME: I should feel very bad for using C++ -flibit
#include <SDL.h>
#include <string>
#include <vector>

enum Kybrd
{
    KEYBOARD_UP = SDLK_UP,
    KEYBOARD_DOWN = SDLK_DOWN,
    KEYBOARD_LEFT = SDLK_LEFT,
    KEYBOARD_RIGHT = SDLK_RIGHT,
    KEYBOARD_ENTER = SDLK_RETURN,
    KEYBOARD_SPACE = SDLK_SPACE,

    KEYBOARD_w = SDLK_w,
    KEYBOARD_s = SDLK_s,
    KEYBOARD_a = SDLK_a,
    KEYBOARD_d = SDLK_d,
    KEYBOARD_e = SDLK_e,
    KEYBOARD_m = SDLK_m,
    KEYBOARD_n = SDLK_n,

    KEYBOARD_v = SDLK_v,
    KEYBOARD_z = SDLK_z,

    KEYBOARD_BACKSPACE = SDLK_BACKSPACE
};

class KeyPoll
{
public:
    std::map<SDL_Keycode, bool> keymap;

    bool isActive;

    bool resetWindow;

    void toggleFullscreen(void);

    int sensitivity;

    int inline getThreshold(void);

    KeyPoll(void);

    void enabletextentry(void);

    void disabletextentry(void);

    void Poll(void);

    bool isDown(SDL_Keycode key);

    bool isDown(std::vector<SDL_GameControllerButton> buttons);
    bool isDown(SDL_GameControllerButton button);
    bool controllerButtonDown(void);
    bool controllerWantsLeft(bool includeVert);
    bool controllerWantsRight(bool includeVert);
    bool controllerWantsUp(void);
    bool controllerWantsDown(void);
    int controllerRumble(Uint16 intensity, Uint32 duration_ms);
    int controllerRumbleStop(void);

    int leftbutton, rightbutton, middlebutton;
    int mousex;
    int mousey;

    bool textentry(void);
    bool pressedbackspace;
    std::string keybuffer;

    bool linealreadyemptykludge;

private:
    std::map<SDL_JoystickID, SDL_GameController*> controllers;
    std::map<SDL_GameControllerButton, bool> buttonmap;
    int xVel, yVel;
    Uint32 wasFullscreen;
};

#ifndef KEY_DEFINITION
extern KeyPoll key;
#endif

#endif /* KEYPOLL_H */
