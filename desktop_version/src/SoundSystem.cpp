#include "SoundSystem.h"

#include <SDL.h>

#include "FileSystemUtils.h"
#include "Vlogging.h"

MusicTrack::MusicTrack(const char* fileName)
{
	m_music = Mix_LoadMUS(fileName);
	m_isValid = true;
	if(m_music == NULL)
	{
		vlog_error("Unable to load Ogg Music file: %s", Mix_GetError());
		m_isValid = false;
	}
}

MusicTrack::MusicTrack(SDL_RWops *rw)
{
	m_music = Mix_LoadMUS_RW(rw, 1);
	m_isValid = true;
	if(m_music == NULL)
	{
		vlog_error("Unable to load Magic Binary Music file: %s", Mix_GetError());
		m_isValid = false;
	}
}

SoundTrack::SoundTrack(const char* fileName)
{
	unsigned char *mem;
	size_t length;

	sound = NULL;

	FILESYSTEM_loadAssetToMemory(fileName, &mem, &length, false);
	if (mem == NULL)
	{
		vlog_error("Unable to load WAV file %s", fileName);
		SDL_assert(0 && "WAV file missing!");
		return;
	}
	SDL_RWops *fileIn = SDL_RWFromConstMem(mem, length);
	sound = Mix_LoadWAV_RW(fileIn, 1);
	FILESYSTEM_freeMemory(&mem);

	if (sound == NULL)
	{
		vlog_error("Unable to load WAV file: %s", Mix_GetError());
	}
}

SoundSystem::SoundSystem(void)
{
	int audio_rate = 44100;
	Uint16 audio_format = AUDIO_S16SYS;
	int audio_channels = 2;
	int audio_buffers = 1024;

	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0)
	{
		vlog_error("Unable to initialize audio: %s", Mix_GetError());
		SDL_assert(0 && "Unable to initialize audio!");
	}
}
