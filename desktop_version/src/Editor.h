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

    EditorSubState_SCRIPTS_EDIT,

    EditorSubState_MENU_INPUT
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
    BoxType_PLATFORM,
    BoxType_COPY
};

enum BoxCorner
{
    BoxCorner_FIRST,
    BoxCorner_LAST
};

// Text entry field type
enum TextMode
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
    TEXT_DESC1,
    TEXT_DESC2,
    TEXT_DESC3,
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

    void get_input_line(const enum TextMode mode, const std::string& prompt, std::string* ptr);

    void show_note(const char* text);

    void add_entity(int xp, int yp, int tp, int p1 = 0, int p2 = 0, int p3 = 0, int p4 = 0, int p5 = 320, int p6 = 240);

    void remove_entity(int t);

    int get_entity_at(int xp, int yp);

    void set_tile(int x, int y, int t);

    int autotiling_base(int x, int y);
    int autotiling_background_base(int x, int y);

    TileTypes get_abs_tile_type(int x, int y, bool wrap);
    TileTypes get_tile_type(int x, int y, bool wrap);

    bool is_background(int x, int y);
    int backfree(int x, int y);
    bool lines_can_pass(int x, int y);
    int free(int x, int y);
    int match(int x, int y);
    int outsidematch(int x, int y);
    int backmatch(int x, int y);
    int edgetile(int x, int y);
    int outsideedgetile(int x, int y);
    int backedgetile(int x, int y);
    int labspikedir(int x, int y, int t);
    int spikedir(int x, int y);

    int get_enemy_tile(int t);

    void switch_tileset(const bool reversed);
    void switch_tilecol(const bool reversed);
    void clamp_tilecol(const int rx, const int ry, const bool wrap);
    void switch_enemy(const bool reversed);
    void switch_warpdir(const bool reversed);

    EditorStates state;
    EditorSubStates substate;

    const char* tool_names[NUM_EditorTools];
    const char* tool_key_chars[NUM_EditorTools];
    SDL_KeyCode tool_keys[NUM_EditorTools];
    bool tool_requires_shift[NUM_EditorTools];

    EditorTools current_tool;

    BoxTypes box_type;
    BoxCorner box_corner;

    SDL_Point box_point;

    int entcol;
    SDL_Color entcolreal;

    int kludgewarpdir[customlevelclass::numrooms];

    int note_timer;
    int old_note_timer;
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
    std::string old_entity_text;

    enum TextMode current_text_mode; // In text entry
    std::string* current_text_ptr; // Pointer to text we're changing
    std::string current_text_desc; // Description (for editor mode text fields)
    union
    {
        int desc; // Which description row we're changing
        int textent; // Entity ID for text prompt
    };
    bool xmod, zmod, cmod, vmod, bmod, hmod, spacemod;

    int roomnamehide;
    bool saveandquit;
    bool shiftmenu, shiftkey;
    bool settingskey;
    int warpent;
    bool updatetiles, changeroom;
    bool deletekeyheld;

    //Script editor stuff
    void removeline(int t);
    void insertline(int t);

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
