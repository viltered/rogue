
#include "map.hpp"


// necessary operators to make Point a valid key for std::map
bool operator==(const Point &left, const Point &right) {
    return left.x == right.x and left.y == right.y;
}
bool const operator<(const Point &left, const Point &right) {
    return left.x < right.x or (left.x == right.x and left.y < right.y);
}


Map::Map(int size_x_, int size_y_) {
    size_x = size_x_;
    size_y = size_y_;
    player = new Player();
    player_x = size_x / 2;
    player_y = size_y / 2;

    // seed random number generator
    srand(time(0));
}

Map::~Map() {
    for (auto pos_spell_pair : dropped_spells) {
        delete pos_spell_pair.second;
    }
    for (Enemy *enemy : enemies) {
        delete enemy;
    }
    delete player;
}


bool Map::is_out_of_bounds(int x, int y) {
    return x < 0 or y < 0 or x >= size_x or y >= size_y;
}

bool Map::is_blocked(int x, int y) {
    // check if coordinates are within bounds
    if (is_out_of_bounds(x, y)) {
        return true;
    }
    // check if an entity is blocking square
    if (position_to_enemy.count(Point(x, y)) == 1) {
        return true;
    }
    // check if player is blocking square
    return player_x == x and player_y == y;
}


bool Map::player_move(int dx, int dy) {
    int i = 0;
    while (i < player->speed && ! is_blocked(player_x + dx * (i + 1), player_y + dy * (i + 1))) {
        i++;
    }
    player_x += dx * i;
    player_y += dy * i;
    if (player->direction_dx != dx or player->direction_dy != dy) {
        player->direction_dx = dx;
        player->direction_dy = dy;
        return true;
    }
    // if i > 0 movement was successful
    return i > 0;
}

bool Map::player_cast(int spell_index) {
    if (player->spells.size() <= spell_index) {
        return false;
    }
    return player->spells[spell_index]->cast(*this, *player, player_x, player_y);
}

bool Map::player_pickup_spell(int spell_index) {
    Point position = Point(player_x, player_y);
    if (dropped_spells.count(position) != 0) {
        // replace spell on the ground with spell being held
        Spell *dropped_by_player = player->spells[spell_index];
        dropped_by_player->reset(*this, *player);
        player->spells[spell_index] = dropped_spells[position];
        dropped_spells[position] = dropped_by_player;
        return true;
    } else {
        return false;
    }
}

bool Map::spawn_enemy(int x, int y, Enemy *enemy) {
    if (is_blocked(x, y)) {
        return false;
    } else {
        Point desired_position = Point(x, y);
        enemies.push_back(enemy);
        position_to_enemy[desired_position] = enemy;
        enemy_to_position[enemy] = desired_position;
        return true;
    }
}

void Map::spawn_wave() {
    // spawn new wave of enemies with difficulty based on score
    // increase in difficulty of waves is linear while score increase is quadratic
    int remaining_difficulty = std::sqrt(player->score) + 2;
    int max_difficulty_per_mob = (remaining_difficulty + 1) / 2;
    while (remaining_difficulty > 0) {
        Enemy *next_enemy;
        int next_difficulty = rand() % std::min(max_difficulty_per_mob, remaining_difficulty) + 1;
        // find match in enemies ordered by difficulty
        if (next_difficulty >= Bomber::difficulty) {
            next_enemy = new Bomber();
        } else if (next_difficulty >= Zombie::difficulty) {
            next_enemy = new Zombie();
        }
        remaining_difficulty -= next_enemy->difficulty;
        // try to spawn enemy at random positions until one is valid
        while (not spawn_enemy(rand() % size_x, rand() % size_y, next_enemy));
    }
}

