#define ED_DEFINITION
#include "Editor.h"

#include <string>
#include <vector>

#include "Constants.h"
#include "CustomLevels.h"
#include "DeferCallbacks.h"
#include "Entity.h"
#include "Enums.h"
#include "Font.h"
#include "Game.h"
#include "Graphics.h"
#include "GraphicsUtil.h"
#include "KeyPoll.h"
#include "Localization.h"
#include "Map.h"
#include "Maths.h"
#include "Music.h"
#include "Screen.h"
#include "Script.h"
#include "UTF8.h"
#include "UtilityClass.h"
#include "VFormat.h"
#include "Vlogging.h"

#define SCRIPT_LINE_PADDING 6

editorclass::editorclass(void)
{
    reset();

    register_tool(EditorTool_WALLS, "Walls", "1", SDLK_1, false);
    register_tool(EditorTool_BACKING, "Backing", "2", SDLK_2, false);
    register_tool(EditorTool_SPIKES, "Spikes", "3", SDLK_3, false);
    register_tool(EditorTool_TRINKETS, "Trinkets", "4", SDLK_4, false);
    register_tool(EditorTool_CHECKPOINTS, "Checkpoints", "5", SDLK_5, false);
    register_tool(EditorTool_DISAPPEARING_PLATFORMS, "Disappearing Platforms", "6", SDLK_6, false);
    register_tool(EditorTool_CONVEYORS, "Conveyors", "7", SDLK_7, false);
    register_tool(EditorTool_MOVING_PLATFORMS, "Moving Platforms", "8", SDLK_8, false);
    register_tool(EditorTool_ENEMIES, "Enemies", "9", SDLK_9, false);
    register_tool(EditorTool_GRAVITY_LINES, "Gravity Lines", "0", SDLK_0, false);
    register_tool(EditorTool_ROOMTEXT, "Roomtext", "R", SDLK_r, false);
    register_tool(EditorTool_TERMINALS, "Terminals", "T", SDLK_t, false);
    register_tool(EditorTool_SCRIPTS, "Script Boxes", "Y", SDLK_y, false);
    register_tool(EditorTool_WARP_TOKENS, "Warp Tokens", "U", SDLK_u, false);
    register_tool(EditorTool_WARP_LINES, "Warp Lines", "I", SDLK_i, false);
    register_tool(EditorTool_CREWMATES, "Crewmates", "O", SDLK_o, false);
    register_tool(EditorTool_START_POINT, "Start Point", "P", SDLK_p, false);

    static const short basic[] = {
        121, 121, 121, 121, 121, 121, 121, 160, 121, 121, 121, 121, 121, 121, 121,
        160, 121, 121, 121, 121, 121, 121, 121, 160, 121, 121, 121, 121, 80, 80, 80,
        120, 121, 121, 121, 121, 121, 121, 121, 160, 121, 121, 121, 121, 121, 121,
        121, 160, 121, 121, 121, 121, 121, 121, 121, 160, 121, 121, 121, 121, 80, 80,
        80, 120, 121, 121, 121, 121, 121, 121, 121, 160, 121, 121, 121, 121, 121, 121,
        121, 160, 121, 121, 121, 121, 121, 121, 121, 160, 121, 121, 121, 121, 80, 80,
        80, 120, 121, 121, 121, 121, 121, 121, 121, 160, 121, 121, 121, 121, 121, 121,
        121, 160, 82, 82, 82, 82, 82, 82, 82, 0, 82, 82, 82, 82, 81, 81, 81, 42, 121,
        121, 121, 121, 121, 121, 121, 160, 121, 121, 121, 121, 121, 121, 121, 160, 121,
        121, 121, 121, 121, 121, 121, 160, 121, 121, 121, 121, 80, 80, 80, 120, 121,
        121, 121, 121, 121, 121, 121, 160, 121, 121, 121, 121, 121, 121, 121, 160, 121,
        121, 121, 121, 121, 121, 121, 160, 121, 121, 121, 121, 80, 80, 80, 120, 121,
        162, 121, 162, 121, 162, 121, 161, 121, 162, 121, 162, 121, 162, 121, 161, 121,
        162, 121, 162, 121, 162, 121, 161, 121, 162, 121, 162, 80, 0, 121, 2, 121, 162,
        121, 162, 121, 162, 121, 161, 121, 162, 121, 162, 121, 162, 121, 161, 82, 122,
        82, 122, 82, 122, 82, 1, 82, 122, 82, 122, 81, 41, 81, 0
    };

    std::vector<int> basic_vec;
    basic_vec.assign(basic, basic + SDL_arraysize(basic));
    autotile_types["basic"] = basic_vec;

    static const short lab_cyan[] = {
        121, 184, 121, 184, 24, 121, 24, 160, 121, 184, 121, 184, 24, 184, 24, 160,
        104, 144, 104, 144, 121, 121, 104, 145, 104, 144, 104, 144, 80, 185, 80, 120,
        121, 184, 121, 184, 24, 121, 24, 160, 121, 184, 121, 184, 24, 121, 24, 160,
        104, 144, 104, 144, 24, 121, 121, 145, 104, 144, 104, 144, 80, 185, 80, 120,
        26, 121, 26, 26, 25, 121, 25, 106, 26, 121, 26, 26, 25, 121, 25, 106, 121,
        121, 121, 121, 121, 121, 121, 160, 26, 121, 121, 26, 66, 80, 80, 120, 26, 184,
        26, 121, 25, 121, 25, 106, 26, 121, 26, 121, 25, 184, 25, 106, 82, 186, 82,
        186, 65, 82, 65, 0, 82, 82, 82, 82, 81, 199, 81, 42, 121, 184, 121, 184, 24,
        24, 24, 160, 121, 121, 121, 184, 24, 121, 24, 160, 104, 144, 104, 144, 121,
        121, 121, 160, 121, 121, 121, 121, 80, 80, 80, 120, 121, 184, 121, 184, 24,
        121, 121, 160, 121, 121, 121, 184, 121, 121, 24, 160, 121, 144, 121, 121, 121,
        24, 121, 160, 104, 121, 104, 144, 80, 80, 80, 120, 26, 162, 121, 162, 25, 105,
        25, 161, 121, 162, 121, 162, 25, 105, 25, 161, 104, 146, 121, 146, 121, 162,
        121, 64, 121, 121, 121, 162, 66, 0, 121, 2, 26, 162, 26, 162, 25, 162, 25, 161,
        26, 162, 26, 162, 25, 105, 25, 161, 82, 122, 82, 122, 65, 122, 82, 1, 82, 122,
        82, 122, 81, 41, 81, 0
    };

    std::vector<int> lab_cyan_vec;
    lab_cyan_vec.assign(lab_cyan, lab_cyan + SDL_arraysize(lab_cyan));
    autotile_types["lab_cyan"] = lab_cyan_vec;

    static const short lab_red[] = {
        121, 190, 121, 190, 30, 121, 30, 160, 121, 190, 121, 190, 30, 190, 30, 160, 110,
        150, 110, 150, 121, 121, 110, 151, 110, 150, 110, 150, 80, 191, 80, 120, 121,
        190, 121, 190, 30, 121, 30, 160, 121, 190, 121, 190, 30, 121, 30, 160, 110, 150,
        110, 150, 30, 121, 121, 151, 110, 150, 110, 150, 80, 191, 80, 120, 32, 121, 32,
        32, 31, 121, 31, 112, 32, 121, 32, 32, 31, 121, 31, 112, 121, 121, 121, 121, 121,
        121, 121, 160, 32, 121, 121, 32, 72, 80, 80, 120, 32, 190, 32, 121, 31, 121, 31,
        112, 32, 121, 32, 121, 31, 190, 31, 112, 82, 192, 82, 192, 71, 82, 71, 0, 82, 82,
        82, 82, 81, 70, 81, 42, 121, 190, 121, 190, 30, 30, 30, 160, 121, 121, 121, 190,
        30, 121, 30, 160, 110, 150, 110, 150, 121, 121, 121, 160, 121, 121, 121, 121, 80,
        80, 80, 120, 121, 190, 121, 190, 30, 121, 121, 160, 121, 121, 121, 190, 121, 121,
        30, 160, 121, 150, 121, 121, 121, 30, 121, 160, 110, 121, 110, 150, 80, 80, 80,
        120, 32, 162, 121, 162, 31, 111, 31, 161, 121, 162, 121, 162, 31, 111, 31, 161,
        110, 152, 121, 152, 121, 162, 121, 76, 121, 121, 121, 162, 72, 0, 121, 2, 32, 162,
        32, 162, 31, 162, 31, 161, 32, 162, 32, 162, 31, 111, 31, 161, 82, 122, 82, 122,
        71, 156, 82, 1, 82, 122, 82, 122, 81, 41, 81, 0
    };

    std::vector<int> lab_red_vec;
    lab_red_vec.assign(lab_red, lab_red + SDL_arraysize(lab_red));
    autotile_types["lab_red"] = lab_red_vec;

    static const short lab_pink[] = {
        121, 184, 121, 184, 24, 121, 24, 160, 121, 184, 121, 184, 24, 184, 24, 160, 104,
        144, 104, 144, 121, 121, 104, 145, 104, 144, 104, 144, 80, 185, 80, 120, 121,
        184, 121, 184, 24, 121, 24, 160, 121, 184, 121, 184, 24, 121, 24, 160, 104, 144,
        104, 144, 24, 121, 121, 145, 104, 144, 104, 144, 80, 185, 80, 120, 26, 121, 26,
        26, 25, 121, 25, 106, 26, 121, 26, 26, 25, 121, 25, 106, 121, 121, 121, 121, 121,
        121, 121, 160, 26, 121, 121, 26, 66, 80, 80, 120, 26, 184, 26, 121, 25, 121, 25,
        106, 26, 121, 26, 121, 25, 184, 25, 106, 82, 186, 82, 186, 65, 82, 65, 0, 82, 82,
        82, 82, 81, 64, 81, 42, 121, 184, 121, 184, 24, 24, 24, 160, 121, 121, 121, 184,
        24, 121, 24, 160, 104, 144, 104, 144, 121, 121, 121, 160, 121, 121, 121, 121, 80,
        80, 80, 120, 121, 184, 121, 184, 24, 121, 121, 160, 121, 121, 121, 184, 121, 121,
        24, 160, 121, 144, 121, 121, 121, 24, 121, 160, 104, 121, 104, 144, 80, 80, 80,
        120, 26, 162, 121, 162, 25, 105, 25, 161, 121, 162, 121, 162, 25, 105, 25, 161,
        104, 146, 121, 146, 121, 162, 121, 113, 121, 121, 121, 162, 66, 0, 121, 2, 26,
        162, 26, 162, 25, 162, 25, 161, 26, 162, 26, 162, 25, 105, 25, 161, 82, 122, 82,
        122, 65, 122, 82, 1, 82, 122, 82, 122, 81, 41, 81, 0
    };

    std::vector<int> lab_pink_vec;
    lab_pink_vec.assign(lab_pink, lab_pink + SDL_arraysize(lab_pink));
    autotile_types["lab_pink"] = lab_pink_vec;

    static const short lab_yellow[] = {
        121, 175, 121, 175, 15, 121, 15, 160, 121, 175, 121, 175, 15, 175, 15, 160, 95,
        135, 95, 135, 121, 121, 95, 136, 95, 135, 95, 135, 80, 176, 80, 120, 121, 175,
        121, 175, 15, 121, 15, 160, 121, 175, 121, 175, 15, 121, 15, 160, 95, 135, 95,
        135, 15, 121, 121, 136, 95, 135, 95, 135, 80, 176, 80, 120, 17, 121, 17, 17, 16,
        121, 16, 97, 17, 121, 17, 17, 16, 121, 16, 97, 121, 121, 121, 121, 121, 121, 121,
        160, 17, 121, 121, 17, 57, 80, 80, 120, 17, 175, 17, 121, 16, 121, 16, 97, 17,
        121, 17, 121, 16, 175, 16, 97, 82, 177, 82, 177, 56, 82, 56, 0, 82, 82, 82, 82,
        81, 55, 81, 42, 121, 175, 121, 175, 15, 15, 15, 160, 121, 121, 121, 175, 15, 121,
        15, 160, 95, 135, 95, 135, 121, 121, 121, 160, 121, 121, 121, 121, 80, 80, 80,
        120, 121, 175, 121, 175, 15, 121, 121, 160, 121, 121, 121, 175, 121, 121, 15, 160,
        121, 135, 121, 121, 121, 15, 121, 160, 95, 121, 95, 135, 80, 80, 80, 120, 17, 162,
        121, 162, 16, 96, 16, 161, 121, 162, 121, 162, 16, 96, 16, 161, 95, 137, 121, 137,
        121, 162, 121, 27, 121, 121, 121, 162, 57, 0, 121, 2, 17, 162, 17, 162, 16, 162,
        16, 161, 17, 162, 17, 162, 16, 96, 16, 161, 82, 122, 82, 122, 56, 122, 82, 1, 82,
        122, 82, 122, 81, 41, 81, 0
    };

    std::vector<int> lab_yellow_vec;
    lab_yellow_vec.assign(lab_yellow, lab_yellow + SDL_arraysize(lab_yellow));
    autotile_types["lab_yellow"] = lab_yellow_vec;

    static const short lab_green[] = {121, 181, 121, 181, 21, 121, 21, 160, 121, 181,
        121, 181, 21, 181, 21, 160, 101, 141, 101, 141, 121, 121, 101, 142, 101, 141,
        101, 141, 80, 182, 80, 120, 121, 181, 121, 181, 21, 121, 21, 160, 121, 181, 121,
        181, 21, 121, 21, 160, 101, 141, 101, 141, 21, 121, 121, 142, 101, 141, 101,
        141, 80, 182, 80, 120, 23, 121, 23, 23, 22, 121, 22, 103, 23, 121, 23, 23, 22,
        121, 22, 103, 121, 121, 121, 121, 121, 121, 121, 160, 23, 121, 121, 23, 63, 80,
        80, 120, 23, 181, 23, 121, 22, 121, 25, 103, 23, 121, 23, 121, 22, 181, 22, 103,
        82, 183, 82, 183, 62, 82, 62, 0, 82, 82, 82, 82, 81, 61, 81, 42, 121, 181, 121,
        181, 21, 21, 21, 160, 121, 121, 121, 181, 21, 121, 21, 160, 101, 141, 101, 141,
        121, 121, 121, 160, 121, 121, 121, 121, 80, 80, 80, 120, 121, 181, 121, 181, 21,
        121, 121, 160, 121, 121, 121, 181, 121, 121, 21, 160, 121, 141, 121, 121, 121, 21,
        121, 160, 101, 121, 101, 141, 80, 80, 80, 120, 23, 162, 121, 162, 22, 102, 22,
        161, 121, 162, 121, 162, 22, 102, 22, 161, 101, 143, 121, 143, 121, 162, 121, 161,
        121, 121, 121, 162, 63, 0, 121, 2, 23, 162, 23, 162, 22, 162, 22, 161, 23, 162,
        23, 162, 22, 102, 22, 161, 82, 122, 82, 122, 62, 122, 82, 1, 82, 122, 82, 122, 81,
        41, 81, 0
    };

    std::vector<int> lab_green_vec;
    lab_green_vec.assign(lab_green, lab_green + SDL_arraysize(lab_green));
    autotile_types["lab_green"] = lab_green_vec;

    static const short outside[] = {
        2, 0, 2, 0, 1, 2, 1, 2, 2, 0, 2, 2, 2, 2, 1, 2, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0,
        0, 2, 2, 2, 2, 2, 0, 2, 0, 1, 2, 1, 2, 2, 0, 2, 0, 1, 2, 1, 2, 0, 0, 2, 0, 2, 2,
        2, 2, 0, 0, 0, 0, 2, 2, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2,
        1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 0, 1, 2, 1,
        2, 2, 0, 2, 0, 1, 2, 1, 2, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0,
        2, 0, 1, 2, 1, 2, 2, 0, 2, 0, 1, 2, 1, 2, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 2,
        2, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
    };

    std::vector<int> outside_vec;
    outside_vec.assign(outside, outside + SDL_arraysize(outside));
    autotile_types["outside"] = outside_vec;

    // Everything gets initialized to 0 by default
    static const short none[256] = {};

    std::vector<int> none_vec;
    none_vec.assign(none, none + SDL_arraysize(none));
    autotile_types["none"] = none_vec;

    SDL_zeroa(tileset_min_colour);
    SDL_zeroa(tileset_max_colour);
    SDL_zeroa(tileset_min_colour_direct);
    SDL_zeroa(tileset_max_colour_direct);

    register_tileset(EditorTileset_SPACE_STATION, "Space Station");
    register_tileset(EditorTileset_OUTSIDE, "Outside");
    register_tileset(EditorTileset_LAB, "Lab");
    register_tileset(EditorTileset_WARP_ZONE, "Warp Zone");
    register_tileset(EditorTileset_SHIP, "Ship");

    register_tilecol(EditorTileset_SPACE_STATION, -1, "basic", 80, "basic", 680);
    register_tilecol(EditorTileset_SPACE_STATION, 0, "basic", 83, "basic", 680);
    register_tilecol(EditorTileset_SPACE_STATION, 1, "basic", 86, "basic", 698);
    register_tilecol(EditorTileset_SPACE_STATION, 2, "basic", 89, "basic", 695);
    register_tilecol(EditorTileset_SPACE_STATION, 3, "basic", 92, "basic", 683);
    register_tilecol(EditorTileset_SPACE_STATION, 4, "basic", 95, "basic", 689);
    register_tilecol(EditorTileset_SPACE_STATION, 5, "basic", 98, "basic", 680);
    register_tilecol(EditorTileset_SPACE_STATION, 6, "basic", 101, "basic", 695);
    register_tilecol(EditorTileset_SPACE_STATION, 7, "basic", 104, "basic", 704);
    register_tilecol(EditorTileset_SPACE_STATION, 8, "basic", 107, "basic", 689);
    register_tilecol(EditorTileset_SPACE_STATION, 9, "basic", 110, "basic", 686);
    register_tilecol(EditorTileset_SPACE_STATION, 10, "basic", 113, "basic", 698);

    register_tilecol(EditorTileset_SPACE_STATION, 11, "basic", 283, "basic", 695);
    register_tilecol(EditorTileset_SPACE_STATION, 12, "basic", 286, "basic", 686);
    register_tilecol(EditorTileset_SPACE_STATION, 13, "basic", 289, "basic", 704);
    register_tilecol(EditorTileset_SPACE_STATION, 14, "basic", 292, "basic", 701);
    register_tilecol(EditorTileset_SPACE_STATION, 15, "basic", 295, "basic", 698);
    register_tilecol(EditorTileset_SPACE_STATION, 16, "basic", 298, "basic", 683);
    register_tilecol(EditorTileset_SPACE_STATION, 17, "basic", 301, "basic", 704);
    register_tilecol(EditorTileset_SPACE_STATION, 18, "basic", 304, "basic", 701);
    register_tilecol(EditorTileset_SPACE_STATION, 19, "basic", 307, "basic", 698);
    register_tilecol(EditorTileset_SPACE_STATION, 20, "basic", 310, "basic", 692);
    register_tilecol(EditorTileset_SPACE_STATION, 21, "basic", 313, "basic", 686);

    register_tilecol(EditorTileset_SPACE_STATION, 22, "basic", 483, "basic", 695);
    register_tilecol(EditorTileset_SPACE_STATION, 23, "basic", 486, "basic", 683);
    register_tilecol(EditorTileset_SPACE_STATION, 24, "basic", 489, "basic", 689);
    register_tilecol(EditorTileset_SPACE_STATION, 25, "basic", 492, "basic", 704);
    register_tilecol(EditorTileset_SPACE_STATION, 26, "basic", 495, "basic", 680);
    register_tilecol(EditorTileset_SPACE_STATION, 27, "basic", 498, "basic", 695);
    register_tilecol(EditorTileset_SPACE_STATION, 28, "basic", 501, "basic", 689);
    register_tilecol(EditorTileset_SPACE_STATION, 29, "basic", 504, "basic", 692);
    register_tilecol(EditorTileset_SPACE_STATION, 30, "basic", 507, "basic", 689);
    register_tilecol(EditorTileset_SPACE_STATION, 31, "basic", 510, "basic", 698);

    register_tilecol(EditorTileset_OUTSIDE, 0, "basic", 480, "outside", 680);
    register_tilecol(EditorTileset_OUTSIDE, 1, "basic", 483, "outside", 683);
    register_tilecol(EditorTileset_OUTSIDE, 2, "basic", 486, "outside", 686);
    register_tilecol(EditorTileset_OUTSIDE, 3, "basic", 489, "outside", 689);
    register_tilecol(EditorTileset_OUTSIDE, 4, "basic", 492, "outside", 692);
    register_tilecol(EditorTileset_OUTSIDE, 5, "basic", 495, "outside", 695);
    register_tilecol(EditorTileset_OUTSIDE, 6, "basic", 498, "outside", 698);
    register_tilecol(EditorTileset_OUTSIDE, 7, "basic", 501, "outside", 701);

    register_tilecol(EditorTileset_LAB, 0, "lab_cyan", 280, "none", 0);
    register_tilecol(EditorTileset_LAB, 1, "lab_red", 283, "none", 0);
    register_tilecol(EditorTileset_LAB, 2, "lab_pink", 286, "none", 0);
    register_tilecol(EditorTileset_LAB, 3, "basic", 289, "none", 0);
    register_tilecol(EditorTileset_LAB, 4, "lab_yellow", 292, "none", 0);
    register_tilecol(EditorTileset_LAB, 5, "lab_green", 295, "none", 0);
    register_tilecol(EditorTileset_LAB, 6, "none", 0, "none", 0, true);

    register_tilecol(EditorTileset_WARP_ZONE, 0, "basic", 80, "none", 120);
    register_tilecol(EditorTileset_WARP_ZONE, 1, "basic", 83, "none", 123);
    register_tilecol(EditorTileset_WARP_ZONE, 2, "basic", 86, "none", 126);
    register_tilecol(EditorTileset_WARP_ZONE, 3, "basic", 89, "none", 129);
    register_tilecol(EditorTileset_WARP_ZONE, 4, "basic", 92, "none", 132);
    register_tilecol(EditorTileset_WARP_ZONE, 5, "basic", 95, "none", 135);
    register_tilecol(EditorTileset_WARP_ZONE, 6, "basic", 98, "none", 138);

    register_tilecol(EditorTileset_SHIP, 0, "basic", 101, "basic", 741);
    register_tilecol(EditorTileset_SHIP, 1, "basic", 104, "basic", 744);
    register_tilecol(EditorTileset_SHIP, 2, "basic", 107, "basic", 747);
    register_tilecol(EditorTileset_SHIP, 3, "basic", 110, "basic", 750);
    register_tilecol(EditorTileset_SHIP, 4, "basic", 113, "basic", 753);
    register_tilecol(EditorTileset_SHIP, 5, "basic", 116, "basic", 756);
}

