
#pragma once

#include <vector>
#include <map>
#include <list>
#include <math.h>
#include <random>
#include <ctime>

#include "terminal.hpp"
#include "entity.hpp"


// forward-declarations
class Entity;
class Enemy;
class Player;
class Spell;


struct Point {
    int x, y;
    Point(int x_, int y_) { x = x_; y = y_; }
    Point() { x = 0; y = 0; }
};


class Map {
private:
    int size_x, size_y;

    std::list<Enemy*> enemies;
    std::map<Point, Enemy*> position_to_enemy;
    std::map<Enemy*, Point> enemy_to_position;

    std::map<Point, int> damage_field;
    std::map<Point, Terminal::Tile> spell_tiles_field;

    std::map<Point, Spell*> dropped_spells;
    int spell_draw_index = 0;

public:
    Player *player;
    int player_x, player_y;

    Map(int size_x, int size_y);
    ~Map();

    // general purpose 
    bool is_blocked(int x, int y);
    bool is_out_of_bounds(int x, int y);

    // methods to manipulate player
    bool player_move(int dx, int dy);
    bool player_cast(int spell);
    bool player_pickup_spell(int index);

    // spawn enemies
    bool spawn_enemy(int x, int y, Enemy *enemy);
    void spawn_wave();

    // methods to move entities
    bool enemy_move_diagonal(Enemy& enemy, int destination_x, int destination_y, int speed);
    bool enemy_move_single_step(Enemy& enemy, int destination_x, int destination_y);

    // apply damage
    void apply_damage(int x, int y, int damage, Terminal::Tile tile);
    void apply_damage_disc(int center_x, int center_y, float radius, int damage, Terminal::Tile tile);

    // drop spells to the ground
    bool drop_spell(int x, int y, Spell *spell, Entity& dropper);

    // advance time
    void update();

    // draw current map state to the terminal
    void draw(int offset_x, int offset_y);
};
