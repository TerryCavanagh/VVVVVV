#ifndef MUSIC_H
#define MUSIC_H

#include "SoundSystem.h"

#include <vector>

#define musicroom(rx, ry) ((rx) + ((ry) * 20))

class musicclass
{
public:
	void init();

	void play(int t);
	void loopmusic();
	void stopmusic();
	void haltdasmusik();
	void silencedasmusik();
	void fadeMusicVolumeIn(int ms);
	void fadeout();
	void processmusicfade();
	void processmusicfadein();
	void processmusic();
	void niceplay(int t);

	void changemusicarea(int x, int y);

	// public var musicchan:Array = new Array();
	// public var musicchannel:SoundChannel, musicchannel2:SoundChannel;
	// public var currentmusicchan:int, musicchanlen:int, musicchancur:int, musicstopother:int, resumesong:int;
	// public var currentsong:int, musicfade:int, musicfadein:int;
	int currentsong, musicfade, musicfadein;
	int resumesong;

	//public var nicefade:int, nicechange:int;

	void playef(int t, int offset = 0);

	std::vector<SoundTrack> soundTracks;
	std::vector<MusicTrack> musicTracks;
	SoundSystem soundSystem;
	bool safeToProcessMusic;

	int nicechange;
	int nicefade;

	bool m_doFadeInVol;
	int FadeVolAmountPerFrame;
	int musicVolume;

	float volume;

	bool custompd;

	int fadeoutqueuesong; // -1 if no song queued
	bool dontquickfade;

	// MMMMMM mod settings
	bool mmmmmm;
	bool usingmmmmmm;
};

extern musicclass music;

#endif /* MUSIC_H */
