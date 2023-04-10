#include "TextInput.h"

#include <SDL.h>
#include <string>
#include <vector>

#include "Font.h"
#include "Graphics.h"
#include "KeyPoll.h"
#include "Screen.h"
#include "UTF8.h"
#include "UtilityClass.h"
#include "Vlogging.h"

namespace TextInput
{
    bool taking_input;
    bool selecting;
    int flash_timer;
    bool using_vector;
    bool can_select;
    bool can_move_cursor;
    std::string* current_text_line;
    std::vector<std::string>* current_text;
    SDL_Point cursor_pos;
    SDL_Point cursor_select_pos;
    int cursor_x_tallest;
    int display_offset;

    void init(void)
    {
        taking_input = false;
    }

    void attach_input(std::vector<std::string>* text)
    {
        taking_input = true;
        current_text = text;
        selecting = false;
        flash_timer = 0;
        using_vector = true;
        can_select = true;
        can_move_cursor = true;
        display_offset = 0;
        SDL_StartTextInput();

        send_cursor_to_end();
    }

    void attach_input(std::string* text)
    {
        taking_input = true;
        current_text_line = text;
        selecting = false;
        flash_timer = 0;
        using_vector = false;
        display_offset = 0;
        SDL_StartTextInput();

        // Temporary
        can_select = false;
        can_move_cursor = false;

        cursor_pos.x = UTF8_total_codepoints(current_text_line->c_str());
        cursor_pos.y = 0;
        cursor_x_tallest = cursor_pos.x;
    }

    void detach_input(void)
    {
        taking_input = false;
        SDL_StopTextInput();
    }

    void send_cursor_to_end(void)
    {
        cursor_pos.y = get_lines() - 1;
        cursor_pos.x = UTF8_total_codepoints(get_line(cursor_pos.y));
        cursor_x_tallest = cursor_pos.x;
    }

    void insert_text(std::string text)
    {
        // Insert text at cursor position, respecting newlines
        // Don't bother deleting selection, already done

        std::string::iterator it;
        for (it = text.begin(); it != text.end(); it++)
        {
            if (*it == '\n')
            {
                insert_newline();
            }
            else if (*it != '\r' && *it != '\0')
            {
                // Add character
                std::string current = get_line(cursor_pos.y);
                current.insert(current.begin() + cursor_pos.x, *it);
                set_line(cursor_pos.y, current.c_str());

                cursor_pos.x++;
            }
        }
        cursor_x_tallest = cursor_pos.x;
    }

    void insert_newline(void)
    {
        if (!using_vector)
        {
            insert_text(" ");
            return;
        }

        char* first_part = UTF8_substr(get_line(cursor_pos.y), 0, cursor_pos.x);
        char* second_part = UTF8_substr(get_line(cursor_pos.y), cursor_pos.x, UTF8_total_codepoints(get_line(cursor_pos.y)));

        set_line(cursor_pos.y, first_part);
        current_text->insert(current_text->begin() + cursor_pos.y + 1, second_part);

        SDL_free(first_part);
        SDL_free(second_part);

        cursor_pos.y++;
        cursor_pos.x = 0;
        cursor_x_tallest = 0;
    }

    void select_all(void)
    {
        cursor_pos.x = 0;
        cursor_pos.y = 0;
        cursor_select_pos.x = UTF8_total_codepoints(get_line(get_lines() - 1));
        cursor_select_pos.y = get_lines() - 1;
        selecting = true;
    }

    int get_lines(void)
    {
        if (using_vector)
        {
            return current_text->size();
        }
        return 1;
    }

    const char* get_line(int line)
    {
        if (using_vector)
        {
            return current_text->at(line).c_str();
        }
        return current_text_line->c_str();
    }

    void set_line(int line, const char* text)
    {
        if (using_vector)
        {
            current_text->at(line) = text;
        }
        else
        {
            current_text_line->assign(text);
        }
    }

    void add_to_line(int line, const char* text)
    {
        if (using_vector)
        {
            current_text->at(line) += text;
        }
        else
        {
            std::string temp = *current_text_line + text;
            current_text_line->assign(temp);
        }
    }

    bool process_selection(void)
    {
        if (SDL_GetModState() & KMOD_SHIFT)
        {
            if (!selecting && can_select)
            {
                cursor_select_pos = cursor_pos;
                selecting = true;
            }
        }
        else
        {
            if (selecting)
            {
                selecting = false;
                return true;
            }
        }
        return false;
    }