bool Map::enemy_move_diagonal(Enemy &enemy, int destination_x, int destination_y, int speed) {
    // todo: move to Entity base class
    // simple algorithm to move in a diagonal line towards the player
    // forming a perfect line is impossible as previous locations are not known and step sizes are finite
    Point old_position = enemy_to_position[&enemy];
    
    // if only one step must to be taken, use other algorithm which behaves better at short distances
    if (std::max(abs(old_position.x - destination_x), abs(old_position.y - destination_y)) - is_blocked(destination_x, destination_y) <= 1) {
        return enemy_move_single_step(enemy, destination_x, destination_y);
    }

    // see how far the entity can move in the desired direction, by taking steps of size move_unit
    float move_unit_x = (float)(destination_x - old_position.x);
    float move_unit_y = (float)(destination_y - old_position.y);
    // normalize move_unit
    float length = sqrt(move_unit_x * move_unit_x + move_unit_y * move_unit_y);
    move_unit_x /= length;
    move_unit_y /= length;

    // step forward until before an invalid step occurs or until the goal is reached
    int steps = 0;
    int step_x, step_y;
    do {
        steps++;
        if (step_x == destination_x and step_y == destination_y) {
            break;
        }
        step_x = floor(move_unit_x * (float)(steps) + 0.5);
        step_y = floor(move_unit_y * (float)(steps) + 0.5);
    } while (std::max(abs(step_x), abs(step_y)) <= speed and not 
           is_blocked(step_x + old_position.x, step_y + old_position.y));

    if (steps <= 1) {
        // not a single step could be taken
        return false;
    } else {
        // move one step less, as the last step was failing one of the conditions
        step_x = floor(move_unit_x * (float)(steps - 1) + 0.5);
        step_y = floor(move_unit_y * (float)(steps - 1) + 0.5);
        Point new_position = Point(step_x + old_position.x, step_y + old_position.y);

        // calculate direction
        int dx = 0, dy = 0;
        if (step_x * 2 > abs(step_y)) {
            dx = 1;
        } else if (step_x * 2 < - abs(step_y)) {
            dx = -1;
        }
        if (step_y * 2 > abs(step_x)) {
            dy = 1;
        } else if (step_y * 2 < - abs(step_x)) {
            dy = -1;
        }

        // apply changes
        position_to_enemy.erase(old_position);
        position_to_enemy[new_position] = &enemy;
        enemy_to_position[&enemy] = new_position;
        enemy.direction_dx = dx;
        enemy.direction_dy = dy;

        return true;
    }
}

bool Map::enemy_move_single_step(Enemy &enemy, int destination_x, int destination_y) {
    // todo: move to Entity base class
    Point old_position = enemy_to_position[&enemy];

    // calculate desired step
    int dx = (destination_x > old_position.x) - (destination_x < old_position.x);
    int dy = (destination_y > old_position.y) - (destination_y < old_position.y);
    Point new_position = Point(old_position.x + dx, old_position.y + dy);

    if (is_blocked(new_position.x, new_position.y)) {
        if (enemy.direction_dx != dx or enemy.direction_dy != dy) {
            // cannot move, only change direction
            enemy.direction_dx = dx;
            enemy.direction_dy = dy;
            return true;
        } else {
            // cannot move or change direction, movement failed
            return false;
        }
    } else {
        // can move and change direction
        position_to_enemy.erase(old_position);
        position_to_enemy[new_position] = &enemy;
        enemy_to_position[&enemy] = new_position;
        enemy.direction_dx = dx;
        enemy.direction_dy = dy;
        return true;
    }
}


void Map::apply_damage(int x, int y, int damage, Terminal::Tile tile) {
    if (not is_out_of_bounds(x, y)) {
        Point p = Point(x, y);
        damage_field[p] += damage;
        if (spell_tiles_field.count(p) == 0) {
            spell_tiles_field[p] = tile;
        } else {
            // overwrite tile foreground and background if any. later spells get priority
            spell_tiles_field[p] += tile;
        }
    }
}

void Map::apply_damage_disc(int center_x, int center_y, float radius, int damage, Terminal::Tile tile) {
    for (int x = 0; x < (int)(radius + .5); x++) {
        int max_y_squared = radius * radius - x * x;
        for (int y = 1; y * y < max_y_squared; y++) {
            // fill tile for each of the four quadrants
            apply_damage(center_x + x, center_y + y, damage, tile);
            apply_damage(center_x - y, center_y + x, damage, tile);
            apply_damage(center_x - x, center_y - y, damage, tile);
            apply_damage(center_x + y, center_y - x, damage, tile);
        }
    }
    // fill in center tile
    apply_damage(center_x, center_y, damage, tile);
}