void editorclass::register_tileset(EditorTilesets tileset, const char* name)
{
    tileset_names[tileset] = name;
}

void editorclass::register_tilecol(
    EditorTilesets tileset,
    const int index,
    const char* foreground_type,
    const int foreground_base,
    const char* background_type,
    const int background_base,
    const bool direct
) {
    EditorTilecolInfo info;
    info.foreground_type = foreground_type;
    info.foreground_base = foreground_base;
    info.background_type = background_type;
    info.background_base = background_base;
    info.direct_mode = direct;
    tileset_colors[tileset][index] = info;

    if (!direct)
    {
        tileset_min_colour[tileset] = SDL_min(tileset_min_colour[tileset], index);
        tileset_max_colour[tileset] = SDL_max(tileset_max_colour[tileset], index);
    }

    tileset_min_colour_direct[tileset] = SDL_min(tileset_min_colour_direct[tileset], index);
    tileset_max_colour_direct[tileset] = SDL_max(tileset_max_colour_direct[tileset], index);
}

void editorclass::register_tilecol(
    EditorTilesets tileset,
    const int index,
    const char* foreground_type,
    const int foreground_base,
    const char* background_type,
    const int background_base
) {
    register_tilecol(tileset, index, foreground_type, foreground_base, background_type, background_base, false);
}

void editorclass::reset(void)
{
    current_tool = EditorTool_WALLS;

    roomnamehide = 0;
    z_modifier = false;
    x_modifier = false;
    c_modifier = false;
    v_modifier = false;
    h_modifier = false;
    b_modifier = false;
    f_modifier = false;
    toolbox_open = false;
    help_open = false;
    shiftkey = false;
    saveandquit = false;
    note = "";
    note_timer = 0;
    old_note_timer = 0;
    backspace_held = false;
    current_text_mode = TEXT_NONE;

    warp_token_entity = -1;

    text_entity = 0;
    scripttexttype = 0;

    direct_mode_tile = 0;
    direct_mode_drawer = 0;
    entcol = 0;

    old_tilex = 0;
    old_tiley = 0;
    tilex = 0;
    tiley = 0;
    levx = 0;
    levy = 0;
    keydelay = 0;
    lclickdelay = 0;
    savekey = false;
    loadkey = false;
    updatetiles = true;
    changeroom = true;

    entframe = 0;
    entframedelay = 0;

    SDL_zeroa(kludgewarpdir);

    script_buffer.clear();

    clear_script_buffer();

    script_cursor_x = 0;
    script_cursor_y = 0;
    script_offset = 0;
    lines_visible = 25;
    current_script = "null";

    script_list_offset = 0;
    selected_script = 0;

    return_message_timer = 0;
    old_return_message_timer = 0;

    ghosts.clear();
    current_ghosts = 0;

    loaded_filepath = "";

    state = EditorState_DRAW;
    substate = EditorSubState_MAIN;
}

void editorclass::show_note(const char* text)
{
    note_timer = 45;
    note = text;
}

void editorclass::register_tool(EditorTools tool, const char* name, const char* keychar, const SDL_KeyCode key, const bool shift)
{
    tool_names[tool] = name;
    tool_key_chars[tool] = keychar;
    tool_keys[tool] = key;
    tool_requires_shift[tool] = shift;
}

void editorclass::load_script_in_editor(const std::string& name)
{
    // Load script t into the script editor
    clear_script_buffer();

    for (size_t i = 0; i < script.customscripts.size(); i++)
    {
        if (script.customscripts[i].name == name)
        {
            script_buffer = script.customscripts[i].contents;
            break;
        }
    }

    if (script_buffer.empty())
    {
        // Always have one line or we'll have problems
        script_buffer.resize(1);
    }
}

void editorclass::remove_script(const std::string& name)
{
    for (size_t i = 0; i < script.customscripts.size(); i++)
    {
        if (script.customscripts[i].name == name)
        {
            script.customscripts.erase(script.customscripts.begin() + i);
            break;
        }
    }
}

void editorclass::create_script(const std::string& name, const std::vector<std::string>& contents)
{
    // Add a script. If there's an old one, delete it.

    remove_script(name);

    Script script_;
    script_.name = name;
    script_.contents = contents;

    script.customscripts.push_back(script_);
}

void editorclass::create_script(const std::string& name)
{
    // Add an empty script.

    Script script_;
    script_.name = name;
    script_.contents.resize(1);

    script.customscripts.push_back(script_);
}

bool editorclass::script_exists(const std::string& name)
{
    for (size_t i = 0; i < script.customscripts.size(); i++)
    {
        if (script.customscripts[i].name == name)
        {
            return true;
        }
    }
    return false;
}


void editorclass::clear_script_buffer(void)
{
    script_buffer.clear();
}

void editorclass::remove_line(int t)
{
    //Remove line t from the script
    if ((int) script_buffer.size() > 1)
    {
        script_buffer.erase(script_buffer.begin() + t);
    }
}

void editorclass::insert_line(int t)
{
    //insert a blank line into script at line t
    script_buffer.insert(script_buffer.begin() + t, "");
}

static void editormenurender(int tr, int tg, int tb)
{
    extern editorclass ed;

    switch (game.currentmenuname)
    {
    case Menu::ed_settings:
        font::print(PR_2X | PR_CEN, -1, 75, loc::gettext("Map Settings"), tr, tg, tb);
        if (game.currentmenuoption == 3)
        {
            if (!game.ghostsenabled)
                font::print(0, 2, 230, loc::gettext("Editor ghost trail is OFF"), tr/2, tg/2, tb/2);
            else
                font::print(0, 2, 230, loc::gettext("Editor ghost trail is ON"), tr, tg, tb);
        }
        break;
    case Menu::ed_desc:
    {
        const std::string input_text = key.keybuffer + ((ed.entframe < 2) ? "_" : " ");

        if (ed.current_text_mode == TEXT_TITLE)
        {
            font::print(PR_2X | PR_CEN | PR_FONT_LEVEL, -1, 35, input_text, tr, tg, tb);
        }
        else
        {
            bool title_is_gettext;
            std::string title = translate_title(cl.title, &title_is_gettext);
            font::print(PR_2X | PR_CEN | (title_is_gettext ? PR_FONT_INTERFACE : PR_FONT_LEVEL), -1, 35, title, tr, tg, tb);
        }

        bool creator_is_gettext = false;
        std::string creator = (ed.current_text_mode == TEXT_CREATOR) ? input_text : translate_creator(cl.creator, &creator_is_gettext);

        int sp = SDL_max(10, font::height(PR_FONT_LEVEL));
        graphics.print_level_creator((creator_is_gettext ? PR_FONT_INTERFACE : PR_FONT_LEVEL), 60, creator, tr, tg, tb);

        font::print(PR_CEN | PR_FONT_LEVEL, -1, 60 + sp, (ed.current_text_mode == TEXT_WEBSITE) ? input_text : cl.website, tr, tg, tb);
        font::print(PR_CEN | PR_FONT_LEVEL, -1, 60 + sp * 3, (ed.current_text_mode == TEXT_DESC1) ? input_text : cl.Desc1, tr, tg, tb);
        font::print(PR_CEN | PR_FONT_LEVEL, -1, 60 + sp * 4, (ed.current_text_mode == TEXT_DESC2) ? input_text : cl.Desc2, tr, tg, tb);


        if (ed.current_text_mode == TEXT_DESC3)
        {
            font::print(PR_CEN | PR_FONT_LEVEL, -1, 60 + sp * 5, input_text, tr, tg, tb);
        }
        else if (sp <= 10)
        {
            font::print(PR_CEN | PR_FONT_LEVEL, -1, 60 + sp * 5, cl.Desc3, tr, tg, tb);
        }

        const char* label = loc::gettext("Font: ");
        int len_label = font::len(0, label);
        const char* name = font::get_level_font_display_name();

        font::print(0, 2, 230, label, tr / 2, tg / 2, tb / 2);
        font::print(PR_FONT_LEVEL, 2 + len_label, 230, name, tr / 2, tg / 2, tb / 2);
        break;
    }
    case Menu::ed_music:
    {
        font::print(PR_2X | PR_CEN | PR_CJK_HIGH, -1, 65, loc::gettext("Map Music"), tr, tg, tb);

        font::print_wrap(PR_CEN | PR_CJK_LOW, -1, 85, loc::gettext("Current map music:"), tr, tg, tb);
        const char* songname;
        switch(cl.levmusic)
        {
        case 0:
            songname = loc::gettext("No background music");
            break;
        case Music_PUSHINGONWARDS:
            songname = loc::gettext("1: Pushing Onwards");
            break;
        case Music_POSITIVEFORCE:
            songname = loc::gettext("2: Positive Force");
            break;
        case Music_POTENTIALFORANYTHING:
            songname = loc::gettext("3: Potential for Anything");
            break;
        case Music_PASSIONFOREXPLORING:
            songname = loc::gettext("4: Passion for Exploring");
            break;
        case Music_PAUSE:
            songname = loc::gettext("N/A: Pause");
            break;
        case Music_PRESENTINGVVVVVV:
            songname = loc::gettext("5: Presenting VVVVVV");
            break;
        case Music_PLENARY:
            songname = loc::gettext("N/A: Plenary");
            break;
        case Music_PREDESTINEDFATE:
            songname = loc::gettext("6: Predestined Fate");
            break;
        case Music_POSITIVEFORCEREVERSED:
            songname = loc::gettext("N/A: ecroF evitisoP");
            break;
        case Music_POPULARPOTPOURRI:
            songname = loc::gettext("7: Popular Potpourri");
            break;
        case Music_PIPEDREAM:
            songname = loc::gettext("8: Pipe Dream");
            break;
        case Music_PRESSURECOOKER:
            songname = loc::gettext("9: Pressure Cooker");
            break;
        case Music_PACEDENERGY:
            songname = loc::gettext("10: Paced Energy");
            break;
        case Music_PIERCINGTHESKY:
            songname = loc::gettext("11: Piercing the Sky");
            break;
        case Music_PREDESTINEDFATEREMIX:
            songname = loc::gettext("N/A: Predestined Fate Remix");
            break;
        default:
            songname = loc::gettext("?: something else");
            break;
        }
        font::print_wrap(PR_CEN, -1, 120, songname, tr, tg, tb);
        break;
    }
    case Menu::ed_quit:
        font::print_wrap(PR_CEN, -1, 90, loc::gettext("Save before quitting?"), tr, tg, tb);
        break;
    case Menu::ed_font:
    {
        font::print(PR_2X | PR_CEN, -1, 30, loc::gettext("Level Font"), tr, tg, tb);
        font::print_wrap(PR_CEN, -1, 65, loc::gettext("Select the language in which the text in this level is written."), tr, tg, tb);

        const char* label = loc::gettext("Font: ");
        int len_label = font::len(0, label);
        const char* name = font::get_level_font_display_name();

        font::print(0, 2, 230, label, tr/2, tg/2, tb/2);
        font::print(PR_FONT_LEVEL, 2+len_label, 230, name, tr/2, tg/2, tb/2);
        break;
    }
    default:
        break;
    }
}

static void draw_background_grid(void)
{
    for (int j = 0; j < 30; j++)
    {
        for (int i = 0; i < 40; i++)
        {
            if (i == 19 || i == 20 || j == 14 || j == 29)
            {
                // Major guidelines
                graphics.draw_rect(i * 8, j * 8, 7, 7, graphics.getRGB(32, 32, 32));
            }
            else if (i == 9 || i == 30 || j == 6 || j == 7 || j == 21 || j == 22)
            {
                // Minor guidelines
                graphics.draw_rect(i * 8, j * 8, 7, 7, graphics.getRGB(24, 24, 24));
            }
            else if (i % 4 == 0 || j % 4 == 0)
            {
                graphics.draw_rect(i * 8, j * 8, 7, 7, graphics.getRGB(16, 16, 16));
            }
            else
            {
                graphics.draw_rect(i * 8, j * 8, 7, 7, graphics.getRGB(8, 8, 8));
            }
        }
    }
}

static void draw_background(int warpdir)
{
    extern editorclass ed;

    switch (warpdir)
    {
    case 1:
        graphics.rcol = cl.getwarpbackground(ed.levx, ed.levy);
        graphics.drawbackground(3);
        break;
    case 2:
        graphics.rcol = cl.getwarpbackground(ed.levx, ed.levy);
        graphics.drawbackground(4);
        break;
    case 3:
        graphics.rcol = cl.getwarpbackground(ed.levx, ed.levy);
        graphics.drawbackground(5);
        break;
    default:
        break;
    }
}

static void draw_edgeguide(const TileTypes type, const int x, const int y, const bool vertical)
{
    static const SDL_Color white = graphics.getRGB(255 - help.glow, 255, 255);
    static const SDL_Color red = graphics.getRGB(255 - help.glow, 127, 127);

    if (type != TileType_SOLID && type != TileType_SPIKE)
    {
        return;
    }

    if (vertical)
    {
        graphics.fill_rect(x, y, 8, 2, (type == TileType_SOLID) ? white : red);
    }
    else
    {
        graphics.fill_rect(x, y, 2, 8, (type == TileType_SOLID) ? white : red);
    }
}

