#include "KeyPoll.h"
#include "Graphics.h"
#include "Music.h"
#include <stdio.h>
#include <string.h>
#include <utf8/unchecked.h>

void KeyPoll::setSensitivity(int _value)
{
	switch (_value)
	{
		case 0:
			sensitivity = 28000;
			break;
		case 1:
			sensitivity = 16000;
			break;
		case 2:
			sensitivity = 8000;
			break;
		case 3:
			sensitivity = 4000;
			break;
		case 4:
			sensitivity = 2000;
			break;
	}

}

KeyPoll::KeyPoll()
{
	xVel = 0;
	yVel = 0;
	setSensitivity(2);

	quitProgram = 0;
	textentrymode=true;
	keybuffer="";
	leftbutton=0; rightbutton=0; middlebutton=0;
	mx=0; my=0;
	resetWindow = 0;
	toggleFullscreen = false;
	pressedbackspace=false;

	useFullscreenSpaces = false;
	if (strcmp(SDL_GetPlatform(), "Mac OS X") == 0)
	{
		useFullscreenSpaces = true;
		const char *hint = SDL_GetHint(SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES);
		if (hint != NULL)
		{
			useFullscreenSpaces = (strcmp(hint, "1") == 0);
		}
	}

	linealreadyemptykludge = false;

	pauseStart = 0;
}

void KeyPoll::enabletextentry()
{
	keybuffer="";
	textentrymode = true;
	SDL_StartTextInput();
}

void KeyPoll::disabletextentry()
{
	textentrymode = false;
	SDL_StopTextInput();
}

