#define KEY_DEFINITION
#include "KeyPoll.h"

#include <string.h>

#include "Alloc.h"
#include "ButtonGlyphs.h"
#include "Constants.h"
#include "Exit.h"
#include "Game.h"
#include "GlitchrunnerMode.h"
#include "Graphics.h"
#include "GraphicsUtil.h"
#include "Localization.h"
#include "LocalizationStorage.h"
#include "Music.h"
#include "Screen.h"
#include "UTF8.h"
#include "Vlogging.h"

bool SaveScreenshot(void);

int inline KeyPoll::getThreshold(void)
{
    switch (sensitivity)
    {
    case 0:
        return 28000;
    case 1:
        return 16000;
    case 2:
        return 8000;
    case 3:
        return 4000;
    case 4:
        return 2000;
    }

    return 8000;

}

KeyPoll::KeyPoll(void)
{
    xVel = 0;
    yVel = 0;
    // 0..5
    sensitivity = 2;

    keybuffer="";
    leftbutton=0; rightbutton=0; middlebutton=0;
    mousex = 0;
    mousey = 0;
    resetWindow = 0;
    pressedbackspace=false;

    linealreadyemptykludge = false;

    isActive = true;
}

void KeyPoll::enabletextentry(void)
{
    keybuffer="";
    SDL_StartTextInput();
}

void KeyPoll::disabletextentry(void)
{
    SDL_StopTextInput();
}

bool KeyPoll::textentry(void)
{
    return SDL_IsTextInputActive() == SDL_TRUE;
}

void KeyPoll::toggleFullscreen(void)
{
    gameScreen.toggleFullScreen();

    keymap.clear(); /* we lost the input due to a new window. */
    if (GlitchrunnerMode_less_than_or_equal(Glitchrunner2_2))
    {
        game.press_left = false;
        game.press_right = false;
        game.press_action = true;
        game.press_map = false;
    }
}

static int changemousestate(
    int timeout,
    const bool show,
    const bool hide
) {
    int prev;
    int new_;

    if (timeout > 0)
    {
        return --timeout;
    }

    /* If we want to both show and hide at the same time, prioritize showing */
    if (show)
    {
        new_ = SDL_ENABLE;
    }
    else if (hide)
    {
        new_ = SDL_DISABLE;
    }
    else
    {
        return timeout;
    }

    prev = SDL_ShowCursor(SDL_QUERY);

    if (prev == new_)
    {
        return timeout;
    }

    SDL_ShowCursor(new_);

    switch (new_)
    {
    case SDL_DISABLE:
        timeout = 0;
        break;
    case SDL_ENABLE:
        timeout = 30;
        break;
    }

    return timeout;
}