static void draw_edgeguides(void)
{
    extern editorclass ed;

    const int global_x = ed.levx * 40;
    const int global_y = ed.levy * 30;

    // Draw edge-guides, so there's no room misalignments!

    for (int i = 0; i < 40; i++)
    {
        if (i < 30)
        {
            // Left edge
            draw_edgeguide(ed.get_abs_tile_type(global_x - 1, global_y + i, true), 0, i * 8, false);
            // Right edge
            draw_edgeguide(ed.get_abs_tile_type(global_x + 40, global_y + i, true), 318, i * 8, false);
        }

        // Top edge
        draw_edgeguide(ed.get_abs_tile_type(global_x + i, global_y - 1, true), i * 8, 0, true);
        // Bottom edge
        draw_edgeguide(ed.get_abs_tile_type(global_x + i, global_y + 30, true), i * 8, 238, true);
    }

    static const SDL_Color green = graphics.getRGB(127, 255 - help.glow, 127);

    // Horizontal gravity line edge-guides

    for (size_t i = 0; i < customentities.size(); ++i)
    {
        const CustomEntity* entity = &customentities[i];
        const bool is_horizontal_gravity_line = entity->t == 11 && entity->p1 == 0;
        if (!is_horizontal_gravity_line)
        {
            continue;
        }
        const int x = entity->p2 * 8;
        const int w = entity->p3;
        if (entity->ry != ed.levy)
        {
            continue;
        }
        if (entity->rx == POS_MOD(ed.levx - 1, cl.mapwidth)
            // It's to the left...
            && x + w >= 312)
        {
            // And touching the right edge!
            graphics.fill_rect(x, entity->y * 8, 2, 8, green);
        }
        else if (entity->rx == POS_MOD(ed.levx + 1, cl.mapwidth)
            // It's to the right...
            && x <= 0)
        {
            // And touching the left edge!
            graphics.fill_rect(x + w - 2, entity->y * 8, 2, 8, green);
        }
    }
}

static void update_entities(void)
{
    extern editorclass ed;

    for (size_t i = 0; i < customentities.size(); ++i)
    {
        CustomEntity* entity = &customentities[i];

        if (entity->rx != ed.levx || entity->ry != ed.levy)
        {
            // It's not in this room, so just continue
            continue;
        }

        bool grav_line = (entity->t == 11);
        bool warp_line = (entity->t == 50);

        if ((grav_line || warp_line) && entity->p4 != 1)
        {
            // If it's a grav line or a warp line, and it's not locked
            if ((grav_line && entity->p1 == 0) || (warp_line && entity->p1 >= 2))
            {
                /* Horizontal */
                int tx = entity->x;
                int tx2 = tx;
                int ty = entity->y;
                while (ed.lines_can_pass(tx, ty))
                {
                    --tx;
                }
                while (ed.lines_can_pass(tx2, ty))
                {
                    ++tx2;
                }
                ++tx;
                entity->p2 = tx;
                entity->p3 = (tx2 - tx) * 8;
            }
            else
            {
                /* Vertical */
                int tx = entity->x;
                int ty = entity->y;
                int ty2 = ty;
                while (ed.lines_can_pass(tx, ty))
                {
                    --ty;
                }
                while (ed.lines_can_pass(tx, ty2))
                {
                    ++ty2;
                }
                ++ty;
                entity->p2 = ty;
                entity->p3 = (ty2 - ty) * 8;
            }
        }
    }
}

static void draw_entities(void)
{
    extern editorclass ed;

    const RoomProperty* const room = cl.getroomprop(ed.levx, ed.levy);

    //Draw entities
    obj.customplatformtile = game.customcol * 12;

    const int edent_under_cursor = ed.get_entity_at(ed.levx, ed.levy, ed.tilex, ed.tiley);

    // Special case for drawing gray entities
    bool custom_gray = room->tileset == 3 && room->tilecol == 6;

    // Draw entities backward to remain accurate with ingame
    for (int i = customentities.size() - 1; i >= 0; i--)
    {
        CustomEntity* entity = &customentities[i];

        // If the entity is in the current room, draw it
        if (entity->rx == ed.levx && entity->ry == ed.levy)
        {
            const int x = entity->x * 8;
            const int y = entity->y * 8;
            static const char arrows[] = "V^<>";

            switch (entity->t)
            {
            case 1: // Enemies
            {
                const int movement = entity->p1;

                if (custom_gray)
                {
                    ed.entcolreal = graphics.getcol(18);
                }

                graphics.draw_sprite(x, y, ed.get_enemy_tile(room->enemytype), ed.entcolreal);

                if (movement >= 0 && movement < 4)
                {
                    // If they have a basic movement type, draw an arrow to indicate direction
                    font::print(PR_FONT_8X8, x + 4, y + 4, std::string(1, arrows[movement]), 255, 255, 255 - help.glow);
                }

                graphics.draw_rect(x, y, 16, 16, graphics.getRGB(255, 164, 255));
                break;
            }
            case 2: // Conveyors & Platforms
            {
                const int movement = entity->p1;
                const short length = (movement == 7 || movement == 8) ? 8 : 4;
                const short glow = 255 - help.glow;

                for (int j = 0; j < length; j++) {
                    graphics.draw_grid_tile(custom_gray ? graphics.grphx.im_entcolours_tint : graphics.grphx.im_entcolours, obj.customplatformtile, x + (j * 8), y, 8, 8);
                }

                switch (movement)
                {
                case 0:
                case 1:
                case 2:
                case 3:
                    // If they have a basic movement type, draw an arrow to indicate direction
                    font::print(PR_FONT_8X8, x + 12, y, std::string(1, arrows[movement]), glow, glow, glow);
                    break;
                case 4:
                    // Always move right, stopping when hitting collision
                    font::print(PR_FONT_8X8, x + 8, y, ">I", glow, glow, glow);
                    break;
                case 5:
                    font::print(PR_FONT_8X8, x, y, ">>>>", glow, glow, glow);
                    break;
                case 6:
                    font::print(PR_FONT_8X8, x, y, "<<<<", glow, glow, glow);
                    break;
                case 7:
                    font::print(PR_FONT_8X8, x + 4, y, "> > > > ", glow, glow, glow);
                    break;
                case 8:
                    font::print(PR_FONT_8X8, x + 4, y, "< < < < ", glow, glow, glow);
                    break;
                }

                if (movement < 0)
                {
                    // Well, it's a negative type, so it'll just be still.
                    font::print(PR_FONT_8X8, x + 8, y, "[]", glow, glow, glow);
                }
                else if (movement > 8)
                {
                    // Invalid... draw a scary red X
                    font::print(PR_FONT_8X8, x + 12, y, "X", glow, 0, 0);
                }

                graphics.draw_rect(x, y, 8 * length, 8, graphics.getRGB(255, 255, 255));
                break;
            }
            case 3: // Disappearing Platforms
                for (int j = 0; j < 4; j++) {
                    graphics.draw_grid_tile(custom_gray ? graphics.grphx.im_entcolours_tint : graphics.grphx.im_entcolours, obj.customplatformtile, x + (j * 8), y, 8, 8);
                }

                font::print(PR_FONT_8X8, x, y, "////", 255 - help.glow, 255 - help.glow, 255 - help.glow);
                graphics.draw_rect(x, y, 32, 8, graphics.getRGB(255, 255, 255));
                break;
            case 9: // Shiny Trinkets
                graphics.draw_sprite(x, y, 22, 196, 196, 196);
                graphics.draw_rect(x, y, 16, 16, graphics.getRGB(255, 164, 164));
                break;
            case 10: // Checkpoints
                graphics.draw_sprite(x, y, 20 + entity->p1, 196, 196, 196);
                graphics.draw_rect(x, y, 16, 16, graphics.getRGB(255, 164, 164));
                break;
            case 11: // Gravity Lines
                // p2 is in tiles, and p3 is in pixels
                if (entity->p1 == 0)
                {
                    // Horizontal gravity line
                    const int left = entity->p2 * 8;
                    const int width = entity->p3;
                    graphics.fill_rect(left, y + 4, width, 1, graphics.getRGB(194, 194, 194));
                }
                else
                {
                    // Vertical gravity line
                    const int top = entity->p2 * 8;
                    const int height = entity->p3;
                    graphics.fill_rect(x + 3, top, 1, height, graphics.getRGB(194, 194, 194));
                }
                graphics.draw_rect(x, y, 8, 8, graphics.getRGB(164, 255, 164));
                break;
            case 13: // Warp Tokens
            {
                std::string text;

                graphics.draw_sprite(x, y, 18 + (ed.entframe % 2), 196, 196, 196);
                graphics.draw_rect(x, y, 16, 16, graphics.getRGB(255, 164, 164));

                if (i == edent_under_cursor)
                {
                    text = "(" + help.String(entity->p1 / 40 + 1) + "," + help.String(entity->p2 / 30 + 1) + ")";
                }
                else
                {
                    text = help.String(cl.findwarptoken(i));
                }

                font::print(PR_BOR | PR_CJK_HIGH, x, y - 8, text, 210, 210, 255);
                break;
            }
            case 15: // Crewmates
                graphics.draw_sprite(x - 4, y, 144, graphics.crewcolourreal(entity->p1));
                graphics.draw_rect(x, y, 16, 24, graphics.getRGB(164, 164, 164));
                break;
            case 16: // Start Point
            {
                const short labelcol = ed.entframe < 2 ? 255 : 196;

                if (entity->p1 == 0)  // Facing right
                {
                    graphics.draw_sprite(x - 4, y, 0, graphics.col_crewcyan);
                }
                else // Non-zero is facing left
                {
                    graphics.draw_sprite(x - 4, y, 3, graphics.col_crewcyan);
                }

                graphics.draw_rect(x, y, 16, 24, graphics.getRGB(255, 255, 164));
                font::print(PR_BOR | PR_CEN | PR_CJK_HIGH, x + 8, y - 8, loc::gettext("START"), labelcol, labelcol, labelcol);
                break;
            }
            case 17: // Roomtext
            {
                int width = 8;
                int height = 8;

                if (entity->scriptname.length() > 0)
                {
                    width = font::len(PR_FONT_LEVEL, entity->scriptname.c_str());
                    height = font::height(PR_FONT_LEVEL);
                }

                graphics.draw_rect(x, y, width, height, graphics.getRGB(96, 96, 96));
                font::print(PR_FONT_LEVEL | PR_CJK_LOW, x, y, entity->scriptname, 196, 196, 255 - help.glow);
                break;
            }
            case 18: // Terminals
            {
                int sprite = entity->p1;
                int corrected_y = y;

                // Not a boolean: just swapping 0 and 1, leaving the rest alone
                if (sprite == 0)
                {
                    sprite = 1; // Unflipped
                }
                else if (sprite == 1)
                {
                    sprite = 0; // Flipped;
                    corrected_y -= 8;
                }

                graphics.draw_sprite(x, corrected_y + 8, sprite + 16, 96, 96, 96);
                graphics.draw_rect(x, y, 16, 24, graphics.getRGB(164, 164, 164));
                if (i == edent_under_cursor)
                {
                    font::print(PR_FONT_LEVEL | PR_BOR | PR_CJK_HIGH, x, y - 8, entity->scriptname, 210, 210, 255);
                }
                break;
            }
            case 19: // Script Triggers
                graphics.draw_rect(x, y, entity->p1 * 8, entity->p2 * 8, graphics.getRGB(255, 164, 255));
                graphics.draw_rect(x, y, 8, 8, graphics.getRGB(255, 255, 255));
                if (i == edent_under_cursor)
                {
                    font::print(PR_FONT_LEVEL | PR_BOR | PR_CJK_HIGH, x, y - 8, entity->scriptname, 210, 210, 255);
                }
                break;
            case 50: // Warp Lines
                if (entity->p1 >= 2) // Horizontal
                {

                    int left = entity->p2;
                    int right = left + entity->p3 / 8;

                    graphics.draw_rect((left * 8), y + 1, (right - left) * 8, 6, graphics.getRGB(194, 255, 255));
                    graphics.draw_rect(x, y, 8, 8, graphics.getRGB(164, 255, 255));
                }
                else // Vertical
                {
                    int top = entity->p2;
                    int bottom = top + entity->p3 / 8;

                    graphics.draw_rect(x + 1, (top * 8), 6, (bottom - top) * 8, graphics.getRGB(194, 255, 255));
                    graphics.draw_rect(x, y, 8, 8, graphics.getRGB(164, 255, 255));
                }
                break;
            }
        }

        // Need to also check warp point destinations
        if (entity->t == 13 && ed.warp_token_entity != i &&
            entity->p1 / 40 == ed.levx && entity->p2 / 30 == ed.levy)
        {
            const int x = entity->p1 % 40 * 8;
            const int y = entity->p2 % 30 * 8;
            std::string text;

            graphics.draw_sprite(x, y, 18 + (ed.entframe % 2), 64, 64, 64);
            graphics.draw_rect((entity->p1 * 8) - (ed.levx * 40 * 8), (entity->p2 * 8) - (ed.levy * 30 * 8), 16, 16, graphics.getRGB(96, 64, 64));

            if (ed.tilex == x / 8 && ed.tiley == y / 8)
            {
                text = "(" + help.String(entity->rx + 1) + "," + help.String(entity->ry + 1) + ")";
            }
            else
            {
                text = help.String(cl.findwarptoken(i));
            }

            font::print(PR_BOR | PR_CJK_HIGH, x, y - 8, text, 190, 190, 225);
        }
    }
}

static void draw_ghosts(void)
{
    extern editorclass ed;

    //Draw ghosts (spooky!)
    if (game.ghostsenabled) {
        graphics.set_render_target(graphics.ghostTexture);
        graphics.set_blendmode(graphics.ghostTexture, SDL_BLENDMODE_BLEND);
        graphics.clear(0, 0, 0, 0);

        for (int i = 0; i < (int) ed.ghosts.size(); i++) {
            if (i <= ed.current_ghosts) { // We don't want all of them to show up at once :)
                if (ed.ghosts[i].rx != ed.levx || ed.ghosts[i].ry != ed.levy)
                    continue;
                SDL_Color ct = ed.ghosts[i].realcol;
                const int alpha = 3 * ct.a / 4;
                ct.a = (Uint8)alpha;
                graphics.draw_sprite(ed.ghosts[i].x, ed.ghosts[i].y, ed.ghosts[i].frame, ct);
            }
        }

        graphics.set_render_target(graphics.gameTexture);
        graphics.set_texture_alpha_mod(graphics.ghostTexture, 128);
        graphics.copy_texture(graphics.ghostTexture, NULL, NULL);
    }
}

static void adjust_box_coordinates(int x1, int y1, int x2, int y2, int* left, int* right, int* top, int* bottom)
{
    if (x1 < x2 + 8)
    {
        *right = x2 + 8;
        *left = x1;
    }
    else
    {
        *right = x1 + 8;
        *left = x2;
    }

    if (y1 < y2 + 8)
    {
        *bottom = y2 + 8;
        *top = y1;
    }
    else
    {
        *bottom = y1 + 8;
        *top = y2;
    }
}

static void draw_bounds(void)
{
    extern editorclass ed;

    const RoomProperty* const room = cl.getroomprop(ed.levx, ed.levy);

    // Draw boundaries
    if (room->enemyx1 != 0 || room->enemyy1 != 0 || room->enemyx2 != 320 || room->enemyy2 != 240)
    {
        graphics.draw_rect(room->enemyx1, room->enemyy1, room->enemyx2 - room->enemyx1, room->enemyy2 - room->enemyy1, graphics.getRGB(255 - (help.glow / 2), 64, 64));
    }

    if (room->platx1 != 0 || room->platy1 != 0 || room->platx2 != 320 || room->platy2 != 240)
    {
        graphics.draw_rect(room->platx1, room->platy1, room->platx2 - room->platx1, room->platy2 - room->platy1, graphics.getRGB(64, 64, 255 - (help.glow / 2)));
    }

    if (ed.substate == EditorSubState_DRAW_BOX)
    {
        if (ed.box_corner == BoxCorner_FIRST)
        {
            graphics.draw_rect(ed.tilex * 8, ed.tiley * 8, 8, 8, graphics.getRGB(210 + help.glow / 2, 191 + help.glow, 255 - help.glow / 2));
            graphics.draw_rect((ed.tilex * 8) + 2, (ed.tiley * 8) + 2, 4, 4, graphics.getRGB(105 + help.glow / 4, 100 + help.glow / 2, 128 - help.glow / 4));
        }
        else
        {
            int left;
            int right;
            int top;
            int bottom;

            adjust_box_coordinates(ed.box_point.x, ed.box_point.y, ed.tilex * 8, ed.tiley * 8, &left, &right, &top, &bottom);

            graphics.draw_rect(left, top, right - left, bottom - top, graphics.getRGB(210 + help.glow / 2, 191 + help.glow, 255 - help.glow / 2));
            graphics.draw_rect(left + 2, top + 2, (right - left) - 4, (bottom - top) - 4, graphics.getRGB(105 + help.glow / 4, 100 + help.glow / 2, 128 - help.glow / 4));
        }
    }
}

static inline bool check_point(bool connected[SCREEN_HEIGHT_TILES][SCREEN_WIDTH_TILES], int x, int y)
{
    if (x < 0 || x >= SCREEN_WIDTH_TILES || y < 0 || y >= SCREEN_HEIGHT_TILES)
    {
        return false;
    }

    return connected[y][x];
}