void KeyPoll::Poll()
{
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
			bool altpressed = keymap[SDLK_LGUI] || keymap[SDLK_RGUI];
#else
			bool altpressed = keymap[SDLK_LALT] || keymap[SDLK_RALT];
#endif
			bool returnpressed = evt.key.keysym.sym == SDLK_RETURN;
			bool fpressed = evt.key.keysym.sym == SDLK_f;
			bool f11pressed = evt.key.keysym.sym == SDLK_F11;
			if ((altpressed && (returnpressed || fpressed)) || f11pressed)
			{
				toggleFullscreen = true;
			}

			if (textentrymode)
			{
				if (evt.key.keysym.sym == SDLK_BACKSPACE && !keybuffer.empty())
				{
					std::string::iterator iter = keybuffer.end();
					utf8::unchecked::prior(iter);
					keybuffer = keybuffer.substr(0, iter - keybuffer.begin());
					if (keybuffer.empty())
					{
						linealreadyemptykludge = true;
					}
				}
				else if (	evt.key.keysym.sym == SDLK_v &&
						keymap[SDLK_LCTRL]	)
				{
					keybuffer += SDL_GetClipboardText();
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
			keybuffer += evt.text.text;
			break;

		/* Mouse Input */
		case SDL_MOUSEMOTION:
			mx = evt.motion.x;
			my = evt.motion.y;
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (evt.button.button)
			{
			case SDL_BUTTON_LEFT:
				mx = evt.button.x;
				my = evt.button.y;
				leftbutton = 1;
				break;
			case SDL_BUTTON_RIGHT:
				mx = evt.button.x;
				my = evt.button.y;
				rightbutton = 1;
				break;
			case SDL_BUTTON_MIDDLE:
				mx = evt.button.x;
				my = evt.button.y;
				middlebutton = 1;
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch (evt.button.button)
			{
			case SDL_BUTTON_LEFT:
				mx = evt.button.x;
				my = evt.button.y;
				leftbutton=0;
				break;
			case SDL_BUTTON_RIGHT:
				mx = evt.button.x;
				my = evt.button.y;
				rightbutton=0;
				break;
			case SDL_BUTTON_MIDDLE:
				mx = evt.button.x;
				my = evt.button.y;
				middlebutton=0;
				break;
			}
			break;

		/* Controller Input */
		case SDL_CONTROLLERBUTTONDOWN:
			buttonmap[(SDL_GameControllerButton) evt.cbutton.button] = true;
			break;
		case SDL_CONTROLLERBUTTONUP:
			buttonmap[(SDL_GameControllerButton) evt.cbutton.button] = false;
			break;
		case SDL_CONTROLLERAXISMOTION:
			switch (evt.caxis.axis)
			{
			case SDL_CONTROLLER_AXIS_LEFTX:
				if (	evt.caxis.value > -sensitivity &&
					evt.caxis.value < sensitivity	)
				{
					xVel = 0;
				}
				else
				{
					xVel = (evt.caxis.value > 0) ? 1 : -1;
				}
				break;
			case SDL_CONTROLLER_AXIS_LEFTY:
				if (	evt.caxis.value > -sensitivity &&
					evt.caxis.value < sensitivity	)
				{
					yVel = 0;
				}
				else
				{
					yVel = (evt.caxis.value > 0) ? 1 : -1;
				}
				break;
			}
			break;
		case SDL_CONTROLLERDEVICEADDED:
		{
			SDL_GameController *toOpen = SDL_GameControllerOpen(evt.cdevice.which);
			printf(
				"Opened SDL_GameController ID #%i, %s\n",
				evt.cdevice.which,
				SDL_GameControllerName(toOpen)
			);
			controllers[SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(toOpen))] = toOpen;
			break;
		}
		case SDL_CONTROLLERDEVICEREMOVED:
		{
			SDL_GameController *toClose = controllers[evt.cdevice.which];
			controllers.erase(evt.cdevice.which);
			printf("Closing %s\n", SDL_GameControllerName(toClose));
			SDL_GameControllerClose(toClose);
			break;
		}

		/* Window Events */
		case SDL_WINDOWEVENT:
			switch (evt.window.event)
			{
			/* Window Resize */
			case SDL_WINDOWEVENT_RESIZED:
				resetWindow = true;
				break;

			/* Window Focus */
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				isActive = true;
				if (!useFullscreenSpaces)
				{
					if (wasFullscreen)
					{
						graphics.screenbuffer->isWindowed = false;
						SDL_SetWindowFullscreen(
							SDL_GetWindowFromID(evt.window.windowID),
							SDL_WINDOW_FULLSCREEN_DESKTOP
						);
					}
				}
				SDL_DisableScreenSaver();
				if (Mix_PlayingMusic())
				{
					// Correct songStart for how long we were paused
					music.songStart += SDL_GetPerformanceCounter() - pauseStart;
				}
				break;
			case SDL_WINDOWEVENT_FOCUS_LOST:
				isActive = false;
				if (!useFullscreenSpaces)
				{
					wasFullscreen = !graphics.screenbuffer->isWindowed;
					graphics.screenbuffer->isWindowed = true;
					SDL_SetWindowFullscreen(
						SDL_GetWindowFromID(evt.window.windowID),
						0
					);
				}
				SDL_EnableScreenSaver();
				pauseStart = SDL_GetPerformanceCounter();
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
			quitProgram = true;
			break;
		}
	}
}

bool KeyPoll::isDown(SDL_Keycode key)
{
	return keymap[key];
}

bool KeyPoll::isUp(SDL_Keycode key)
{
	return !keymap[key];
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

bool KeyPoll::controllerButtonDown()
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
	return (	buttonmap[SDL_CONTROLLER_BUTTON_DPAD_LEFT] ||
			xVel < 0 ||
			(	includeVert &&
				(	buttonmap[SDL_CONTROLLER_BUTTON_DPAD_UP] ||
					yVel < 0	)	)	);
}

bool KeyPoll::controllerWantsRight(bool includeVert)
{
	return (	buttonmap[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] ||
			xVel > 0 ||
			(	includeVert &&
				(	buttonmap[SDL_CONTROLLER_BUTTON_DPAD_DOWN] ||
					yVel > 0	)	)	);
}