void KeyPoll::Poll(void)
{
    static int raw_mousex = 0;
    static int raw_mousey = 0;
    static int mousetoggletimeout = 0;
    bool showmouse = false;
    bool hidemouse = false;
    bool altpressed = false;
    bool fullscreenkeybind = false;
    SDL_GameController *controller = NULL;
    SDL_Event evt;
    while (SDL_PollEvent(&evt))
    {
        switch (evt.type)
        {
        /* Keyboard Input */
        case SDL_KEYDOWN:
        {
            keymap[evt.key.keysym.sym] = true;

            if (evt.key.keysym.sym == SDLK_BACKSPACE)
            {
                pressedbackspace = true;
            }

#ifdef __APPLE__ /* OSX prefers the command keys over the alt keys. -flibit */
            altpressed = keymap[SDLK_LGUI] || keymap[SDLK_RGUI];
#else
            altpressed = keymap[SDLK_LALT] || keymap[SDLK_RALT];
#endif
            bool returnpressed = evt.key.keysym.sym == SDLK_RETURN;
            bool fpressed = evt.key.keysym.sym == SDLK_f;
            bool f11pressed = evt.key.keysym.sym == SDLK_F11;
            if ((altpressed && (returnpressed || fpressed)) || f11pressed)
            {
                fullscreenkeybind = true;
            }

            if (loc::show_translator_menu && evt.key.keysym.sym == SDLK_F8 && !evt.key.repeat)
            {
                /* Reload language files */
                loc::loadtext(false);
                graphics.grphx.init_translations();
                music.playef(Sound_COIN);
            }

            if (evt.key.keysym.sym == SDLK_F6 && !evt.key.repeat)
            {
                SaveScreenshot();
            }

            BUTTONGLYPHS_keyboard_set_active(true);

            if (textentry())
            {
                if (evt.key.keysym.sym == SDLK_BACKSPACE && !keybuffer.empty())
                {
                    keybuffer.erase(UTF8_backspace(keybuffer.c_str(), keybuffer.length()));
                    if (keybuffer.empty())
                    {
                        linealreadyemptykludge = true;
                    }
                }
                else if (    evt.key.keysym.sym == SDLK_v &&
                        keymap[SDLK_LCTRL]    )
                {
                    char* text = SDL_GetClipboardText();
                    if (text != NULL)
                    {
                        keybuffer += text;
                        VVV_free(text);
                    }
                }
                else if (    evt.key.keysym.sym == SDLK_x &&
                        keymap[SDLK_LCTRL]    )
                {
                    if (SDL_SetClipboardText(keybuffer.c_str()) == 0)
                    {
                        keybuffer = "";
                    }
                }
            }
            break;
        }
        case SDL_KEYUP:
            keymap[evt.key.keysym.sym] = false;
            if (evt.key.keysym.sym == SDLK_BACKSPACE)
            {
                pressedbackspace = false;
            }
            break;
        case SDL_TEXTINPUT:
            if (!altpressed)
            {
                keybuffer += evt.text.text;
            }
            break;

        /* Mouse Input */
        case SDL_MOUSEMOTION:
            raw_mousex = evt.motion.x;
            raw_mousey = evt.motion.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
            switch (evt.button.button)
            {
            case SDL_BUTTON_LEFT:
                raw_mousex = evt.button.x;
                raw_mousey = evt.button.y;
                leftbutton = 1;
                break;
            case SDL_BUTTON_RIGHT:
                raw_mousex = evt.button.x;
                raw_mousey = evt.button.y;
                rightbutton = 1;
                break;
            case SDL_BUTTON_MIDDLE:
                raw_mousex = evt.button.x;
                raw_mousey = evt.button.y;
                middlebutton = 1;
                break;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            switch (evt.button.button)
            {
            case SDL_BUTTON_LEFT:
                raw_mousex = evt.button.x;
                raw_mousey = evt.button.y;
                leftbutton=0;
                break;
            case SDL_BUTTON_RIGHT:
                raw_mousex = evt.button.x;
                raw_mousey = evt.button.y;
                rightbutton=0;
                break;
            case SDL_BUTTON_MIDDLE:
                raw_mousex = evt.button.x;
                raw_mousey = evt.button.y;
                middlebutton=0;
                break;
            }
            break;

        /* Controller Input */
        case SDL_CONTROLLERBUTTONDOWN:
            buttonmap[(SDL_GameControllerButton) evt.cbutton.button] = true;
            BUTTONGLYPHS_keyboard_set_active(false);

            controller = controllers[evt.cbutton.which];
            BUTTONGLYPHS_update_layout(controller);
            break;
        case SDL_CONTROLLERBUTTONUP:
            buttonmap[(SDL_GameControllerButton) evt.cbutton.button] = false;
            break;
        case SDL_CONTROLLERAXISMOTION:
        {
            const int threshold = getThreshold();
            switch (evt.caxis.axis)
            {
            case SDL_CONTROLLER_AXIS_LEFTX:
                if (    evt.caxis.value > -threshold &&
                    evt.caxis.value < threshold    )
                {
                    xVel = 0;
                }
                else
                {
                    xVel = (evt.caxis.value > 0) ? 1 : -1;
                }
                break;
            case SDL_CONTROLLER_AXIS_LEFTY:
                if (    evt.caxis.value > -threshold &&
                    evt.caxis.value < threshold    )
                {
                    yVel = 0;
                }
                else
                {
                    yVel = (evt.caxis.value > 0) ? 1 : -1;
                }
                break;
            }
            BUTTONGLYPHS_keyboard_set_active(false);

            controller = controllers[evt.caxis.which];
            BUTTONGLYPHS_update_layout(controller);
            break;
        }
        case SDL_CONTROLLERDEVICEADDED:
        {
            controller = SDL_GameControllerOpen(evt.cdevice.which);
            vlog_info(
                "Opened SDL_GameController ID #%i, %s",
                evt.cdevice.which,
                SDL_GameControllerName(controller)
            );
            controllers[SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller))] = controller;
            BUTTONGLYPHS_keyboard_set_active(false);
            BUTTONGLYPHS_update_layout(controller);
            break;
        }
        case SDL_CONTROLLERDEVICEREMOVED:
        {
            controller = controllers[evt.cdevice.which];
            controllers.erase(evt.cdevice.which);
            vlog_info("Closing %s", SDL_GameControllerName(controller));
            SDL_GameControllerClose(controller);
            if (controllers.empty())
            {
                BUTTONGLYPHS_keyboard_set_active(true);
            }
            break;
        }

        /* Window Events */
        case SDL_WINDOWEVENT:
            switch (evt.window.event)
            {
            /* Window Resize */
            case SDL_WINDOWEVENT_RESIZED:
                if (SDL_GetWindowFlags(
                    SDL_GetWindowFromID(evt.window.windowID)
                ) & SDL_WINDOW_INPUT_FOCUS)
                {
                    resetWindow = true;
                }
                break;

            /* Window Focus */
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                if (!game.disablepause)
                {
                    isActive = true;
                    if ((!game.disableaudiopause || !game.disabletemporaryaudiopause) && music.currentsong != -1)
                    {
                        music.resume();
                        music.resumeef();
                    }
                }
                if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0)
                {
                    if (wasFullscreen)
                    {
                        gameScreen.isWindowed = false;
                        SDL_SetWindowFullscreen(
                            SDL_GetWindowFromID(evt.window.windowID),
                            SDL_WINDOW_FULLSCREEN_DESKTOP
                        );
                    }
                }
                SDL_DisableScreenSaver();
                gameScreen.recacheTextures();
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                if (!game.disablepause)
                {
                    isActive = false;
                    if (!game.disableaudiopause || !game.disabletemporaryaudiopause)
                    {
                        music.pause();
                        music.pauseef();
                    }
                }

                if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0)
                {
                    wasFullscreen = !gameScreen.isWindowed;
                    gameScreen.isWindowed = true;
                    SDL_SetWindowFullscreen(
                        SDL_GetWindowFromID(evt.window.windowID),
                        0
                    );
                }
                SDL_EnableScreenSaver();
                break;

            /* Mouse Focus */
            case SDL_WINDOWEVENT_ENTER:
                SDL_DisableScreenSaver();
                break;
            case SDL_WINDOWEVENT_LEAVE:
                SDL_EnableScreenSaver();
                break;
            }
            break;

        /* Quit Event */
        case SDL_QUIT:
            VVV_exit(0);
            break;
        }

        switch (evt.type)
        {
        case SDL_KEYDOWN:
            if (evt.key.repeat == 0)
            {
                hidemouse = true;
            }
            break;
        case SDL_TEXTINPUT:
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERAXISMOTION:
            hidemouse = true;
            break;
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
            showmouse = true;
            break;
        }
    }

    mousetoggletimeout = changemousestate(
        mousetoggletimeout,
        showmouse,
        hidemouse
    );

    if (fullscreenkeybind)
    {
        toggleFullscreen();
    }

    if (gameScreen.scalingMode == SCALING_STRETCH)
    {
        /* In this mode specifically, we have to fix the mouse coordinates */
        int actualscreenwidth;
        int actualscreenheight;
        gameScreen.GetScreenSize(&actualscreenwidth, &actualscreenheight);

        mousex = raw_mousex * SCREEN_WIDTH_PIXELS / actualscreenwidth;
        mousey = raw_mousey * SCREEN_HEIGHT_PIXELS / actualscreenheight;
    }
    else
    {
        mousex = raw_mousex;
        mousey = raw_mousey;
    }
}

