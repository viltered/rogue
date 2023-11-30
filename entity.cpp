
#include "entity.hpp"


Entity::~Entity() {
    // any spells that are left over (not dropped) must be deleted
    for (auto spell : spells) {
        delete spell;
    }
}

void Entity::update(Map& map, int x, int y) {
    for (auto spell : spells) {
        spell->update(map, *this);
    }
}



Player::Player() {
    health = 100.;
    speed = 1;
    representation = Terminal::Tile("@");
    spells.push_back(new Fireball());
    spells.push_back(new Llaberif());
    spells.push_back(new Laser());
    spells.push_back(new Haste());
}


void Enemy::die(Map& map, int x, int y) {
    for (std::vector<Spell*>::iterator it = spells.begin(); it != spells.end(); ) {
        if (rand() % 5 == 0 and map.drop_spell(x, y, *it, *this)) {
            spells.erase(it);
        } else {
            // if the spell could not be dropped, keep it. that way it will by cleaned by ~Entity()
            it++;
        }
    }
}


// zombie
Zombie::Zombie() {
    health = 100;
    speed = 1;
    representation = Terminal::Tile("z", Terminal::GREEN);
    spells.push_back(new Bite());
}

void Zombie::move_or_cast(Map& map, int x, int y){
    // short circuit evaluation ensures the zombie won't move and bite at the same time
    if (rand() % 4 == 0 or not map.enemy_move_diagonal(*this, map.player_x, map.player_y, speed)) {
        spells[0]->cast(map, *this, x, y);
    }
}


// bomber
Bomber::Bomber() {
    health = 60;
    speed = 2;
    representation = Terminal::Tile("%", Terminal::RED);
    spells.push_back(new SelfDestruct());
}

void Bomber::move_or_cast(Map& map, int x, int y) {
    if (std::max(std::abs(map.player_x - x), std::abs(map.player_y - y)) <= 2) {
        is_detonating = true;
    }
    if (is_detonating) { 
        if (countdown == 0) {
            spells[0]->cast(map, *this, x, y);
        } else {
            countdown--;
            representation = Terminal::Tile(std::to_string(countdown), Terminal::RED);
        }
    } else {
        map.enemy_move_diagonal(*this, map.player_x, map.player_y, speed);
    }
}
