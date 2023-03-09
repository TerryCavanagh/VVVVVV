#if !defined(NO_CUSTOM_LEVELS) && !defined(NO_EDITOR)

#define ED_DEFINITION
#include "Editor.h"

#include <string>

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
        case 1:
            songname = loc::gettext("1: Pushing Onwards");
            break;
        case 2:
            songname = loc::gettext("2: Positive Force");
            break;
        case 3:
            songname = loc::gettext("3: Potential for Anything");
            break;
        case 4:
            songname = loc::gettext("4: Passion for Exploring");
            break;
        case 5:
            songname = loc::gettext("N/A: Pause");
            break;
        case 6:
            songname = loc::gettext("5: Presenting VVVVVV");
            break;
        case 7:
            songname = loc::gettext("N/A: Plenary");
            break;
        case 8:
            songname = loc::gettext("6: Predestined Fate");
            break;
        case 9:
            songname = loc::gettext("N/A: ecroF evitisoP");
            break;
        case 10:
            songname = loc::gettext("7: Popular Potpourri");
            break;
        case 11:
            songname = loc::gettext("8: Pipe Dream");
            break;
        case 12:
            songname = loc::gettext("9: Pressure Cooker");
            break;
        case 13:
            songname = loc::gettext("10: Paced Energy");
            break;
        case 14:
            songname = loc::gettext("11: Piercing the Sky");
            break;
        case 15:
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
}

