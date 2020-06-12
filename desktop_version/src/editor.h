#if !defined(NO_CUSTOM_LEVELS)

#ifndef EDITOR_H
#define EDITOR_H

#include <vector>
#include <string>
#include "Script.h"

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

  void saveconvertor();
  void reset();
  std::vector<int> loadlevel(int rxi, int ryi);

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

  //Colouring stuff
  int getwarpbackground(int rx, int ry);

  std::vector<std::string> getLevelDirFileNames( );
  std::vector <int> contents;
  std::vector <int> vmult;
  int numtrinkets();
  int numcrewmates();
  edlevelclass level[400]; //Maxwidth*maxheight
  int kludgewarpdir[400]; //Also maxwidth*maxheight

  int temp;
  int notedelay;
  std::string note;
  std::string keybuffer;
  std::string filename;

  int drawmode;
  int tilex, tiley;
  int keydelay, lclickdelay;
  bool savekey, loadkey;
  int levx, levy;
  int entframe, entframedelay;

  bool roomtextmod;

  bool scripttextmod;
  int textent;
  int scripttexttype;
  std::string oldenttext;

  bool xmod, zmod, spacemod, warpmod, roomnamemod, textentry, savemod, loadmod;
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
  int maxwidth, maxheight; //Special; the physical max the engine allows

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

  int returneditoralpha;

  std::vector<GhostInfo> ghosts;
  int currentghosts = 0;
};

void addedentity(int xp, int yp, int tp, int p1=0, int p2=0, int p3=0, int p4=0, int p5=320, int p6=240);

void removeedentity(int t);

int edentat(int xp, int yp);


bool edentclear(int xp, int yp);

void fillbox(int x, int y, int x2, int y2, int c);

void fillboxabs(int x, int y, int x2, int y2, int c);

void editorrender();

void editorlogic();

void editorinput();

extern editorclass ed;

#endif /* EDITOR_H */

#endif /* NO_CUSTOM_LEVELS */
