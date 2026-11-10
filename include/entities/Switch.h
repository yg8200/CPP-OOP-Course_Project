#pragma once

#include "GameObject.h"

class Tile;

class Switch_class : public GameObject {
private:
    bool pressed = false;
    Tile* tile = nullptr;

public:
    Switch_class(int _x, int _y, char _sign, ObjectType _type)
        : GameObject(_x, _y, _sign, _type) {}

    void pressSwitch();

    bool get_switch_state() const;
    void set_tile(Tile* t);
    Tile* get_tile() const;
};
