#ifndef MUSIC_H
#define MUSIC_H

#include "SoundSystem.h"

#include <vector>
#include <map>
#include <string>

#define musicroom(rx, ry) ((rx) + ((ry) * 20))

class musicclass
{
public:
	musicclass();

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

	// Play a sound effect! There are 16 channels, which iterate
	void initefchannels();

	void playfile(const char* t, std::string track);
	void stopfile(std::string track);

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

	// MMMMMM mod settings
	bool mmmmmm;
	bool usingmmmmmm;

	bool muted = false;

	std::map<std::string, SoundTrack> custom_files;
	std::map<std::string, int> custom_file_channels;
};


#endif /* MUSIC_H */
