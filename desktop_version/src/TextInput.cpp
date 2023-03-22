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
        std::stringstream string_stream(text); // Create a stringstream for the text we're inserting
        std::string output;                    // We'll need the output later
        while (std::getline(string_stream, output)) { // Iterate through all lines,
            output.erase(std::remove(output.begin(), output.end(), '\r'), output.end()); // Strip \r... dammit Windows.
            current_text->at(cursor_pos.y).insert(cursor_pos.x, output); // Insert the current line of text into our text
            cursor_pos.x += UTF8_total_codepoints(output.c_str()); // Update cursor position
            if (!string_stream.eof()) { // If we haven't hit the end of the file,
                insert_newline(); // Insert a newline
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
    }

    void move_cursor_left(void)
    {
        bool reset = process_selection();  // Only returns true if you don't hold shift
        if (reset) {
            cursor_pos.x = cursor_select_pos.x;
        }

        if (cursor_pos.x > 0) {
            cursor_pos.x--;
        }
        else if (cursor_pos.y > 0)
        {
            cursor_pos.y--;
            cursor_pos.x = UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str());
        }

        cursor_x_tallest = cursor_pos.x;
    }

    void move_cursor_right(void)
    {
        bool reset = process_selection();  // Only returns true if you don't hold shift
        if (reset) {
            cursor_pos.x = cursor_select_pos.x;
        }

        if (cursor_pos.x < UTF8_total_codepoints(current_text->at(cursor_pos.y).c_str())) {
            cursor_pos.x++;
        }
        else if (cursor_pos.y < current_text->size() - 1)
        {
            cursor_pos.y++;
            cursor_pos.x = 0;
        }

        cursor_x_tallest = cursor_pos.x;
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
            // Erase the character before the cursor
            char* erased = UTF8_erase(current_text->at(cursor_pos.y).c_str(), cursor_pos.x - 1, cursor_pos.x);
            current_text->at(cursor_pos.y) = erased;
            SDL_free(erased);

            // Move the cursor back
            cursor_pos.x--;
            cursor_x_tallest = cursor_pos.x;
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
            else if (e.key.keysym.sym == SDLK_a && SDL_GetModState() & KMOD_CTRL) {
                select_all();
            }
            else if (e.key.keysym.sym == SDLK_RETURN) {
                insert_newline();
            }
            else if (e.key.keysym.sym == SDLK_UP) {
                move_cursor_up();
            }
            else if (e.key.keysym.sym == SDLK_DOWN) {
                move_cursor_down();
            }
            else if (e.key.keysym.sym == SDLK_LEFT) {
                move_cursor_left();
            }
            else if (e.key.keysym.sym == SDLK_RIGHT) {
                move_cursor_right();
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
