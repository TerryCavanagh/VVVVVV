#if !defined(NO_CUSTOM_LEVELS)

#ifndef CUSTOMLEVELS_H
#define CUSTOMLEVELS_H

#include <SDL.h>
#include <string>
#include <vector>
#include <map>

class CustomEntity
{
public:
    int x, y, t;
    //parameters
    int p1, p2, p3, p4, p5, p6;
    std::string scriptname;
};


#define ROOM_PROPERTIES \
    FOREACH_PROP(tileset, int) \
    FOREACH_PROP(tilecol, int) \
    FOREACH_PROP(roomname, std::string) \
    FOREACH_PROP(warpdir, int) \
    FOREACH_PROP(platx1, int) \
    FOREACH_PROP(platy1, int) \
    FOREACH_PROP(platx2, int) \
    FOREACH_PROP(platy2, int) \
    FOREACH_PROP(platv, int) \
    FOREACH_PROP(enemyx1, int) \
    FOREACH_PROP(enemyy1, int) \
    FOREACH_PROP(enemyx2, int) \
    FOREACH_PROP(enemyy2, int) \
    FOREACH_PROP(enemytype, int) \
    FOREACH_PROP(directmode, int)

class RoomProperty
{
public:
    RoomProperty(void);
#define FOREACH_PROP(NAME, TYPE) TYPE NAME;
    ROOM_PROPERTIES
#undef FOREACH_PROP
};

struct LevelMetaData
{
    std::string title;
    std::string creator;
    std::string Desc1;
    std::string Desc2;
    std::string Desc3;
    std::string website;
    std::string filename;

    std::string modifier;
    std::string timeCreated;
    std::string timeModified;

    int version;

    /* true if a system "Untitled Level"/"Unknown"
     * was stored in this struct (for the levels list) */
    bool title_is_gettext;
    bool creator_is_gettext;

    /* This is for the metadata in the levels list,
     * so it will only be a main font (no custom ones). */
    uint8_t level_main_font_idx;
};

struct CliPlaytestArgs
{
    int x;
    int y;
    int rx;
    int ry;
    int gc;
    int music;
    bool valid;
};


extern std::vector<CustomEntity> customentities;

class customlevelclass
{
public:
    customlevelclass(void);

    std::string title;
    std::string creator;
    std::string modifier;
    std::string Desc1;
    std::string Desc2;
    std::string Desc3;
    std::string website;

    std::vector<LevelMetaData> ListOfMetaData;

    void loadZips(void);
    void getDirectoryData(void);
    bool getLevelMetaDataAndPlaytestArgs(const std::string& filename, LevelMetaData& _data, CliPlaytestArgs* pt_args);
    bool getLevelMetaData(const std::string& filename, LevelMetaData& _data);

    void reset(void);
    const int* loadlevel(int rxi, int ryi);

    int gettileidx(
      const int rx,
      const int ry,
      const int x,
      const int y
    );
    void settile(
      const int rx,
      const int ry,
      const int x,
      const int y,
      const int t
    );
    int gettile(
      const int rx,
      const int ry,
      const int x,
      const int y
    );
    int getabstile(const int x, const int y);

    int getroompropidx(const int rx, const int ry);
    const RoomProperty* getroomprop(const int rx, const int ry);
#define FOREACH_PROP(NAME, TYPE) \
    void setroom##NAME(const int rx, const int ry, const TYPE NAME);
    ROOM_PROPERTIES
#undef FOREACH_PROP

    int absfree(int x, int y);

    bool load(std::string& _path);
#ifndef NO_EDITOR
    bool save(const std::string& _path);
#endif
    void generatecustomminimap(void);

    int findtrinket(int t);
    int findcrewmate(int t);
    int findwarptoken(int t);
    void findstartpoint(void);
    int getlevelcol(const int tileset, const int tilecol);
    int getenemycol(int t);

    //Colouring stuff
    int getwarpbackground(int rx, int ry);

    static const int maxwidth = 20, maxheight = 20; //Special; the physical max the engine allows
    static const int numrooms = maxwidth * maxheight;
    int contents[40 * 30 * numrooms];
    int numtrinkets(void);
    int numcrewmates(void);
    RoomProperty roomproperties[numrooms]; //Maxwidth*maxheight

    int levmusic;
    int mapwidth, mapheight; //Actual width and height of stage

    std::string level_font_name;

    int version;

    SDL_Color getonewaycol(int rx, int ry);
    SDL_Color getonewaycol(void);
    bool onewaycol_override;

    std::map<std::string, SDL_Color> customcolours;
};

std::string translate_title(const std::string& title, bool* is_gettext);

std::string translate_creator(const std::string& creator, bool* is_gettext);

#ifndef CL_DEFINITION
extern customlevelclass cl;
#endif

#endif /* CUSTOMLEVELS_H */

#endif /* NO_CUSTOM_LEVELS */