    char* get_selected_text(void)
    {
        /* Caller must free */

        SelectionRect rect = reorder_selection_positions();

        if (rect.y == rect.y2)
        {
            return UTF8_substr(get_line(rect.y), rect.x, rect.x2);
        }

        char* select_part_first_line = UTF8_substr(get_line(rect.y), rect.x, UTF8_total_codepoints(get_line(rect.y)) - rect.x);
        char* select_part_last_line = UTF8_substr(get_line(rect.y2), 0, rect.x2);

        // Loop through the lines in between
        int total_length = SDL_strlen(select_part_first_line) + SDL_strlen(select_part_last_line) + 1;
        for (int i = rect.y + 1; i < rect.y2; i++)
        {
            total_length += SDL_strlen(get_line(i)) + 1;
        }

        char* select_part = (char*)SDL_malloc(total_length);
        strcpy(select_part, select_part_first_line);
        strcat(select_part, "\n");
        for (int i = rect.y + 1; i < rect.y2; i++)
        {
            strcat(select_part, get_line(i));
            strcat(select_part, "\n");
        }
        strcat(select_part, select_part_last_line);

        SDL_free(select_part_first_line);
        SDL_free(select_part_last_line);

        return select_part;
    }

    SelectionRect reorder_selection_positions(void)
    {
        SelectionRect positions;
        bool in_front = false;

        if (cursor_pos.y > cursor_select_pos.y)
        {
            in_front = true;
        }
        else if (cursor_pos.y == cursor_select_pos.y)
        {
            if (cursor_pos.x >= cursor_select_pos.x)
            {
                in_front = true;
            }
        }

        if (in_front)
        {
            positions.x = cursor_select_pos.x;
            positions.x2 = cursor_pos.x;
            positions.y = cursor_select_pos.y;
            positions.y2 = cursor_pos.y;
        }
        else
        {
            positions.x = cursor_pos.x;
            positions.x2 = cursor_select_pos.x;
            positions.y = cursor_pos.y;
            positions.y2 = cursor_select_pos.y;
        }

        return positions;
    }

    void remove_characters(int x, int y, int x2, int y2)
    {
        if (x == x2 && y == y2)
        {
            return;
        }
        // Get the rest of the last line
        char* rest_of_string = UTF8_substr(get_line(y2), x2, UTF8_total_codepoints(get_line(y2)));

        for (int i = y2; i > y; i--)
        {
            if (cursor_pos.y >= i)
            {
                cursor_pos.y--;
            }
            if (cursor_select_pos.y >= i)
            {
                cursor_select_pos.y--;
            }

            // Erase the current line
            current_text->erase(current_text->begin() + i);
        }

        // Erase from the start of the selection to the end
        char* erased = UTF8_erase(get_line(y), x, UTF8_total_codepoints(get_line(y)));
        set_line(y, erased);
        // Add the rest of the last line to the end of the first line
        add_to_line(cursor_pos.y, rest_of_string);
        cursor_pos.x = x;
        SDL_free(erased);
        SDL_free(rest_of_string);
    }

    void remove_selection(void)
    {
        SelectionRect positions = reorder_selection_positions();
        remove_characters(positions.x, positions.y, positions.x2, positions.y2);
        selecting = false;
    }

    void move_cursor_up(void)
    {
        bool reset = process_selection();  // Only returns true if you don't hold shift
        if (reset && cursor_pos.y > cursor_select_pos.y)
        {
            cursor_pos.y = cursor_select_pos.y;
        }

        if (cursor_pos.y > 0)
        {
            cursor_pos.y--;
            cursor_pos.x = cursor_x_tallest;
            if (cursor_pos.x > (int) UTF8_total_codepoints(get_line(cursor_pos.y)))
            {
                cursor_pos.x = UTF8_total_codepoints(get_line(cursor_pos.y));
            }
        }
    }

    void move_cursor_down(void)
    {
        bool reset = process_selection();  // Only returns true if you don't hold shift
        if (reset && cursor_pos.y < cursor_select_pos.y)
        {
            cursor_pos.y = cursor_select_pos.y;
        }

        if (cursor_pos.y < get_lines() - 1)
        {
            cursor_pos.y++;
            cursor_pos.x = cursor_x_tallest;
            if (cursor_pos.x > (int) UTF8_total_codepoints(get_line(cursor_pos.y)))
            {
                cursor_pos.x = UTF8_total_codepoints(get_line(cursor_pos.y));
            }
        }
        else
        {
            cursor_pos.x = UTF8_total_codepoints(get_line(cursor_pos.y));
        }
    }

