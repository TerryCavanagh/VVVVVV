#ifndef SCREEN_H
#define SCREEN_H

#include <SDL.h>

#include "ScreenSettings.h"

class Screen
{
public:
    void init(const struct ScreenSettings* settings);
    void destroy(void);

    void GetSettings(struct ScreenSettings* settings);

    void LoadIcon(void);

    void ResizeScreen(int x, int y);
    void ResizeToNearestMultiple(void);
    void GetWindowSize(int* x, int* y);

    void RenderPresent(void);

    void toggleFullScreen(void);
    void toggleScalingMode(void);
    void toggleLinearFilter(void);
    void toggleVSync(void);

    void recacheTextures(void);

    bool isForcedFullscreen(void);

    bool isWindowed;
    bool isFiltered;
    bool badSignalEffect;
    int scalingMode;
    bool vsync;

    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
};

#ifndef GAMESCREEN_DEFINITION
extern Screen gameScreen;
#endif

#endif /* SCREEN_H */