bool KeyPoll::isDown(SDL_Keycode key)
{
    return keymap[key];
}

bool KeyPoll::isDown(std::vector<SDL_GameControllerButton> buttons)
{
    for (size_t i = 0; i < buttons.size(); i += 1)
    {
        if (buttonmap[buttons[i]])
        {
            return true;
        }
    }
    return false;
}

bool KeyPoll::isDown(SDL_GameControllerButton button)
{
    return buttonmap[button];
}

bool KeyPoll::controllerButtonDown(void)
{
    for (
        SDL_GameControllerButton button = SDL_CONTROLLER_BUTTON_A;
        button < SDL_CONTROLLER_BUTTON_DPAD_UP;
        button = (SDL_GameControllerButton) (button + 1)
    ) {
        if (isDown(button))
        {
            return true;
        }
    }
    return false;
}

bool KeyPoll::controllerWantsLeft(bool includeVert)
{
    return (    buttonmap[SDL_CONTROLLER_BUTTON_DPAD_LEFT] ||
            xVel < 0 ||
            (    includeVert &&
                (    buttonmap[SDL_CONTROLLER_BUTTON_DPAD_UP] ||
                    yVel < 0    )    )    );
}

bool KeyPoll::controllerWantsRight(bool includeVert)
{
    return (    buttonmap[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] ||
            xVel > 0 ||
            (    includeVert &&
                (    buttonmap[SDL_CONTROLLER_BUTTON_DPAD_DOWN] ||
                    yVel > 0    )    )    );
}

bool KeyPoll::controllerWantsUp(void)
{
    return buttonmap[SDL_CONTROLLER_BUTTON_DPAD_UP] || yVel < 0;
}

bool KeyPoll::controllerWantsDown(void)
{
    return buttonmap[SDL_CONTROLLER_BUTTON_DPAD_DOWN] || yVel > 0;
}