    bool is_word_part(unsigned char character)
    {
        return character > 0x7F || SDL_isdigit(character) || SDL_isalpha(character) || character == '_' || character == '-';
    }

    void move_cursor_left(void)
    {
        bool reset = process_selection();  // Only returns true if you don't hold shift
        if (reset)
        {
            cursor_pos.x = cursor_select_pos.x;
        }

        if (cursor_pos.x == 0 && cursor_pos.y == 0)
        {
            cursor_x_tallest = 0;
            return;
        }

        if (SDL_GetModState() & KMOD_CTRL)
        {
            // If the current character is a word part, move to the start of the word. Else, move to the start of the next non-word.
            char* character = UTF8_substr(get_line(cursor_pos.y), cursor_pos.x - 1, cursor_pos.x);
            bool is_word = is_word_part(character[0]);
            SDL_free(character);

            while (true)
            {
                character = UTF8_substr(get_line(cursor_pos.y), cursor_pos.x - 1, cursor_pos.x);
                if (is_word_part(character[0]) != is_word)
                {
                    SDL_free(character);
                    break;
                }
                SDL_free(character);
                cursor_pos.x--;
                if (cursor_pos.x <= 0 && cursor_pos.y > 0)
                {
                    cursor_pos.y--;
                    cursor_pos.x = UTF8_total_codepoints(get_line(cursor_pos.y));
                    break;
                }
            }
        }
        else
        {
            if (cursor_pos.x > 0)
            {
                cursor_pos.x--;
            }
            else if (cursor_pos.y > 0)
            {
                cursor_pos.y--;
                cursor_pos.x = UTF8_total_codepoints(get_line(cursor_pos.y));
            }
        }

        cursor_x_tallest = cursor_pos.x;
    }

    void move_cursor_right(void)
    {
        bool reset = process_selection();  // Only returns true if you don't hold shift
        if (reset)
        {
            cursor_pos.x = cursor_select_pos.x;
        }

        if (cursor_pos.x >= (int) UTF8_total_codepoints(get_line(cursor_pos.y)) && cursor_pos.y == get_lines() - 1)
        {
            cursor_x_tallest = cursor_pos.x;
            return;
        }

        if (SDL_GetModState() & KMOD_CTRL)
        {
            // If the current character is a word part, move to the end of the word. Else, move to the end of the next non-word.
            char* character = UTF8_substr(get_line(cursor_pos.y), cursor_pos.x, cursor_pos.x + 1);
            bool is_word = is_word_part(character[0]);
            SDL_free(character);

            while (true)
            {
                character = UTF8_substr(get_line(cursor_pos.y), cursor_pos.x, cursor_pos.x + 1);
                if (is_word_part(character[0]) != is_word)
                {
                    SDL_free(character);
                    break;
                }
                SDL_free(character);
                cursor_pos.x++;
                if (cursor_pos.x >= (int) UTF8_total_codepoints(get_line(cursor_pos.y)) && cursor_pos.y < get_lines() - 1)
                {
                    cursor_pos.y++;
                    cursor_pos.x = 0;
                    break;
                }
            }
        }
        else
        {

            if (cursor_pos.x < (int) UTF8_total_codepoints(get_line(cursor_pos.y)))
            {
                cursor_pos.x++;
            }
            else if (cursor_pos.y < get_lines() - 1)
            {
                cursor_pos.y++;
                cursor_pos.x = 0;
            }
        }

        cursor_x_tallest = cursor_pos.x;
    }

