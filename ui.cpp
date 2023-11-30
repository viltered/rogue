
#include "ui.hpp"


void UI::draw_ui(Player *player, int size_x, int size_y) {
    // draw frame
    std::string horizontal_border = "";
    for (int i = 0; i < size_x - 2; i++) {
        horizontal_border += "═";
    }
    Terminal::draw_string(1, 0, horizontal_border.c_str());
    Terminal::draw_string(1, size_y - 4, horizontal_border.c_str());
    Terminal::draw_string(1, size_y - 1, horizontal_border.c_str());
    for (int y = 1; y < size_y - 1; y++) {
        Terminal::draw_string(0, y, "║");
        Terminal::draw_string(size_x - 1, y, "║");
    }
    Terminal::draw_string(0, 0, "╔");
    Terminal::draw_string(size_x - 1, 0, "╗");
    Terminal::draw_string(0, size_y - 4, "╠");
    Terminal::draw_string(size_x - 1, size_y - 4, "╣");
    Terminal::draw_string(0, size_y - 1, "╚");
    Terminal::draw_string(size_x - 1, size_y - 1, "╝");
    for (int i = 0; i < 4; i++) {
        Terminal::draw_string(size_x - 3 * i - 4, size_y - 4, "╦");
        Terminal::draw_string(size_x - 3 * i - 4, size_y - 3, "║");
        Terminal::draw_string(size_x - 3 * i - 4, size_y - 2, "║");
        Terminal::draw_string(size_x - 3 * i - 4, size_y - 1, "╩");
    }
    
    // draw health bar
    int health_bar_size = std::max(int(std::min(player->health / 100., 1.) * float(size_x - 14) + 0.5), 0);
    int damage_bar_size = std::max(int(std::min((player->last_turn_damage + (player->health < 0 ? player->health : 0)) / 100., 1.) * float(size_x - 14) + 0.5), 0);
    
    for (int i = 1; i < health_bar_size + 1; i++) {
        Terminal::draw_string(i, size_y - 3, "█", Terminal::BLUE);
        Terminal::draw_string(i, size_y - 2, "█", Terminal::BLUE);
    }
    for (int i = health_bar_size + 1; i < health_bar_size + damage_bar_size + 1; i++) {
        Terminal::draw_string(i, size_y - 3, "▒", Terminal::RED);
        Terminal::draw_string(i, size_y - 2, "▒", Terminal::RED);
    }

    // draw health number above bar
    Terminal::draw_string(1, size_y - 4, std::to_string(int(player->health)).c_str());
    
    // draw spells
    for (int i = 0; i < 4; i++) {
        Terminal::draw_spell_icon(size_x - 12 + i * 3, size_y - 3, player->spells[i]->spell_icon);
    }
}

void UI::draw_death_screen(Player *player, int size_x, int size_y) {
    const int window_width = 29;
    const std::array<const char*, 14> window = 
        {"╔═══════════════════════════╗",
         "║                           ║", 
         "║                           ║", 
         "║    ██    You died!        ║", 
         "║    ██                     ║", 
         "║  ██████                   ║", 
         "║  ██████  Score:           ║", 
         "║    ██                     ║", 
         "║    ██                     ║", 
         "║    ██    Press enter to   ║", 
         "║    ██    restart.         ║", 
         "║                           ║", 
         "║                           ║", 
         "╚═════════════════════ENTER═╝"};

    // calculate window coordinates
    int window_x = (size_x - window_width) / 2;
    int window_y = (size_y - window.size()) / 2;

    // print window
    int y = 0;
    for (const char *row : window) {
        Terminal::draw_string(window_x, window_y + y, row);
        y++;
    }

    // print score
    Terminal::draw_string(window_x + 18, window_y + 6, std::to_string(int(player->score)).c_str());
}