static void update_entities(void)
{
    extern editorclass ed;

    for (size_t i = 0; i < customentities.size(); ++i)
    {
        CustomEntity* entity = &customentities[i];

        if (entity->x / 40 != ed.levx || entity->y / 30 != ed.levy)
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
                int tx = entity->x % 40;
                int tx2 = tx;
                int ty = entity->y % 30;
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
                int tx = entity->x % 40;
                int ty = entity->y % 30;
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

    const int edent_under_cursor = ed.get_entity_at(ed.tilex + ed.levx * 40, ed.tiley + ed.levy * 30);

    // Special case for drawing gray entities
    bool custom_gray = room->tileset == 3 && room->tilecol == 6;

    // Draw entities backward to remain accurate with ingame
    for (int i = customentities.size() - 1; i >= 0; i--)
    {
        CustomEntity* entity = &customentities[i];

        // If the entity is in the current room, draw it
        if (entity->x / 40 == ed.levx && entity->y / 30 == ed.levy)
        {
            const int x = entity->x % 40 * 8;
            const int y = entity->y % 30 * 8;
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
        if (entity->t == 13 && ed.warp_token_entity != i)
        {
            // Is the destination in this room?
            if (entity->p1 / 40 == ed.levx && entity->p2 / 30 == ed.levy)
            {
                const int x = entity->p1 % 40 * 8;
                const int y = entity->p2 % 30 * 8;
                std::string text;

                graphics.draw_sprite(x, y, 18 + (ed.entframe % 2), 64, 64, 64);
                graphics.draw_rect((entity->p1 * 8) - (ed.levx * 40 * 8), (entity->p2 * 8) - (ed.levy * 30 * 8), 16, 16, graphics.getRGB(96, 64, 64));

                if (ed.tilex == x / 8 && ed.tiley == y / 8)
                {
                    text = "(" + help.String(entity->x / 40 + 1) + "," + help.String(entity->y / 30 + 1) + ")";
                }
                else
                {
                    text = help.String(cl.findwarptoken(i));
                }

                font::print(PR_BOR | PR_CJK_HIGH, x, y - 8, text, 190, 190, 225);
            }
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
        if (ed.b_modifier) graphics.draw_rect(x, 0, 8, 240, blue); // Vertical
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

        for (int x = 0; x < 40; x++)
        {
            for (int y = 0; y < 5; y++)
            {
                if (tileset == 0)
                {
                    graphics.drawtile(x * 8, (y * 8) - t2, (temp + numtiles + (y * 40) + x) % numtiles);
                }
                else
                {
                    graphics.drawtile2(x * 8, (y * 8) - t2, (temp + numtiles + (y * 40) + x) % numtiles);
                }
            }
        }

        // Highlight our little block
        graphics.draw_rect(((ed.direct_mode_tile % 40) * 8) - 2, 16 - t2 - 2, 12, 12, graphics.getRGB(255 - help.glow, 196, 196));
        graphics.draw_rect(((ed.direct_mode_tile % 40) * 8) - 1, 16 - t2 - 1, 10, 10, graphics.getRGB(0, 0, 0));
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
    int tx = 6;
    const int ty = 210;
    const int tg = 32;

    const int page = ed.current_tool / 10;
    const int max_pages = SDL_ceil(NUM_EditorTools / 10);
    const int page_tool_count = SDL_min(10, NUM_EditorTools - (page * 10));

    for (int i = 0; i < page_tool_count; i++)
    {
        const int current_tool_id = i + (page * 10);

        // First, draw the background
        graphics.fill_rect(4 + (i * tg), 208, 20, 20, graphics.getRGB(32, 32, 32));

        // Draw the actual tool icon
        ed.draw_tool((EditorTools)current_tool_id, 4 + (i * tg) + 2, 208 + 2);

        // Draw the tool outline...
        graphics.draw_rect(4 + (i * tg), 208, 20, 20, (current_tool_id == ed.current_tool) ? graphics.getRGB(200, 200, 200) : graphics.getRGB(96, 96, 96));

        // ...and the hotkey
        const int col = current_tool_id == ed.current_tool ? 255 : 164;
        font::print(PR_FONT_8X8 | PR_BOR, 22 + i * tg - 4, 224 - 4, ed.tool_key_chars[current_tool_id], col, col, col);
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
    extern editorclass ed;

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
        graphics.draw_sprite(x, y, 78 + ed.entframe, 196, 196, 196);
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
        graphics.draw_sprite(x, y, 18 + (ed.entframe % 2), 196, 196, 196);
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
    {
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
        }

        break;
    }
    case EditorState_SCRIPTS:
        // Intended to look like Commodore 64's UI

        graphics.fill_rect(0, 0, 320, 240, graphics.getRGB(123, 111, 218));
        graphics.fill_rect(14, 16, 292, 208, graphics.getRGB(61, 48, 162));

        if (ed.substate == EditorSubState_MAIN)
        {

            font::print(PR_CEN, -1, 28, loc::gettext("**** VVVVVV SCRIPT EDITOR ****"), 123, 111, 218);
            font::print(PR_CEN, -1, 44, loc::gettext("PRESS ESC TO RETURN TO MENU"), 123, 111, 218);

            if (!script.customscripts.empty())
            {
                for (int i = 0; i < 9; i++)
                {
                    if (ed.script_list_offset + i < (int)script.customscripts.size())
                    {
                        if (ed.script_list_offset + i == ed.selected_script)
                        {
                            std::string text_upper(loc::toupper(script.customscripts[(script.customscripts.size() - 1) - (ed.script_list_offset + i)].name));

                            char buffer[SCREEN_WIDTH_CHARS + 1];
                            vformat_buf(buffer, sizeof(buffer), loc::get_langmeta()->menu_select.c_str(), "label:str", text_upper.c_str());
                            font::print(PR_CEN, -1, 68 + (i * 16), buffer, 123, 111, 218);
                        }
                        else
                        {
                            font::print(PR_CEN, -1, 68 + (i * 16), script.customscripts[(script.customscripts.size() - 1) - (ed.script_list_offset + i)].name, 123, 111, 218);
                        }
                    }
                }
            }
            else
            {
                font::print(PR_CEN, -1, 110, loc::gettext("NO SCRIPT IDS FOUND"), 123, 111, 218);
                font::print_wrap(PR_CEN, -1, 130, loc::gettext("CREATE A SCRIPT WITH EITHER THE TERMINAL OR SCRIPT BOX TOOLS"), 123, 111, 218, 10, 288);
            }
        }
        else if (ed.substate == EditorSubState_SCRIPTS_EDIT)
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
        music.play(6); //should be before game.quittomenu()
    }
}

void editorclass::add_entity(int xp, int yp, int tp, int p1, int p2, int p3, int p4, int p5, int p6)
{
    CustomEntity entity;

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

int editorclass::get_entity_at(int xp, int yp)
{
    for (size_t i = 0; i < customentities.size(); i++)
    {
        if (customentities[i].x == xp && customentities[i].y == yp) return i;
    }
    return -1;
}

static void set_tile_interpolated(const int x1, const int x2, const int y1, const int y2, const int tile)
{
    extern editorclass ed;

    // draw a line between (x1, y1) and (x2, y2)

    const int dx = x2 - x1;
    const int dy = y2 - y1;

    const int steps = SDL_max(abs(dx), abs(dy));

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

void editorclass::handle_tile_placement(const int tile)
{
    int range = 1;

    if (b_modifier)
    {
        // Vertical line
        for (int i = 0; i < 30; i++)
        {
            set_tile_interpolated(old_tilex, tilex, i, i, tile);
        }
        return;
    }
    else if (h_modifier)
    {
        // Horizontal line
        for (int i = 0; i < 40; i++)
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
        if (customentities[i].x == tilex + (levx * 40) && customentities[i].y == tiley + (levy * 30))
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

    const int entity = get_entity_at(tilex + (levx * 40), tiley + (levy * 30));
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
            add_entity(tilex + (levx * 40), tiley + (levy * 30), 9);
            lclickdelay = 1;
        }
        else
        {
            show_note(loc::gettext("ERROR: Max number of trinkets is 100"));
        }
        break;
    case EditorTool_CHECKPOINTS:
        add_entity(tilex + (levx * 40), tiley + (levy * 30), 10, 1);
        lclickdelay = 1;
        break;
    case EditorTool_DISAPPEARING_PLATFORMS:
        add_entity(tilex + (levx * 40), tiley + (levy * 30), 3);
        lclickdelay = 1;
        break;
    case EditorTool_CONVEYORS:
        add_entity(tilex + (levx * 40), tiley + (levy * 30), 2, 5);
        lclickdelay = 1;
        break;
    case EditorTool_MOVING_PLATFORMS:
        add_entity(tilex + (levx * 40), tiley + (levy * 30), 2, 0);
        lclickdelay = 1;
        break;
    case EditorTool_ENEMIES:
        add_entity(tilex + (levx * 40), tiley + (levy * 30), 1, 0);
        lclickdelay = 1;
        break;
    case EditorTool_GRAVITY_LINES:
        add_entity(tilex + (levx * 40), tiley + (levy * 30), 11, 0);
        lclickdelay = 1;
        break;
    case EditorTool_ROOMTEXT:
        lclickdelay = 1;
        text_entity = customentities.size();
        add_entity(tilex + (levx * 40), tiley + (levy * 30), 17);
        get_input_line(TEXT_ROOMTEXT, loc::gettext("Enter roomtext:"), &(customentities[text_entity].scriptname));
        break;
    case EditorTool_TERMINALS:
        lclickdelay = 1;
        text_entity = customentities.size();
        add_entity(tilex + (levx * 40), tiley + (levy * 30), 18, 0);
        get_input_line(TEXT_SCRIPT, loc::gettext("Enter script name:"), &(customentities[text_entity].scriptname));
        break;
    case EditorTool_SCRIPTS:
        substate = EditorSubState_DRAW_BOX;
        box_corner = BoxCorner_LAST;
        box_type = BoxType_SCRIPT;
        box_point = { tilex * 8, tiley * 8 };

        lclickdelay = 1;
        break;
    case EditorTool_WARP_TOKENS:
        substate = EditorSubState_DRAW_WARPTOKEN;
        warp_token_entity = customentities.size();
        add_entity(tilex + (levx * 40), tiley + (levy * 30), 13);
        lclickdelay = 1;
        break;
    case EditorTool_WARP_LINES:
        //Warp lines
        if (tilex == 0)
        {
            add_entity(tilex + (levx * 40), tiley + (levy * 30), 50, 0);
        }
        else if (tilex == 39)
        {
            add_entity(tilex + (levx * 40), tiley + (levy * 30), 50, 1);
        }
        else if (tiley == 0)
        {
            add_entity(tilex + (levx * 40), tiley + (levy * 30), 50, 2);
        }
        else if (tiley == 29)
        {
            add_entity(tilex + (levx * 40), tiley + (levy * 30), 50, 3);
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
            add_entity(tilex + (levx * 40), tiley + (levy * 30), 15, int(fRandom() * 6));
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
        add_entity(tilex + (levx * 40), tiley + (levy * 30), 16, 0);
        lclickdelay = 1;
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
        music.playef(11);
        break;
    case Menu::ed_settings:
        switch (game.currentmenuoption)
        {
        case 0:
            //Change level description stuff
            music.playef(11);
            game.createmenu(Menu::ed_desc);
            map.nexttowercolour();
            break;
        case 1:
            //Enter script editormode
            music.playef(11);

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
            music.playef(11);
            game.createmenu(Menu::ed_music);
            map.nexttowercolour();
            if(cl.levmusic>0) music.play(cl.levmusic);
            break;
        case 3:
            music.playef(11);
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
            music.playef(11);
            game.gamestate = TITLEMODE;
            game.ingame_titlemode = true;
            game.ingame_editormode = true;

            DEFER_CALLBACK(creategameoptions);
            DEFER_CALLBACK(nextbgcolor);
            break;
        default:
            music.playef(11);
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

            music.playef(11);
            break;
        case 2:
            music.playef(11);
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
            music.playef(11);
            music.fadeout();
            graphics.fademode = FADE_START_FADEOUT;
            graphics.backgrounddrawn = false;
            break;
        case 2:
            //Go back to editor
            music.playef(11);
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
        music.playef(11);
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
        if (customentities[i].t == 16 || customentities[i].t == 10)
        {
            int tx = customentities[i].x / 40;
            int ty = customentities[i].y / 30;
            if (tx == ed.levx && ty == ed.levy)
            {
                startpoint = customentities[i].t == 16;
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
        }
    }

    if (testeditor == -1)
    {
        ed.show_note(loc::gettext("ERROR: No checkpoint to spawn at"));
    }
    else
    {
        ed.current_ghosts = 0;

        int tx = customentities[testeditor].x / 40;
        int ty = customentities[testeditor].y / 30;
        game.edsavex = (customentities[testeditor].x % 40) * 8 - 4;
        game.edsavey = (customentities[testeditor].y % 30) * 8;
        game.edsaverx = 100 + tx;
        game.edsavery = 100 + ty;

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
                // Kludge fix for rainbow BG here...
                if (cl.getroomprop(ed.levx, ed.levy)->tileset == 2
                    && cl.getroomprop(ed.levx, ed.levy)->tilecol == 6)
                {
                    cl.setroomtilecol(ed.levx, ed.levy, 0);
                }
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
            if (key.keymap[ed.tool_keys[i]])
            {
                if ((shift_down && ed.tool_requires_shift[i]) || (!shift_down && !ed.tool_requires_shift[i]))
                {
                    ed.current_tool = (EditorTools) i;
                }
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

        ed.h_modifier = key.keymap[SDLK_h];
        ed.v_modifier = key.keymap[SDLK_v];
        ed.b_modifier = key.keymap[SDLK_b];
        ed.c_modifier = key.keymap[SDLK_c];
        ed.x_modifier = key.keymap[SDLK_x];
        ed.z_modifier = key.keymap[SDLK_z];

        if (key.keymap[SDLK_COMMA])
        {
            ed.current_tool = (EditorTools)POS_MOD(ed.current_tool - 1, NUM_EditorTools);
            ed.keydelay = 6;
        }
        else if (key.keymap[SDLK_PERIOD])
        {
            ed.current_tool = (EditorTools)POS_MOD(ed.current_tool + 1, NUM_EditorTools);
            ed.keydelay = 6;
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

    ed.tilex = (key.mx - (key.mx % 8)) / 8;
    ed.tiley = (key.my - (key.my % 8)) / 8;

    if (gameScreen.scalingMode == SCALING_STRETCH) {
        // In this mode specifically, we have to fix the mouse coordinates
        int winwidth, winheight;
        gameScreen.GetWindowSize(&winwidth, &winheight);
        ed.tilex = ed.tilex * 320 / winwidth;
        ed.tiley = ed.tiley * 240 / winheight;
    }

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
                music.playef(11);
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

            if (key.leftbutton)
            {
                if (ed.lclickdelay == 0)
                {
                    if (ed.box_corner == BoxCorner_FIRST)
                    {
                        ed.lclickdelay = 1;
                        ed.box_point = { ed.tilex * 8, ed.tiley * 8 };
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

                            ed.add_entity((left / 8) + (ed.levx * 40), (top / 8) + (ed.levy * 30), 19, (right - left) / 8, (bottom - top) / 8);

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
            }
            else
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
                    if (ed.free(ed.tilex, ed.tiley) == 0)
                    {
                        customentities[ed.warp_token_entity].p1 = ed.tilex + (ed.levx * 40);
                        customentities[ed.warp_token_entity].p2 = ed.tiley + (ed.levy * 30);

                        ed.substate = EditorSubState_MAIN;

                        ed.warp_token_entity = -1;
                        ed.lclickdelay = 1;
                    }
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
                music.playef(11);

                if (game.currentmenuname == Menu::ed_settings)
                {
                    ed.state = EditorState_DRAW;
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

                music.playef(11);
            }

            if (enter_pressed)
            {
                input_submitted();
            }
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
                music.playef(11);
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
                music.playef(2);
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

                    music.playef(11);
                }
            }
            break;
        }
        case EditorSubState_SCRIPTS_EDIT:
        {
            // Script editor!
            if (escape_pressed)
            {
                music.playef(11);
                ed.substate = EditorSubState_MAIN;

                // Alright, now re-add the script.
                ed.create_script(ed.current_script, ed.script_buffer);
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
                ed.script_cursor_y = SDL_min((int)ed.script_buffer.size() - 1, ed.script_cursor_y + 1);

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
        }
        break;
    }
    }

    if (ed.updatetiles && cl.getroomprop(ed.levx, ed.levy)->directmode == 0)
    {
        ed.updatetiles = false;
        // Correctly set the tiles in the current room
        switch (cl.getroomprop(ed.levx, ed.levy)->tileset)
        {
        case 0: // The Space Station
            for (int j = 0; j < 30; j++)
            {
                for (int i = 0; i < 40; i++)
                {
                    int temp = cl.gettile(ed.levx, ed.levy, i, j);
                    if (temp >= 3 && temp < 80)
                    {
                        // Fix spikes
                        cl.settile(ed.levx, ed.levy, i, j, ed.spikedir(i, j));
                    }
                    else if (temp == 2 || temp >= 680)
                    {
                        // Fix background
                        cl.settile(
                            ed.levx,
                            ed.levy,
                            i,
                            j,
                            ed.backedgetile(i, j) + ed.autotiling_background_base(ed.levx, ed.levy)
                        );
                    }
                    else if (temp > 0)
                    {
                        // Fix tiles
                        cl.settile(
                            ed.levx,
                            ed.levy,
                            i,
                            j,
                            ed.edgetile(i, j) + ed.autotiling_base(ed.levx, ed.levy)
                        );
                    }
                }
            }
            break;
        case 1: // Outside
            for (int j = 0; j < 30; j++)
            {
                for (int i = 0; i < 40; i++)
                {
                    int temp = cl.gettile(ed.levx, ed.levy, i, j);
                    if (temp >= 3 && temp < 80)
                    {
                        // Fix spikes
                        cl.settile(ed.levx, ed.levy, i, j, ed.spikedir(i, j));
                    }
                    else if (temp == 2 || temp >= 680)
                    {
                        // Fix background
                        cl.settile(
                            ed.levx,
                            ed.levy,
                            i,
                            j,
                            ed.outsideedgetile(i, j) + ed.autotiling_background_base(ed.levx, ed.levy)
                        );
                    }
                    else if (temp > 0)
                    {
                        // Fix tiles
                        cl.settile(
                            ed.levx,
                            ed.levy,
                            i,
                            j,
                            ed.edgetile(i, j) + ed.autotiling_base(ed.levx, ed.levy)
                        );
                    }
                }
            }
            break;
        case 2: // Lab
            for (int j = 0; j < 30; j++)
            {
                for (int i = 0; i < 40; i++)
                {
                    int temp = cl.gettile(ed.levx, ed.levy, i, j);
                    if (temp >= 3 && temp < 80)
                    {
                        // Fix spikes
                        cl.settile(
                            ed.levx,
                            ed.levy,
                            i,
                            j,
                            ed.labspikedir(
                                i,
                                j,
                                cl.getroomprop(ed.levx, ed.levy)->tilecol
                            )
                        );
                    }
                    else if (temp == 2 || temp >= 680)
                    {
                        // Fix background
                        cl.settile(ed.levx, ed.levy, i, j, 713);
                    }
                    else if (temp > 0)
                    {
                        // Fix tiles
                        cl.settile(
                            ed.levx,
                            ed.levy,
                            i,
                            j,
                            ed.edgetile(i, j) + ed.autotiling_base(ed.levx, ed.levy)
                        );
                    }
                }
            }
            break;
        case 3: // Warp Zone/Intermission
            for (int j = 0; j < 30; j++)
            {
                for (int i = 0; i < 40; i++)
                {
                    int temp = cl.gettile(ed.levx, ed.levy, i, j);
                    if (temp >= 3 && temp < 80)
                    {
                        // Fix spikes
                        cl.settile(ed.levx, ed.levy, i, j, ed.spikedir(i, j));
                    }
                    else if (temp == 2 || temp >= 680)
                    {
                        // Fix background
                        cl.settile(ed.levx, ed.levy, i, j, 713);
                    }
                    else if (temp > 0)
                    {
                        // Fix tiles
                        cl.settile(
                            ed.levx,
                            ed.levy,
                            i,
                            j,
                            ed.edgetile(i, j) + ed.autotiling_base(ed.levx, ed.levy)
                        );
                    }
                }
            }
            break;
        case 4: // The ship
            for (int j = 0; j < 30; j++)
            {
                for (int i = 0; i < 40; i++)
                {
                    int temp = cl.gettile(ed.levx, ed.levy, i, j);
                    if (temp >= 3 && temp < 80)
                    {
                        // Fix spikes
                        cl.settile(ed.levx, ed.levy, i, j, ed.spikedir(i, j));
                    }
                    else if (temp == 2 || temp >= 680)
                    {
                        // Fix background
                        cl.settile(
                            ed.levx,
                            ed.levy,
                            i,
                            j,
                            ed.backedgetile(i, j) + ed.autotiling_background_base(ed.levx, ed.levy)
                        );
                    }
                    else if (temp > 0)
                    {
                        // Fix tiles
                        cl.settile(
                            ed.levx,
                            ed.levy,
                            i,
                            j,
                            ed.edgetile(i, j) + ed.autotiling_base(ed.levx, ed.levy)
                        );
                    }
                }
            }
            break;
        case 5: // The Tower
            break;
        case 6: // Custom Set 1
            break;
        case 7: // Custom Set 2
            break;
        case 8: // Custom Set 3
            break;
        case 9: // Custom Set 4
            break;
        }
        graphics.foregrounddrawn = false;
    }
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
    if (x >= 0 && y >= 0 && x < 40 && y < 30)
    {
        cl.settile(levx, levy, x, y, t);
    }
    graphics.foregrounddrawn = false;
    updatetiles = true;
}

int editorclass::autotiling_base(int x, int y)
{
    //Return the base tile for the given tileset and colour
    const RoomProperty* const room = cl.getroomprop(x, y);
    if (room->tileset == 0)  //Space Station
    {
        if (room->tilecol >= 22)
        {
            return 483 + ((room->tilecol - 22) * 3);
        }
        else if (room->tilecol >= 11)
        {
            return 283 + ((room->tilecol - 11) * 3);
        }
        else
        {
            return 83 + (room->tilecol * 3);
        }
    }
    else if (room->tileset == 1)   //Outside
    {
        return 480 + (room->tilecol * 3);
    }
    else if (room->tileset == 2)   //Lab
    {
        return 280 + (room->tilecol * 3);
    }
    else if (room->tileset == 3)   //Warp Zone/Intermission
    {
        return 80 + (room->tilecol * 3);
    }
    else if (room->tileset == 4)   //SHIP
    {
        return 101 + (room->tilecol * 3);
    }
    return 0;
}

int editorclass::autotiling_background_base( int x, int y )
{
    //Return the base tile for the background of the given tileset and colour
    const RoomProperty* const room = cl.getroomprop(x, y);
    if (room->tileset == 0)  //Space Station
    {
        //Pick depending on tilecol
        switch (room->tilecol)
        {
        case 0:
        case 5:
        case 26:
            return 680; //Blue
            break;
        case 3:
        case 16:
        case 23:
            return 683; //Yellow
            break;
        case 9:
        case 12:
        case 21:
            return 686; //Greeny Cyan
            break;
        case 4:
        case 8:
        case 24:
        case 28:
        case 30:
            return 689; //Green
            break;
        case 20:
        case 29:
            return 692; //Orange
            break;
        case 2:
        case 6:
        case 11:
        case 22:
        case 27:
            return 695; //Red
            break;
        case 1:
        case 10:
        case 15:
        case 19:
        case 31:
            return 698; //Pink
            break;
        case 14:
        case 18:
            return 701; //Dark Blue
            break;
        case 7:
        case 13:
        case 17:
        case 25:
            return 704; //Cyan
            break;
        default:
            return 680;
            break;
        }

    }
    else if (room->tileset == 1)   //outside
    {
        return 680 + (room->tilecol * 3);
    }
    else if (room->tileset == 2)   //Lab
    {
        return 0;
    }
    else if (room->tileset == 3)   //Warp Zone/Intermission
    {
        return 120 + (room->tilecol * 3);
    }
    else if (room->tileset == 4)   //SHIP
    {
        return 741 + (room->tilecol * 3);
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

bool editorclass::is_background(int x, int y)
{
    x = SDL_clamp(x, 0, 39);
    y = SDL_clamp(y, 0, 29);

    return (cl.gettile(levx, levy, x, y) >= 680);
}

int editorclass::backfree( int x, int y )
{
    // Returns 0 if tile is not a block or background tile, 1 otherwise
    x = SDL_clamp(x, 0, 39);
    y = SDL_clamp(y, 0, 29);

    if (x >= 0 && y >= 0 && x < 40 && y < 30)
    {
        if (cl.gettile(levx, levy, x, y) == 0)
        {
            return 0;
        }
    }
    return 1;
}

bool editorclass::lines_can_pass(int x, int y)
{
    // Returns 0 if tile is not a block or spike, 1 otherwise

    if (x >= 0 && y >= 0 && x < 40 && y < 30)
    {
        return (cl.gettile(levx, levy, x, y) == 0) || (cl.gettile(levx, levy, x, y) >= 680);
    }
    return false;
}

int editorclass::free( int x, int y )
{
    // Returns 0 if tile is not a block, 1 otherwise

    x = SDL_clamp(x, 0, 39);
    y = SDL_clamp(y, 0, 29);

    if (cl.gettile(levx, levy, x, y) == 0)
    {
        return 0;
    }
    else
    {
        if (cl.gettile(levx, levy, x, y) >= 2 && cl.gettile(levx, levy, x, y) < 80)
        {
            return 0;
        }
        if (cl.gettile(levx, levy, x, y) >= 680)
        {
            return 0;
        }
    }
    return 1;
}

int editorclass::match( int x, int y )
{
    if (free(x - 1, y) == 0 && free(x, y - 1) == 0 && free(x + 1, y) == 0 && free(x, y + 1) == 0) return 0;

    if (free(x - 1, y) == 0 && free(x, y - 1) == 0) return 10;
    if (free(x + 1, y) == 0 && free(x, y - 1) == 0) return 11;
    if (free(x - 1, y) == 0 && free(x, y + 1) == 0) return 12;
    if (free(x + 1, y) == 0 && free(x, y + 1) == 0) return 13;

    if (free(x, y - 1) == 0) return 1;
    if (free(x - 1, y) == 0) return 2;
    if (free(x, y + 1) == 0) return 3;
    if (free(x + 1, y) == 0) return 4;
    if (free(x - 1, y - 1) == 0) return 5;
    if (free(x + 1, y - 1) == 0) return 6;
    if (free(x - 1, y + 1) == 0) return 7;
    if (free(x + 1, y + 1) == 0) return 8;

    return 0;
}

int editorclass::outsidematch( int x, int y )
{
    if (is_background(x - 1, y) == 0 && is_background(x + 1, y) == 0) return 2;
    if (is_background(x, y - 1) == 0 && is_background(x, y + 1) == 0) return 1;

    return 0;
}

int editorclass::backmatch( int x, int y )
{
    //Returns the first position match for a border
    // 5 1 6
    // 2 X 4
    // 7 3 8
    if (backfree(x - 1, y) == 0 && backfree(x, y - 1) == 0 && backfree(x + 1, y) == 0 && backfree(x, y + 1) == 0) return 0;

    if (backfree(x - 1, y) == 0 && backfree(x, y - 1) == 0) return 10;
    if (backfree(x + 1, y) == 0 && backfree(x, y - 1) == 0) return 11;
    if (backfree(x - 1, y) == 0 && backfree(x, y + 1) == 0) return 12;
    if (backfree(x + 1, y) == 0 && backfree(x, y + 1) == 0) return 13;

    if (backfree(x, y - 1) == 0) return 1;
    if (backfree(x - 1, y) == 0) return 2;
    if (backfree(x, y + 1) == 0) return 3;
    if (backfree(x + 1, y) == 0) return 4;
    if (backfree(x - 1, y - 1) == 0) return 5;
    if (backfree(x + 1, y - 1) == 0) return 6;
    if (backfree(x - 1, y + 1) == 0) return 7;
    if (backfree(x + 1, y + 1) == 0) return 8;

    return 0;
}

int editorclass::edgetile( int x, int y )
{
    switch(match(x,y))
    {
    case 14:
        return 0;
        break;
    case 10:
        return 80;
        break;
    case 11:
        return 82;
        break;
    case 12:
        return 160;
        break;
    case 13:
        return 162;
        break;
    case 1:
        return 81;
        break;
    case 2:
        return 120;
        break;
    case 3:
        return 161;
        break;
    case 4:
        return 122;
        break;
    case 5:
        return 42;
        break;
    case 6:
        return 41;
        break;
    case 7:
        return 2;
        break;
    case 8:
        return 1;
        break;
    case 0:
    default:
        return 0;
        break;
    }
}

int editorclass::outsideedgetile( int x, int y )
{
    switch(outsidematch(x,y))
    {
    case 2:
        return 0;
        break;
    case 1:
        return 1;
        break;
    case 0:
    default:
        return 2;
        break;
    }
}


int editorclass::backedgetile( int x, int y )
{
    switch(backmatch(x,y))
    {
    case 14:
        return 0;
        break;
    case 10:
        return 80;
        break;
    case 11:
        return 82;
        break;
    case 12:
        return 160;
        break;
    case 13:
        return 162;
        break;
    case 1:
        return 81;
        break;
    case 2:
        return 120;
        break;
    case 3:
        return 161;
        break;
    case 4:
        return 122;
        break;
    case 5:
        return 42;
        break;
    case 6:
        return 41;
        break;
    case 7:
        return 2;
        break;
    case 8:
        return 1;
        break;
    case 0:
    default:
        return 0;
        break;
    }
}

int editorclass::labspikedir( int x, int y, int t )
{
    // a slightly more tricky case
    if (free(x, y + 1) == 1) return 63 + (t * 2);
    if (free(x, y - 1) == 1) return 64 + (t * 2);
    if (free(x - 1, y) == 1) return 51 + (t * 2);
    if (free(x + 1, y) == 1) return 52 + (t * 2);
    return 63 + (t * 2);
}

int editorclass::spikedir( int x, int y )
{
    if (free(x, y + 1) == 1) return 8;
    if (free(x, y - 1) == 1) return 9;
    if (free(x - 1, y) == 1) return 49;
    if (free(x + 1, y) == 1) return 50;
    return 8;
}

void editorclass::switch_tileset(const bool reversed)
{
    const char* tilesets[] = {"Space Station", "Outside", "Lab", "Warp Zone", "Ship"};

    int tiles = cl.getroomprop(levx, levy)->tileset;

    if (reversed)
    {
        tiles--;
    }
    else
    {
        tiles++;
    }

    const int modulus = SDL_arraysize(tilesets);
    tiles = POS_MOD(tiles, modulus);
    cl.setroomtileset(levx, levy, tiles);

    clamp_tilecol(levx, levy, false);

    char buffer[3*SCREEN_WIDTH_CHARS + 1];
    vformat_buf(
        buffer, sizeof(buffer),
        loc::gettext("Now using {area} Tileset"),
        "area:str",
        loc::gettext(tilesets[tiles])
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

    int mincol = -1;
    int maxcol = 5;

    // Only Space Station allows tileset -1
    if (tileset != 0)
    {
        mincol = 0;
    }

    switch (tileset)
    {
    case 0:
        maxcol = 31;
        break;
    case 1:
        maxcol = 7;
        break;
    case 2:
        if (room->directmode)
        {
            maxcol = 6;
        }
        break;
    case 3:
        maxcol = 6;
        break;
    case 5:
        maxcol = 29;
        break;
    }

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

#endif /* NO_CUSTOM_LEVELS and NO_EDITOR */