    void backspace(void)
    {
        // The user pressed backspace.
        if (selecting)
        {
            remove_selection();
            return;
        }

        if (cursor_pos.x == 0)
        {
            if (cursor_pos.y > 0)
            {
                // Get the rest of the last line
                char* rest_of_string = UTF8_substr(get_line(cursor_pos.y), 0, UTF8_total_codepoints(get_line(cursor_pos.y)));

                // Erase the current line
                current_text->erase(current_text->begin() + cursor_pos.y);

                // Move the cursor up
                cursor_pos.y--;

                // Move the cursor to the end of the line
                cursor_pos.x = UTF8_total_codepoints(get_line(cursor_pos.y));
                cursor_x_tallest = cursor_pos.x;

                // Add the rest of the last line to the end of the first line
                add_to_line(cursor_pos.y, rest_of_string);
                SDL_free(rest_of_string);
            }
        }
        else
        {
            if (SDL_GetModState() & KMOD_CTRL)
            {
                char* character = UTF8_substr(get_line(cursor_pos.y), cursor_pos.x - 1, cursor_pos.x);
                bool is_word = is_word_part(character[0]);
                SDL_free(character);

                const int remove_y = cursor_pos.y;
                int remove_x = cursor_pos.x;
                int start_x = cursor_pos.x;
                while (true)
                {
                    character = UTF8_substr(get_line(cursor_pos.y), cursor_pos.x - 1, cursor_pos.x);
                    if (is_word_part(character[0]) != is_word)
                    {
                        SDL_free(character);
                        break;
                    }
                    SDL_free(character);
                    cursor_pos.x--;
                    remove_x = cursor_pos.x;

                    // Slightly different logic for wrapping
                    if (cursor_pos.x <= 0 && cursor_pos.y > 0)
                    {
                        cursor_pos.y--;
                        cursor_pos.x = UTF8_total_codepoints(get_line(cursor_pos.y));
                        remove_x = 0;
                        break;
                    }
                }

                // Erase the characters before the cursor
                char* erased = UTF8_erase(get_line(remove_y), remove_x, start_x);
                set_line(remove_y, erased);
                SDL_free(erased);

                if (cursor_pos.y != remove_y)
                {
                    // Remove the line

                    char* rest_of_string = UTF8_substr(get_line(remove_y), 0, UTF8_total_codepoints(get_line(remove_y)));

                    // Erase the current line
                    current_text->erase(current_text->begin() + remove_y);

                    // Add the rest of the last line to the end of the first line
                    add_to_line(cursor_pos.y, rest_of_string);
                    SDL_free(rest_of_string);
                }
            }
            else
            {
                // Erase the character before the cursor
                char* erased = UTF8_erase(get_line(cursor_pos.y), cursor_pos.x - 1, cursor_pos.x);
                set_line(cursor_pos.y, erased);
                SDL_free(erased);

                // Move the cursor back
                cursor_pos.x--;
                cursor_x_tallest = cursor_pos.x;
            }
        }
    }

    void delete_key(void)
    {
        // The user pressed delete.
        if (selecting)
        {
            remove_selection();
            return;
        }

        if (cursor_pos.x == (int) UTF8_total_codepoints(get_line(cursor_pos.y)))
        {
            if (cursor_pos.y < get_lines() - 1)
            {
                // Get the rest of the last line
                char* rest_of_string = UTF8_substr(get_line(cursor_pos.y + 1), 0, UTF8_total_codepoints(get_line(cursor_pos.y + 1)));

                // Erase the current line
                current_text->erase(current_text->begin() + cursor_pos.y + 1);

                // Add the rest of the last line to the end of the first line
                add_to_line(cursor_pos.y, rest_of_string);
                SDL_free(rest_of_string);
            }
        }
        else
        {
            if (SDL_GetModState() & KMOD_CTRL)
            {
                char* character = UTF8_substr(get_line(cursor_pos.y), cursor_pos.x, cursor_pos.x + 1);
                bool is_word = is_word_part(character[0]);
                SDL_free(character);

                const int remove_y = cursor_pos.y;
                int remove_x = cursor_pos.x;
                int start_x = cursor_pos.x;
                while (true)
                {
                    character = UTF8_substr(get_line(cursor_pos.y), cursor_pos.x, cursor_pos.x + 1);
                    if (is_word_part(character[0]) != is_word)
                    {
                        SDL_free(character);
                        break;
                    }
                    SDL_free(character);
                    cursor_pos.x++;
                    remove_x = cursor_pos.x;
                }

                // Erase the characters before the cursor
                char* erased = UTF8_erase(get_line(remove_y), start_x, remove_x);
                set_line(remove_y, erased);
                SDL_free(erased);

                cursor_pos.x = start_x;
            }
            else
            {
                // Erase the character before the cursor
                char* erased = UTF8_erase(get_line(cursor_pos.y), cursor_pos.x, cursor_pos.x + 1);
                set_line(cursor_pos.y, erased);
                SDL_free(erased);
            }
        }
    }

