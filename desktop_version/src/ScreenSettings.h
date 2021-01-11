#pragma once

struct ScreenSettings
{
	ScreenSettings();

	int windowWidth;
	int windowHeight;
	bool fullscreen;
	bool useVsync;
	int stretch;
	bool linearFilter;
	bool badSignal;
};
