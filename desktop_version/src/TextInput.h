#ifndef TEXTINPUT_H
#define TEXTINPUT_H

struct Selection_Rect
{
    int x;
    int y;
    int x2;
    int y2;
};

namespace TextInput
{
    void send_cursor_to_end(void);
    void insert_newline(void);
    Selection_Rect reorder_selection_positions(void);
    void handle_events(SDL_Event e);
}

#endif /* TEXTINPUT_H */