    void handle_events(SDL_Event e)
    {
        if (!taking_input) return;

        if (e.type == SDL_KEYDOWN)
        {
            // Show cursor!!
            flash_timer = 0;

            // Handle backspace
            if (e.key.keysym.sym == SDLK_BACKSPACE)
            {
                // Remove the character
                backspace();
            }
            else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
            {
                if (selecting)
                {
                    remove_selection();
                }
                char* clipboard_text = SDL_GetClipboardText();
                insert_text(clipboard_text);
                SDL_free(clipboard_text);
            }
            else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
            {
                if (!can_select)
                {
                    SDL_SetClipboardText(get_line(cursor_pos.y));
                    return;
                }
                char* selected = get_selected_text();
                SDL_SetClipboardText(selected);
                SDL_free(selected);
            }
            else if (e.key.keysym.sym == SDLK_x && SDL_GetModState() & KMOD_CTRL)
            {
                if (!can_select)
                {
                    SDL_SetClipboardText(get_line(cursor_pos.y));
                    set_line(cursor_pos.y, "");
                    cursor_pos.x = 0;
                    cursor_x_tallest = 0;
                    return;
                }
                char* selected = get_selected_text();
                SDL_SetClipboardText(selected);
                SDL_free(selected);
                remove_selection();
            }
            else if (e.key.keysym.sym == SDLK_a && SDL_GetModState() & KMOD_CTRL && can_select)
            {
                select_all();
            }
            else if (e.key.keysym.sym == SDLK_RETURN && using_vector)
            {
                insert_newline();
            }
            else if (e.key.keysym.sym == SDLK_UP && can_move_cursor)
            {
                move_cursor_up();
            }
            else if (e.key.keysym.sym == SDLK_DOWN && can_move_cursor)
            {
                move_cursor_down();
            }
            else if (e.key.keysym.sym == SDLK_LEFT && can_move_cursor)
            {
                move_cursor_left();
            }
            else if (e.key.keysym.sym == SDLK_RIGHT && can_move_cursor)
            {
                move_cursor_right();
            }
            else if (e.key.keysym.sym == SDLK_HOME && can_move_cursor)
            {
                cursor_pos.x = 0;
                cursor_x_tallest = 0;
                if (SDL_GetModState() & KMOD_CTRL)
                {
                    cursor_pos.y = 0;
                }
            }
            else if (e.key.keysym.sym == SDLK_END && can_move_cursor)
            {
                if (SDL_GetModState() & KMOD_CTRL)
                {
                    cursor_pos.y = get_lines() - 1;
                }
                cursor_pos.x = UTF8_total_codepoints(get_line(cursor_pos.y));
                cursor_x_tallest = cursor_pos.x;
            }
            else if (e.key.keysym.sym == SDLK_DELETE)
            {
                delete_key();
            }
            else if (e.key.keysym.sym == SDLK_PAGEUP && can_move_cursor)
            {
                cursor_pos.y -= 10;
                if (cursor_pos.y < 0)
                {
                    cursor_pos.y = 0;
                }
                if (cursor_pos.x > (int) UTF8_total_codepoints(get_line(cursor_pos.y)))
                {
                    cursor_pos.x = UTF8_total_codepoints(get_line(cursor_pos.y));
                }
            }
            else if (e.key.keysym.sym == SDLK_PAGEDOWN && can_move_cursor)
            {
                cursor_pos.y += 10;
                if (cursor_pos.y >= get_lines())
                {
                    cursor_pos.y = get_lines() - 1;
                }
                if (cursor_pos.x > (int) UTF8_total_codepoints(get_line(cursor_pos.y)))
                {
                    cursor_pos.x = UTF8_total_codepoints(get_line(cursor_pos.y));
                }
            }
        }
        //Special text input event
        else if (e.type == SDL_TEXTINPUT)
        {
            // Show cursor!!
            flash_timer = 0;

            //Append character(s)
            if (selecting)
            {
                remove_selection();
            }
            insert_text(e.text.text);
        }
    }

