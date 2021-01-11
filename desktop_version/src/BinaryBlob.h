#pragma once

#include <vector>

/* Laaaazyyyyyyy -flibit */
// #define VVV_COMPILEMUSIC

#define TRACK_NAMES \
	FOREACH_TRACK("data/music/0levelcomplete.ogg") \
	FOREACH_TRACK("data/music/1pushingonwards.ogg") \
	FOREACH_TRACK("data/music/2positiveforce.ogg") \
	FOREACH_TRACK("data/music/3potentialforanything.ogg") \
	FOREACH_TRACK("data/music/4passionforexploring.ogg") \
	FOREACH_TRACK("data/music/5intermission.ogg") \
	FOREACH_TRACK("data/music/6presentingvvvvvv.ogg") \
	FOREACH_TRACK("data/music/7gamecomplete.ogg") \
	FOREACH_TRACK("data/music/8predestinedfate.ogg") \
	FOREACH_TRACK("data/music/9positiveforcereversed.ogg") \
	FOREACH_TRACK("data/music/10popularpotpourri.ogg") \
	FOREACH_TRACK("data/music/11pipedream.ogg") \
	FOREACH_TRACK("data/music/12pressurecooker.ogg") \
	FOREACH_TRACK("data/music/13pacedenergy.ogg") \
	FOREACH_TRACK("data/music/14piercingthesky.ogg") \
	FOREACH_TRACK("data/music/predestinedfatefinallevel.ogg")

struct resourceheader
{
	char name[48];
	int start_UNUSED;
	int size;
	bool valid;
};

class binaryBlob
{
public:
	binaryBlob();

#ifdef VVV_COMPILEMUSIC
	void AddFileToBinaryBlob(const char* _path);

	void writeBinaryBlob(const char* _name);
#endif

	bool unPackBinary(const char* _name);

	int getIndex(const char* _name);

	int getSize(int _index);

	std::vector<int> getExtra();

	char* getAddress(int _index);

	void clear();

	static const int max_headers = 128;

private:
	int numberofHeaders;
	resourceheader m_headers[max_headers];
	char* m_memblocks[max_headers];
};
