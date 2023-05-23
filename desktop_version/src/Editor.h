#if !defined(NO_CUSTOM_LEVELS) && !defined(NO_EDITOR)

#ifndef EDITOR_H
#define EDITOR_H

#include "Constants.h"
#include "CustomLevels.h"

#include <map>
#include <SDL.h>
#include <string>
#include <vector>

enum EditorTilesets
{
    EditorTileset_SPACE_STATION = 0,
    EditorTileset_OUTSIDE = 1,
    EditorTileset_LAB = 2,
    EditorTileset_WARP_ZONE = 3,
    EditorTileset_SHIP = 4,

    NUM_EditorTilesets
};

struct EditorTilecolInfo
{
    const char* foreground_type;
    int foreground_base;
    const char* background_type;
    int background_base;
};

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

    void register_tileset(EditorTilesets tileset, const char* name);
    void register_tilecol(EditorTilesets tileset, int index, const char* foreground_type, int foreground_base, const char* background_type, int background_base);

    void register_tool(EditorTools tool, const char* name, const char* keychar, SDL_KeyCode key, bool shift);

    void draw_tool(EditorTools tool, int x, int y);

    void get_tile_fill(int tilex, int tiley, int tile, bool connected[SCREEN_HEIGHT_TILES][SCREEN_WIDTH_TILES]);

    void handle_tile_placement(int tile);

    void tool_remove();
    void entity_clicked(int index);
    void tool_place();

    void get_input_line(enum TextMode mode, const std::string& prompt, std::string* ptr);

    void show_note(const char* text);

    void add_entity(int rx, int ry, int xp, int yp, int tp, int p1 = 0, int p2 = 0, int p3 = 0, int p4 = 0, int p5 = 320, int p6 = 240);

    void remove_entity(int t);

    int get_entity_at(int rx, int ry, int xp, int yp);

    void set_tile(int x, int y, int t);
    int get_tile(int x, int y);

    bool is_warp_zone_background(int tile);
    int autotile(int tile_x, int tile_y);
    bool autotile_connector(int x, int y, TileTypes original_type);
    EditorTilecolInfo get_tilecol_data(void);

    TileTypes get_abs_tile_type(int x, int y, bool wrap);
    TileTypes get_tile_type(int x, int y, bool wrap);

    bool lines_can_pass(int x, int y);

    int get_enemy_tile(int t);

    void switch_tileset(const bool reversed);
    void switch_tilecol(const bool reversed);
    void clamp_tilecol(const int rx, const int ry, const bool wrap);
    void switch_enemy(const bool reversed);
    void switch_warpdir(const bool reversed);

    EditorStates state;
    EditorSubStates substate;

    std::map<std::string, std::vector<int> > autotile_types;
    std::map<EditorTilesets, std::map<int, EditorTilecolInfo> > tileset_colors;

    const char* tileset_names[NUM_EditorTilesets];
    int tileset_min_colour[NUM_EditorTilesets];
    int tileset_max_colour[NUM_EditorTilesets];

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

    int old_tilex, old_tiley;
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
        int text_entity; // Entity ID for text prompt
    };
    bool x_modifier, z_modifier, c_modifier, v_modifier, b_modifier, h_modifier, f_modifier, toolbox_open;

    int roomnamehide;
    bool saveandquit;
    bool help_open, shiftkey;
    bool settingskey;
    int warp_token_entity;
    bool updatetiles, changeroom;
    bool backspace_held;

    //Script editor stuff
    void remove_line(int t);
    void insert_line(int t);

    std::vector<std::string> script_buffer;
    std::string current_script;
    int script_cursor_x, script_cursor_y;
    int script_offset;
    int lines_visible;

    //Functions for interfacing with the script:
    void create_script(const std::string& name, const std::vector<std::string>& contents);
    void create_script(const std::string& name);
    void remove_script(const std::string& name);
    void load_script_in_editor(const std::string& name);
    void clear_script_buffer(void);
    bool script_exists(const std::string& name);

    int script_list_offset, selected_script;

    //Direct Mode variables
    int direct_mode_tile;
    int direct_mode_drawer;

    int return_message_timer;
    int old_return_message_timer;

    std::vector<GhostInfo> ghosts;
    int current_ghosts;
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