    void draw_text(uint32_t flags, int text_x, int text_y, std::vector<std::string>* text, TextInputInfo info)
    {
        const SDL_Color text_color = info.text_color;
        const SDL_Color selected_color = info.selected_color;
        const int visible_lines = info.visible_lines;
        const int visible_padding = info.visible_padding;

        if (cursor_pos.y < display_offset + visible_padding)
        {
            display_offset = SDL_max(0, TextInput::cursor_pos.y - visible_padding);
        }

        if (cursor_pos.y > display_offset + visible_lines - visible_padding)
        {
            display_offset = SDL_min((int) text->size() - visible_lines + visible_padding, cursor_pos.y - visible_lines + visible_padding);
        }

        // Draw text
        int font_height = font::height(flags);
        for (int i = 0; i < visible_lines; i++)
        {
            if (i + display_offset < (int) text->size())
            {
                font::print(flags, text_x, text_y + (i * font_height), text->at(i + display_offset), text_color.r, text_color.g, text_color.b);
            }
        }

        /* Draw selection boxes OVER the text,
         * so we can redraw the text over top
         * with a different color. */

        if (selecting)
        {
            const int y = cursor_select_pos.y;
            const int h = cursor_pos.y - y;

            const SelectionRect rect = reorder_selection_positions();

            if (h == 0)
            {
                // If the selection is only a single line

                char* offset_x = UTF8_substr(text->at(rect.y).c_str(), 0, rect.x);
                char* cut_string = UTF8_substr(text->at(rect.y).c_str(), rect.x, rect.x2);

                graphics.fill_rect(text_x + font::len(flags, offset_x), text_y + (y - display_offset) * font_height, font::len(flags, cut_string), font_height, text_color);
                font::print(flags, text_x + font::len(flags, offset_x), text_y + (rect.y2 - display_offset) * font_height, cut_string, selected_color.r, selected_color.g, selected_color.b);

                SDL_free(offset_x);
                SDL_free(cut_string);
            }
            else
            {
                // It's multiple lines, so draw multiple selection rectangles

                if (rect.y - display_offset >= 0)
                {
                    const char* line = text->at(rect.y).c_str();
                    char* offset_x = UTF8_substr(line, 0, rect.x);
                    char* selection_w = UTF8_substr(line, rect.x, UTF8_total_codepoints(line));

                    graphics.fill_rect(text_x + font::len(flags, offset_x), text_y + (rect.y - display_offset) * font_height, SDL_max(font::len(PR_FONT_LEVEL, selection_w), 1), font_height, text_color);
                    font::print(flags, text_x + font::len(flags, offset_x), text_y + (rect.y - display_offset) * font_height, selection_w, selected_color.r, selected_color.g, selected_color.b);

                    SDL_free(offset_x);
                    SDL_free(selection_w);
                }

                for (int i = 1; i < rect.y2 - rect.y; i++)
                {
                    const int local_y = rect.y + i - display_offset;
                    if (local_y >= 0 && local_y < visible_lines)
                    {
                        const int line_width = SDL_max(font::len(flags, text->at(rect.y + i).c_str()), 1);

                        graphics.fill_rect(text_x, text_y + local_y * font_height, line_width, font_height, text_color);
                        font::print(flags, text_x, text_y + local_y * font_height, text->at(rect.y + i).c_str(), selected_color.r, selected_color.g, selected_color.b);
                    }
                }

                if (rect.y2 - display_offset < visible_lines)
                {
                    const char* line_2 = text->at(rect.y2).c_str();
                    char* selection_w = UTF8_substr(line_2, 0, rect.x2);
                    const int line_width = SDL_max(font::len(flags, selection_w), 1);

                    graphics.fill_rect(text_x, text_y + (rect.y2 - display_offset) * font_height, line_width, font_height, text_color);
                    font::print(flags, text_x, text_y + (rect.y2 - display_offset) * font_height, selection_w, selected_color.r, selected_color.g, selected_color.b);

                    SDL_free(selection_w);
                }
            }
        }

        // Draw cursor
        if (TextInput::flash_timer < 15)
        {
            char* substr = UTF8_substr(text->at(TextInput::cursor_pos.y).c_str(), 0, TextInput::cursor_pos.x);

            if (TextInput::cursor_pos.x < (int) text->at(TextInput::cursor_pos.y).size() || TextInput::selecting)
            {
                graphics.set_color(text_color);
                int x = text_x + font::len(flags, substr);
                int y = text_y + ((TextInput::cursor_pos.y - display_offset) * font_height);
                SDL_RenderDrawLine(gameScreen.m_renderer, x, y, x, y + font_height - 1);
            }
            else
            {
                font::print(flags, text_x + font::len(flags, substr), text_y + ((TextInput::cursor_pos.y - display_offset) * font_height), "_", text_color.r, text_color.g, text_color.b);
            }
            SDL_free(substr);
        }
    }
}