static void draw_cursor(void)
{
    extern editorclass ed;

    static const SDL_Color blue = graphics.getRGB(32, 32, 200);

    const int x = ed.tilex * 8;
    const int y = ed.tiley * 8;

    if (ed.substate == EditorSubState_DRAW_BOX)
    {
        // Just draw a 1x1 cursor, overriding everything else
        graphics.draw_rect(x, y, 8, 8, blue);
        return;
    }


    switch (ed.current_tool)
    {
    case EditorTool_WALLS:
    case EditorTool_BACKING:
        // Modifiers!
        if (ed.f_modifier)
        {
            bool connected[SCREEN_HEIGHT_TILES][SCREEN_WIDTH_TILES];
            SDL_zeroa(connected);

            ed.get_tile_fill(ed.tilex, ed.tiley, cl.gettile(ed.levx, ed.levy, ed.tilex, ed.tiley), connected);

            graphics.set_color(blue);

            for (int i = 0; i < SCREEN_WIDTH_TILES * SCREEN_HEIGHT_TILES; i++)
            {
                const int x = i % SCREEN_WIDTH_TILES;
                const int y = i / SCREEN_WIDTH_TILES;

                if (!connected[y][x])
                    continue;

                bool top_left = true;
                bool top_right = true;
                bool bottom_left = true;
                bool bottom_right = true;

                if (!check_point(connected, x - 1, y))
                {
                    top_left = false;
                    bottom_left = false;
                    SDL_RenderDrawLine(gameScreen.m_renderer, x * 8, y * 8, x * 8, y * 8 + 7);
                }
                if (!check_point(connected, x + 1, y))
                {
                    top_right = false;
                    bottom_right = false;
                    SDL_RenderDrawLine(gameScreen.m_renderer, x * 8 + 7, y * 8, x * 8 + 7, y * 8 + 7);
                }
                if (!check_point(connected, x, y - 1))
                {
                    top_left = false;
                    top_right = false;
                    SDL_RenderDrawLine(gameScreen.m_renderer, x * 8, y * 8, x * 8 + 7, y * 8);
                }
                if (!check_point(connected, x, y + 1))
                {
                    bottom_left = false;
                    bottom_right = false;
                    SDL_RenderDrawLine(gameScreen.m_renderer, x * 8, y * 8 + 7, x * 8 + 7, y * 8 + 7);
                }

                if (!check_point(connected, x - 1, y - 1) && top_left)
                    SDL_RenderDrawPoint(gameScreen.m_renderer, x * 8, y * 8);
                if (!check_point(connected, x - 1, y + 1) && top_right)
                    SDL_RenderDrawPoint(gameScreen.m_renderer, x * 8, y * 8 + 7);
                if (!check_point(connected, x + 1, y - 1) && bottom_left)
                    SDL_RenderDrawPoint(gameScreen.m_renderer, x * 8 + 7, y * 8);
                if (!check_point(connected, x + 1, y + 1) && bottom_right)
                    SDL_RenderDrawPoint(gameScreen.m_renderer, x * 8 + 7, y * 8 + 7);
            }
        }
        else if (ed.b_modifier) graphics.draw_rect(x, 0, 8, 240, blue); // Vertical
        else if (ed.h_modifier) graphics.draw_rect(0, y, 320, 8, blue); // Horizontal
        else if (ed.v_modifier) graphics.draw_rect(x - 32, y - 32, 24 + 48, 24 + 48, blue); // 9x9
        else if (ed.c_modifier) graphics.draw_rect(x - 24, y - 24, 24 + 32, 24 + 32, blue); // 7x7
        else if (ed.x_modifier) graphics.draw_rect(x - 16, y - 16, 24 + 16, 24 + 16, blue); // 5x5
        else if (ed.z_modifier) graphics.draw_rect(x - 8, y - 8, 24, 24, blue); // 3x3
        SDL_FALLTHROUGH;
    case EditorTool_SPIKES:
    case EditorTool_GRAVITY_LINES:
    case EditorTool_ROOMTEXT:
    case EditorTool_SCRIPTS:
        // 1x1
        graphics.draw_rect(x, y, 8, 8, blue);
        break;
    case EditorTool_TRINKETS:
    case EditorTool_CHECKPOINTS:
    case EditorTool_ENEMIES:
    case EditorTool_WARP_TOKENS:
        // 2x2
        graphics.draw_rect(x, y, 16, 16, blue);
        break;
    case EditorTool_DISAPPEARING_PLATFORMS:
    case EditorTool_CONVEYORS:
    case EditorTool_MOVING_PLATFORMS:
        // 1x4 (platforms)
        graphics.draw_rect(x, y, 32, 8, blue);
        break;
    case EditorTool_WARP_LINES:
        // 1x1, but X if not on an edge (warp lines)
        if (ed.tilex == 0 || ed.tilex == 39 || ed.tiley == 0 || ed.tiley == 29)
        {
            graphics.draw_rect(x, y, 8, 8, blue);
        }
        else
        {
            font::print(PR_FONT_8X8, x, y, "X", 255, 0, 0);
        }
        break;
    case EditorTool_TERMINALS:
    case EditorTool_CREWMATES:
    case EditorTool_START_POINT:
        // 2x3
        graphics.draw_rect(x, y, 16, 24, blue);
        break;
    default:
        break;
    }
}

static void draw_tile_drawer(int tileset)
{
    extern editorclass ed;

    // Tile drawer for direct mode
    int t2 = 0;
    if (ed.direct_mode_drawer > 0)
    {
        if (ed.direct_mode_drawer <= 4)
        {
            t2 = graphics.lerp((4 - ed.direct_mode_drawer + 1) * 12, (4 - ed.direct_mode_drawer) * 12);
        }

        // Draw five lines of the editor
        const int temp = ed.direct_mode_tile - (ed.direct_mode_tile % 40) - 80;
        graphics.fill_rect(0, -t2, 320, 40, graphics.getRGB(0, 0, 0));
        graphics.fill_rect(0, -t2 + 40, 320, 2, graphics.getRGB(255, 255, 255));

        int texturewidth;
        int textureheight;

        if (graphics.query_texture(graphics.grphx.im_tiles, NULL, NULL, &texturewidth, &textureheight) != 0)
        {
            return;
        }

        const int numtiles = (int)(texturewidth / 8) * (textureheight / 8);

        for (int x = 0; x < SCREEN_WIDTH_TILES; x++)
        {
            for (int y = 0; y < 5; y++)
            {
                if (tileset == 0)
                {
                    graphics.drawtile(x * 8, (y * 8) - t2, (temp + numtiles + (y * SCREEN_WIDTH_TILES) + x) % numtiles);
                }
                else
                {
                    graphics.drawtile2(x * 8, (y * 8) - t2, (temp + numtiles + (y * SCREEN_WIDTH_TILES) + x) % numtiles);
                }
            }
        }

        // Highlight our little block
        graphics.draw_rect(((ed.direct_mode_tile % SCREEN_WIDTH_TILES) * 8) - 2, 16 - t2 - 2, 12, 12, graphics.getRGB(255 - help.glow, 196, 196));
        graphics.draw_rect(((ed.direct_mode_tile % SCREEN_WIDTH_TILES) * 8) - 1, 16 - t2 - 1, 10, 10, graphics.getRGB(0, 0, 0));
    }

    if (ed.direct_mode_drawer > 0 && t2 <= 30)
    {
        short labellen = 2 + font::len(0, loc::gettext("Tile:"));
        font::print(PR_BOR, 2, 45 - t2, loc::gettext("Tile:"), 196, 196, 255 - help.glow);
        font::print(PR_BOR, labellen + 16, 45 - t2, help.String(ed.direct_mode_tile), 196, 196, 255 - help.glow);
        graphics.fill_rect(labellen + 2, 44 - t2, 10, 10, graphics.getRGB(255 - help.glow, 196, 196));
        graphics.fill_rect(labellen + 3, 45 - t2, 8, 8, graphics.getRGB(0, 0, 0));

        if (tileset == 0)
        {
            graphics.drawtile(labellen + 3, 45 - t2, ed.direct_mode_tile);
        }
        else
        {
            graphics.drawtile2(labellen + 3, 45 - t2, ed.direct_mode_tile);
        }
    }
    else
    {
        short labellen = 2 + font::len(0, loc::gettext("Tile:"));
        int y = 2 + font::height(0);
        y = SDL_max(y, 12);
        font::print(PR_BOR, 2, y, loc::gettext("Tile:"), 196, 196, 255 - help.glow);
        font::print(PR_BOR, labellen + 16, y, help.String(ed.direct_mode_tile), 196, 196, 255 - help.glow);
        graphics.fill_rect(labellen + 2, y - 1, 10, 10, graphics.getRGB(255 - help.glow, 196, 196));
        graphics.fill_rect(labellen + 3, y, 8, 8, graphics.getRGB(0, 0, 0));

        if (tileset == 0)
        {
            graphics.drawtile(labellen + 3, 12, ed.direct_mode_tile);
        }
        else
        {
            graphics.drawtile2(labellen + 3, 12, ed.direct_mode_tile);
        }
    }
}

static void draw_box_placer()
{
    extern editorclass ed;

    std::string message;
    if (ed.box_corner == BoxCorner_FIRST)
    {
        switch (ed.box_type)
        {
        case BoxType_SCRIPT:
            message = loc::gettext("SCRIPT BOX: Click on the first corner");
            break;
        case BoxType_ENEMY:
            message = loc::gettext("ENEMY BOUNDS: Click on the first corner");
            break;
        case BoxType_PLATFORM:
            message = loc::gettext("PLATFORM BOUNDS: Click on the first corner");
            break;
        default:
            message = loc::gettext("Click on the first corner");
            break;
        }
    }
    else if (ed.box_corner == BoxCorner_LAST)
    {
        switch (ed.box_type)
        {
        case BoxType_SCRIPT:
            message = loc::gettext("SCRIPT BOX: Click on the last corner");
            break;
        case BoxType_ENEMY:
            message = loc::gettext("ENEMY BOUNDS: Click on the last corner");
            break;
        case BoxType_PLATFORM:
            message = loc::gettext("PLATFORM BOUNDS: Click on the last corner");
            break;
        default:
            message = loc::gettext("Click on the last corner");
            break;
        }
    }

    short lines;
    message = font::string_wordwrap(0, message, 312, &lines);
    short textheight = font::height(0) * lines;

    graphics.fill_rect(0, 238 - textheight, 320, 240, graphics.getRGB(32, 32, 32));
    graphics.fill_rect(0, 239 - textheight, 320, 240, graphics.getRGB(0, 0, 0));

    font::print_wrap(0, 4, 240 - textheight, message.c_str(), 255, 255, 255, 8, 312);
}

static void draw_note()
{
    extern editorclass ed;

    if (ed.note_timer > 0 || ed.old_note_timer > 0)
    {
        short lines;
        std::string wrapped = font::string_wordwrap(0, ed.note, 304, &lines);
        short textheight = 8 + (lines - 1) * SDL_max(10, font::height(0));
        short banner_y = 120 - textheight / 2 - 5;

        float alpha = graphics.lerp(ed.old_note_timer, ed.note_timer);
        graphics.fill_rect(0, banner_y, 320, 10 + textheight, graphics.getRGB(92, 92, 92));
        graphics.fill_rect(0, banner_y + 1, 320, 8 + textheight, graphics.getRGB(0, 0, 0));
        font::print_wrap(PR_CEN, -1, banner_y + 5, wrapped.c_str(), 196 - ((45.0f - alpha) * 4), 196 - ((45.0f - alpha) * 4), 196 - ((45.0f - alpha) * 4));
    }
}

static void draw_toolbox(const char* coords)
{
    extern editorclass ed;

    // Draw the toolbox background
    graphics.fill_rect(0, 207, 320, 240, graphics.getRGB(32, 32, 32));
    graphics.fill_rect(0, 208, 320, 240, graphics.getRGB(0, 0, 0));

    // Draw all tools!
    const int tool_gap = 32;

    const int page = ed.current_tool / 10;
    const int max_pages = SDL_ceil(NUM_EditorTools / 10);
    const int page_tool_count = SDL_min(10, NUM_EditorTools - (page * 10));

    for (int i = 0; i < page_tool_count; i++)
    {
        const int current_tool_id = i + (page * 10);

        // First, draw the background
        graphics.fill_rect(4 + (i * tool_gap), 208, 20, 20, graphics.getRGB(32, 32, 32));

        // Draw the actual tool icon
        ed.draw_tool((EditorTools)current_tool_id, 4 + (i * tool_gap) + 2, 208 + 2);

        // Draw the tool outline...
        graphics.draw_rect(4 + (i * tool_gap), 208, 20, 20, (current_tool_id == ed.current_tool) ? graphics.getRGB(200, 200, 200) : graphics.getRGB(96, 96, 96));

        // ...and the hotkey
        const int col = current_tool_id == ed.current_tool ? 255 : 164;
        font::print(PR_FONT_8X8 | PR_BOR, 22 + i * tool_gap - 4, 224 - 4, ed.tool_key_chars[current_tool_id], col, col, col);
    }

    // Draw the page number, limit is 1 digit, so the max is 9 pages
    char buffer[4];
    SDL_snprintf(buffer, sizeof(buffer), "%d/%d", page + 1, max_pages + 1);
    font::print(PR_CJK_HIGH, 4, 232, buffer, 196, 196, 255 - help.glow);

    // Draw the button hint text
    char changetooltext[SCREEN_WIDTH_CHARS + 1];
    vformat_buf(changetooltext, sizeof(changetooltext),
        loc::gettext("{button1} and {button2} keys change tool"),
        "button1:str, button2:str",
        ",", "."
    );
    font::print(PR_CJK_HIGH | PR_RIGHT, 320, 232, changetooltext, 196, 196, 255 - help.glow);

    // Draw the current tool name
    char toolname_english[SCREEN_WIDTH_CHARS + 1];
    SDL_snprintf(toolname_english, sizeof(toolname_english), "%s: %s", ed.tool_key_chars[ed.current_tool], ed.tool_names[ed.current_tool]);

    const char* toolname = loc::gettext(toolname_english);

    int bgheight = 2 + font::height(0);
    int toolnamelen = font::len(0, toolname);
    graphics.fill_rect(0, 206 - bgheight, toolnamelen + 8, bgheight + 1, graphics.getRGB(32, 32, 32));
    graphics.fill_rect(0, 207 - bgheight, toolnamelen + 7, bgheight, graphics.getRGB(0, 0, 0));
    font::print(PR_BOR | PR_CJK_HIGH, 2, 198, toolname, 196, 196, 255 - help.glow);

    // And finally, draw the current room's coordinates
    int coordslen = font::len(0, coords);
    graphics.fill_rect(319 - coordslen - 8, 206 - bgheight, coordslen + 8, bgheight + 1, graphics.getRGB(32, 32, 32));
    graphics.fill_rect(320 - coordslen - 8, 207 - bgheight, coordslen + 8, bgheight, graphics.getRGB(0, 0, 0));
    font::print(PR_BOR | PR_CJK_HIGH | PR_RIGHT, 316, 198, coords, 196, 196, 255 - help.glow);
}

static void draw_main_ui(void)
{
    extern editorclass ed;

    const RoomProperty* const room = cl.getroomprop(ed.levx, ed.levy);

    char coords[8];
    SDL_snprintf(coords, sizeof(coords), "(%d,%d)", ed.levx + 1, ed.levy + 1);

    if (ed.toolbox_open)
    {
        draw_toolbox(coords);
    }
    else
    {
        if (room->roomname != "")
        {
            int font_height = font::height(PR_FONT_LEVEL);
            if (font_height <= 8)
            {
                graphics.footerrect.h = font_height + 2;
            }
            else
            {
                graphics.footerrect.h = font_height + 1;
            }
            graphics.footerrect.y = 240 - graphics.footerrect.h + ed.roomnamehide;

            graphics.set_blendmode(SDL_BLENDMODE_BLEND);
            graphics.fill_rect(&graphics.footerrect, graphics.getRGBA(0, 0, 0, graphics.translucentroomname ? 127 : 255));
            graphics.set_blendmode(SDL_BLENDMODE_NONE);

            font::print(PR_CEN | PR_BOR | PR_FONT_LEVEL | PR_CJK_LOW, -1, graphics.footerrect.y + 1 + ed.roomnamehide, room->roomname, 196, 196, 255 - help.glow);
            font::print(PR_BOR | PR_CJK_HIGH, 4, 232 - graphics.footerrect.h, loc::gettext("SPACE ^  SHIFT ^"), 196, 196, 255 - help.glow);
            font::print(PR_BOR | PR_CJK_HIGH | PR_RIGHT, 316, 232 - graphics.footerrect.h, coords, 196, 196, 255 - help.glow);
        }
        else
        {
            font::print(PR_BOR | PR_CJK_HIGH, 4, 232, loc::gettext("SPACE ^  SHIFT ^"), 196, 196, 255 - help.glow);
            font::print(PR_BOR | PR_CJK_HIGH | PR_RIGHT, 316, 232, coords, 196, 196, 255 - help.glow);
        }
    }

    if (ed.help_open)
    {
        const char* shiftmenuoptions[] = {
            loc::gettext("F1: Change Tileset"),
            loc::gettext("F2: Change Colour"),
            loc::gettext("F3: Change Enemies"),
            loc::gettext("F4: Enemy Bounds"),
            loc::gettext("F5: Platform Bounds"),
            "",
            loc::gettext("F9: Reload Resources"),
            loc::gettext("F10: Direct Mode"),
            "",
            loc::gettext("W: Change Warp Dir"),
            loc::gettext("E: Change Roomname"),
        };
        int menuwidth = 0;
        for (size_t i = 0; i < SDL_arraysize(shiftmenuoptions); i++)
        {
            int len = font::len(0, shiftmenuoptions[i]);
            if (len > menuwidth)
                menuwidth = len;
        }

        int lineheight = font::height(0);
        lineheight = SDL_max(10, lineheight);
        int left_y = 230 - SDL_arraysize(shiftmenuoptions) * lineheight;

        graphics.draw_rect(0, left_y - 3, menuwidth + 17, 240, graphics.getRGB(64, 64, 64));
        graphics.fill_rect(0, left_y - 2, menuwidth + 16, 240, graphics.getRGB(0, 0, 0));
        for (size_t i = 0; i < SDL_arraysize(shiftmenuoptions); i++)
            font::print(0, 4, left_y + i * lineheight, shiftmenuoptions[i], 164, 164, 164);

        graphics.draw_rect(220, 207, 100, 60, graphics.getRGB(64, 64, 64));
        graphics.fill_rect(221, 208, 160, 60, graphics.getRGB(0, 0, 0));
        font::print(0, 224, 210, loc::gettext("S: Save Map"), 164, 164, 164);
        font::print(0, 224, 210 + lineheight, loc::gettext("L: Load Map"), 164, 164, 164);
    }
}

