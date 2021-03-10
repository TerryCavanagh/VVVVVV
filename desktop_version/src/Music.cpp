#define MUSIC_DEFINITION
#include "Music.h"

#include <SDL.h>
#include <stdio.h>

#include "BinaryBlob.h"
#include "Map.h"
#include "UtilityClass.h"

static void songend(void);

musicclass::musicclass(void)
{
	safeToProcessMusic= false;
	m_doFadeInVol = false;
	musicVolume = MIX_MAX_VOLUME;
	FadeVolAmountPerFrame = 0;

	currentsong = 0;
	nicechange = -1;
	nicefade = false;
	resumesong = 0;
	quick_fade = true;

	songStart = 0;
	songEnd = 0;

	Mix_HookMusicFinished(&songend);

	usingmmmmmm = false;
}

void musicclass::init(void)
{
	soundTracks.push_back(SoundTrack( "sounds/jump.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/jump2.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/hurt.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/souleyeminijingle.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/coin.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/save.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/crumble.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/vanish.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/blip.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/preteleport.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/teleport.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/crew1.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/crew2.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/crew3.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/crew4.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/crew5.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/crew6.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/terminal.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/gamesaved.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/crashing.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/blip2.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/countdown.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/go.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/crash.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/combine.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/newrecord.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/trophy.wav" ));
	soundTracks.push_back(SoundTrack( "sounds/rescue.wav" ));

#ifdef VVV_COMPILEMUSIC
	binaryBlob musicWriteBlob;
#define FOREACH_TRACK(blob, track_name) blob.AddFileToBinaryBlob(track_name);
	TRACK_NAMES(musicWriteBlob)
#undef FOREACH_TRACK

	musicWriteBlob.writeBinaryBlob("data/BinaryMusic.vvv");
	musicWriteBlob.clear();
#endif

	num_mmmmmm_tracks = 0;
	num_pppppp_tracks = 0;

	if (!mmmmmm_blob.unPackBinary("mmmmmm.vvv"))
	{
		mmmmmm = false;
		usingmmmmmm=false;
		bool ohCrap = pppppp_blob.unPackBinary("vvvvvvmusic.vvv");
		SDL_assert(ohCrap && "Music not found!");
	}
	else
	{
		mmmmmm = true;
		int index;
		SDL_RWops *rw;

#define FOREACH_TRACK(blob, track_name) \
	index = blob.getIndex(track_name); \
	if (index >= 0 && index < blob.max_headers) \
	{ \
		rw = SDL_RWFromConstMem(blob.getAddress(index), blob.getSize(index)); \
		if (rw == NULL) \
		{ \
			printf("Unable to read music file header: %s\n", SDL_GetError()); \
		} \
		else \
		{ \
			musicTracks.push_back(MusicTrack( rw )); \
		} \
	}

		TRACK_NAMES(mmmmmm_blob)

		num_mmmmmm_tracks += musicTracks.size();

		size_t index_ = 0;
		while (mmmmmm_blob.nextExtra(&index_))
		{
			rw = SDL_RWFromConstMem(mmmmmm_blob.getAddress(index_), mmmmmm_blob.getSize(index_));
			musicTracks.push_back(MusicTrack( rw ));

			num_mmmmmm_tracks++;
			index_++;
		}

		bool ohCrap = pppppp_blob.unPackBinary("vvvvvvmusic.vvv");
		SDL_assert(ohCrap && "Music not found!");
	}

	int index;
	SDL_RWops *rw;

	TRACK_NAMES(pppppp_blob)

#undef FOREACH_TRACK

	num_pppppp_tracks += musicTracks.size() - num_mmmmmm_tracks;

	size_t index_ = 0;
	while (pppppp_blob.nextExtra(&index_))
	{
		rw = SDL_RWFromConstMem(pppppp_blob.getAddress(index_), pppppp_blob.getSize(index_));
		musicTracks.push_back(MusicTrack( rw ));

		num_pppppp_tracks++;
		index_++;
	}
}

static void songend(void)
{
	extern musicclass music;
	music.songEnd = SDL_GetPerformanceCounter();
	music.resumesong = music.currentsong;
	music.currentsong = -1;
}

void musicclass::destroy(void)
{
	for (size_t i = 0; i < soundTracks.size(); ++i)
	{
		Mix_FreeChunk(soundTracks[i].sound);
	}
	soundTracks.clear();

	// Before we free all the music: stop playing music, else SDL2_mixer
	// will call SDL_Delay() if we are fading, resulting in no-draw frames
	Mix_HaltMusic();

	for (size_t i = 0; i < musicTracks.size(); ++i)
	{
		Mix_FreeMusic(musicTracks[i].m_music);
	}
	musicTracks.clear();

	pppppp_blob.clear();
	mmmmmm_blob.clear();
}

