#ifndef SCREENSETTINGS_H
#define SCREENSETTINGS_H

struct ScreenSettings
{
    ScreenSettings(void);

    int windowWidth;
    int windowHeight;
    bool fullscreen;
    bool useVsync;
    int stretch;
    bool linearFilter;
    bool badSignal;
};

#endif /* SCREENSETTINGS_H */