void editorclass::draw_tool(EditorTools tool, int x, int y)
{
    switch (tool)
    {
    case EditorTool_WALLS:
        graphics.drawtile(x, y, 83);
        graphics.drawtile(x + 8, y, 83);
        graphics.drawtile(x, y + 8, 83);
        graphics.drawtile(x + 8, y + 8, 83);
        break;
    case EditorTool_BACKING:
        graphics.drawtile(x, y, 680);
        graphics.drawtile(x + 8, y, 680);
        graphics.drawtile(x, y + 8, 680);
        graphics.drawtile(x + 8, y + 8, 680);
        break;
    case EditorTool_SPIKES:
        graphics.drawtile(x + 4, y + 4, 8);
        break;
    case EditorTool_TRINKETS:
        graphics.draw_sprite(x, y, 22, 196, 196, 196);
        break;
    case EditorTool_CHECKPOINTS:
        graphics.draw_sprite(x, y, 21, 196, 196, 196);
        break;
    case EditorTool_DISAPPEARING_PLATFORMS:
        graphics.drawtile(x, y + 4, 3);
        graphics.drawtile(x + 8, y + 4, 4);
        break;
    case EditorTool_CONVEYORS:
        graphics.drawtile(x, y + 4, 24);
        graphics.drawtile(x + 8, y + 4, 24);
        break;
    case EditorTool_MOVING_PLATFORMS:
        graphics.drawtile(x, y + 4, 1);
        graphics.drawtile(x + 8, y + 4, 1);
        break;
    case EditorTool_ENEMIES:
        graphics.draw_sprite(x, y, 78 + entframe, 196, 196, 196);
        break;
    case EditorTool_GRAVITY_LINES:
        graphics.fill_rect(x + 2, y + 8, 12, 1, graphics.getRGB(255, 255, 255));
        break;
    case EditorTool_ROOMTEXT:
        font::print(PR_FONT_8X8, x + 1, y, "AB", 196, 196, 255 - help.glow);
        font::print(PR_FONT_8X8, x + 1, y + 9, "CD", 196, 196, 255 - help.glow);
        break;
    case EditorTool_TERMINALS:
        graphics.draw_sprite(x, y, 17, 196, 196, 196);
        break;
    case EditorTool_SCRIPTS:
        graphics.draw_rect(x + 4, y + 4, 8, 8, graphics.getRGB(96, 96, 96));
        break;
    case EditorTool_WARP_TOKENS:
        graphics.draw_sprite(x, y, 18 + (entframe % 2), 196, 196, 196);
        break;
    case EditorTool_WARP_LINES:
        graphics.fill_rect(x + 6, y + 2, 4, 12, graphics.getRGB(255, 255, 255));
        break;
    case EditorTool_CREWMATES:
        graphics.draw_sprite(x, y, 186, graphics.col_crewblue);
        break;
    case EditorTool_START_POINT:
        graphics.draw_sprite(x, y, 184, graphics.col_crewcyan);
        break;
    default:
        break;
    }
}

void editorrender(void)
{
    extern editorclass ed;

    const RoomProperty* const room = cl.getroomprop(ed.levx, ed.levy);

    graphics.clear();

    switch (ed.state)
    {
    case EditorState_DRAW:
        // Draw the editor guidelines
        draw_background_grid();

        // Draw the background, if any, over the guidelines
        draw_background(room->warpdir);

        graphics.drawmap();
        draw_edgeguides();

        draw_entities();
        draw_ghosts();

        draw_bounds();

        if (room->directmode == 1)
        {
            draw_tile_drawer(room->tileset);
        }

        draw_cursor();

        switch (ed.substate)
        {
        case EditorSubState_MAIN:
        {
            draw_main_ui();

            // Draw the current tool name
            char toolname_english[SCREEN_WIDTH_CHARS + 1];
            SDL_snprintf(toolname_english, sizeof(toolname_english), "%s: %s", ed.tool_key_chars[ed.current_tool], ed.tool_names[ed.current_tool]);

            const char* toolname = loc::gettext(toolname_english);

            font::print(PR_BOR, 2, 2, toolname, 196, 196, 255 - help.glow);

            break;
        }
        case EditorSubState_DRAW_BOX:
            draw_box_placer();
            break;
        case EditorSubState_DRAW_INPUT:
        {
            short lines;
            std::string wrapped = font::string_wordwrap(0, ed.current_text_desc, 312, &lines);
            short textheight = font::height(0) * lines + font::height(PR_FONT_LEVEL);

            graphics.fill_rect(0, 238 - textheight, 320, 240, graphics.getRGB(32, 32, 32));
            graphics.fill_rect(0, 239 - textheight, 320, 240, graphics.getRGB(0, 0, 0));
            font::print_wrap(0, 4, 240 - textheight, wrapped.c_str(), 255, 255, 255, 8, 312);
            std::string input = key.keybuffer;
            if (ed.entframe < 2)
            {
                input += "_";
            }
            else
            {
                input += " ";
            }
            font::print(PR_CEN | PR_FONT_LEVEL | PR_CJK_HIGH, -1, 232, input, 196, 196, 255 - help.glow);
            break;
        }
        case EditorSubState_DRAW_WARPTOKEN:
        {
            // Placing warp token
            int textheight = font::height(0);
            graphics.fill_rect(0, 237 - textheight * 2, 320, 240, graphics.getRGB(32, 32, 32));
            graphics.fill_rect(0, 238 - textheight * 2, 320, 240, graphics.getRGB(0, 0, 0));
            font::print(PR_CJK_LOW, 4, 240 - textheight * 2, loc::gettext("Left click to place warp destination"), 196, 196, 255 - help.glow);
            font::print(PR_CJK_LOW, 4, 240 - textheight, loc::gettext("Right click to cancel"), 196, 196, 255 - help.glow);

            break;
        }
        default:
            break;
        }

        break;
    case EditorState_SCRIPTS:
        // Intended to look like Commodore 64's UI

        graphics.fill_rect(0, 0, 320, 240, graphics.getRGB(123, 111, 218));
        graphics.fill_rect(14, 16, 292, 208, graphics.getRGB(61, 48, 162));

        switch (ed.substate)
        {
        case EditorSubState_MAIN:
            font::print(PR_CEN, -1, 28, loc::gettext("**** VVVVVV SCRIPT EDITOR ****"), 123, 111, 218);
            font::print(PR_CEN, -1, 44, loc::gettext("PRESS ESC TO RETURN TO MENU"), 123, 111, 218);

            if (script.customscripts.empty())
            {
                font::print(PR_CEN, -1, 110, loc::gettext("NO SCRIPT IDS FOUND"), 123, 111, 218);
                font::print_wrap(PR_CEN, -1, 130, loc::gettext("CREATE A SCRIPT WITH EITHER THE TERMINAL OR SCRIPT BOX TOOLS"), 123, 111, 218, 10, 288);
                break;
            }
            for (int i = 0; i < 9; i++)
            {
                const int offset = ed.script_list_offset + i;
                const bool draw = offset < (int) script.customscripts.size();
                if (!draw)
                {
                    continue;
                }
                if (offset == ed.selected_script)
                {
                    std::string text_upper(loc::toupper(script.customscripts[script.customscripts.size() - 1 - offset].name));
                    char buffer[SCREEN_WIDTH_CHARS + 1];
                    vformat_buf(buffer, sizeof(buffer), loc::get_langmeta()->menu_select.c_str(), "label:str", text_upper.c_str());
                    font::print(PR_CEN, -1, 68 + (i * 16), buffer, 123, 111, 218);
                }
                else
                {
                    font::print(PR_CEN, -1, 68 + (i * 16), script.customscripts[script.customscripts.size() - 1 - offset].name, 123, 111, 218);
                }
            }
            break;
        case EditorSubState_SCRIPTS_EDIT:
        {
            // Draw the current script's name
            graphics.fill_rect(14, 226, 292, 12, graphics.getRGB(61, 48, 162));
            char namebuffer[SCREEN_WIDTH_CHARS + 1];
            vformat_buf(
                namebuffer, sizeof(namebuffer),
                loc::gettext("CURRENT SCRIPT: {name}"),
                "name:str",
                ed.current_script.c_str()
            );
            font::print(PR_CEN, -1, 228, namebuffer, 123, 111, 218);

            // Draw text
            int font_height = font::height(PR_FONT_LEVEL);
            for (int i = 0; i < ed.lines_visible; i++)
            {
                if (i + ed.script_offset < (int) ed.script_buffer.size())
                {
                    font::print(PR_FONT_LEVEL | PR_CJK_LOW, 16, 20 + (i * font_height), ed.script_buffer[i + ed.script_offset], 123, 111, 218);
                }
            }

            // Draw cursor
            if (ed.entframe < 2)
            {
                font::print(PR_FONT_LEVEL | PR_CJK_LOW, 16 + font::len(PR_FONT_LEVEL, ed.script_buffer[ed.script_cursor_y].c_str()), 20 + ((ed.script_cursor_y - ed.script_offset) * font_height), "_", 123, 111, 218);
            }
            break;
        }
        default:
            break;
        }
        break;
    case EditorState_MENU:
    {
        if (!game.colourblindmode)
        {
            graphics.drawtowerbackground(graphics.titlebg);
        }
        else
        {
            graphics.clear();
        }

        int tr = SDL_clamp(graphics.titlebg.r - (help.glow / 4) - int(fRandom() * 4), 0, 255);
        int tg = SDL_clamp(graphics.titlebg.g - (help.glow / 4) - int(fRandom() * 4), 0, 255);
        int tb = SDL_clamp(graphics.titlebg.b - (help.glow / 4) - int(fRandom() * 4), 0, 255);

        editormenurender(tr, tg, tb);
        graphics.drawmenu(tr, tg, tb, game.currentmenuname);
        break;
    }
    default:
        break;
    }

    draw_note();

    graphics.drawfade();

    graphics.render();
}

void editorrenderfixed(void)
{
    extern editorclass ed;
    const RoomProperty* const room = cl.getroomprop(ed.levx, ed.levy);
    graphics.updatetitlecolours();

    game.customcol = cl.getlevelcol(room->tileset, room->tilecol) + 1;
    ed.entcol = cl.getenemycol(game.customcol);

    ed.entcolreal = graphics.getcol(ed.entcol);

    if (game.ghostsenabled)
    {
        for (size_t i = 0; i < ed.ghosts.size(); i++)
        {
            GhostInfo* ghost = &ed.ghosts[i];

            if ((int) i > ed.current_ghosts || ghost->rx != ed.levx || ghost->ry != ed.levy)
            {
                continue;
            }

            ghost->realcol = graphics.getcol(ghost->col);
        }

        ed.current_ghosts++;
        if (ed.z_modifier)
        {
            ed.current_ghosts++;
        }

        ed.current_ghosts = SDL_min(ed.current_ghosts, ed.ghosts.size());
    }

    switch (ed.state)
    {
    case EditorState_DRAW:
        switch (room->warpdir)
        {
        case 1:
            graphics.rcol = cl.getwarpbackground(ed.levx, ed.levy);
            graphics.updatebackground(3);
            break;
        case 2:
            graphics.rcol = cl.getwarpbackground(ed.levx, ed.levy);
            graphics.updatebackground(4);
            break;
        case 3:
            graphics.rcol = cl.getwarpbackground(ed.levx, ed.levy);
            graphics.updatebackground(5);
            break;
        default:
            break;
        }
        break;
    case EditorState_MENU:
        graphics.titlebg.bypos -= 2;
        graphics.titlebg.bscroll = -2;
        graphics.updatetowerbackground(graphics.titlebg);
        break;
    default:
        break;
    }

    if (cl.getroomprop(ed.levx, ed.levy)->directmode == 1)
    {
        if (ed.direct_mode_drawer > 0)
        {
            ed.direct_mode_drawer--;
        }
    }
    else
    {
        ed.direct_mode_drawer = 0;
    }

    if (cl.getroomprop(ed.levx, ed.levy)->roomname != "")
    {
        if (ed.tiley < 28)
        {
            if (ed.roomnamehide > 0)
            {
                ed.roomnamehide--;
            }
        }
        else
        {
            if (ed.roomnamehide < 14)
            {
                ed.roomnamehide++;
            }
        }
    }
    else
    {
        if (ed.tiley < 28)
        {
            ed.roomnamehide = 0;
        }
        else
        {
            ed.roomnamehide = 14;
        }
    }
}

static void input_submitted(void)
{
    extern editorclass ed;

    *ed.current_text_ptr = key.keybuffer;

    ed.help_open = false;
    ed.shiftkey = false;

    bool reset_text_mode = true;

    key.disabletextentry();

    ed.substate = EditorSubState_MAIN;

    switch (ed.current_text_mode)
    {
    case TEXT_GOTOROOM:
    {
        char coord_x[16];
        char coord_y[16];

        const char* comma = SDL_strchr(key.keybuffer.c_str(), ',');

        bool valid_input = comma != NULL;

        if (valid_input)
        {
            SDL_strlcpy(
                coord_x,
                key.keybuffer.c_str(),
                SDL_min((size_t) (comma - key.keybuffer.c_str() + 1), sizeof(coord_x))
            );
            SDL_strlcpy(coord_y, &comma[1], sizeof(coord_y));

            valid_input = is_number(coord_x) && is_number(coord_y);
        }

        if (!valid_input)
        {
            ed.show_note(loc::gettext("ERROR: Invalid format"));
            break;
        }

        ed.levx = SDL_clamp(help.Int(coord_x) - 1, 0, cl.mapwidth - 1);
        ed.levy = SDL_clamp(help.Int(coord_y) - 1, 0, cl.mapheight - 1);
        graphics.backgrounddrawn = false;
        break;
    }
    case TEXT_LOAD:
    {
        std::string loadstring = ed.filename + ".vvvvvv";
        if (cl.load(loadstring))
        {
            // don't use filename, it has the full path
            char buffer[3 * SCREEN_WIDTH_CHARS + 1];
            vformat_buf(buffer, sizeof(buffer), loc::gettext("Loaded map: {filename}.vvvvvv"), "filename:str", ed.filename.c_str());
            ed.show_note(buffer);
        }
        else
        {
            ed.show_note(loc::gettext("ERROR: Could not load level"));
        }
        graphics.foregrounddrawn = false;
        graphics.backgrounddrawn = false;
        ed.substate = EditorSubState_MAIN;
        break;
    }
    case TEXT_SAVE:
    {
        std::string savestring = ed.filename + ".vvvvvv";
        if (cl.save(savestring))
        {
            char buffer[3 * SCREEN_WIDTH_CHARS + 1];
            vformat_buf(buffer, sizeof(buffer), loc::gettext("Saved map: {filename}.vvvvvv"), "filename:str", ed.filename.c_str());
            ed.show_note(buffer);
        }
        else
        {
            ed.show_note(loc::gettext("ERROR: Could not save level!"));
            ed.saveandquit = false;
        }
        ed.note_timer = 45;

        if (ed.saveandquit)
        {
            graphics.fademode = FADE_START_FADEOUT; /* quit editor */
        }
        break;
    }
    case TEXT_SCRIPT:
        ed.clear_script_buffer();
        if (!ed.script_exists(key.keybuffer))
        {
            ed.create_script(key.keybuffer);
        }
        break;
    case TEXT_TITLE:
        cl.title = key.keybuffer;
        if (cl.title == "")
        {
            cl.title = "Untitled Level";
        }
        break;
    case TEXT_CREATOR:
        cl.creator = key.keybuffer;
        if (cl.creator == "")
        {
            cl.creator = "Unknown";
        }
        break;
    case TEXT_WEBSITE:
        cl.website = key.keybuffer;
        break;
    case TEXT_DESC1:
        cl.Desc1 = key.keybuffer;
        ed.current_text_mode = TEXT_DESC2;
        ed.substate = EditorSubState_MENU_INPUT;
        reset_text_mode = false;
        key.enabletextentry();
        ed.current_text_ptr = &(key.keybuffer);
        key.keybuffer = cl.Desc2;
        break;
    case TEXT_DESC2:
        cl.Desc2 = key.keybuffer;

        if (font::height(PR_FONT_LEVEL) <= 10)
        {
            ed.current_text_mode = TEXT_DESC3;
            key.enabletextentry();
            ed.substate = EditorSubState_MENU_INPUT;
            reset_text_mode = false;
            ed.current_text_ptr = &(key.keybuffer);
            key.keybuffer = cl.Desc3;
        }
        else
        {
            cl.Desc3 = "";
        }

        break;
    case TEXT_DESC3:
        cl.Desc3 = key.keybuffer;
        break;
    default:
        break;
    }

    if (reset_text_mode)
    {
        ed.current_text_mode = TEXT_NONE;
    }
}

void editorlogic(void)
{
    extern editorclass ed;

    //Misc
    help.updateglow();

    ed.entframedelay--;
    if (ed.entframedelay <= 0)
    {
        ed.entframe = (ed.entframe + 1) % 4;
        ed.entframedelay = 8;
    }

    ed.old_note_timer = ed.note_timer;
    ed.note_timer = SDL_max(ed.note_timer - 1, 0);

    update_entities();

    if (graphics.fademode == FADE_FULLY_BLACK)
    {
        //Return to game
        graphics.titlebg.colstate = 10;
        map.nexttowercolour();
        game.quittomenu();
        music.play(Music_PRESENTINGVVVVVV); // should be before game.quittomenu()
    }
}

