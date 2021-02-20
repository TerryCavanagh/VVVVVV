#if !defined(NO_CUSTOM_LEVELS)

#ifndef EDITOR_H
#define EDITOR_H

#include <SDL.h>
#include <string>
#include <vector>

// Text entry field type
enum textmode {
  TEXT_NONE,

  // In-editor text fields
  TEXT_LOAD,
  TEXT_SAVE,
  TEXT_ROOMNAME,
  TEXT_SCRIPT,
  TEXT_ROOMTEXT,
  TEXT_GOTOROOM,
  LAST_EDTEXT = TEXT_GOTOROOM,

  // Settings-mode text fields
  TEXT_TITLE,
  TEXT_DESC,
  TEXT_WEBSITE,
  TEXT_CREATOR,
  NUM_TEXTMODES,

  // Text modes with an entity
  FIRST_ENTTEXT = TEXT_SCRIPT,
  LAST_ENTTEXT = TEXT_ROOMTEXT
};

class edentities{
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

class edlevelclass{
public:
  edlevelclass(void);
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
};


extern std::vector<edentities> edentity;

class EditorData
{
  public:

  static EditorData& GetInstance(void)
  {
    static EditorData  instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }


  std::string title;
  std::string creator;

  std::string modifier;
};

struct GhostInfo {
    int rx; // game.roomx-100
    int ry; // game.roomy-100
    int x; // .xp
    int y; // .yp
    int col; // .colour
    Uint32 realcol;
    int frame; // .drawframe
};

class editorclass{
  //Special class to handle ALL editor variables locally
  public:
  editorclass(void);

  std::string Desc1;
  std::string Desc2;
  std::string Desc3;
  std::string website;

  std::vector<LevelMetaData> ListOfMetaData;

  void loadZips(void);
  void getDirectoryData(void);
  bool getLevelMetaData(std::string& filename, LevelMetaData& _data );

  void reset(void);
  void getlin(const enum textmode mode, const std::string& prompt, std::string* ptr);
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
  const edlevelclass* getroomprop(const int rx, const int ry);
#define FOREACH_PROP(NAME, TYPE) \
  void setroom##NAME(const int rx, const int ry, const TYPE NAME);
  ROOM_PROPERTIES
#undef FOREACH_PROP

  void placetilelocal(int x, int y, int t);

  int getenemyframe(int t);
  int base(int x, int y);

  int backbase(int x, int y);

  int at(int x, int y);

  int freewrap(int x, int y);

  int backonlyfree(int x, int y);

  int backfree(int x, int y);

  int spikefree(int x, int y);
  int free(int x, int y);
  int absfree(int x, int y);

  int match(int x, int y);
  int outsidematch(int x, int y);

  int backmatch(int x, int y);

  void switch_tileset(const bool reversed);
  void switch_tilecol(const bool reversed);
  void clamp_tilecol(const int rx, const int ry, const bool wrap);
  void switch_enemy(const bool reversed);
  void switch_warpdir(const bool reversed);

  bool load(std::string& _path);
  bool save(std::string& _path);
  void generatecustomminimap(void);
  int edgetile(int x, int y);
  int outsideedgetile(int x, int y);

  int backedgetile(int x, int y);

  int labspikedir(int x, int y, int t);
  int spikedir(int x, int y);
  int findtrinket(int t);
  int findcrewmate(int t);
  int findwarptoken(int t);
  void findstartpoint(void);
  int getlevelcol(const int tileset, const int tilecol);
  int getenemycol(int t);
  int entcol;
  Uint32 entcolreal;

  //Colouring stuff
  int getwarpbackground(int rx, int ry);

  std::vector<std::string> getLevelDirFileNames( );
  static const int maxwidth = 20, maxheight = 20; //Special; the physical max the engine allows
  static const int numrooms = maxwidth * maxheight;
  int contents[40 * 30 * numrooms];
  int vmult[30 * maxheight];
  int numtrinkets(void);
  int numcrewmates(void);
  edlevelclass level[numrooms]; //Maxwidth*maxheight
  int kludgewarpdir[numrooms]; //Also maxwidth*maxheight

  int notedelay;
  int oldnotedelay;
  std::string note;
  std::string keybuffer;
  std::string filename;
  std::string loaded_filepath;

  int drawmode;
  int tilex, tiley;
  int keydelay, lclickdelay;
  bool savekey, loadkey;
  int levx, levy;
  int entframe, entframedelay;

  int scripttexttype;
  std::string oldenttext;

  enum textmode textmod; // In text entry
  std::string* textptr; // Pointer to text we're changing
  std::string textdesc; // Description (for editor mode text fields)
  union {
    int desc; // Which description row we're changing
    int textent; // Entity ID for text prompt
  };
  bool xmod, zmod, cmod, vmod, bmod, hmod, spacemod, warpmod;
  bool titlemod, creatormod, desc1mod, desc2mod, desc3mod, websitemod;

  int roomnamehide;
  bool saveandquit;
  bool shiftmenu, shiftkey;
  int spacemenu;
  bool settingsmod, settingskey;
  int warpent;
  bool updatetiles, changeroom;
  int deletekeyheld;

  int boundarymod, boundarytype;
  int boundx1, boundx2, boundy1, boundy2;

  int levmusic;
  int mapwidth, mapheight; //Actual width and height of stage

  int version;

  //Script editor stuff
  void removeline(int t);
  void insertline(int t);

  bool scripteditmod;
  int scripthelppage, scripthelppagedelay;
  std::vector<std::string> sb;
  std::string sbscript;
  int sbx, sby;
  int pagey;

  //Functions for interfacing with the script:
  void addhook(std::string t);
  void removehook(std::string t);
  void addhooktoscript(std::string t);
  void removehookfromscript(std::string t);
  void loadhookineditor(std::string t);
  void clearscriptbuffer(void);
  void gethooks(void);
  bool checkhook(std::string t);
  std::vector<std::string> hooklist;

  int hookmenupage, hookmenu;

  //Direct Mode variables
  int dmtile;
  int dmtileeditor;

  Uint32 getonewaycol(const int rx, const int ry);
  Uint32 getonewaycol(void);
  bool onewaycol_override;

  int returneditoralpha;
  int oldreturneditoralpha;

  std::vector<GhostInfo> ghosts;
  int currentghosts;
};

#if !defined(NO_EDITOR)
void editorrender(void);

void editorrenderfixed(void);

void editorlogic(void);

void editorinput(void);
#endif

#ifndef ED_DEFINITION
extern editorclass ed;
#endif

#endif /* EDITOR_H */

#endif /* NO_CUSTOM_LEVELS */
