#if !defined(NO_CUSTOM_LEVELS) && !defined(NO_EDITOR)

#ifndef EDITOR_H
#define EDITOR_H

#include "CustomLevels.h"

#include <SDL.h>
#include <string>
#include <vector>

enum EditorTools
{
    EditorTool_WALLS,
    EditorTool_BACKING,
    EditorTool_SPIKES,
    EditorTool_TRINKETS,
    EditorTool_CHECKPOINTS,
    EditorTool_DISAPPEARING_PLATFORMS,
    EditorTool_CONVEYORS,
    EditorTool_MOVING_PLATFORMS,
    EditorTool_ENEMIES,
    EditorTool_GRAVITY_LINES,
    EditorTool_ROOMTEXT,
    EditorTool_TERMINALS,
    EditorTool_SCRIPTS,
    EditorTool_WARP_TOKENS,
    EditorTool_WARP_LINES,
    EditorTool_CREWMATES,
    EditorTool_START_POINT,

    NUM_EditorTools
};

enum EditorStates
{
    EditorState_DRAW,
    EditorState_SCRIPTS,
    EditorState_MENU
};

enum EditorSubStates
{
    EditorSubState_MAIN,

    EditorSubState_DRAW_INPUT,
    EditorSubState_DRAW_BOX,
    EditorSubState_DRAW_WARPTOKEN,

    EditorSubState_SCRIPTS_EDIT
};

enum TileTypes
{
    TileType_NONSOLID,
    TileType_SOLID,
    TileType_BACKGROUND,
    TileType_SPIKE
};

enum BoxTypes
{
    BoxType_SCRIPT,
    BoxType_ENEMY,
    BoxType_PLATFORM
};

// Text entry field type
enum textmode
{
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

struct GhostInfo
{
    int rx; // game.roomx-100
    int ry; // game.roomy-100
    int x; // .xp
    int y; // .yp
    int col; // .colour
    SDL_Color realcol;
    int frame; // .drawframe
};

class editorclass
{
public:
    editorclass(void);
    void reset(void);

    void register_tool(EditorTools tool, const char* name, const char* keychar, SDL_KeyCode key, bool shift);

    void draw_tool(EditorTools tool, int x, int y);

    void handle_tile_placement(int tile);

    void tool_remove();
    void entity_clicked(int index);
    void tool_place();

    void getlin(const enum textmode mode, const std::string& prompt, std::string* ptr);

    void addedentity(int xp, int yp, int tp, int p1 = 0, int p2 = 0, int p3 = 0, int p4 = 0, int p5 = 320, int p6 = 240);

    void removeedentity(int t);

    int edentat(int xp, int yp);

    void settile(int x, int y, int t);

    int base(int x, int y);

    int backbase(int x, int y);

    int at(int x, int y);

    int tile_type_wrap(int x, int y);

    int backonlyfree(int x, int y);

    int backfree(int x, int y);

    int spikefree(int x, int y);
    int free(int x, int y);

    int match(int x, int y);
    int outsidematch(int x, int y);

    int backmatch(int x, int y);

    int edgetile(int x, int y);
    int outsideedgetile(int x, int y);

    int backedgetile(int x, int y);

    int labspikedir(int x, int y, int t);
    int spikedir(int x, int y);

    int getenemyframe(int t);

    void switch_tileset(const bool reversed);
    void switch_tilecol(const bool reversed);
    void clamp_tilecol(const int rx, const int ry, const bool wrap);
    void switch_enemy(const bool reversed);
    void switch_warpdir(const bool reversed);

    EditorStates state;
    EditorSubStates substate;

    const char* toolnames[NUM_EditorTools];
    const char* toolkeychar[NUM_EditorTools];
    SDL_KeyCode toolkey[NUM_EditorTools];
    bool toolshift[NUM_EditorTools];

    EditorTools current_tool;

    int entcol;
    SDL_Color entcolreal;

    int kludgewarpdir[customlevelclass::numrooms];

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
    union
    {
        int desc; // Which description row we're changing
        int textent; // Entity ID for text prompt
    };
    bool xmod, zmod, cmod, vmod, bmod, hmod, spacemod, warpmod;
    bool titlemod, creatormod, desc1mod, desc2mod, desc3mod, websitemod;

    int roomnamehide;
    bool saveandquit;
    bool shiftmenu, shiftkey;
    bool settingsmod, settingskey;
    int warpent;
    bool updatetiles, changeroom;
    int deletekeyheld;

    int boundarymod, boundarytype;
    int boundx1, boundx2, boundy1, boundy2;

    //Script editor stuff
    void removeline(int t);
    void insertline(int t);

    bool scripteditmod;
    int scripthelppage, scripthelppagedelay;
    std::vector<std::string> sb;
    std::string sbscript;
    int sbx, sby;
    int pagey;
    int lines_visible;

    //Functions for interfacing with the script:
    void addhook(const std::string& t);
    void removehook(const std::string& t);
    void addhooktoscript(const std::string& t);
    void removehookfromscript(const std::string& t);
    void loadhookineditor(const std::string& t);
    void clearscriptbuffer(void);
    void gethooks(void);
    bool checkhook(const std::string& t);
    std::vector<std::string> hooklist;

    int hookmenupage, hookmenu;

    //Direct Mode variables
    int dmtile;
    int dmtileeditor;

    int returneditoralpha;
    int oldreturneditoralpha;

    std::vector<GhostInfo> ghosts;
    int currentghosts;
};

void editorrender(void);

void editorrenderfixed(void);

void editorlogic(void);

void editorinput(void);

#ifndef ED_DEFINITION
extern editorclass ed;
#endif

#endif /* EDITOR_H */

#endif /* NO_CUSTOM_LEVELS and NO_EDITOR */
