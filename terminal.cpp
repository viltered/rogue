
#include "terminal.hpp"


Terminal::Tile::Tile(std::string character_, short foreground_color, short background_color) {
	character = character_;
	color = foreground_color * 8 + background_color;
}

void Terminal::Tile::operator+=(Terminal::Tile& other) {
	// if background color of the new tile is black, allow the old background color to persist
	if (other.color % 8 == Terminal::BLACK) {
		color = other.color - other.color % 8 + color % 8;
	} else {
		color = other.color;
	}
	character = other.character;
}

// perform necessary initialization for ncurses
void Terminal::initialize() {
	setlocale(LC_ALL, "");      // set locale to enable unicode support
	initscr();
	clear();
	raw();						// disable line buffering
	keypad(stdscr, TRUE);		// enable function keys, arrow keys...
	noecho();					// don't echo characters read through getch()
	curs_set(0);				// hide cursor
	// nodelay(stdscr, TRUE); 	    // don't wait until a key is pressed when calling getch()
	if (! has_colors()) {
		endwin();
		std::cerr << "Error: terminal does not have color support.\n";
		exit(1);
	}
	start_color();              // enable color printing

	// initialize all possible colors.
	for (int foreground = 0; foreground < 8; foreground++) {
		for (int background = 0; background < 8; background++) {
			init_pair(foreground * 8 + background, foreground, background);
		}
	}
}

void Terminal::stop() {
	endwin();
}


void Terminal::update() {
	refresh();
}

void Terminal::get_max_xy(int & max_x, int & max_y) {
	getmaxyx(stdscr, max_y, max_x);
}

void Terminal::draw_clear() {
	clear();
}

void Terminal::draw_tile(int x, int y, Terminal::Tile tile) {
	attron(COLOR_PAIR(tile.color));
	mvaddstr(y, x, tile.character.c_str());
}

void Terminal::draw_spell_icon(int x, int y, SpellIcon spell_icon) {
	draw_tile(x, y, spell_icon.top_left);
	draw_tile(x + 1, y, spell_icon.top_right);
	draw_tile(x, y + 1, spell_icon.bottom_left);
	draw_tile(x + 1, y + 1, spell_icon.bottom_right);
}

void Terminal::draw_string(int x, int y, const char *string, short foreground_color, short background_color) {
	attron(COLOR_PAIR(foreground_color * 8 + background_color));
	mvaddstr(y, x, string);
}


int Terminal::get_key() {
	int c = getch();
	switch (c) {
	// arrow keys are valid alternatives to num pad controls for left, right, up, down
	case KEY_UP:
		return UP;
	case KEY_DOWN:
		return DOWN;
	case KEY_LEFT:
		return LEFT;
	case KEY_RIGHT:
		return RIGHT;
	// other keys have the same constants assigned to them as ncurses uses
	default:
		return c;
	}
}