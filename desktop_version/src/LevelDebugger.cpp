#include "LevelDebugger.h"

#include "Constants.h"
#include "Entity.h"
#include "Font.h"
#include "Graphics.h"
#include "KeyPoll.h"
#include "Localization.h"
#include "Map.h"
#include "Script.h"
#include "UtilityClass.h"
#include "VFormat.h"

namespace level_debugger
{
    bool active = false;
    bool should_pause = true;
    bool tab_held = false;
    bool debug_held = false;
    bool forced = false;

    // Moving entities/blocks
    bool mouse_held = false;
    int held_entity = -1;
    int held_block = -1;
    int grabber_offset_x = 0;
    int grabber_offset_y = 0;

    bool right_mouse_held = false;

    bool is_pausing(void)
    {
        return active && should_pause;
    }

    bool is_active(void)
    {
        return active;
    }

    void toggle_active(void)
    {
        active = !active;
    }

    bool mouse_within(SDL_Rect* rect)
    {
        return key.mx >= rect->x && key.mx < rect->x + rect->w &&
            key.my >= rect->y && key.my < rect->y + rect->h;
    }

    void set_forced(void)
    {
        forced = true;
    }

    void input(void)
    {
        if (!forced && (!map.custommode || map.custommodeforreal))
        {
            active = false;
            return;
        }

        if (key.isDown(SDLK_y))
        {
            if (!debug_held)
            {
                debug_held = true;
                active = !active;
            }
        }
        else
        {
            debug_held = false;
        }

        if (!active)
        {
            return;
        }

        if (key.isDown(SDLK_TAB))
        {
            if (!tab_held)
            {
                tab_held = true;
                should_pause = !should_pause;
            }
        }
        else
        {
            tab_held = false;
        }

        for (int i = 0; i < (int) obj.entities.size(); i++)
        {
            SDL_Rect bounding_box = {
                obj.entities[i].xp + obj.entities[i].cx,
                obj.entities[i].yp + obj.entities[i].cy,
                obj.entities[i].w,
                obj.entities[i].h
            };

            if (key.leftbutton)
            {
                if (mouse_within(&bounding_box))
                {
                    if (!mouse_held)
                    {
                        mouse_held = true;
                        held_entity = i;
                        grabber_offset_x = key.mx - obj.entities[i].xp;
                        grabber_offset_y = key.my - obj.entities[i].yp;

                        if (!key.keymap[SDLK_LSHIFT] && !key.keymap[SDLK_RSHIFT])
                        {
                            for (int j = 0; j < (int) obj.blocks.size(); j++)
                            {
                                if (obj.entities[i].xp == obj.blocks[j].rect.x && obj.entities[i].yp == obj.blocks[j].rect.y)
                                {
                                    held_block = j;
                                }
                            }
                        }
                    }
                    break;
                }
            }
            else
            {
                mouse_held = false;
                held_entity = -1;
                held_block = -1;
            }

            if (key.rightbutton)
            {
                if (mouse_within(&bounding_box))
                {
                    if (!right_mouse_held)
                    {
                        right_mouse_held = true;
                        obj.entities[i].state = obj.entities[i].onentity;
                        obj.entities[i].statedelay = -1;
                    }
                    break;
                }
            }
            else
            {
                right_mouse_held = false;
            }
        }

        if (held_entity == -1)
        {
            for (int i = 0; i < (int) obj.blocks.size(); i++)
            {
                SDL_Rect bounding_box = {
                    obj.blocks[i].rect.x,
                    obj.blocks[i].rect.y,
                    obj.blocks[i].rect.w,
                    obj.blocks[i].rect.h
                };

                if (key.leftbutton)
                {
                    if (mouse_within(&bounding_box))
                    {
                        if (!mouse_held)
                        {
                            mouse_held = true;
                            held_block = i;
                            grabber_offset_x = key.mx - obj.blocks[i].rect.x;
                            grabber_offset_y = key.my - obj.blocks[i].rect.y;
                        }
                        break;
                    }
                }
                else
                {
                    held_entity = -1;
                    mouse_held = false;
                    held_block = -1;
                }
            }
        }
    }

