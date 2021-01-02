#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#include <SDL_mixer.h>

class MusicTrack
{
public:
	MusicTrack(const char* fileName);
	MusicTrack(SDL_RWops *rw);
	Mix_Music *m_music;
	bool m_isValid;
};

class SoundTrack
{
public:
	SoundTrack(const char* fileName);
	Mix_Chunk *sound;
};

class SoundSystem
{
public:
	SoundSystem();
};

#endif /* SOUNDSYSTEM_H */
