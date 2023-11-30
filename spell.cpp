

#include "spell.hpp"

// fireball
Fireball::Fireball() {
    damage = 50;
    tile = Terminal::Tile("*", Terminal::YELLOW, Terminal::RED);
    spell_icon = Terminal::SpellIcon(
        Terminal::Tile(">", Terminal::YELLOW, Terminal::RED),
        Terminal::Tile("@", Terminal::YELLOW, Terminal::RED),
        Terminal::Tile("*", Terminal::YELLOW, Terminal::RED),
        Terminal::Tile("*", Terminal::YELLOW, Terminal::RED)
    );
}

bool Fireball::cast(Map& map, Entity& caster, int x, int y) {
    if (map.is_out_of_bounds(x + caster.direction_dx * speed, y + caster.direction_dy * speed)) {
        return false;
    } else {
        projectiles.push_back(Projectile(x, y, caster.direction_dx, caster.direction_dy));
        return true;
    }
}

void Fireball::update(Map& map, Entity& caster) {
    std::list<Projectile>::iterator it = projectiles.begin();
    while (it != projectiles.end()) {
        // move forward
        (*it).x += (*it).dx * speed;
        (*it).y += (*it).dy * speed;
        if (map.is_out_of_bounds((*it).x, (*it).y)) {
            // if bullet has moved out of the screen, delete it
            projectiles.erase(it++);
        }
        else {
            // if not, allow it to do damage this frame
            map.apply_damage((*it).x, (*it).y, damage, tile);
            it++;
        }
    }
}

void Fireball::reset(Map& map, Entity& dropper) {
    projectiles.clear();
}


// llaberif
Llaberif::Llaberif () {
    damage = 25;
    tile = Terminal::Tile("*", Terminal::RED, Terminal::YELLOW);
    spell_icon = Terminal::SpellIcon(
        Terminal::Tile("@", Terminal::RED, Terminal::YELLOW),
        Terminal::Tile("<", Terminal::RED, Terminal::YELLOW),
        Terminal::Tile("*", Terminal::RED, Terminal::YELLOW),
        Terminal::Tile("*", Terminal::RED, Terminal::YELLOW)
    );
}

bool Llaberif::cast(Map& map, Entity& caster, int x, int y) {
    if (map.is_out_of_bounds(x - caster.direction_dx * speed, y - caster.direction_dy * speed)) {
        return false;
    } else {
        projectiles.push_back(Projectile(x, y, -caster.direction_dx, -caster.direction_dy));
        return true;
    }
}

// laserbeam
Laser::Laser() {
    spell_icon = Terminal::SpellIcon(
        Terminal::Tile("|", Terminal::WHITE),
        Terminal::Tile(" ", Terminal::WHITE, Terminal::CYAN),
        Terminal::Tile("|", Terminal::WHITE),
        Terminal::Tile(" ", Terminal::WHITE, Terminal::CYAN)
    );
}

bool Laser::cast(Map& map, Entity& caster, int x, int y) {
    if (map.is_out_of_bounds(x + caster.direction_dx, y + caster.direction_dy)) {
        return false;
    } else {
        beams.push_back(Beam(x + caster.direction_dx, y + caster.direction_dy, caster.direction_dx, caster.direction_dy));
        return true;
    }
}

void Laser::update(Map& map, Entity& caster) {
    std::list<Beam>::iterator it = beams.begin();
    while (it != beams.end()) {
        (*it).time_alive++;
        if ((*it).time_alive > beam_time + charge_time) {
            // beam is done beaming, erase it
            beams.erase(it++);
        }
        else if ((*it).time_alive > charge_time){
            // beam has been charged and is applying damage
            int x = (*it).x, y = (*it).y;
            while (not map.is_out_of_bounds(x, y))
            {
                map.apply_damage(x, y, damage, beam_tile);
                x += (*it).dx;
                y += (*it).dy;
            }

            it++;
        } else {
            // beam is charging
            // determine the right tile to display based on orientation
            Terminal::Tile *tile;
            if ((*it).dx == 0) {
                tile = &charge_tile_vertical;
            } else if ((*it).dy == 0) {
                tile = &charge_tile_horizontal;
            } else if ((*it).dx * (*it).dy == 1) {
                tile = &charge_tile_up_left;
            } else {
                tile = &charge_tile_up_right;
            }
            
            // draw tile
            int x = (*it).x, y = (*it).y;
            while (not map.is_out_of_bounds(x, y)) {
                map.apply_damage(x, y, 0, *tile);
                y += (*it).dy;
                x += (*it).dx;
            }

            it++;
        }
    }
}