    void logic(void)
    {
        if (INBOUNDS_VEC(held_entity, obj.entities))
        {
            int new_xp = key.mx - grabber_offset_x;
            int new_yp = key.my - grabber_offset_y;

            if (key.isDown(SDLK_LSHIFT) || key.isDown(SDLK_RSHIFT))
            {
                new_xp -= new_xp % 8;
                new_yp -= new_yp % 8;
            }

            obj.entities[held_entity].xp = new_xp;
            obj.entities[held_entity].yp = new_yp;
            obj.entities[held_entity].lerpoldxp = new_xp;
            obj.entities[held_entity].lerpoldyp = new_yp;
            obj.entities[held_entity].oldxp = new_xp;
            obj.entities[held_entity].oldyp = new_yp;
        }

        if (INBOUNDS_VEC(held_block, obj.blocks))
        {
            int new_xp = key.mx - grabber_offset_x;
            int new_yp = key.my - grabber_offset_y;

            if (key.isDown(SDLK_LSHIFT) || key.isDown(SDLK_RSHIFT))
            {
                new_xp -= new_xp % 8;
                new_yp -= new_yp % 8;
            }

            obj.blocks[held_block].xp = new_xp;
            obj.blocks[held_block].yp = new_yp;
            obj.blocks[held_block].rect.x = new_xp;
            obj.blocks[held_block].rect.y = new_yp;
        }
    }

