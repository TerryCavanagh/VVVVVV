#define MUSIC_DEFINITION
#include "Music.h"

#include <SDL.h>
#include <stdio.h>

#include "BinaryBlob.h"
#include "Game.h"
#include "Map.h"
#include "UtilityClass.h"

musicclass::musicclass(void)
{
	safeToProcessMusic= false;
	m_doFadeInVol = false;
	m_doFadeOutVol = false;
	musicVolume = 0;
	FadeVolAmountPerFrame = 0;

	user_music_volume = USER_VOLUME_MAX;
	user_sound_volume = USER_VOLUME_MAX;

	currentsong = 0;
	nicechange = -1;
	nicefade = false;
	quick_fade = true;

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

void musicclass::play(int t)
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

	if (currentsong == t && !m_doFadeOutVol)
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
		if (Mix_PlayMusic(musicTracks[t].m_music, 0) == -1)
		{
			printf("Mix_PlayMusic: %s\n", Mix_GetError());
		}
		else
		{
			musicVolume = MIX_MAX_VOLUME;
		}
	}
	else
	{
		if (m_doFadeOutVol)
		{
			// We're already fading out
			nicechange = t;
			nicefade = true;
			currentsong = -1;

			if (quick_fade)
			{
				fadeMusicVolumeOut(500); // fade out quicker
			}
			else
			{
				quick_fade = true;
			}
		}
		else if (Mix_PlayMusic(musicTracks[t].m_music, -1) == -1)
		{
			printf("Mix_PlayMusic: %s\n", Mix_GetError());
		}
		else
		{
			fadeMusicVolumeIn(3000);
		}
	}
}

void musicclass::resume()
{
	Mix_ResumeMusic();
}

void musicclass::resumefade(const int fadein_ms)
{
	resume();
	fadeMusicVolumeIn(fadein_ms);
}

void musicclass::fadein(void)
{
	resumefade(3000); // 3000 ms fadein
}

void musicclass::pause(void)
{
	Mix_PauseMusic();
}

void musicclass::haltdasmusik(void)
{
	/* Just pauses music. This is intended. */
	pause();
	currentsong = -1;
}

void musicclass::silencedasmusik(void)
{
	musicVolume = 0;
}

void musicclass::setfadeamount(const int fade_ms)
{
	if (fade_ms == 0)
	{
		FadeVolAmountPerFrame = MIX_MAX_VOLUME;
		return;
	}
	FadeVolAmountPerFrame = MIX_MAX_VOLUME / (fade_ms / game.get_timestep());
}

void musicclass::fadeMusicVolumeIn(int ms)
{
	m_doFadeInVol = true;
	m_doFadeOutVol = false;

	/* Ensure it starts at 0 */
	musicVolume = 0;

	/* Fix 1-frame glitch */
	Mix_VolumeMusic(0);

	setfadeamount(ms);
}

void musicclass::fadeMusicVolumeOut(const int fadeout_ms)
{
	m_doFadeInVol = false;
	m_doFadeOutVol = true;
	setfadeamount(fadeout_ms);
}

void musicclass::fadeout(const bool quick_fade_ /*= true*/)
{
	fadeMusicVolumeOut(2000);
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

void musicclass::processmusicfadeout(void)
{
	musicVolume -= FadeVolAmountPerFrame;
	if (musicVolume < 0)
	{
		musicVolume = 0;
		m_doFadeOutVol = false;
		pause();
	}
}

void musicclass::processmusic(void)
{
	if(!safeToProcessMusic)
	{
		return;
	}

	if (nicefade && Mix_PausedMusic() == 1)
	{
		play(nicechange);
		nicechange = -1;
		nicefade = false;
	}

	if(m_doFadeInVol)
	{
		processmusicfadein();
	}

	if (m_doFadeOutVol)
	{
		processmusicfadeout();
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

void musicclass::pauseef(void)
{
	Mix_Pause(-1);
}

void musicclass::resumeef(void)
{
	Mix_Resume(-1);
}
