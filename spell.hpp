
#pragma once

#include <list>

#include "map.hpp"
#include "entity.hpp"

// forward-declare Map and Entity for circular dependency  Map->Entity->Spell->Map
class Map;
class Entity;


class Spell {
    public:
        Terminal::SpellIcon spell_icon = Terminal::SpellIcon(
            Terminal::Tile(),
            Terminal::Tile(),
            Terminal::Tile(),
            Terminal::Tile()
        );
        
        virtual bool cast(Map& map, Entity& caster, int x, int y) = 0;
        virtual void update(Map& map, Entity& caster) = 0;  // move forward one frame
        virtual void reset(Map& map, Entity& caster) {};
};


//fireball
struct Projectile {
    int x, y;
    int dx, dy;
    Projectile(int x_, int y_, int dx_, int dy_) {
        x = x_;
        y = y_;
        dx = dx_;
        dy = dy_;
    }
};

class Fireball: public Spell {
    protected:
        const static int speed = 1;
        int damage;
        Terminal::Tile tile;  // tile to represent the projectiles flying
        std::list<Projectile> projectiles;
    public:
        Fireball();
        bool cast(Map& map, Entity& caster, int x, int y);
        void update(Map& map, Entity& caster);
        void reset(Map& map, Entity& dropper);
};


//llaberif
class Llaberif : public Fireball {
    public:
        Llaberif();
        bool cast(Map& map, Entity& caster, int x, int y);
};


// laser
struct Beam {
    int x, y;
    int dx, dy;
    int time_alive = 0;
    Beam(int x_, int y_, int dx_, int dy_) {
        x = x_;
        y = y_;
        dx = dx_;
        dy = dy_;
    }
};

class Laser: public Spell {
    private:
        static const int damage = 10;
        static const int charge_time = 4;
        static const int beam_time = 12;
        Terminal::Tile beam_tile = Terminal::Tile(" ", Terminal::BLACK, Terminal::CYAN);
        Terminal::Tile charge_tile_horizontal = Terminal::Tile("-", Terminal::WHITE);
        Terminal::Tile charge_tile_up_left = Terminal::Tile("\\", Terminal::WHITE);
        Terminal::Tile charge_tile_vertical = Terminal::Tile("|", Terminal::WHITE);
        Terminal::Tile charge_tile_up_right = Terminal::Tile("/", Terminal::WHITE);
        std::list<Beam> beams;
    public:
        Laser();
        bool cast(Map& map, Entity& caster, int x, int y);
        void update(Map& map, Entity& caster);
        void reset(Map& map, Entity& dropper);
};

// speed haste
class Haste: public Spell {
    private:
        int time_remaining = 0;
    public:
        Haste();
        bool cast(Map& map, Entity& caster, int x, int y);
        void update(Map& map, Entity& caster);
        virtual void reset(Map& map, Entity& dropper);
};

// bite
class Bite: public Spell {
    private:
        const int damage = 80;
        Terminal::Tile biting_tile = Terminal::Tile("#", Terminal::WHITE, Terminal::RED);
        bool is_biting = false;
        int bite_x, bite_y;
    public:
        Bite();
        bool cast(Map& map, Entity& caster, int x, int y);
        void update(Map& map, Entity& caster);
};


// self destruct
class SelfDestruct: public Spell {
    private:
        bool activated = false;
        int x, y;
        Terminal::Tile blast_tile = Terminal::Tile(" ", Terminal::BLACK, Terminal::MAGENTA);
    public:
        SelfDestruct();
        bool cast(Map& map, Entity& caster, int x, int y);
        void update(Map& map, Entity& caster);
};

