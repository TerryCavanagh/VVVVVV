#ifndef KEYPOLL_H
#define KEYPOLL_H

#include <string>
#include <vector>
#include <map> // FIXME: I should feel very bad for using C++ -flibit

#include "SDL.h"

#include "Screen.h"

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

	bool escapeWasPressedPreviously;
	bool quitProgram;
	bool toggleFullscreen;

	int sensitivity;

	void setSensitivity(int _value);

	KeyPoll();

	void enabletextentry();

	void disabletextentry();

	void Poll();

	bool isDown(SDL_Keycode key);

	bool isUp(SDL_Keycode key);

	bool isDown(std::vector<SDL_GameControllerButton> buttons);
	bool isDown(SDL_GameControllerButton button);
	bool controllerButtonDown();
	bool controllerWantsLeft(bool includeVert);
	bool controllerWantsRight(bool includeVert);

	int leftbutton, rightbutton, middlebutton;
	int mx, my;

	bool textentrymode;
	int keyentered, keybufferlen;
	bool pressedbackspace;
	std::string keybuffer;

	bool linealreadyemptykludge;

	Uint64 pauseStart;

private:
	std::map<SDL_JoystickID, SDL_GameController*> controllers;
	std::map<SDL_GameControllerButton, bool> buttonmap;
	int xVel, yVel;
	bool useFullscreenSpaces;
	Uint32 wasFullscreen;
};

extern KeyPoll key;

#endif /* KEYPOLL_H */
