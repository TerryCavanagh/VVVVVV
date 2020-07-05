#if !defined(NO_CUSTOM_LEVELS)

#ifndef EDITOR_H
#define EDITOR_H

#include <vector>
#include <string>
#include "Script.h"
#include "Graphics.h"

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


class edlevelclass{
public:
  edlevelclass();
  int tileset, tilecol;
  std::string roomname;
  int warpdir;
  int platx1, platy1, platx2, platy2, platv;
  int enemyx1, enemyy1, enemyx2, enemyy2, enemytype;
  int directmode;
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

  static EditorData& GetInstance()
  {
    static EditorData  instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }


  std::string title;
  std::string creator;

  std::string modifier;
  std::string timeCreated;
  std::string timeModified;

private:


  EditorData()
  {
  }

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
  editorclass();

  std::string Desc1;
  std::string Desc2;
  std::string Desc3;
  std::string website;

  std::vector<std::string> directoryList;
  std::vector<LevelMetaData> ListOfMetaData;

  void loadZips();
  void getDirectoryData();
  bool getLevelMetaData(std::string& filename, LevelMetaData& _data );

  void reset();
  void getlin(const enum textmode mode, const std::string& prompt, std::string* ptr);
  const int* loadlevel(int rxi, int ryi);

  void placetile(int x, int y, int t);

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
  int warpzonematch(int x, int y);
  int outsidematch(int x, int y);

  int backmatch(int x, int y);

  void switch_tileset(const bool reversed = false);
  void switch_tilecol(const bool reversed = false);
  void clamp_tilecol(const int rx, const int ry, const bool wrap = false);
  void switch_enemy(const bool reversed = false);

  bool load(std::string& _path);
  bool save(std::string& _path);
  void generatecustomminimap();
  int edgetile(int x, int y);
  int warpzoneedgetile(int x, int y);
  int outsideedgetile(int x, int y);

  int backedgetile(int x, int y);

  int labspikedir(int x, int y, int t);
  int spikedir(int x, int y);
  int findtrinket(int t);
  int findcrewmate(int t);
  int findwarptoken(int t);
  void findstartpoint();
  int getlevelcol(int t);
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
  int numtrinkets();
  int numcrewmates();
  edlevelclass level[numrooms]; //Maxwidth*maxheight
  int kludgewarpdir[numrooms]; //Also maxwidth*maxheight

  int notedelay;
  int oldnotedelay;
  std::string note;
  std::string keybuffer;
  std::string filename;

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
  bool xmod, zmod, cmod, vmod, bmod, hmod, spacemod, warpmod, textentry;
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

  std::string author;
  std::string description;
  std::string title;

  //Functions for interfacing with the script:
  void addhook(std::string t);
  void removehook(std::string t);
  void addhooktoscript(std::string t);
  void removehookfromscript(std::string t);
  void loadhookineditor(std::string t);
  void clearscriptbuffer();
  void gethooks();
  bool checkhook(std::string t);
  std::vector<std::string> hooklist;

  int hookmenupage, hookmenu;

  //Direct Mode variables
  int dmtile;
  int dmtileeditor;

  Uint32 getonewaycol(const int rx, const int ry);
  Uint32 getonewaycol();
  bool onewaycol_override;

  int returneditoralpha;
  int oldreturneditoralpha;

  std::vector<GhostInfo> ghosts;
  int currentghosts;
};

void addedentity(int xp, int yp, int tp, int p1=0, int p2=0, int p3=0, int p4=0, int p5=320, int p6=240);

void removeedentity(int t);

int edentat(int xp, int yp);


bool edentclear(int xp, int yp);

void fillbox(int x, int y, int x2, int y2, int c);

void fillboxabs(int x, int y, int x2, int y2, int c);

#if !defined(NO_EDITOR)
void editorrender();

void editorlogic();

void editorinput();
#endif

extern editorclass ed;

#endif /* EDITOR_H */

#endif /* NO_CUSTOM_LEVELS */