void editorclass::add_entity(int rx, int ry, int xp, int yp, int tp, int p1, int p2, int p3, int p4, int p5, int p6)
{
    CustomEntity entity;

    entity.rx = rx;
    entity.ry = ry;
    entity.x = xp;
    entity.y = yp;
    entity.t = tp;
    entity.p1 = p1;
    entity.p2 = p2;
    entity.p3 = p3;
    entity.p4 = p4;
    entity.p5 = p5;
    entity.p6 = p6;
    entity.scriptname = "";

    customentities.push_back(entity);
}

void editorclass::remove_entity(int t)
{
    customentities.erase(customentities.begin() + t);
}

int editorclass::get_entity_at(int rx, int ry, int xp, int yp)
{
    for (size_t i = 0; i < customentities.size(); i++)
    {
        const CustomEntity* entity = &customentities[i];
        if (entity->rx == rx && entity->ry == ry &&
            entity->x == xp && entity->y == yp)
        {
            return i;
        }
    }
    return -1;
}

static void set_tile_interpolated(const int x1, const int x2, const int y1, const int y2, const int tile)
{
    extern editorclass ed;

    // draw a line between (x1, y1) and (x2, y2)

    const int dx = x2 - x1;
    const int dy = y2 - y1;

    const int steps = SDL_max(SDL_abs(dx), SDL_abs(dy));

    if (steps == 0)
    {
        ed.set_tile(x1, y1, tile);
        return;
    }

    for (int i = 0; i <= steps; i++)
    {
        const int x = x1 + (dx * i) / steps;
        const int y = y1 + (dy * i) / steps;

        ed.set_tile(x, y, tile);
    }
}

void editorclass::get_tile_fill(int tilex, int tiley, int tile, bool connected[SCREEN_HEIGHT_TILES][SCREEN_WIDTH_TILES])
{
    if (tilex < 0 || tilex >= SCREEN_WIDTH_TILES || tiley < 0 || tiley >= SCREEN_HEIGHT_TILES)
    {
        // It's out of bounds
        return;
    }

    if (connected[tiley][tilex])
    {
        // We've already visited this tile
        return;
    }

    if (cl.gettile(levx, levy, tilex, tiley) != tile)
    {
        // It's not the same tile
        return;
    }

    // Yep, this is connected!
    connected[tiley][tilex] = true;

    // Check surrounding 4 tiles
    get_tile_fill(tilex - 1, tiley, tile, connected);
    get_tile_fill(tilex + 1, tiley, tile, connected);
    get_tile_fill(tilex, tiley - 1, tile, connected);
    get_tile_fill(tilex, tiley + 1, tile, connected);
}

void editorclass::handle_tile_placement(const int tile)
{
    int range = 1;

    if (f_modifier)
    {
        bool connected[SCREEN_HEIGHT_TILES][SCREEN_WIDTH_TILES];
        SDL_zeroa(connected);

        get_tile_fill(tilex, tiley, cl.gettile(levx, levy, tilex, tiley), connected);

        for (int i = 0; i < SCREEN_WIDTH_TILES * SCREEN_HEIGHT_TILES; i++)
        {
            const int x = i % SCREEN_WIDTH_TILES;
            const int y = i / SCREEN_WIDTH_TILES;

            if (connected[y][x])
            {
                set_tile(x, y, tile);
            }
        }
        return;
    }
    else if (b_modifier)
    {
        // Vertical line
        for (int i = 0; i < SCREEN_HEIGHT_TILES; i++)
        {
            set_tile_interpolated(old_tilex, tilex, i, i, tile);
        }
        return;
    }
    else if (h_modifier)
    {
        // Horizontal line
        for (int i = 0; i < SCREEN_WIDTH_TILES; i++)
        {
            set_tile_interpolated(i, i, old_tiley, tiley, tile);
        }
        return;
    }
    else if (v_modifier)
    {
        range = 4;
    }
    else if (c_modifier)
    {
        range = 3;
    }
    else if (x_modifier)
    {
        range = 2;
    }
    else if (z_modifier)
    {
        range = 1;
    }
    else
    {
        set_tile_interpolated(old_tilex, tilex, old_tiley, tiley, tile);
        return;
    }

    for (int i = -range; i <= range; i++)
    {
        for (int j = -range; j <= range; j++)
        {
            set_tile_interpolated(old_tilex + i, tilex + i, old_tiley + j, tiley + j, tile);
        }
    }
}

void editorclass::tool_remove()
{
    switch (current_tool)
    {
    case EditorTool_WALLS:
    case EditorTool_BACKING:
        handle_tile_placement(0);
        break;
    case EditorTool_SPIKES:
        set_tile_interpolated(old_tilex, tilex, old_tiley, tiley, 0);
        break;
    default:
        break;
    }

    for (size_t i = 0; i < customentities.size(); i++)
    {
        const CustomEntity* entity = &customentities[i];
        if (entity->rx == levx && entity->ry == levy &&
            entity->x == tilex && entity->y == tiley)
        {
            remove_entity(i);
        }
    }
}

void editorclass::entity_clicked(const int index)
{
    CustomEntity* entity = &customentities[index];

    lclickdelay = 1;

    switch (entity->t)
    {
    case 1:
        // Enemies
        entity->p1 = (entity->p1 + 1) % 4;
        break;
    case 2:
    {
        // Moving Platforms and Conveyors
        const bool conveyor = entity->p1 >= 5;
        entity->p1++;
        if (conveyor)
        {
            entity->p1 = (entity->p1 - 5) % 4 + 5;
        }
        else
        {
            entity->p1 %= 4;
        }
        break;
    }
    case 10:
        // Checkpoints
        // If it's not textured as a checkpoint, then just leave it be
        if (entity->p1 == 0 || entity->p1 == 1)
        {
            entity->p1 = (entity->p1 + 1) % 2;
        }
        break;
    case 11:
    case 16:
        // Gravity Lines, Start Point
        entity->p1 = (entity->p1 + 1) % 2;
        break;
    case 15:
        // Crewmates
        entity->p1 = (entity->p1 + 1) % 6;
        break;
    case 17:
        // Roomtext
        get_input_line(TEXT_ROOMTEXT, loc::gettext("Enter roomtext:"), &entity->scriptname);
        text_entity = index;
        break;
    case 18:
        // Terminals
        if (entity->p1 == 0 || entity->p1 == 1)
        {
            // Flip the terminal, but if it's not textured as a terminal leave it alone
            entity->p1 = (entity->p1 + 1) % 2;
        }
        SDL_FALLTHROUGH;
    case 19:
        // Script Boxes (and terminals)
        get_input_line(TEXT_SCRIPT, loc::gettext("Enter script name:"), &entity->scriptname);
        text_entity = index;
        break;
    }
}

void editorclass::tool_place()
{
    const int entity = get_entity_at(levx, levy, tilex, tiley);
    if (entity != -1)
    {
        entity_clicked(entity);
        return;
    }

    switch (current_tool)
    {
    case EditorTool_WALLS:
    case EditorTool_BACKING:
    {
        int tile = 0;

        if (cl.getroomprop(levx, levy)->directmode >= 1)
        {
            tile = direct_mode_tile;
        }
        else if (current_tool == EditorTool_WALLS)
        {
            tile = 1;
        }
        else if (current_tool == EditorTool_BACKING)
        {
            tile = 2;
        }

        handle_tile_placement(tile);
        break;
    }
    case EditorTool_SPIKES:
        set_tile_interpolated(old_tilex, tilex, old_tiley, tiley, 8);
        break;
    case EditorTool_TRINKETS:
        if (cl.numtrinkets() < 100)
        {
            add_entity(levx, levy, tilex, tiley, 9);
            lclickdelay = 1;
        }
        else
        {
            show_note(loc::gettext("ERROR: Max number of trinkets is 100"));
        }
        break;
    case EditorTool_CHECKPOINTS:
        add_entity(levx, levy, tilex, tiley, 10, 1);
        lclickdelay = 1;
        break;
    case EditorTool_DISAPPEARING_PLATFORMS:
        add_entity(levx, levy, tilex, tiley, 3);
        lclickdelay = 1;
        break;
    case EditorTool_CONVEYORS:
        add_entity(levx, levy, tilex, tiley, 2, 5);
        lclickdelay = 1;
        break;
    case EditorTool_MOVING_PLATFORMS:
        add_entity(levx, levy, tilex, tiley, 2, 0);
        lclickdelay = 1;
        break;
    case EditorTool_ENEMIES:
        add_entity(levx, levy, tilex, tiley, 1, 0);
        lclickdelay = 1;
        break;
    case EditorTool_GRAVITY_LINES:
        add_entity(levx, levy, tilex, tiley, 11, 0);
        lclickdelay = 1;
        break;
    case EditorTool_ROOMTEXT:
        lclickdelay = 1;
        text_entity = customentities.size();
        add_entity(levx, levy, tilex, tiley, 17);
        get_input_line(TEXT_ROOMTEXT, loc::gettext("Enter roomtext:"), &(customentities[text_entity].scriptname));
        break;
    case EditorTool_TERMINALS:
        lclickdelay = 1;
        text_entity = customentities.size();
        add_entity(levx, levy, tilex, tiley, 18, 0);
        get_input_line(TEXT_SCRIPT, loc::gettext("Enter script name:"), &(customentities[text_entity].scriptname));
        break;
    case EditorTool_SCRIPTS:
        substate = EditorSubState_DRAW_BOX;
        box_corner = BoxCorner_LAST;
        box_type = BoxType_SCRIPT;
        box_point.x = tilex * 8;
        box_point.y = tiley * 8;

        lclickdelay = 1;
        break;
    case EditorTool_WARP_TOKENS:
        substate = EditorSubState_DRAW_WARPTOKEN;
        warp_token_entity = customentities.size();
        add_entity(levx, levy, tilex, tiley, 13);
        lclickdelay = 1;
        break;
    case EditorTool_WARP_LINES:
        //Warp lines
        if (tilex == 0)
        {
            add_entity(levx, levy, tilex, tiley, 50, 0);
        }
        else if (tilex == 39)
        {
            add_entity(levx, levy, tilex, tiley, 50, 1);
        }
        else if (tiley == 0)
        {
            add_entity(levx, levy, tilex, tiley, 50, 2);
        }
        else if (tiley == 29)
        {
            add_entity(levx, levy, tilex, tiley, 50, 3);
        }
        else
        {
            show_note(loc::gettext("ERROR: Warp lines must be on edges"));
        }
        lclickdelay = 1;
        break;
    case EditorTool_CREWMATES:
        if (cl.numcrewmates() < 100)
        {
            add_entity(levx, levy, tilex, tiley, 15, int(fRandom() * 6));
            lclickdelay = 1;
        }
        else
        {
            show_note(loc::gettext("ERROR: Max number of crewmates is 100"));
        }
        break;
    case EditorTool_START_POINT:
        //If there is another start point, destroy it
        for (size_t i = 0; i < customentities.size(); i++)
        {
            if (customentities[i].t == 16)
            {
                remove_entity(i);
                i--;
            }
        }
        add_entity(levx, levy, tilex, tiley, 16, 0);
        lclickdelay = 1;
        break;
    default:
        break;
    }
}

static void creategameoptions(void)
{
    game.createmenu(Menu::options);
}

static void nextbgcolor(void)
{
    map.nexttowercolour();
}

