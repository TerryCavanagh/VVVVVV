#include <SDL.h>
#include <stdio.h>
#include "Music.h"
#include "BinaryBlob.h"
#include "Map.h"

void songend();

void musicclass::init()
{
	for (size_t i = 0; i < soundTracks.size(); ++i) {
		Mix_FreeChunk(soundTracks[i].sound);
	}
	soundTracks.clear();
	for (size_t i = 0; i < musicTracks.size(); ++i) {
		Mix_FreeMusic(musicTracks[i].m_music);
	}
	musicTracks.clear();

	musicReadBlob.clear();

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
#define FOREACH_TRACK(track_name) musicWriteBlob.AddFileToBinaryBlob(track_name);
	TRACK_NAMES
#undef FOREACH_TRACK

	musicWriteBlob.writeBinaryBlob("data/BinaryMusic.vvv");
#endif

	if (!musicReadBlob.unPackBinary("mmmmmm.vvv"))
	{
		mmmmmm = false;
		usingmmmmmm=false;
		bool ohCrap = musicReadBlob.unPackBinary("vvvvvvmusic.vvv");
		SDL_assert(ohCrap && "Music not found!");
	}
	else
	{
		mmmmmm = true;
		usingmmmmmm = true;
		int index;
		SDL_RWops *rw;

#define FOREACH_TRACK(track_name) \
	index = musicReadBlob.getIndex(track_name); \
	rw = SDL_RWFromMem(musicReadBlob.getAddress(index), musicReadBlob.getSize(index)); \
	musicTracks.push_back(MusicTrack( rw ));

		TRACK_NAMES

		num_mmmmmm_tracks += 16;

		const std::vector<int> extra = musicReadBlob.getExtra();
		for (size_t i = 0; i < extra.size(); i++)
		{
			const int& index = extra[i];
			rw = SDL_RWFromMem(musicReadBlob.getAddress(index), musicReadBlob.getSize(index));
			musicTracks.push_back(MusicTrack( rw ));

			num_mmmmmm_tracks++;
		}

		bool ohCrap = musicReadBlob.unPackBinary("vvvvvvmusic.vvv");
		SDL_assert(ohCrap && "Music not found!");
	}

	int index;
	SDL_RWops *rw;

	TRACK_NAMES

#undef FOREACH_TRACK

	num_pppppp_tracks += 16;

	const std::vector<int> extra = musicReadBlob.getExtra();
	for (size_t i = 0; i < extra.size(); i++)
	{
		const int& index = extra[i];
		rw = SDL_RWFromMem(musicReadBlob.getAddress(index), musicReadBlob.getSize(index));
		musicTracks.push_back(MusicTrack( rw ));

		num_pppppp_tracks++;
	}
}

musicclass::musicclass()
{
	num_mmmmmm_tracks = 0;
	num_pppppp_tracks = 0;

	safeToProcessMusic= false;
	m_doFadeInVol = false;
	musicVolume = 128;
	FadeVolAmountPerFrame = 0;

	currentsong = 0;
	nicechange = 0;
	nicefade = 0;
	resumesong = 0;
	fadeoutqueuesong = -1;
	dontquickfade = false;

	songStart = 0;
	songEnd = 0;

	Mix_HookMusicFinished(&songend);
}

void songend()
{
	music.songEnd = SDL_GetPerformanceCounter();
	music.currentsong = -1;
}

void musicclass::play(int t, const double position_sec /*= 0.0*/, const int fadein_ms /*= 3000*/)
{
	// No need to check if num_tracks is greater than 0, we wouldn't be here if it wasn't
	if (mmmmmm && usingmmmmmm)
	{
		t %= num_mmmmmm_tracks;
	}
	else
	{
		t %= num_pppppp_tracks;
	}

	if(mmmmmm && !usingmmmmmm)
	{
		t += num_mmmmmm_tracks;
	}
	safeToProcessMusic = true;
	Mix_VolumeMusic(128);
	if (currentsong !=t)
	{
		if (t != -1)
		{
			currentsong = t;
			if (!INBOUNDS(t, musicTracks))
			{
				puts("play() out-of-bounds!");
				currentsong = -1;
				return;
			}
			if (currentsong == 0 || currentsong == 7 || (!map.custommode && (currentsong == 0+num_pppppp_tracks || currentsong == 7+num_pppppp_tracks)))
			{
				// Level Complete theme, no fade in or repeat
				if(Mix_FadeInMusicPos(musicTracks[t].m_music, 0, 0, position_sec)==-1)
				{
					printf("Mix_FadeInMusicPos: %s\n", Mix_GetError());
				}
			}
			else
			{
				if (Mix_FadingMusic() == MIX_FADING_OUT) {
					// We're already fading out
					fadeoutqueuesong = t;
					currentsong = -1;
					if (!dontquickfade)
						Mix_FadeOutMusic(500); // fade out quicker
					else
						dontquickfade = false;
				}
				else if(Mix_FadeInMusicPos(musicTracks[t].m_music, -1, fadein_ms, position_sec)==-1)
				{
					printf("Mix_FadeInMusicPos: %s\n", Mix_GetError());
				}
			}

			songStart = SDL_GetPerformanceCounter();
		}
		else
		{
			currentsong = -1;
		}
	}
}

void musicclass::resume(const int fadein_ms /*= 0*/)
{
	const double offset = static_cast<double>(songEnd - songStart);
	const double frequency = static_cast<double>(SDL_GetPerformanceFrequency());

	const double position_sec = offset / frequency;

	play(resumesong, position_sec, fadein_ms);
}

void musicclass::fadein()
{
	resume(3000); // 3000 ms fadein
}

void musicclass::haltdasmusik()
{
	Mix_HaltMusic();
	resumesong = currentsong;
}

void musicclass::silencedasmusik()
{
	Mix_VolumeMusic(0) ;
	musicVolume = 0;
}

void musicclass::fadeMusicVolumeIn(int ms)
{
	m_doFadeInVol = true;
	FadeVolAmountPerFrame =  MIX_MAX_VOLUME / (ms / 33);
}

void musicclass::fadeout()
{
	Mix_FadeOutMusic(2000);
	resumesong = currentsong;
}

void musicclass::processmusicfadein()
{
	musicVolume += FadeVolAmountPerFrame;
	Mix_VolumeMusic(musicVolume);
	if (musicVolume >= MIX_MAX_VOLUME)
	{
		m_doFadeInVol = false;
	}
}

void musicclass::processmusic()
{
	if(!safeToProcessMusic)
	{
		return;
	}

	if (fadeoutqueuesong != -1 && Mix_PlayingMusic() == 0) {
		play(fadeoutqueuesong);
		fadeoutqueuesong = -1;
	}

	if (nicefade == 1 && Mix_PlayingMusic() == 0)
	{
		play(nicechange);
		nicechange = -1; nicefade = 0;
	}

	if(m_doFadeInVol)
	{
		processmusicfadein();
	}
}


void musicclass::niceplay(int t)
{
	// important: do nothing if the correct song is playing!
	if((!mmmmmm && currentsong!=t) || (mmmmmm && usingmmmmmm && currentsong!=t) || (mmmmmm && !usingmmmmmm && currentsong!=t+16))
	{
		if(currentsong!=-1)
		{
			dontquickfade = true;
			fadeout();
		}
		nicefade = 1;
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
	if (t < 0 || t >= (int) soundTracks.size())
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
