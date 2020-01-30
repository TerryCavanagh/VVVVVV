#include <SDL.h>
#include "SoundSystem.h"
#include "FileSystemUtils.h"

MusicTrack::MusicTrack(const char* fileName)
{
	m_music = Mix_LoadMUS(fileName);
	m_isValid = true;
	if(m_music == NULL)
	{
		fprintf(stderr, "Unable to load Ogg Music file: %s\n", Mix_GetError());;
		m_isValid = false;
	}
}

MusicTrack::MusicTrack(SDL_RWops *rw)
{
	m_music = Mix_LoadMUS_RW(rw, 0);
	m_isValid = true;
	if(m_music == NULL)
	{
		fprintf(stderr, "Unable to load Magic Binary Music file: %s\n", Mix_GetError());
		m_isValid = false;
	}
}

MusicTrack::MusicTrack(MusicTrack&& moved) : m_music(move(moved.m_music)), m_isValid(move(moved.m_isValid)) {
	moved.m_isValid = false;
}

MusicTrack::~MusicTrack() {
	if (m_isValid) Mix_FreeMusic(m_music);
	m_isValid = false;
}

SoundTrack::SoundTrack(const char* fileName)
{
	sound = NULL;

	unsigned char *mem;
	size_t length = 0;
	FILESYSTEM_loadFileToMemory(fileName, &mem, &length);
	SDL_RWops *fileIn = SDL_RWFromMem(mem, length);
	sound = Mix_LoadWAV_RW(fileIn, 1);
	if (length)
	{
		FILESYSTEM_freeMemory(&mem);
	}

	if (sound == NULL) {
		fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
	} else {
		isValid = true;
	}
}

SoundTrack::SoundTrack(SoundTrack&& moved) : sound(move(moved.sound)), isValid(move(moved.isValid)) {
	moved.isValid = false;
}

SoundTrack::~SoundTrack() {
	if (isValid) Mix_FreeChunk(sound);
	isValid = false;
}

SoundSystem::SoundSystem()
{
	int audio_rate = 44100;
	Uint16 audio_format = AUDIO_S16SYS;
	int audio_channels = 2;
	int audio_buffers = 1024;

	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0)
	{
		fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
		SDL_assert(0 && "Unable to initialize audio!");
	}
}

void SoundSystem::playMusic(MusicTrack* music)
{
	if(!music->m_isValid)
	{
		fprintf(stderr, "Invalid mix specified: %s\n", Mix_GetError());
	}
	if(Mix_PlayMusic(music->m_music, 0) == -1)
	{
		fprintf(stderr, "Unable to play Ogg file: %s\n", Mix_GetError());
	}
}
