
#pragma once

#include <vector>
#include <random>

#include "map.hpp"
#include "terminal.hpp"
#include "spell.hpp"


class Map;
class Spell;


class Entity {
    public:
        int health;
        int speed;
        float last_turn_damage = 0.;
        int direction_dx = 0, direction_dy = -1;
        Terminal::Tile representation;
        std::vector<Spell*> spells;

        virtual ~Entity();
        virtual void update(Map &map, int x, int y);
};


class Player: public Entity {
    public:
        int score = 0;

        Player();
};


class Enemy: public Entity {
    public:
        const static int difficulty = 1;

        virtual void die(Map &map, int x, int y);
        virtual void move_or_cast(Map &map, int x, int y) = 0;
};


class Zombie: public Enemy {
    public:
        Zombie();
        void move_or_cast(Map &map, int x, int y) override;
};


class Bomber: public Enemy {
    private:
        bool is_detonating = false;
        int countdown = 4;
    public:
        const static int difficulty = 2;
        
        Bomber();
        void move_or_cast(Map &map, int x, int y) override;
};
