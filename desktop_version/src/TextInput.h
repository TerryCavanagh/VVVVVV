#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include <SDL.h>
#include <string>
#include <vector>

struct SelectionRect
{
    int x;
    int y;
    int x2;
    int y2;
};

struct TextInputInfo
{
    SDL_Color text_color;
    SDL_Color selected_color;
    int visible_lines;
    int visible_padding;
};

namespace TextInput
{
    extern bool taking_input;
    extern bool selecting;
    extern int flash_timer;
    extern SDL_Point cursor_pos;
    extern SDL_Point cursor_select_pos;

    void send_cursor_to_end(void);
    void insert_newline(void);
    int get_lines(void);
    const char* get_line(int line);
    void set_line(int line, const char* text);
    void add_to_line(int line, const char* text);
    SelectionRect reorder_selection_positions(void);
    void handle_events(SDL_Event e);
    void attach_input(std::vector<std::string>* text);
    void attach_input(std::string* text);
    void detach_input(void);
    void draw_text(uint32_t flags, int x, int y, std::vector<std::string>* text, TextInputInfo info);
}

#endif /* TEXTINPUT_H */