static void editormenuactionpress(void)
{
    extern editorclass ed;

    switch (game.currentmenuname)
    {
    case Menu::ed_desc:
        switch (game.currentmenuoption)
        {
        case 0:
        {
            bool title_is_gettext;
            translate_title(cl.title, &title_is_gettext);

            ed.current_text_mode = TEXT_TITLE;
            ed.substate = EditorSubState_MENU_INPUT;
            key.enabletextentry();
            ed.current_text_ptr = &(key.keybuffer);

            if (title_is_gettext)
            {
                key.keybuffer = "";
            }
            else
            {
                key.keybuffer = cl.title;
            }
            break;
        }
        case 1:
        {
            bool creator_is_gettext;
            translate_creator(cl.creator, &creator_is_gettext);

            ed.current_text_mode = TEXT_CREATOR;
            ed.substate = EditorSubState_MENU_INPUT;
            key.enabletextentry();
            ed.current_text_ptr = &(key.keybuffer);
            if (creator_is_gettext)
            {
                key.keybuffer = "";
            }
            else
            {
                key.keybuffer = cl.creator;
            }
            break;
        }
        case 2:
            ed.current_text_mode = TEXT_DESC1;
            ed.substate = EditorSubState_MENU_INPUT;
            key.enabletextentry();
            ed.current_text_ptr = &(key.keybuffer);
            key.keybuffer = cl.Desc1;
            break;
        case 3:
            ed.current_text_mode = TEXT_WEBSITE;
            ed.substate = EditorSubState_MENU_INPUT;
            key.enabletextentry();
            ed.current_text_ptr = &(key.keybuffer);
            key.keybuffer=cl.website;
            break;
        case 4:
            game.createmenu(Menu::ed_font);
            map.nexttowercolour();
            break;
        case 5:
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        music.playef(Sound_VIRIDIAN);
        break;
    case Menu::ed_settings:
        switch (game.currentmenuoption)
        {
        case 0:
            //Change level description stuff
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::ed_desc);
            map.nexttowercolour();
            break;
        case 1:
            //Enter script editormode
            music.playef(Sound_VIRIDIAN);

            ed.state = EditorState_SCRIPTS;
            ed.substate = EditorSubState_MAIN;

            ed.clear_script_buffer();
            key.keybuffer = "";
            ed.script_list_offset = 0;
            ed.selected_script = 0;

            ed.script_cursor_y = 0;
            ed.script_cursor_x = 0;
            ed.script_offset = 0;
            ed.lines_visible = 200 / font::height(PR_FONT_LEVEL);
            break;
        case 2:
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::ed_music);
            map.nexttowercolour();
            if(cl.levmusic>0) music.play(cl.levmusic);
            break;
        case 3:
            music.playef(Sound_VIRIDIAN);
            game.ghostsenabled = !game.ghostsenabled;
            break;
        case 4:
            //Load level
            map.nexttowercolour();

            ed.keydelay = 6;
            ed.get_input_line(TEXT_LOAD, loc::gettext("Enter map filename to load:"), &(ed.filename));
            game.mapheld = true;
            graphics.backgrounddrawn = false;
            break;
        case 5:
            //Save level
            map.nexttowercolour();

            ed.keydelay = 6;
            ed.get_input_line(TEXT_SAVE, loc::gettext("Enter map filename to save as:"), &(ed.filename));
            game.mapheld = true;
            graphics.backgrounddrawn = false;
            break;
        case 6:
            /* Game options */
            music.playef(Sound_VIRIDIAN);
            game.gamestate = TITLEMODE;
            game.ingame_titlemode = true;
            game.ingame_editormode = true;

            DEFER_CALLBACK(creategameoptions);
            DEFER_CALLBACK(nextbgcolor);
            break;
        default:
            music.playef(Sound_VIRIDIAN);
            game.createmenu(Menu::ed_quit);
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::ed_music:
        switch (game.currentmenuoption)
        {
        case 0:
        case 1:
            switch (game.currentmenuoption)
            {
            case 0:
                cl.levmusic++;
                break;
            case 1:
                cl.levmusic--;
                break;
            }

            cl.levmusic = POS_MOD(cl.levmusic, 16);

            if (cl.levmusic > 0)
            {
                music.play(cl.levmusic);
            }
            else
            {
                music.haltdasmusik();
            }

            music.playef(Sound_VIRIDIAN);
            break;
        case 2:
            music.playef(Sound_VIRIDIAN);
            music.fadeout();
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::ed_quit:
        switch (game.currentmenuoption)
        {
        case 0:
            //Saving and quit
            ed.saveandquit = true;

            map.nexttowercolour();

            ed.keydelay = 6;
            ed.get_input_line(TEXT_SAVE, loc::gettext("Enter map filename to save as:"), &(ed.filename));
            game.mapheld = true;
            graphics.backgrounddrawn = false;
            break;
        case 1:
            //Quit without saving
            music.playef(Sound_VIRIDIAN);
            music.fadeout();
            graphics.fademode = FADE_START_FADEOUT;
            graphics.backgrounddrawn = false;
            break;
        case 2:
            //Go back to editor
            music.playef(Sound_VIRIDIAN);
            game.returnmenu();
            map.nexttowercolour();
            break;
        }
        break;
    case Menu::ed_font:
    {
        uint8_t idx_selected = font::font_idx_options[game.currentmenuoption];
        cl.level_font_name = font::get_main_font_name(idx_selected);
        if (idx_selected == loc::get_langmeta()->font_idx)
        {
            loc::new_level_font = "";
        }
        else
        {
            loc::new_level_font = cl.level_font_name;
        }
        font::set_level_font(cl.level_font_name.c_str());
        music.playef(Sound_VIRIDIAN);
        game.returnmenu();
        map.nexttowercolour();
        game.savestatsandsettings_menu();
        break;
    }
    default:
        break;
    }
}

static void start_at_checkpoint(void)
{
    extern editorclass ed;

    // Scan the room for a start point or a checkpoint, the start point taking priority
    int testeditor = -1;
    bool startpoint = false;

    for (size_t i = 0; i < customentities.size(); i++)
    {
        startpoint = customentities[i].t == 16;
        const bool is_startpoint_or_checkpoint = startpoint ||
            customentities[i].t == 10;
        if (!is_startpoint_or_checkpoint)
        {
            continue;
        }

        const bool in_room = customentities[i].rx == ed.levx &&
            customentities[i].ry == ed.levy;
        if (!in_room)
        {
            continue;
        }

        if (startpoint)
        {
            // Oh, there's a start point! Let's use this.
            testeditor = i;
            break;
        }
        else if (testeditor == -1)
        {
            // This is the first thing we found, so let's use it (unless we find a start point later)
            testeditor = i;
        }
    }

    if (testeditor == -1)
    {
        ed.show_note(loc::gettext("ERROR: No checkpoint to spawn at"));
    }
    else
    {
        ed.current_ghosts = 0;

        game.edsavex = (customentities[testeditor].x) * 8 - 4;
        game.edsavey = customentities[testeditor].y * 8;
        game.edsaverx = 100 + customentities[testeditor].rx;
        game.edsavery = 100 + customentities[testeditor].ry;

        if (!startpoint)
        {
            // Checkpoint spawn
            if (customentities[testeditor].p1 == 0) // NOT a bool check!
            {
                game.edsavegc = 1;
                game.edsavey -= 2;
            }
            else
            {
                game.edsavegc = 0;
                game.edsavey -= 7;
            }
            game.edsavedir = 0;
        }
        else
        {
            // Start point spawn
            game.edsavegc = 0;
            game.edsavey++;
            game.edsavedir = 1 - customentities[testeditor].p1;
        }

        music.haltdasmusik();
        ed.return_message_timer = 1000; // Let's start it higher than 255 since it gets clamped
        ed.old_return_message_timer = 1000;
        script.startgamemode(Start_EDITORPLAYTESTING);
    }
}

static void handle_draw_input()
{
    extern editorclass ed;

    bool shift_down = key.keymap[SDLK_LSHIFT] || key.keymap[SDLK_RSHIFT];

    if (shift_down && !ed.shiftkey)
    {
        ed.shiftkey = true;
        ed.help_open = !ed.help_open;
    }
    else if (!shift_down)
    {
        ed.shiftkey = false;
    }

    if (ed.keydelay > 0)
    {
        ed.keydelay--;
    }
    else
    {
        if (key.keymap[SDLK_F1])
        {
            ed.switch_tileset(shift_down);
            ed.keydelay = 6;
        }
        if (key.keymap[SDLK_F2])
        {
            ed.switch_tilecol(shift_down);
            ed.keydelay = 6;
        }
        if (key.keymap[SDLK_F3])
        {
            ed.switch_enemy(shift_down);
            ed.keydelay = 6;
        }
        if (key.keymap[SDLK_F4])
        {
            ed.keydelay = 6;
            ed.substate = EditorSubState_DRAW_BOX;
            ed.box_corner = BoxCorner_FIRST;
            ed.box_type = BoxType_ENEMY;
        }
        if (key.keymap[SDLK_F5])
        {
            ed.keydelay = 6;
            ed.substate = EditorSubState_DRAW_BOX;
            ed.box_corner = BoxCorner_FIRST;
            ed.box_type = BoxType_PLATFORM;
        }
        if (key.keymap[SDLK_F10])
        {
            if (cl.getroomprop(ed.levx, ed.levy)->directmode == 1)
            {
                cl.setroomdirectmode(ed.levx, ed.levy, 0);
                ed.show_note(loc::gettext("Direct Mode Disabled"));
                ed.clamp_tilecol(ed.levx, ed.levy, true);
            }
            else
            {
                cl.setroomdirectmode(ed.levx, ed.levy, 1);
                ed.show_note(loc::gettext("Direct Mode Enabled"));
            }
            graphics.backgrounddrawn = false;

            ed.updatetiles = true;
            ed.keydelay = 6;
        }

        for (int i = 0; i < NUM_EditorTools; i++)
        {
            if (key.keymap[ed.tool_keys[i]] &&
                ((shift_down && ed.tool_requires_shift[i]) ||
                    (!shift_down && !ed.tool_requires_shift[i])))
            {
                ed.current_tool = (EditorTools) i;
            }
        }

        if (key.keymap[SDLK_w])
        {
            ed.switch_warpdir(shift_down);
            ed.keydelay = 6;
        }
        if (key.keymap[SDLK_e])
        {
            ed.keydelay = 6;
            ed.get_input_line(TEXT_ROOMNAME, loc::gettext("Enter new room name:"), const_cast<std::string*>(&(cl.getroomprop(ed.levx, ed.levy)->roomname)));
            game.mapheld = true;
        }
        if (key.keymap[SDLK_g])
        {
            ed.keydelay = 6;
            ed.get_input_line(TEXT_GOTOROOM, loc::gettext("Enter room coordinates x,y:"), NULL);
            game.mapheld = true;
        }

        //Save and load
        if (key.keymap[SDLK_s])
        {
            ed.keydelay = 6;
            ed.get_input_line(TEXT_SAVE, loc::gettext("Enter map filename to save as:"), &(ed.filename));
            game.mapheld = true;
        }

        if (key.keymap[SDLK_l])
        {
            ed.keydelay = 6;
            ed.get_input_line(TEXT_LOAD, loc::gettext("Enter map filename to load:"), &(ed.filename));
            game.mapheld = true;
        }

        ed.f_modifier = key.keymap[SDLK_f];
        ed.h_modifier = key.keymap[SDLK_h];
        ed.v_modifier = key.keymap[SDLK_v];
        ed.b_modifier = key.keymap[SDLK_b];
        ed.c_modifier = key.keymap[SDLK_c];
        ed.x_modifier = key.keymap[SDLK_x];
        ed.z_modifier = key.keymap[SDLK_z];

        const int room = ed.levx + ed.levy * cl.maxwidth;
        const int plat_speed = cl.roomproperties[room].platv;

        if (key.keymap[SDLK_COMMA])
        {
            if (key.keymap[SDLK_LCTRL] || key.keymap[SDLK_RCTRL])
            {
                cl.roomproperties[room].platv = plat_speed - 1;
            }
            else
            {
                ed.current_tool = (EditorTools) POS_MOD(ed.current_tool - 1, NUM_EditorTools);
            }
            ed.keydelay = 6;
        }
        else if (key.keymap[SDLK_PERIOD])
        {
            if (key.keymap[SDLK_LCTRL] || key.keymap[SDLK_RCTRL])
            {
                cl.roomproperties[room].platv = plat_speed + 1;
            }
            else
            {
                ed.current_tool = (EditorTools) POS_MOD(ed.current_tool + 1, NUM_EditorTools);
            }
            ed.keydelay = 6;
        }

        if (plat_speed != cl.roomproperties[room].platv)
        {
            char buffer[3 * SCREEN_WIDTH_CHARS + 1];
            vformat_buf(
                buffer, sizeof(buffer),
                loc::gettext("Platform speed is now {speed}"),
                "speed:int",
                cl.roomproperties[room].platv
            );
            ed.show_note(buffer);
        }

        if (key.keymap[SDLK_SPACE])
        {
            ed.toolbox_open = !ed.toolbox_open;
            ed.keydelay = 6;
        }
    }
}

void editorclass::get_input_line(const enum TextMode mode, const std::string& prompt, std::string* ptr)
{
    state = EditorState_DRAW;
    substate = EditorSubState_DRAW_INPUT;
    current_text_mode = mode;
    current_text_ptr = ptr;
    current_text_desc = prompt;
    key.enabletextentry();
    if (ptr)
    {
        key.keybuffer = *ptr;
    }
    else
    {
        key.keybuffer = "";
        current_text_ptr = &(key.keybuffer);
    }

    old_entity_text = key.keybuffer;
}

void editorinput(void)
{
    extern editorclass ed;

    if (graphics.fademode == FADE_FADING_OUT)
    {
        return;
    }

    ed.old_tilex = ed.tilex;
    ed.old_tiley = ed.tiley;

    ed.tilex = key.mousex / 8;
    ed.tiley = key.mousey / 8;

    bool up_pressed = key.isDown(SDLK_UP) || key.isDown(SDL_CONTROLLER_BUTTON_DPAD_UP);
    bool down_pressed = key.isDown(SDLK_DOWN) || key.isDown(SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    bool left_pressed = key.isDown(SDLK_LEFT) || key.isDown(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    bool right_pressed = key.isDown(SDLK_RIGHT) || key.isDown(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

    game.press_left = false;
    game.press_right = false;
    game.press_action = false;
    game.press_map = false;
    game.press_interact = false;

    if (key.isDown(KEYBOARD_LEFT) || key.isDown(KEYBOARD_a) || key.controllerWantsLeft(false))
    {
        game.press_left = true;
    }
    if (key.isDown(KEYBOARD_RIGHT) || key.isDown(KEYBOARD_d) || key.controllerWantsRight(false))
    {
        game.press_right = true;
    }
    if (key.isDown(KEYBOARD_z) || key.isDown(KEYBOARD_SPACE) || key.isDown(KEYBOARD_v) || key.isDown(game.controllerButton_flip))
    {
        game.press_action = true;
    };

    if (key.keymap[SDLK_F9] && (ed.keydelay == 0)) {
        ed.keydelay = 30;
        ed.show_note(loc::gettext("Reloaded resources"));
        graphics.reloadresources();
    }

    // Was escape just pressed?
    bool escape_pressed = false;
    if (key.isDown(27) && !ed.settingskey)
    {
        ed.settingskey = true;
        escape_pressed = true;
    }
    else if (!key.isDown(27))
    {
        ed.settingskey = false;
    }

    // What about enter?
    bool enter_pressed = false;
    if (key.isDown(KEYBOARD_ENTER) && !game.mapheld)
    {
        game.mapheld = true;
        enter_pressed = true;
    }
    else if (!key.isDown(KEYBOARD_ENTER))
    {
        game.mapheld = false;
    }

    bool shift_down = key.keymap[SDLK_LSHIFT] || key.keymap[SDLK_RSHIFT];
    bool ctrl_down = key.keymap[SDLK_LCTRL] || key.keymap[SDLK_RCTRL];

    // Do different things depending on the current state (and substate)
    switch (ed.state)
    {
        // Draw mode, aka placing tiles and entities down
    case EditorState_DRAW:
        switch (ed.substate)
        {
        case EditorSubState_MAIN:
            if (escape_pressed)
            {
                // We're just in draw mode, so go to the settings menu
                music.playef(Sound_VIRIDIAN);
                ed.state = EditorState_MENU;
                ed.substate = EditorSubState_MAIN;
                game.createmenu(Menu::ed_settings);
            }

            if (ctrl_down)
            {
                // Holding ctrl, show the direct mode tile drawer
                ed.direct_mode_drawer = 10;
            }

            if (ed.keydelay > 0)
            {
                ed.keydelay--;
            }
            else if (up_pressed || down_pressed || left_pressed || right_pressed)
            {
                ed.keydelay = 6;

                if (ctrl_down)
                {
                    // The direct mode drawer is open, so arrow keys should change what tile you have selected
                    // Also let's make it a little faster
                    ed.keydelay = 3;

                    int texturewidth;
                    int textureheight;

                    bool tiles1 = (cl.getroomprop(ed.levx, ed.levy)->tileset == 0);

                    if (graphics.query_texture(tiles1 ? graphics.grphx.im_tiles : graphics.grphx.im_tiles2, NULL, NULL, &texturewidth, &textureheight) != 0)
                        return;

                    const int numtiles = (int)(texturewidth / 8) * (textureheight / 8);

                    if (left_pressed) ed.direct_mode_tile--;
                    if (right_pressed) ed.direct_mode_tile++;
                    if (up_pressed) ed.direct_mode_tile -= 40;
                    if (down_pressed) ed.direct_mode_tile += 40;

                    ed.direct_mode_tile = POS_MOD(ed.direct_mode_tile, numtiles);
                }
                else if (shift_down)
                {

                    if (up_pressed) cl.mapheight--;
                    if (down_pressed) cl.mapheight++;
                    if (left_pressed) cl.mapwidth--;
                    if (right_pressed) cl.mapwidth++;

                    cl.mapwidth = SDL_clamp(cl.mapwidth, 1, cl.maxwidth);
                    cl.mapheight = SDL_clamp(cl.mapheight, 1, cl.maxheight);

                    char buffer[3 * SCREEN_WIDTH_CHARS + 1];
                    vformat_buf(
                        buffer, sizeof(buffer),
                        loc::gettext("Mapsize is now [{width},{height}]"),
                        "width:int, height:int",
                        cl.mapwidth, cl.mapheight
                    );

                    ed.show_note(buffer);
                }
                else
                {
                    ed.updatetiles = true;
                    ed.changeroom = true;
                    graphics.backgrounddrawn = false;
                    graphics.foregrounddrawn = false;

                    if (up_pressed) ed.levy--;
                    if (down_pressed) ed.levy++;
                    if (left_pressed) ed.levx--;
                    if (right_pressed) ed.levx++;

                    ed.levx = POS_MOD(ed.levx, cl.mapwidth);
                    ed.levy = POS_MOD(ed.levy, cl.mapheight);

                }
            }
            else
            {
                handle_draw_input();
            }

            // Mouse input
            if (key.leftbutton && ed.lclickdelay == 0)
            {
                ed.tool_place();
            }
            else if (!key.leftbutton)
            {
                ed.lclickdelay = 0;
            }

            if (key.rightbutton)
            {
                ed.tool_remove();
            }

            if (key.middlebutton)
            {
                ed.direct_mode_tile = cl.gettile(ed.levx, ed.levy, ed.tilex, ed.tiley);
            }

            if (enter_pressed)
            {
                start_at_checkpoint();
            }

            break;

        case EditorSubState_DRAW_BOX:
            if (escape_pressed)
            {
                // Cancel box placement
                ed.substate = EditorSubState_MAIN;
            }

            if (key.leftbutton && ed.lclickdelay == 0)
            {
                if (ed.box_corner == BoxCorner_FIRST)
                {
                    ed.lclickdelay = 1;
                    ed.box_point.x = ed.tilex * 8;
                    ed.box_point.y = ed.tiley * 8;
                    ed.box_corner = BoxCorner_LAST;
                }
                else if (ed.box_corner == BoxCorner_LAST)
                {
                    int left;
                    int right;
                    int top;
                    int bottom;

                    adjust_box_coordinates(ed.box_point.x, ed.box_point.y, ed.tilex * 8, ed.tiley * 8, &left, &right, &top, &bottom);

                    ed.lclickdelay = 1;
                    ed.substate = EditorSubState_MAIN;

                    switch (ed.box_type)
                    {
                    case BoxType_SCRIPT:
                        ed.text_entity = customentities.size();

                        ed.add_entity(ed.levx, ed.levy, left / 8, top / 8, 19, (right - left) / 8, (bottom - top) / 8);

                        ed.get_input_line(TEXT_SCRIPT, loc::gettext("Enter script name:"), &(customentities[ed.text_entity].scriptname));
                        break;
                    case BoxType_ENEMY:
                        cl.setroomenemyx1(ed.levx, ed.levy, left);
                        cl.setroomenemyy1(ed.levx, ed.levy, top);
                        cl.setroomenemyx2(ed.levx, ed.levy, right);
                        cl.setroomenemyy2(ed.levx, ed.levy, bottom);
                        break;
                    case BoxType_PLATFORM:
                        cl.setroomplatx1(ed.levx, ed.levy, left);
                        cl.setroomplaty1(ed.levx, ed.levy, top);
                        cl.setroomplatx2(ed.levx, ed.levy, right);
                        cl.setroomplaty2(ed.levx, ed.levy, bottom);
                        break;
                    case BoxType_COPY:
                        // Unused
                        break;
                    }
                }
            }
            else if (!key.leftbutton)
            {
                ed.lclickdelay = 0;
            }

            if (key.rightbutton)
            {
                ed.substate = EditorSubState_MAIN;
            }

            break;

        case EditorSubState_DRAW_WARPTOKEN:
            if (escape_pressed || key.rightbutton)
            {
                // Cancel warp token placement
                ed.substate = EditorSubState_MAIN;
                ed.remove_entity(ed.warp_token_entity);
                ed.warp_token_entity = -1;
            }

            // Allow the user to change rooms while placing a warp token
            if (ed.keydelay > 0)
            {
                ed.keydelay--;
            }
            else if (up_pressed || down_pressed || left_pressed || right_pressed)
            {
                ed.keydelay = 6;

                ed.updatetiles = true;
                ed.changeroom = true;
                graphics.backgrounddrawn = false;
                graphics.foregrounddrawn = false;

                if (up_pressed) ed.levy--;
                if (down_pressed) ed.levy++;
                if (left_pressed) ed.levx--;
                if (right_pressed) ed.levx++;

                ed.levx = POS_MOD(ed.levx, cl.mapwidth);
                ed.levy = POS_MOD(ed.levy, cl.mapheight);
            }

            // Left click means place!
            if (key.leftbutton)
            {
                if (ed.lclickdelay == 0)
                {
                    customentities[ed.warp_token_entity].p1 = ed.tilex + (ed.levx * 40);
                    customentities[ed.warp_token_entity].p2 = ed.tiley + (ed.levy * 30);

                    ed.substate = EditorSubState_MAIN;

                    ed.warp_token_entity = -1;
                    ed.lclickdelay = 1;
                }
            }
            else
            {
                ed.lclickdelay = 0;
            }

            break;

        case EditorSubState_DRAW_INPUT:
            // We're taking input!
            if (escape_pressed)
            {
                // Cancel it, and remove the enemy it's tied to if necessary
                key.disabletextentry();
                if (ed.current_text_mode >= FIRST_ENTTEXT && ed.current_text_mode <= LAST_ENTTEXT)
                {
                    *ed.current_text_ptr = ed.old_entity_text;
                    if (ed.old_entity_text == "")
                    {
                        ed.remove_entity(ed.text_entity);
                    }
                }

                ed.current_text_mode = TEXT_NONE;

                ed.help_open = false;

                ed.substate = EditorSubState_MAIN;
            }

            if (enter_pressed)
            {
                input_submitted();
            }
            break;
        default:
            break;
        }
        break;

    // We're in the menu!
    case EditorState_MENU:

        switch (ed.substate)
        {
        case EditorSubState_MAIN:
            if (!game.press_action && !game.press_left && !game.press_right && !up_pressed && !down_pressed)
            {
                game.jumpheld = false;
            }

            if (!game.jumpheld)
            {
                if (game.press_action || game.press_left || game.press_right || game.press_map || up_pressed || down_pressed)
                {
                    game.jumpheld = true;
                }

                if (game.menustart)
                {
                    if (game.press_left || up_pressed)
                    {
                        game.currentmenuoption--;
                    }
                    else if (game.press_right || down_pressed)
                    {
                        game.currentmenuoption++;
                    }
                }

                game.currentmenuoption = POS_MOD(game.currentmenuoption, (int)game.menuoptions.size());

                if (game.press_action)
                {
                    editormenuactionpress();
                }
            }

            // Was escape pressed?
            if (escape_pressed)
            {
                bool esc_from_font = false;
                music.playef(Sound_VIRIDIAN);

                if (game.currentmenuname == Menu::ed_settings)
                {
                    ed.state = EditorState_DRAW;
                    gameScreen.recacheTextures();
                }
                else
                {
                    game.returnmenu();
                    map.nexttowercolour();

                    // Avoid double return
                    esc_from_font = game.currentmenuname == Menu::ed_font;
                }

                if (ed.state == EditorState_MENU && !esc_from_font)
                {
                    bool edsettings_in_stack = game.currentmenuname == Menu::ed_settings;
                    if (!edsettings_in_stack)
                    {
                        size_t i;
                        for (i = 0; i < game.menustack.size(); ++i)
                        {
                            if (game.menustack[i].name == Menu::ed_settings)
                            {
                                edsettings_in_stack = true;
                                break;
                            }
                        }
                    }
                    if (edsettings_in_stack)
                    {
                        game.returntomenu(Menu::ed_settings);
                    }
                    else
                    {
                        game.createmenu(Menu::ed_settings);
                    }
                    map.nexttowercolour();
                }
            }
            break;

        case EditorSubState_MENU_INPUT:
            if (escape_pressed && key.textentry())
            {
                ed.substate = EditorSubState_MAIN;
                key.disabletextentry();
                ed.current_text_mode = TEXT_NONE;

                music.playef(Sound_VIRIDIAN);
            }

            if (enter_pressed)
            {
                input_submitted();
            }
            break;
        default:
            break;
        }
        break;

    case EditorState_SCRIPTS:
    {
        switch (ed.substate)
        {
        case EditorSubState_MAIN:
        {
            if (escape_pressed)
            {
                music.playef(Sound_VIRIDIAN);
                ed.state = EditorState_MENU;
                ed.substate = EditorSubState_MAIN;
            }

            if (ed.keydelay > 0) ed.keydelay--;

            if (up_pressed && ed.keydelay <= 0)
            {
                ed.keydelay = 3;
                ed.selected_script--;
            }

            if (down_pressed && ed.keydelay <= 0)
            {
                ed.keydelay = 3;
                ed.selected_script++;
            }

            ed.selected_script = SDL_clamp(ed.selected_script, 0, (int) script.customscripts.size() - 1);

            if (ed.selected_script < ed.script_list_offset)
            {
                ed.script_list_offset = ed.selected_script;
            }

            if (ed.selected_script >= ed.script_list_offset + 9)
            {
                ed.script_list_offset = ed.selected_script - 8;
            }

            if (!key.keymap[SDLK_BACKSPACE])
            {
                ed.backspace_held = false;
            }

            if (key.keymap[SDLK_BACKSPACE] && !ed.backspace_held && !script.customscripts.empty())
            {
                ed.backspace_held = true;
                music.playef(Sound_CRY);
                ed.remove_script(script.customscripts[(script.customscripts.size() - 1) - ed.selected_script].name);
            }

            if (!game.press_action && !game.press_left && !game.press_right && !up_pressed && !down_pressed && !key.isDown(27))
            {
                game.jumpheld = false;
            }

            if (!game.jumpheld)
            {
                if (game.press_action || game.press_left || game.press_right || game.press_map || up_pressed || down_pressed || key.isDown(27))
                {
                    game.jumpheld = true;
                }

                if ((game.press_action || game.press_map) && !script.customscripts.empty())
                {
                    game.mapheld = true;
                    ed.substate = EditorSubState_SCRIPTS_EDIT;
                    key.enabletextentry();
                    key.keybuffer = "";
                    ed.current_text_ptr = &(key.keybuffer);
                    ed.current_script = script.customscripts[(script.customscripts.size() - 1) - ed.selected_script].name;
                    ed.load_script_in_editor(ed.current_script);

                    ed.script_cursor_y = ed.script_buffer.size() - 1;
                    ed.script_offset = SDL_max(ed.script_cursor_y - (ed.lines_visible - SCRIPT_LINE_PADDING), 0);

                    key.keybuffer = ed.script_buffer[ed.script_cursor_y];
                    ed.script_cursor_x = UTF8_total_codepoints(ed.script_buffer[ed.script_cursor_y].c_str());

                    music.playef(Sound_VIRIDIAN);
                }
            }
            break;
        }
        case EditorSubState_SCRIPTS_EDIT:
        {
            // Script editor!
            if (escape_pressed)
            {
                music.playef(Sound_VIRIDIAN);
                ed.substate = EditorSubState_MAIN;

                // Alright, now re-add the script.
                ed.create_script(ed.current_script, ed.script_buffer);

                key.disabletextentry();
            }

            if (ed.keydelay > 0) ed.keydelay--;

            if (up_pressed && ed.keydelay <= 0)
            {
                ed.keydelay = 3;
                ed.script_cursor_y = SDL_max(0, ed.script_cursor_y - 1);

                key.keybuffer = ed.script_buffer[ed.script_cursor_y];
            }

            if (down_pressed && ed.keydelay <= 0)
            {
                ed.keydelay = 3;
                ed.script_cursor_y = SDL_min((int) ed.script_buffer.size() - 1, ed.script_cursor_y + 1);

                key.keybuffer = ed.script_buffer[ed.script_cursor_y];
            }

            if (key.linealreadyemptykludge)
            {
                ed.keydelay = 6;
                key.linealreadyemptykludge = false;
            }

            if (key.pressedbackspace && ed.script_buffer[ed.script_cursor_y] == "" && ed.keydelay <= 0)
            {
                //Remove this line completely
                ed.remove_line(ed.script_cursor_y);
                ed.script_cursor_y = SDL_max(0, ed.script_cursor_y - 1);
                key.keybuffer = ed.script_buffer[ed.script_cursor_y];
                ed.keydelay = 6;
            }

            /* Remove all pipes, they are the line separator in the XML
             * When this loop reaches the end, it wraps to SIZE_MAX; SIZE_MAX + 1 is 0 */
            {size_t i; for (i = key.keybuffer.length() - 1; i + 1 > 0; --i)
            {
                if (key.keybuffer[i] == '|')
                {
                    key.keybuffer.erase(key.keybuffer.begin() + i);
                }
            }}

            ed.script_buffer[ed.script_cursor_y] = key.keybuffer;
            ed.script_cursor_x = UTF8_total_codepoints(ed.script_buffer[ed.script_cursor_y].c_str());

            if (enter_pressed)
            {
                //Continue to next line
                if (ed.script_cursor_y >= (int)ed.script_buffer.size()) //we're on the last line
                {
                    ed.script_cursor_y++;

                    key.keybuffer = ed.script_buffer[ed.script_cursor_y];
                    ed.script_cursor_x = UTF8_total_codepoints(ed.script_buffer[ed.script_cursor_y].c_str());
                }
                else
                {
                    //We're not, insert a line instead
                    ed.script_cursor_y++;

                    ed.insert_line(ed.script_cursor_y);
                    key.keybuffer = "";
                    ed.script_cursor_x = 0;
                }
            }

            if (ed.script_cursor_y < ed.script_offset + SCRIPT_LINE_PADDING)
            {
                ed.script_offset = SDL_max(0, ed.script_cursor_y - SCRIPT_LINE_PADDING);
            }

            if (ed.script_cursor_y > ed.script_offset + ed.lines_visible - SCRIPT_LINE_PADDING)
            {
                ed.script_offset = SDL_min((int) ed.script_buffer.size() - ed.lines_visible + SCRIPT_LINE_PADDING, ed.script_cursor_y - ed.lines_visible + SCRIPT_LINE_PADDING);
            }

            break;
        }
        default:
            break;
        }
        break;
    }
    }

    if (ed.updatetiles && cl.getroomprop(ed.levx, ed.levy)->directmode == 0)
    {
        for (int i = 0; i < SCREEN_WIDTH_TILES * SCREEN_HEIGHT_TILES; i++)
        {
            int tile_x = i % SCREEN_WIDTH_TILES;
            int tile_y = i / SCREEN_WIDTH_TILES;
            ed.set_tile(tile_x, tile_y, ed.autotile(tile_x, tile_y));
        }

        ed.updatetiles = false;
        graphics.foregrounddrawn = false;
    }
}

bool editorclass::is_warp_zone_background(int tile)
{
    if (cl.getroomprop(levx, levy)->tileset == EditorTileset_SPACE_STATION)
    {
        return false;
    }

    return (tile == 120 || tile == 123 || tile == 126 || tile == 129 || tile == 132 || tile == 135 || tile == 138);
}

int editorclass::autotile(const int x, const int y)
{
    int tile = get_tile(x, y);
    TileTypes type = get_tile_type(x, y, false);

    if (tile == 0)
    {
        return 0;
    }

    if (type == TileType_SPIKE)
    {
        bool tile_up = get_tile_type(x, y - 1, false) == TileType_SOLID;
        bool tile_down = get_tile_type(x, y + 1, false) == TileType_SOLID;
        bool tile_left = get_tile_type(x - 1, y, false) == TileType_SOLID;
        bool tile_right = get_tile_type(x + 1, y, false) == TileType_SOLID;

        if (cl.getroomprop(levx, levy)->tileset == EditorTileset_LAB)
        {
            // If this is the lab, use the colourful lab spikes!
            int mult = cl.getroomprop(levx, levy)->tilecol;
            if (tile_down)
                return 63 + mult * 2;
            if (tile_up)
                return 64 + mult * 2;
            if (tile_left)
                return 51 + mult * 2;
            if (tile_right)
                return 52 + mult * 2;
            return 63 + mult * 2;
        }

        // Not in the lab, so use the boring normal spikes
        if (tile_down)
            return 8;
        if (tile_up)
            return 9;
        if (tile_left)
            return 49;
        if (tile_right)
            return 50;
        return 8;
    }

    bool tile_up = autotile_connector(x, y - 1, type);
    bool tile_down = autotile_connector(x, y + 1, type);
    bool tile_left = autotile_connector(x - 1, y, type);
    bool tile_right = autotile_connector(x + 1, y, type);

    bool tile_up_left = autotile_connector(x - 1, y - 1, type);
    bool tile_up_right = autotile_connector(x + 1, y - 1, type);
    bool tile_down_left = autotile_connector(x - 1, y + 1, type);
    bool tile_down_right = autotile_connector(x + 1, y + 1, type);

    int tile_value = 0;

    if (tile_up)
        tile_value += 1;
    if (tile_up_right)
        tile_value += 2;
    if (tile_right)
        tile_value += 4;
    if (tile_down_right)
        tile_value += 8;
    if (tile_down)
        tile_value += 16;
    if (tile_down_left)
        tile_value += 32;
    if (tile_left)
        tile_value += 64;
    if (tile_up_left)
        tile_value += 128;

    bool background = (type == TileType_NONSOLID || is_warp_zone_background(tile));
    EditorTilecolInfo data = get_tilecol_data();

    int base = background ? data.background_base : data.foreground_base;
    return base + autotile_types[background ? data.background_type : data.foreground_type][tile_value];
}

EditorTilecolInfo editorclass::get_tilecol_data(void)
{
    EditorTilesets tileset = (EditorTilesets) cl.getroomprop(levx, levy)->tileset;
    int tilecol = cl.getroomprop(levx, levy)->tilecol;

    return tileset_colors[tileset][tilecol];
}

bool editorclass::autotile_connector(int x, int y, TileTypes original_type)
{
    if (x < 0 || x >= SCREEN_WIDTH_TILES || y < 0 || y >= SCREEN_HEIGHT_TILES)
    {
        return true;
    }

    int tile = get_tile(x, y);
    TileTypes new_type = get_tile_type(x, y, false);

    if (tile == 0)
    {
        return false;
    }

    if (new_type == TileType_SOLID && !is_warp_zone_background(tile))
    {
        return true;
    }

    return original_type == TileType_NONSOLID;
}

int editorclass::get_enemy_tile(int t)
{
    switch(t)
    {
    case 0:
        return 78;
        break;
    case 1:
        return 88;
        break;
    case 2:
        return 36;
        break;
    case 3:
        return 164;
        break;
    case 4:
        return 68;
        break;
    case 5:
        return 48;
        break;
    case 6:
        return 176;
        break;
    case 7:
        return 168;
        break;
    case 8:
        return 112;
        break;
    case 9:
        return 114;
        break;
    default:
        return 78;
        break;
    }
}

void editorclass::set_tile(int x, int y, int t)
{
    if (x >= 0 && y >= 0 && x < SCREEN_WIDTH_TILES && y < SCREEN_HEIGHT_TILES)
    {
        cl.settile(levx, levy, x, y, t);
    }
    graphics.foregrounddrawn = false;
    updatetiles = true;
}

int editorclass::get_tile(const int x, const int y)
{
    if (x >= 0 && y >= 0 && x < SCREEN_WIDTH_TILES && y < SCREEN_HEIGHT_TILES)
    {
        return cl.gettile(levx, levy, x, y);
    }

    return 0;
}

TileTypes editorclass::get_abs_tile_type(int x, int y, const bool wrap)
{
    if (wrap)
    {
        x = POS_MOD(x, cl.mapwidth * 40);
        y = POS_MOD(y, cl.mapheight * 30);
    }
    else
    {
        x = SDL_clamp(x, 0, cl.mapwidth * 40 - 1);
        y = SDL_clamp(y, 0, cl.mapheight * 30 - 1);
    }

    const RoomProperty* const room = cl.getroomprop(x / 40, y / 30);
    int tile = cl.getabstile(x, y);

    if (tile == 1 || (tile >= 80 && tile <= 679))
    {
        // It's solid.
        return TileType_SOLID;
    }

    if ((tile >= 6 && tile <= 9) || tile == 49 || tile == 50)
    {
        // It's a spike!
        return TileType_SPIKE;
    }

    if (room->tileset != 0)
    {
        // tiles2.png is slightly different.

        if (tile >= 51 && tile <= 74)
        {
            // It has more spikes!
            return TileType_SPIKE;
        }

        if (tile == 740)
        {
            // And a stray solid.
            return TileType_SOLID;
        }
    }

    return TileType_NONSOLID;
}

TileTypes editorclass::get_tile_type(int x, int y, bool wrap)
{
    if (wrap)
    {
        x = POS_MOD(x, 40);
        y = POS_MOD(y, 30);
    }
    else
    {
        x = SDL_clamp(x, 0, 39);
        y = SDL_clamp(y, 0, 29);
    }

    return get_abs_tile_type(levx * 40 + x, levy * 30 + y, false);
}

bool editorclass::lines_can_pass(int x, int y)
{
    const int tile = cl.gettile(levx, levy, x, y);
    if (x >= 0 && y >= 0 && x < SCREEN_WIDTH_TILES && y < SCREEN_HEIGHT_TILES)
    {
        return tile == 0 || tile >= 680;
    }
    return false;
}

void editorclass::switch_tileset(const bool reversed)
{
    int tiles = cl.getroomprop(levx, levy)->tileset;

    if (reversed)
    {
        tiles--;
    }
    else
    {
        tiles++;
    }

    tiles = POS_MOD(tiles, NUM_EditorTilesets);
    cl.setroomtileset(levx, levy, tiles);

    clamp_tilecol(levx, levy, false);

    char buffer[3*SCREEN_WIDTH_CHARS + 1];
    vformat_buf(
        buffer, sizeof(buffer),
        loc::gettext("Now using {area} Tileset"),
        "area:str",
        loc::gettext(tileset_names[tiles])
    );

    show_note(buffer);

    updatetiles = true;

    graphics.backgrounddrawn = false;
}

void editorclass::switch_tilecol(const bool reversed)
{
    int tilecol = cl.getroomprop(levx, levy)->tilecol;

    if (reversed)
    {
        tilecol--;
    }
    else
    {
        tilecol++;
    }

    cl.setroomtilecol(levx, levy, tilecol);

    clamp_tilecol(levx, levy, true);

    show_note(loc::gettext("Tileset Colour Changed"));

    updatetiles = true;

    graphics.backgrounddrawn = false;
}

void editorclass::clamp_tilecol(const int rx, const int ry, const bool wrap)
{
    const RoomProperty* const room = cl.getroomprop(rx, ry);
    const int tileset = room->tileset;
    int tilecol = room->tilecol;

    int mincol = (room->directmode ? tileset_min_colour_direct : tileset_min_colour)[tileset];
    int maxcol = (room->directmode ? tileset_max_colour_direct : tileset_max_colour)[tileset];

    // If wrap is true, wrap-around, otherwise just cap
    if (tilecol > maxcol)
    {
        tilecol = (wrap ? mincol : maxcol);
    }
    if (tilecol < mincol)
    {
        tilecol = (wrap ? maxcol : mincol);
    }

    cl.setroomtilecol(rx, ry, tilecol);
}

void editorclass::switch_enemy(const bool reversed)
{
    const RoomProperty* const room = cl.getroomprop(levx, levy);

    int enemy = room->enemytype;

    if (reversed)
    {
        enemy--;
    }
    else
    {
        enemy++;
    }

    const int modulus = 10;
    enemy = POS_MOD(enemy, modulus);
    cl.setroomenemytype(levx, levy, enemy);

    show_note(loc::gettext("Enemy Type Changed"));
}

void editorclass::switch_warpdir(const bool reversed)
{
    static const int modulus = 4;
    const RoomProperty* const room = cl.getroomprop(levx, levy);

    int warpdir = room->warpdir;

    if (reversed)
    {
        --warpdir;
    }
    else
    {
        ++warpdir;
    }

    warpdir = POS_MOD(warpdir, modulus);
    cl.setroomwarpdir(levx, levy, warpdir);

    switch (warpdir)
    {
    default:
        show_note(loc::gettext("Room warping disabled"));
        break;
    case 1:
        show_note(loc::gettext("Room warps horizontally"));
        break;
    case 2:
        show_note(loc::gettext("Room warps vertically"));
        break;
    case 3:
        show_note(loc::gettext("Room warps in all directions"));
        break;
    }

    graphics.backgrounddrawn = false;
}
