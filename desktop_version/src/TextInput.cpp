#include <SDL.h>
#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include "KeyPoll.h"
#include "TextInput.h"
#include "UTF8.h"
#include "UtilityClass.h"
#include "Vlogging.h"

namespace TextInput {
    bool taking_input;
    bool selecting;
    int flash_timer;
    std::vector<std::string>* current_text;
    SDL_Point cursor_pos;
    SDL_Point cursor_select_pos;
    int cursor_x_tallest;

    void init(void) {
        taking_input = false;
    }

    void attach_input(std::vector<std::string>* text) {
        taking_input = true;
        current_text = text;
        selecting = false;
        flash_timer = 0;
        SDL_StartTextInput();

        send_cursor_to_end();
    }

    void detach_input(void) {
        taking_input = false;
        SDL_StopTextInput();
    }

    void send_cursor_to_end(void)
    {
        cursor_pos.y = current_text->size() - 1;
        cursor_pos.x = UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str());
        cursor_x_tallest = cursor_pos.x;
    }

    void insert_text(std::string text) {
        // Insert text at cursor position, respecting newlines
        // Don't bother deleting selection, already done

        std::string::iterator it;
        for (it = text.begin(); it != text.end(); it++) {
            if (*it == '\n') {
                insert_newline();
            }
            else if (*it != '\r')
            {
                // Add character
                current_text->at(cursor_pos.y) += *it;
                cursor_pos.x++;
            }
        }
        cursor_x_tallest = cursor_pos.x;
    }

    void insert_newline(void)
    {
        char* first_part = UTF8_substr(current_text->at(cursor_pos.y).c_str(), 0, cursor_pos.x);
        char* second_part = UTF8_substr(current_text->at(cursor_pos.y).c_str(), cursor_pos.x, UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str()));

        current_text->at(cursor_pos.y) = first_part;
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
        cursor_select_pos.x = UTF8_total_codepoints(current_text->at(current_text->size() - 1).c_str());
        cursor_select_pos.y = current_text->size() - 1;
        selecting = true;
    }

    bool process_selection(void)
    {
        if (SDL_GetModState() & KMOD_SHIFT)
        {
            if (!selecting)
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

        Selection_Rect rect = reorder_selection_positions();

        if (rect.y == rect.y2) {
            return UTF8_substr(current_text->at(rect.y).c_str(), rect.x, rect.x2);
        }

        char* select_part_first_line = UTF8_substr(current_text->at(rect.y).c_str(), rect.x, UTF8_total_codepoints(current_text->at(rect.y).c_str()) - rect.x);
        char* select_part_last_line = UTF8_substr(current_text->at(rect.y2).c_str(), 0, rect.x2);

        // Loop through the lines in between
        int total_length = SDL_strlen(select_part_first_line) + SDL_strlen(select_part_last_line) + 1;
        for (int i = rect.y + 1; i < rect.y2; i++) {
            total_length += SDL_strlen(current_text->at(i).c_str()) + 1;
        }

        char* select_part = (char*)SDL_malloc(total_length);
        strcpy(select_part, select_part_first_line);
        strcat(select_part, "\n");
        for (int i = rect.y + 1; i < rect.y2; i++) {
            strcat(select_part, current_text->at(i).c_str());
            strcat(select_part, "\n");
        }
        strcat(select_part, select_part_last_line);

        SDL_free(select_part_first_line);
        SDL_free(select_part_last_line);

        return select_part;
    }

    Selection_Rect reorder_selection_positions(void) {
        Selection_Rect positions;
        bool in_front = false;

        if (cursor_pos.y > cursor_select_pos.y) {
            in_front = true;
        }
        else if (cursor_pos.y == cursor_select_pos.y) {
            if (cursor_pos.x >= cursor_select_pos.x) {
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
        if (x == x2 && y == y2) {
            return;
        }
        // Get the rest of the last line
        char* rest_of_string = UTF8_substr(current_text->at(y2).c_str(), x2, UTF8_total_codepoints(current_text->at(y2).c_str()));

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
        char* erased = UTF8_erase(current_text->at(y).c_str(), x, UTF8_total_codepoints(current_text->at(y).c_str()));
        current_text->at(y) = erased;
        // Add the rest of the last line to the end of the first line
        current_text->at(y) += rest_of_string;
        cursor_pos.x = x;
        SDL_free(erased);
        SDL_free(rest_of_string);
    }

    void remove_selection(void)
    {
        Selection_Rect positions = reorder_selection_positions();
        remove_characters(positions.x, positions.y, positions.x2, positions.y2);
        selecting = false;
    }

    void move_cursor_up(void)
    {
        bool reset = process_selection();  // Only returns true if you don't hold shift
        if (reset && (cursor_pos.y > cursor_select_pos.y)) {
            cursor_pos.y = cursor_select_pos.y;
        }

        if (cursor_pos.y > 0) {
            cursor_pos.y--;
            cursor_pos.x = cursor_x_tallest;
            if (cursor_pos.x > UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str())) {
                cursor_pos.x = UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str());
            }
        }
    }

    void move_cursor_down(void)
    {
        bool reset = process_selection();  // Only returns true if you don't hold shift
        if (reset && (cursor_pos.y < cursor_select_pos.y)) {
            cursor_pos.y = cursor_select_pos.y;
        }

        if (cursor_pos.y < current_text->size() - 1) {
            cursor_pos.y++;
            cursor_pos.x = cursor_x_tallest;
            if (cursor_pos.x > UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str())) {
                cursor_pos.x = UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str());
            }
        }
        else
        {
            cursor_pos.x = UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str());
        }
    }

    bool is_word_part(char character)
    {
        return character > 0x7F || isalpha(character) || isdigit(character) || character == '_' || character == '-';
    }

    void move_cursor_left(void)
    {
        bool reset = process_selection();  // Only returns true if you don't hold shift
        if (reset) {
            cursor_pos.x = cursor_select_pos.x;
        }

        if (cursor_pos.x == 0 && cursor_pos.y == 0) {
            cursor_x_tallest = 0;
            return;
        }

        if (SDL_GetModState() & KMOD_CTRL)
        {
            // If the current character is a word part, move to the start of the word. Else, move to the start of the next non-word.
            char* character = UTF8_substr(current_text->at(cursor_pos.y).c_str(), cursor_pos.x - 1, cursor_pos.x);
            bool is_word = is_word_part(character[0]);
            SDL_free(character);

            while (true)
            {
                char* character = UTF8_substr(current_text->at(cursor_pos.y).c_str(), cursor_pos.x - 1, cursor_pos.x);
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
                    cursor_pos.x = UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str());
                    break;
                }
            }
        }
        else
        {
            if (cursor_pos.x > 0) {
                cursor_pos.x--;
            }
            else if (cursor_pos.y > 0)
            {
                cursor_pos.y--;
                cursor_pos.x = UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str());
            }
        }

        cursor_x_tallest = cursor_pos.x;
    }

    void move_cursor_right(void)
    {
        bool reset = process_selection();  // Only returns true if you don't hold shift
        if (reset) {
            cursor_pos.x = cursor_select_pos.x;
        }

        if (cursor_pos.x >= UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str()) && cursor_pos.y == current_text->size() - 1) {
            cursor_x_tallest = cursor_pos.x;
            return;
        }

        if (SDL_GetModState() & KMOD_CTRL)
        {
            // If the current character is a word part, move to the end of the word. Else, move to the end of the next non-word.
            char* character = UTF8_substr(current_text->at(cursor_pos.y).c_str(), cursor_pos.x, cursor_pos.x + 1);
            bool is_word = is_word_part(character[0]);
            SDL_free(character);

            while (true)
            {
                char* character = UTF8_substr(current_text->at(cursor_pos.y).c_str(), cursor_pos.x, cursor_pos.x + 1);
                if (is_word_part(character[0]) != is_word)
                {
                    SDL_free(character);
                    break;
                }
                SDL_free(character);
                cursor_pos.x++;
                if (cursor_pos.x >= UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str()) && cursor_pos.y < current_text->size() - 1)
                {
                    cursor_pos.y++;
                    cursor_pos.x = 0;
                    break;
                }
            }
        }
        else
        {

            if (cursor_pos.x < UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str())) {
                cursor_pos.x++;
            }
            else if (cursor_pos.y < current_text->size() - 1)
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
        if (selecting) {
            remove_selection();
            return;
        }

        if (cursor_pos.x == 0)
        {
            if (cursor_pos.y > 0)
            {
                // Get the rest of the last line
                char* rest_of_string = UTF8_substr(current_text->at(cursor_pos.y).c_str(), 0, UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str()));

                // Erase the current line
                current_text->erase(current_text->begin() + cursor_pos.y);

                // Move the cursor up
                cursor_pos.y--;

                // Move the cursor to the end of the line
                cursor_pos.x = UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str());
                cursor_x_tallest = cursor_pos.x;

                // Add the rest of the last line to the end of the first line
                current_text->at(cursor_pos.y) += rest_of_string;
                SDL_free(rest_of_string);
            }
        }
        else
        {
            if (SDL_GetModState() & KMOD_CTRL)
            {
                char* character = UTF8_substr(current_text->at(cursor_pos.y).c_str(), cursor_pos.x - 1, cursor_pos.x);
                bool is_word = is_word_part(character[0]);
                SDL_free(character);

                const int remove_y = cursor_pos.y;
                int remove_x = cursor_pos.x;
                int start_x = cursor_pos.x;
                while (true)
                {
                    char* character = UTF8_substr(current_text->at(cursor_pos.y).c_str(), cursor_pos.x - 1, cursor_pos.x);
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
                        cursor_pos.x = UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str());
                        remove_x = 0;
                        break;
                    }
                }

                // Erase the characters before the cursor
                char* erased = UTF8_erase(current_text->at(remove_y).c_str(), remove_x, start_x);
                current_text->at(remove_y) = erased;
                SDL_free(erased);

                if (cursor_pos.y != remove_y)
                {
                    // Remove the line

                    char* rest_of_string = UTF8_substr(current_text->at(remove_y).c_str(), 0, UTF8_total_codepoints(current_text->at(remove_y).c_str()));

                    // Erase the current line
                    current_text->erase(current_text->begin() + remove_y);

                    // Add the rest of the last line to the end of the first line
                    current_text->at(cursor_pos.y) += rest_of_string;

                    SDL_free(rest_of_string);
                }
            }
            else
            {
                // Erase the character before the cursor
                char* erased = UTF8_erase(current_text->at(cursor_pos.y).c_str(), cursor_pos.x - 1, cursor_pos.x);
                current_text->at(cursor_pos.y) = erased;
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
        if (selecting) {
            remove_selection();
            return;
        }

        if (cursor_pos.x == UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str()))
        {
            if (cursor_pos.y < current_text->size() - 1)
            {
                // Get the rest of the last line
                char* rest_of_string = UTF8_substr(current_text->at(cursor_pos.y + 1).c_str(), 0, UTF8_total_codepoints(current_text->at(cursor_pos.y + 1).c_str()));

                // Erase the current line
                current_text->erase(current_text->begin() + cursor_pos.y + 1);

                // Add the rest of the last line to the end of the first line
                current_text->at(cursor_pos.y) += rest_of_string;
                SDL_free(rest_of_string);
            }
        }
        else
        {
            if (SDL_GetModState() & KMOD_CTRL)
            {
                char* character = UTF8_substr(current_text->at(cursor_pos.y).c_str(), cursor_pos.x, cursor_pos.x + 1);
                bool is_word = is_word_part(character[0]);
                SDL_free(character);

                const int remove_y = cursor_pos.y;
                int remove_x = cursor_pos.x;
                int start_x = cursor_pos.x;
                while (true)
                {
                    char* character = UTF8_substr(current_text->at(cursor_pos.y).c_str(), cursor_pos.x, cursor_pos.x + 1);
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
                char* erased = UTF8_erase(current_text->at(remove_y).c_str(), start_x, remove_x);
                current_text->at(remove_y) = erased;
                SDL_free(erased);

                cursor_pos.x = start_x;
            }
            else
            {
                // Erase the character before the cursor
                char* erased = UTF8_erase(current_text->at(cursor_pos.y).c_str(), cursor_pos.x, cursor_pos.x + 1);
                current_text->at(cursor_pos.y) = erased;
                SDL_free(erased);
            }
        }
    }

    void handle_events(SDL_Event e) {
        if (!taking_input) return;

        if (e.type == SDL_KEYDOWN) {
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
                if (selecting) {
                    remove_selection();
                }
                char* clipboard_text = SDL_GetClipboardText();
                insert_text(clipboard_text);
                SDL_free(clipboard_text);
            }
            else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
            {
                char* selected = get_selected_text();
                SDL_SetClipboardText(selected);
                SDL_free(selected);
            }
            else if (e.key.keysym.sym == SDLK_x && SDL_GetModState() & KMOD_CTRL)
            {
                char* selected = get_selected_text();
                SDL_SetClipboardText(selected);
                SDL_free(selected);
                remove_selection();
            }
            else if (e.key.keysym.sym == SDLK_a && SDL_GetModState() & KMOD_CTRL)
            {
                select_all();
            }
            else if (e.key.keysym.sym == SDLK_RETURN)
            {
                insert_newline();
            }
            else if (e.key.keysym.sym == SDLK_UP)
            {
                move_cursor_up();
            }
            else if (e.key.keysym.sym == SDLK_DOWN)
            {
                move_cursor_down();
            }
            else if (e.key.keysym.sym == SDLK_LEFT)
            {
                move_cursor_left();
            }
            else if (e.key.keysym.sym == SDLK_RIGHT)
            {
                move_cursor_right();
            }
            else if (e.key.keysym.sym == SDLK_HOME)
            {
                cursor_pos.x = 0;
                cursor_x_tallest = 0;
                if (SDL_GetModState() & KMOD_CTRL)
                {
                    cursor_pos.y = 0;
                }
            }
            else if (e.key.keysym.sym == SDLK_END)
            {
                if (SDL_GetModState() & KMOD_CTRL)
                {
                    cursor_pos.y = current_text->size() - 1;
                }
                cursor_pos.x = UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str());
                cursor_x_tallest = cursor_pos.x;
            }
            else if (e.key.keysym.sym == SDLK_DELETE)
            {
                delete_key();
            }
            else if (e.key.keysym.sym == SDLK_PAGEUP)
            {
                cursor_pos.y -= 10;
                if (cursor_pos.y < 0) {
                    cursor_pos.y = 0;
                }
                if (cursor_pos.x > UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str())) {
                    cursor_pos.x = UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str());
                }
            }
            else if (e.key.keysym.sym == SDLK_PAGEDOWN)
            {
                cursor_pos.y += 10;
                if (cursor_pos.y >= current_text->size()) {
                    cursor_pos.y = current_text->size() - 1;
                }
                if (cursor_pos.x > UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str())) {
                    cursor_pos.x = UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str());
                }
            }
        }
        //Special text input event
        else if (e.type == SDL_TEXTINPUT)
        {
            // Show cursor!!
            flash_timer = 0;

            //Append character(s)
            if (selecting) {
                remove_selection();
            }
            insert_text(e.text.text);
        }
    }
}
