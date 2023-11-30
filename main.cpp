
#include <iostream>

#include "map.hpp"
#include "ui.hpp"


enum class GameState { Game, DeathScreen };


int main() {
    Terminal::initialize();

	GameState state = GameState::Game;
	GameState previous_state;
	bool quit = false;
	int key = 0;  // pressed key

	int size_x, size_y;
	Terminal::get_max_xy(size_x, size_y);

	// UI borders take up some extra space
	Map *map = new Map(size_x - 2, size_y - 5);

	// main game loop
	while (!quit) {
		
		switch (state) {
		case GameState::Game:
			// handle keyboard input
			switch (key) {
			case Terminal::ESCAPE:
			case Terminal::BACKSPACE:
				quit = true;
				break;
			case Terminal::UP_LEFT:
				map->player_move(-1, -1);
				break;
			case Terminal::UP:
				map->player_move(0, -1);
				break;
			case Terminal::UP_RIGHT:
				map->player_move(1, -1);
				break;
			case Terminal::LEFT:
				map->player_move(-1, 0);
				break;
			case Terminal::RIGHT:
				map->player_move(1, 0);
				break;
			case Terminal::DOWN_LEFT:
				map->player_move(-1, 1);
				break;
			case Terminal::DOWN:
				map->player_move(0, 1);
				break;
			case Terminal::DOWN_RIGHT:
				map->player_move(1, 1);
				break;
			case Terminal::SPELL_1:
				map->player_cast(0);
				break;
			case Terminal::SPELL_2:
				map->player_cast(1);
				break;
			case Terminal::SPELL_3:
				map->player_cast(2);
				break;
			case Terminal::SPELL_4:
				map->player_cast(3);
				break;
			case Terminal::EQUIP_SPELL_1:
				map->player_pickup_spell(0);
				break;
			case Terminal::EQUIP_SPELL_2:
				map->player_pickup_spell(1);
				break;
			case Terminal::EQUIP_SPELL_3:
				map->player_pickup_spell(2);
				break;
			case Terminal::EQUIP_SPELL_4:
				map->player_pickup_spell(3);
				break;


			}
			
			// advance time for the map
			map->update();

			// check if player is dead
			if (map->player->health <= 0) {
				state = GameState::DeathScreen;
			}

			// update screen
			Terminal::draw_clear();
			map->draw(1, 1);
			UI::draw_ui(map->player, size_x, size_y);
			Terminal::update();
			break;

		case GameState::DeathScreen:
			// handle keyboard input
			switch (key) {
			case Terminal::ESCAPE:
			case Terminal::BACKSPACE:
				quit = true;
				break;
			case Terminal::ENTER:
				// refresh map
				delete map;
				map = new Map(size_x - 2, size_y - 5);
				state = GameState::Game;
				break;
			}

			// don't clear the screen, just print the death message on top of whatever's left
			UI::draw_death_screen(map->player, size_x, size_y);
			Terminal::update();
		}

		if (state == previous_state and not quit) {
			key = Terminal::get_key();
		} else {
			previous_state = state;
			key = 0;
		}
	}

	Terminal::stop();
}
