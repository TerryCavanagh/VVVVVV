#define GAMESCREEN_DEFINITION
#include "Screen.h"

#include <SDL.h>

#include "Alloc.h"
#include "Constants.h"
#include "Exit.h"
#include "FileSystemUtils.h"
#include "Game.h"
#include "Graphics.h"
#include "GraphicsUtil.h"
#include "InterimVersion.h"
#include "Render.h"
#include "Vlogging.h"

void ScreenSettings_default(struct ScreenSettings* _this)
{
    _this->windowWidth = SCREEN_WIDTH_PIXELS * 2;
    _this->windowHeight = SCREEN_HEIGHT_PIXELS * 2;
    _this->fullscreen = false;
    _this->useVsync = true; // Now that uncapped is the default...
    _this->scalingMode = SCALING_INTEGER;
    _this->linearFilter = false;
    _this->badSignal = false;
}

void Screen::init(const struct ScreenSettings* settings)
{
    m_window = NULL;
    m_renderer = NULL;
    windowWidth = settings->windowWidth;
    windowHeight = settings->windowHeight;
    isWindowed = !settings->fullscreen;
    scalingMode = settings->scalingMode;
    isFiltered = settings->linearFilter;
    badSignalEffect = settings->badSignal;
    vsync = settings->useVsync;

    SDL_SetHintWithPriority(
        SDL_HINT_RENDER_SCALE_QUALITY,
        isFiltered ? "linear" : "nearest",
        SDL_HINT_OVERRIDE
    );
    SDL_SetHintWithPriority(
        SDL_HINT_RENDER_VSYNC,
        vsync ? "1" : "0",
        SDL_HINT_OVERRIDE
    );

    // Uncomment this next line when you need to debug -flibit
    // SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "software", SDL_HINT_OVERRIDE);

    m_window = SDL_CreateWindow(
        "VVVVVV",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH_PIXELS * 2,
        SCREEN_HEIGHT_PIXELS * 2,
        SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    );

    if (m_window == NULL)
    {
        vlog_error("Could not create window: %s", SDL_GetError());
        VVV_exit(1);
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    if (m_renderer == NULL)
    {
        vlog_error("Could not create renderer: %s", SDL_GetError());
        VVV_exit(1);
    }

#ifdef INTERIM_VERSION_EXISTS
    /* Branch name limits are ill-defined but on GitHub it's ~256 chars
     * ( https://stackoverflow.com/a/24014513/ ).
     * Really though, just don't use super long branch names. */
    char title[256];
    SDL_snprintf(title, sizeof(title), "VVVVVV [%s]", BRANCH_NAME);
    SDL_SetWindowTitle(m_window, title);
#else
    SDL_SetWindowTitle(m_window, "VVVVVV");
#endif

    SDL_SetWindowMinimumSize(m_window, SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS);

    LoadIcon();

    ResizeScreen(windowWidth, windowHeight);
}

void Screen::destroy(void)
{
    /* Order matters! */
    VVV_freefunc(SDL_DestroyRenderer, m_renderer);
    VVV_freefunc(SDL_DestroyWindow, m_window);
}

void Screen::GetSettings(struct ScreenSettings* settings)
{
    settings->windowWidth = windowWidth;
    settings->windowHeight = windowHeight;

    settings->fullscreen = !isWindowed;
    settings->useVsync = vsync;
    settings->scalingMode = scalingMode;
    settings->linearFilter = isFiltered;
    settings->badSignal = badSignalEffect;
}

#ifdef __APPLE__
/* Apple doesn't like icons anymore... */
void Screen::LoadIcon(void)
{

}
#else
SDL_Surface* LoadImageSurface(const char* filename);

void Screen::LoadIcon(void)
{
    SDL_Surface* icon = LoadImageSurface("VVVVVV.png");
    if (icon == NULL)
    {
        return;
    }
    SDL_SetWindowIcon(m_window, icon);
    VVV_freefunc(SDL_FreeSurface, icon);
}
#endif /* __APPLE__ */

void Screen::ResizeScreen(int x, int y)
{
    if (x != -1 && y != -1)
    {
        // This is a user resize!
        windowWidth = x;
        windowHeight = y;
    }

    if (!isWindowed || isForcedFullscreen())
    {
        int result = SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        recacheTextures();
        if (result != 0)
        {
            vlog_error("Error: could not set the game to fullscreen mode: %s", SDL_GetError());
            return;
        }
    }
    else
    {
        int result = SDL_SetWindowFullscreen(m_window, 0);
        recacheTextures();
        if (result != 0)
        {
            vlog_error("Error: could not set the game to windowed mode: %s", SDL_GetError());
            return;
        }
        if (x != -1 && y != -1)
        {
            SDL_SetWindowSize(m_window, windowWidth, windowHeight);
            SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        }
    }
}

void Screen::ResizeToNearestMultiple(void)
{
    int w, h;
    GetScreenSize(&w, &h);

    // Check aspect ratio first
    bool using_width;
    int usethisdimension, usethisratio;

    if ((float) w / (float) h > 4.0 / 3.0)
    {
        // Width is bigger, so it's limited by height
        usethisdimension = h;
        usethisratio = SCREEN_HEIGHT_PIXELS;
        using_width = false;
    }
    else
    {
        // Height is bigger, so it's limited by width. Or we're exactly 4:3 already
        usethisdimension = w;
        usethisratio = SCREEN_WIDTH_PIXELS;
        using_width = true;
    }

    int floor = (usethisdimension / usethisratio) * usethisratio;
    int ceiling = floor + usethisratio;

    int final_dimension;

    if (usethisdimension - floor < ceiling - usethisdimension)
    {
        // Floor is nearest
        final_dimension = floor;
    }
    else
    {
        // Ceiling is nearest. Or we're exactly on a multiple already
        final_dimension = ceiling;
    }

    if (final_dimension == 0)
    {
        // We're way too small!
        ResizeScreen(SCREEN_WIDTH_PIXELS, SCREEN_HEIGHT_PIXELS);
        return;
    }

    if (using_width)
    {
        ResizeScreen(final_dimension, final_dimension / 4 * 3);
    }
    else
    {
        ResizeScreen(final_dimension * 4 / 3, final_dimension);
    }
}

void Screen::GetScreenSize(int* x, int* y)
{
    if (SDL_GetRendererOutputSize(m_renderer, x, y) != 0)
    {
        vlog_error("Could not get window size: %s", SDL_GetError());
        /* Initialize to safe defaults */
        *x = SCREEN_WIDTH_PIXELS;
        *y = SCREEN_HEIGHT_PIXELS;
    }
}

void Screen::UpdateScaling(void)
{
    int width;
    int height;
    if (scalingMode == SCALING_STRETCH)
    {
        GetScreenSize(&width, &height);
    }
    else
    {
        width = SCREEN_WIDTH_PIXELS;
        height = SCREEN_HEIGHT_PIXELS;
    }
    int result = SDL_RenderSetLogicalSize(m_renderer, width, height);
    if (result != 0)
    {
        vlog_error("Error: could not set logical size: %s", SDL_GetError());
        return;
    }

    result = SDL_RenderSetIntegerScale(m_renderer, (SDL_bool) (scalingMode == SCALING_INTEGER));
    if (result != 0)
    {
        vlog_error("Error: could not set scale: %s", SDL_GetError());
    }
}

void Screen::RenderPresent(void)
{
    /* In certain cases, the window size might mismatch with the logical size.
     * So it's better to just always call this. */
    UpdateScaling();

    SDL_RenderPresent(m_renderer);
    graphics.clear();
}

void Screen::toggleFullScreen(void)
{
    isWindowed = !isWindowed;
    ResizeScreen(windowWidth, windowHeight);

    if (game.currentmenuname == Menu::graphicoptions)
    {
        /* Recreate menu to update "resize to nearest" */
        game.createmenu(game.currentmenuname, true);
    }
}

void Screen::toggleScalingMode(void)
{
    scalingMode = (scalingMode + 1) % NUM_SCALING_MODES;
    UpdateScaling();
}

void Screen::toggleLinearFilter(void)
{
    isFiltered = !isFiltered;
    SDL_SetHintWithPriority(
        SDL_HINT_RENDER_SCALE_QUALITY,
        isFiltered ? "linear" : "nearest",
        SDL_HINT_OVERRIDE
    );
    SDL_DestroyTexture(graphics.gameTexture);
    graphics.gameTexture = SDL_CreateTexture(
        m_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_TARGET,
        SCREEN_WIDTH_PIXELS,
        SCREEN_HEIGHT_PIXELS
    );

    if (graphics.gameTexture == NULL)
    {
        vlog_error("Could not create game texture: %s", SDL_GetError());
        return;
    }
}

void Screen::toggleVSync(void)
{
    vsync = !vsync;
    SDL_RenderSetVSync(m_renderer, (int) vsync);

    recacheTextures();
}

void Screen::recacheTextures(void)
{
    // Fix for d3d9, which clears target textures sometimes (ex. toggling vsync, switching fullscreen, etc...)

    // Signal cached textures to be redrawn fully
    graphics.backgrounddrawn = false;
    graphics.foregrounddrawn = false;
    graphics.towerbg.tdrawback = true;
    graphics.titlebg.tdrawback = true;

    if (game.ingame_titlemode)
    {
        // Redraw the cached gameplay texture if we're in the in-game menu.
        // Additionally, reset alpha so things don't jitter when re-entering gameplay.
        float oldAlpha = graphics.alpha;
        graphics.alpha = 0;
        gamerender();
        graphics.alpha = oldAlpha;
    }
}

bool Screen::isForcedFullscreen(void)
{
    /* This is just a check to see if we're on a desktop or tenfoot setup.
     * If you're working on a tenfoot-only build, add a def that always
     * returns true!
     */
    return SDL_GetHintBoolean("SteamTenfoot", SDL_FALSE);
}
