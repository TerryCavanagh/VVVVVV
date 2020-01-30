#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#include <SDL_mixer.h>

class MusicTrack
{
public:
	MusicTrack(const char* fileName);
	MusicTrack(SDL_RWops *rw);
	MusicTrack(MusicTrack&& moved);
	MusicTrack& operator=(const MusicTrack& other) = default;
	~MusicTrack();
	Mix_Music *m_music;
	bool m_isValid;
};

class SoundTrack
{
public:
	SoundTrack(const char* fileName);
	SoundTrack(SoundTrack&& moved);
	SoundTrack& operator=(const SoundTrack& other) = default;
	SoundTrack() = default;
	~SoundTrack();
	Mix_Chunk *sound;
	bool isValid = false;
};

class SoundSystem
{
public:
	SoundSystem();
	void playMusic(MusicTrack* music);
};

// polyfill of std::move
template<typename T> struct remove_reference      {typedef T type;};
template<typename T> struct remove_reference<T&>  {typedef T type;};
template<typename T> struct remove_reference<T&&> {typedef T type;};

template<typename T>
typename remove_reference<T>::type&& move(T&& t) {
	return static_cast<typename remove_reference<T>::type&&>(t);
}

#endif /* SOUNDSYSTEM_H */
