#ifndef SCREENSETTINGS_H
#define SCREENSETTINGS_H

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
