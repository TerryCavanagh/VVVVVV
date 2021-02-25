#ifndef MUSIC_H
#define MUSIC_H

#include <vector>

#include "BinaryBlob.h"
#include "SoundSystem.h"

#define musicroom(rx, ry) ((rx) + ((ry) * 20))

class musicclass
{
public:
	musicclass(void);
	void init(void);
	void destroy(void);

	void play(int t, const double position_sec = 0.0, const int fadein_ms = 3000);
	void resume(const int fadein_ms = 0);
	void haltdasmusik(void);
	void silencedasmusik(void);
	void fadeMusicVolumeIn(int ms);
	void fadeout(const bool quick_fade_ = true);
	void fadein(void);
	void processmusicfadein(void);
	void processmusic(void);
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

	bool quick_fade;

	// MMMMMM mod settings
	bool mmmmmm;
	bool usingmmmmmm;

	binaryBlob pppppp_blob;
	binaryBlob mmmmmm_blob;
	int num_pppppp_tracks;
	int num_mmmmmm_tracks;

	Uint64 songStart;
	Uint64 songEnd;
};

#ifndef MUSIC_DEFINITION
extern musicclass music;
#endif

#endif /* MUSIC_H */
