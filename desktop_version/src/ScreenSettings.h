#ifndef SCREENSETTINGS_H
#define SCREENSETTINGS_H

enum
{
    SCALING_LETTERBOX = 0,
    SCALING_STRETCH = 1,
    SCALING_INTEGER = 2,
    NUM_SCALING_MODES
};

struct ScreenSettings
{
    int windowWidth;
    int windowHeight;
    bool fullscreen;
    bool useVsync;
    int scalingMode;
    bool linearFilter;
    bool badSignal;
};

void ScreenSettings_default(struct ScreenSettings* _this);

#endif /* SCREENSETTINGS_H */