bool Map::drop_spell(int x, int y, Spell *spell, Entity& dropper) {
    if (dropped_spells.count(Point(x, y)) != 0) {
        return false;
    } else {
        dropped_spells[Point(x, y)] = spell;
        spell->reset(*this, dropper);
        return true;
    }
}



void Map::update() {
    damage_field.clear();
    spell_tiles_field.clear();

    // move and update enemies
    for (auto enemy : enemies) {
        // run-time polymorphism of enemy behaviour:
        Point old_position = enemy_to_position[enemy];
        enemy->move_or_cast(*this, old_position.x, old_position.y);
        Point new_position = enemy_to_position[enemy];
        enemy->update(*this, new_position.x, new_position.y);
    }

    // if there are no enemies, spawn new wave
    if (enemies.empty()) {
        spawn_wave();
    }

    // update player
    player->update(*this, player_x, player_y);

    // apply damage to enemies, check if they are dead
    std::list<Enemy*>::iterator it = enemies.begin();
    while (it != enemies.end()) {
        Point position = enemy_to_position[*it];
        if (damage_field.count(position) != 0) {
            (*it)->health -= damage_field[position];
            (*it)->last_turn_damage = damage_field[position];
            if ((*it)->health <= 0.) {
                (*it)->die(*this, position.x, position.y);
                player->score += (*it)->difficulty;
                delete *it;
                position_to_enemy.erase(position);
                enemy_to_position.erase(*it);
                enemies.erase(it++);  // increment itterator first and then delete old element
            } else {
                it++;  // increment without deleting
            }
        } else {
            (*it)->last_turn_damage = 0;
            it++;
        }
    }

    // player heals 1 every turn, up to 100. this happens before(!) damage is applied to player
    if (player->health < 100) {
        player->health++;
    }

    // apply damage to player
    if (damage_field.count(Point(player_x, player_y)) != 0) {
        player->health -= damage_field[Point(player_x, player_y)];
        player->last_turn_damage = damage_field[Point(player_x, player_y)];
    } else {
        player->last_turn_damage = 0;
    }
}

void Map::draw(int offset_x, int offset_y) {
    
    // draw dropped spells
    for (auto pos_spell_pair : dropped_spells) {
        Terminal::Tile tile;
        switch (spell_draw_index) {
        case 0:
            tile = pos_spell_pair.second->spell_icon.top_left;
            break;
        case 1:
            tile = pos_spell_pair.second->spell_icon.top_right;
            break;
        case 2:
            tile = pos_spell_pair.second->spell_icon.bottom_left;
            break;
        case 3:
            tile = pos_spell_pair.second->spell_icon.bottom_right;
            break;
        }
        Terminal::draw_tile(pos_spell_pair.first.x + offset_x, pos_spell_pair.first.y + offset_y, tile);
    }
    spell_draw_index = (spell_draw_index + 1) % 4;

    // draw entities (not affected by spells)
    for (auto pos_enemy_pair : position_to_enemy) {
        if (spell_tiles_field.count(pos_enemy_pair.first) == 0) {
            Terminal::draw_tile(pos_enemy_pair.first.x + offset_x, pos_enemy_pair.first.y + offset_y, pos_enemy_pair.second->representation);
        } else {
            // combine tile with spell tile already present, it will get drawn later
            spell_tiles_field[pos_enemy_pair.first] += pos_enemy_pair.second->representation;
        }
    }

    // draw player (if not affected by spell)
    if (spell_tiles_field.count(Point(player_x, player_y)) == 0) {
        Terminal::draw_tile(player_x + offset_x, player_y + offset_y, player->representation);
    } else {
        spell_tiles_field[Point(player_x, player_y)] += player->representation;
    }

    // draw spell tiles with affected entities and player merged into them
    for (auto pos_spell_tile_pair : spell_tiles_field) {
        Terminal::draw_tile(pos_spell_tile_pair.first.x + offset_x, pos_spell_tile_pair.first.y + offset_y, pos_spell_tile_pair.second);
    }
}