void Laser::reset(Map& map, Entity& dropper) {
    beams.clear();
}


// haste
Haste::Haste() {
    spell_icon = Terminal::SpellIcon(
        Terminal::Tile(">", Terminal::WHITE, Terminal::BLUE),
        Terminal::Tile(">", Terminal::WHITE, Terminal::BLUE),
        Terminal::Tile(">", Terminal::WHITE, Terminal::BLUE),
        Terminal::Tile(">", Terminal::WHITE, Terminal::BLUE)
    );
}

bool Haste::cast(Map& map, Entity& caster, int x, int y) {
    if (time_remaining == 0) {
        time_remaining = 6;
        caster.speed += 2;
        spell_icon = Terminal::SpellIcon(
            Terminal::Tile(">", Terminal::WHITE),
            Terminal::Tile(">", Terminal::WHITE),
            Terminal::Tile("6", Terminal::WHITE),
            Terminal::Tile(">", Terminal::WHITE)
        );
        return true;
    } else {
        return false;
    }
}

void Haste::update(Map& map, Entity& caster) {
    if (time_remaining > 0) {
        time_remaining--;
        if (time_remaining == 0) {
            caster.speed -= 2;
            spell_icon = Terminal::SpellIcon(
                Terminal::Tile(">", Terminal::WHITE, Terminal::BLUE),
                Terminal::Tile(">", Terminal::WHITE, Terminal::BLUE),
                Terminal::Tile(">", Terminal::WHITE, Terminal::BLUE),
                Terminal::Tile(">", Terminal::WHITE, Terminal::BLUE)
            );
        } else {
            spell_icon.bottom_left = Terminal::Tile(std::to_string(time_remaining), Terminal::WHITE);
        }
    }
}

void Haste::reset(Map& map, Entity& dropper) {
    if (time_remaining > 0) {
        time_remaining = 1;
        update(map, dropper);
    }
}


// bite (zombie)
Bite::Bite() {
    spell_icon = Terminal::SpellIcon(
        Terminal::Tile("#", Terminal::WHITE, Terminal::RED),
        Terminal::Tile("#", Terminal::WHITE, Terminal::RED),
        Terminal::Tile("W", Terminal::WHITE, Terminal::RED),
        Terminal::Tile("W", Terminal::WHITE, Terminal::RED)
    );
}

bool Bite::cast(Map& map, Entity& caster, int x, int y) {
    if (map.is_out_of_bounds(x + caster.direction_dx, y + caster.direction_dy)) {
        return false;
    } else {
        is_biting = true;
        bite_x = x + caster.direction_dx;
        bite_y = y + caster.direction_dy;
        return true;
    }
}

void Bite::update(Map& map, Entity& caster) {
    if (is_biting) {
        map.apply_damage(bite_x, bite_y, damage, biting_tile);
        is_biting = false;
    }
}


// self destruct
SelfDestruct::SelfDestruct() {
    spell_icon = Terminal::SpellIcon(
        Terminal::Tile("\\", Terminal::WHITE, Terminal::MAGENTA),
        Terminal::Tile("/", Terminal::WHITE, Terminal::MAGENTA),
        Terminal::Tile("/", Terminal::WHITE, Terminal::MAGENTA),
        Terminal::Tile("\\", Terminal::WHITE, Terminal::MAGENTA)
    );
}

bool SelfDestruct::cast(Map& map, Entity& caster, int x_, int y_) {
    activated = true;
    x = x_;
    y = y_;
    return true;
}

void SelfDestruct::update(Map& map, Entity& caster){
    if (activated) {
        int blast_radius = caster.health / 10;
        map.apply_damage_disc(x, y, blast_radius, caster.health, blast_tile);
        activated = false;
    }
}