void musicclass::play(int t, const double position_sec /*= 0.0*/, const int fadein_ms /*= 3000*/)
{
	if (mmmmmm && usingmmmmmm)
	{
		// Don't conjoin this if-statement with the above one...
		if (num_mmmmmm_tracks > 0)
		{
			t %= num_mmmmmm_tracks;
		}
	}
	else if (num_pppppp_tracks > 0)
	{
		t %= num_pppppp_tracks;
	}

	if (mmmmmm && !usingmmmmmm)
	{
		t += num_mmmmmm_tracks;
	}

	safeToProcessMusic = true;
	musicVolume = MIX_MAX_VOLUME;

	if (currentsong == t && Mix_FadingMusic() != MIX_FADING_OUT)
	{
		return;
	}

	currentsong = t;

	if (t == -1)
	{
		return;
	}

	if (!INBOUNDS_VEC(t, musicTracks))
	{
		puts("play() out-of-bounds!");
		currentsong = -1;
		return;
	}

	if (currentsong == 0 || currentsong == 7 || (!map.custommode && (currentsong == 0+num_mmmmmm_tracks || currentsong == 7+num_mmmmmm_tracks)))
	{
		// Level Complete theme, no fade in or repeat
		if (Mix_FadeInMusicPos(musicTracks[t].m_music, 0, 0, position_sec) == -1)
		{
			printf("Mix_FadeInMusicPos: %s\n", Mix_GetError());
		}
	}
	else
	{
		if (Mix_FadingMusic() == MIX_FADING_OUT)
		{
			// We're already fading out
			nicechange = t;
			nicefade = true;
			currentsong = -1;

			if (quick_fade)
			{
				Mix_FadeOutMusic(500); // fade out quicker
			}
			else
			{
				quick_fade = true;
			}
		}
		else if (Mix_FadeInMusicPos(musicTracks[t].m_music, -1, fadein_ms, position_sec) == -1)
		{
			printf("Mix_FadeInMusicPos: %s\n", Mix_GetError());
		}
	}

	songStart = SDL_GetPerformanceCounter();
}

void musicclass::resume(const int fadein_ms /*= 0*/)
{
	const double offset = static_cast<double>(songEnd - songStart);
	const double frequency = static_cast<double>(SDL_GetPerformanceFrequency());

	const double position_sec = offset / frequency;

	play(resumesong, position_sec, fadein_ms);
}

void musicclass::fadein(void)
{
	resume(3000); // 3000 ms fadein
}

void musicclass::haltdasmusik(void)
{
	Mix_HaltMusic();
}

void musicclass::silencedasmusik(void)
{
	musicVolume = 0;
}

void musicclass::fadeMusicVolumeIn(int ms)
{
	m_doFadeInVol = true;
	FadeVolAmountPerFrame =  MIX_MAX_VOLUME / (ms / 33);
}

void musicclass::fadeout(const bool quick_fade_ /*= true*/)
{
	Mix_FadeOutMusic(2000);
	resumesong = currentsong;
	quick_fade = quick_fade_;
}

void musicclass::processmusicfadein(void)
{
	musicVolume += FadeVolAmountPerFrame;
	if (musicVolume >= MIX_MAX_VOLUME)
	{
		m_doFadeInVol = false;
	}
}

void musicclass::processmusic(void)
{
	if(!safeToProcessMusic)
	{
		return;
	}

	if (nicefade && Mix_PlayingMusic() == 0)
	{
		play(nicechange);
		nicechange = -1;
		nicefade = false;
	}

	if(m_doFadeInVol)
	{
		processmusicfadein();
	}
}


void musicclass::niceplay(int t)
{
	// important: do nothing if the correct song is playing!
	if((!mmmmmm && currentsong!=t) || (mmmmmm && usingmmmmmm && currentsong!=t) || (mmmmmm && !usingmmmmmm && currentsong!=t+num_mmmmmm_tracks))
	{
		if(currentsong!=-1)
		{
			fadeout(false);
		}
		nicefade = true;
		nicechange = t;
	}
}

void musicclass::changemusicarea(int x, int y)
{
	switch(musicroom(x, y))
	{
	case musicroom(11, 4):
		niceplay(2);
		break;

	case musicroom(2, 4):
	case musicroom(7, 15):
		niceplay(3);
		break;

	case musicroom(18, 1):
	case musicroom(15, 0):
		niceplay(12);
		break;

	case musicroom(0, 0):
	case musicroom(0, 16):
	case musicroom(2, 11):
	case musicroom(7, 9):
	case musicroom(8, 11):
	case musicroom(13, 2):
	case musicroom(17, 12):
	case musicroom(14, 19):
	case musicroom(17, 17):
		niceplay(4);
		break;

	default:
		niceplay(1);
		break;
	}
}

void musicclass::playef(int t)
{
	if (!INBOUNDS_VEC(t, soundTracks))
	{
		return;
	}
	int channel;

	channel = Mix_PlayChannel(-1, soundTracks[t].sound, 0);
	if(channel == -1)
	{
		fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
	}
}
