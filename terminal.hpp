
#pragma once


#include <locale>
#include <iostream>
#include <ncurses.h>

namespace Terminal {
    // terminal color constants.
    const short 
        BLACK =	0,
        RED = 1,
        GREEN =	2,
        YELLOW = 3,
        BLUE = 4,
        MAGENTA = 5,
        CYAN = 6,
        WHITE =	7;

    // key constants 
    const int 
        UP_LEFT = 55,
        UP = 56,
        UP_RIGHT = 57,
        LEFT = 52,
        CENTER = 53, 
        RIGHT = 54,
        DOWN_RIGHT = 51,
        DOWN = 50,
        DOWN_LEFT = 49,
        ENTER = 10,
        ESCAPE = 27,
        BACKSPACE = KEY_BACKSPACE,
        SPELL_1 = 113,  // q
        SPELL_2 = 115,  // s
        SPELL_3 = 100,  // d
        SPELL_4 = 102,  // f
        EQUIP_SPELL_1 = 97,  // a
        EQUIP_SPELL_2 = 122,  // z
        EQUIP_SPELL_3 = 101,  // e
        EQUIP_SPELL_4 = 114;  // r

    // a drawable single character
    class Tile {
        public:
            std::string character;
            short color;

            Tile(std::string character_="?", short foreground_color=WHITE, short background_color=BLACK);
            void operator+=(Tile& other);
    };

    struct SpellIcon {
        Tile top_left;
        Tile top_right;
        Tile bottom_left;
        Tile bottom_right;
        SpellIcon(Tile top_left_=Tile(), Tile top_right_=Tile(), Tile bottom_left_=Tile(), Tile bottom_right_=Tile()) {
            top_left = top_left_;
            top_right = top_right_;
            bottom_left = bottom_left_;
            bottom_right = bottom_right_;
        }
    };

    // set up, stop and update terminal
    void initialize();
    void stop();
    void update();

    // get terminal boundaries
    void get_max_xy(int & max_x, int & max_y);

    // draw functions
    void draw_clear();
    void draw_tile(int x, int y, Tile tile);
    void draw_spell_icon(int x, int y, SpellIcon spell_icon);
    void draw_string(int x, int y, const char *string, short foreground_color=Terminal::WHITE, short background_color=Terminal::BLACK);

    // get keyboard input
    int get_key();
}