    void render_info(int y, const char* text)
    {
        font::print(PR_BOR | PR_FONT_8X8, 5, 32 + (10 * y), text, 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
    }

    void render_coords(int y, const char* text, int first, int second)
    {
        char buffer[SCREEN_WIDTH_CHARS + 1];
        vformat_buf(buffer, sizeof(buffer), "{text}: ({first},{second})", "text:str, first:int, second:int", text, first, second);
        render_info(y, buffer);
    }

    void render_info(int y, const char* text, std::string value)
    {
        char buffer[SCREEN_WIDTH_CHARS + 1];
        vformat_buf(buffer, sizeof(buffer), "{text}: {value}", "text:str, value:str", text, value.c_str());
        render_info(y, buffer);
    }

    void render(void)
    {
        if (!active)
        {
            return;
        }

        int hovered = -1;
        bool hovered_entity = true;
        SDL_Rect hover_box;

        for (int i = 0; i < (int) obj.entities.size(); i++)
        {
            SDL_Rect bounding_box = {
                obj.entities[i].xp + obj.entities[i].cx,
                obj.entities[i].yp + obj.entities[i].cy,
                obj.entities[i].w,
                obj.entities[i].h
            };

            if (hovered == -1 && mouse_within(&bounding_box))
            {
                hovered = i;
                hovered_entity = true;
                hover_box = bounding_box;
            }

            graphics.draw_rect(bounding_box.x, bounding_box.y, bounding_box.w, bounding_box.h, graphics.getRGB(15, 90, 90));
        }

        for (int i = 0; i < (int) obj.blocks.size(); i++)
        {
            SDL_Rect bounding_box = {
                obj.blocks[i].rect.x,
                obj.blocks[i].rect.y,
                obj.blocks[i].rect.w,
                obj.blocks[i].rect.h
            };

            if (hovered == -1 && mouse_within(&bounding_box))
            {
                hovered = i;
                hovered_entity = false;
                hover_box = bounding_box;
            }

            graphics.draw_rect(bounding_box.x, bounding_box.y, bounding_box.w, bounding_box.h, graphics.getRGB(90, 15, 15));
        }

        int line = 0;

        if (key.isDown(SDLK_u))
        {
            SDL_Color on = graphics.getRGB(220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
            SDL_Color off = graphics.getRGB(220 / 1.5 - (help.glow), 220 / 1.5 - (help.glow), 255 / 1.5 - (help.glow / 2));

            graphics.set_blendmode(SDL_BLENDMODE_BLEND);
            graphics.fill_rect(NULL, 0, 0, 0, 127);
            graphics.set_blendmode(SDL_BLENDMODE_NONE);

            int x = 0;
            int y = 0;

            for (int i = 0; i < (int) SDL_arraysize(obj.flags); i++)
            {
                SDL_Color color = obj.flags[i] ? on : off;
                font::print(PR_BOR | PR_FONT_8X8, 48 + x * 24, 48 + y * 16, help.String(i), color.r, color.g, color.b);

                x++;
                if (x >= 10)
                {
                    x = 0;
                    y++;
                }
            }
        }
        else if (hovered == -1)
        {
            render_coords(line++, "Room", game.roomx % 100, game.roomy % 100);
            render_coords(line++, "Cursor", key.mx, key.my);
            render_info(line++, "Entities", help.String(obj.entities.size()));
            line++;

            render_info(line++, "State", help.String(game.state));
            render_info(line++, "State Delay", help.String(game.statedelay));
            line++;

            render_info(line++, "AEM Target", help.String(script.i));

            line++;

            if (script.running)
            {
                render_info(line++, "Script", script.scriptname);
                render_info(line++, "Delay", help.String(script.scriptdelay));
                render_info(line++, "Position", help.String(script.position));
                line++;
                render_info(line++, "Loop Line", help.String(script.looppoint));
                render_info(line++, "Loop Count", help.String(script.loopcount));
            }
        }
        else
        {
            if (hovered_entity)
            {
                entclass* entity = &obj.entities[hovered];
                render_info(line++, "Index", help.String(hovered));
                render_coords(line++, "Position", entity->xp, entity->yp);
                render_coords(line++, "Size", entity->w, entity->h);
                line++;
                render_info(line++, "Rule", help.String(entity->rule));
                render_info(line++, "Type", help.String(entity->type));
                render_info(line++, "Behave", help.String(entity->behave));
                render_info(line++, "Para", help.String(entity->para));
                render_info(line++, "State", help.String(entity->state));
                line++;
                render_info(line++, "Tile", help.String(entity->tile));
                render_info(line++, "Draw Frame", help.String(entity->drawframe));
                render_info(line++, "Size", help.String(entity->size));
                render_info(line++, "Direction", help.String(entity->dir));

                line++;

                // Mostly contains duplicates, but for ease of use
                switch (entity->type)
                {
                case 0:
                    // Player
                    render_info(line++, "Gravity", help.String(game.gravitycontrol));
                    render_info(line++, "Checkpoint", help.String(game.savepoint));
                    break;
                case 1:
                    // Moving platforms and enemies
                    render_info(line++, "Speed", help.String(entity->para));
                    render_info(line++, "Movement type", help.String(entity->behave));
                    break;
                case 7:
                    // Trinkets
                    render_info(line++, "ID", help.String(entity->para));
                    break;
                case 8:
                    // Checkpoints
                    render_info(line++, "ID", help.String(entity->para));
                    render_info(line++, "Active", game.savepoint == entity->para ? "True" : "False");
                    break;
                case 9:
                    // Horizontal gravity lines
                    render_info(line++, "Horizontal");
                    break;
                case 10:
                    // Vertical gravity lines
                    render_info(line++, "Vertical");
                    break;
                }


                graphics.draw_rect(hover_box.x, hover_box.y, hover_box.w, hover_box.h, graphics.getRGB(32, 255 - help.glow, 255 - help.glow));
            }
            else
            {
                blockclass* block = &obj.blocks[hovered];
                render_info(line++, "Index", help.String(hovered));
                render_coords(line++, "Position", block->rect.x, block->rect.y);
                render_coords(line++, "Size", block->rect.w, block->rect.h);

                line++;

                if (block->type == TRIGGER || block->type == ACTIVITY)
                {
                    render_info(line++, "Script", block->script);
                    render_info(line++, "State", help.String(block->trigger));
                }
                else if (block->type == DIRECTIONAL)
                {
                    render_info(line++, "Direction", help.String(block->trigger));
                }

                graphics.draw_rect(hover_box.x, hover_box.y, hover_box.w, hover_box.h, graphics.getRGB(255 - help.glow, 32, 32));
            }
        }

        font::print(PR_BOR, 5, 14, loc::gettext("[Press TAB to toggle movement]"), 220 - (help.glow), 220 - (help.glow), 255 - (help.glow / 2));
    }
}
