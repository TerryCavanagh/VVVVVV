#ifndef MUSIC_H
#define MUSIC_H

#include <vector>

#include "BinaryBlob.h"
#include "SoundSystem.h"

#define musicroom(rx, ry) ((rx) + ((ry) * 20))

class musicclass
{
public:
	void init();

	void play(int t, const double position_sec = 0.0, const int fadein_ms = 3000);
	void resume(const int fadein_ms = 0);
	void haltdasmusik();
	void silencedasmusik();
	void fadeMusicVolumeIn(int ms);
	void fadeout();
	void fadein();
	void processmusicfadein();
	void processmusic();
	void niceplay(int t);

	void changemusicarea(int x, int y);

	int currentsong;
	int resumesong;

	void playef(int t);

	std::vector<SoundTrack> soundTracks;
	std::vector<MusicTrack> musicTracks;
	SoundSystem soundSystem;
	bool safeToProcessMusic;

	int nicechange; // -1 if no song queued
	bool nicefade;

	bool m_doFadeInVol;
	int FadeVolAmountPerFrame;
	int musicVolume;

	bool dontquickfade;

	// MMMMMM mod settings
	bool mmmmmm;
	bool usingmmmmmm;

	binaryBlob musicReadBlob;
	int num_pppppp_tracks;
	int num_mmmmmm_tracks;

	Uint64 songStart;
	Uint64 songEnd;
};

#ifndef MUSIC_DEFINITION
extern musicclass music;
#endif

#endif /* MUSIC_H */
