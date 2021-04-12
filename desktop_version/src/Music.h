#ifndef MUSIC_H
#define MUSIC_H

#include <vector>

#include "BinaryBlob.h"
#include "SoundSystem.h"

#define musicroom(rx, ry) ((rx) + ((ry) * 20))

/* The amount of "space" for the scale of the user-set volume. */
#define USER_VOLUME_MAX 256

/* It is advised that USER_VOLUME_MAX be divisible by this. */
#define USER_VOLUME_STEP 32

class musicclass
{
public:
	musicclass(void);
	void init(void);
	void destroy(void);

	void play(int t);
	void resume();
	void resumefade(const int fadein_ms);
	void pause(void);
	void haltdasmusik(void);
	void silencedasmusik(void);
	void setfadeamount(const int fade_ms);
	void fadeMusicVolumeIn(int ms);
	void fadeMusicVolumeOut(const int fadeout_ms);
	void fadeout(const bool quick_fade_ = true);
	void fadein(void);
	void processmusicfadein(void);
	void processmusicfadeout(void);
	void processmusic(void);
	void niceplay(int t);

	void changemusicarea(int x, int y);

	int currentsong;

	void playef(int t);
	void pauseef(void);
	void resumeef(void);

	std::vector<SoundTrack> soundTracks;
	std::vector<MusicTrack> musicTracks;
	SoundSystem soundSystem;
	bool safeToProcessMusic;

	int nicechange; // -1 if no song queued
	bool nicefade;

	bool m_doFadeInVol;
	bool m_doFadeOutVol;
	int FadeVolAmountPerFrame;
	int musicVolume;

	/* 0..USER_VOLUME_MAX */
	int user_music_volume;
	int user_sound_volume;

	bool quick_fade;

	// MMMMMM mod settings
	bool mmmmmm;
	bool usingmmmmmm;

	binaryBlob pppppp_blob;
	binaryBlob mmmmmm_blob;
	int num_pppppp_tracks;
	int num_mmmmmm_tracks;
};

#ifndef MUSIC_DEFINITION
extern musicclass music;
#endif

#endif /* MUSIC_H */
