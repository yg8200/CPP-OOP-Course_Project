#pragma once

#include "GameObject.h"
#include <vector>

class Switch_class;

class Tile : public GameObject {
private:
    std::vector<Switch_class*> switches;
    bool visable = true;

public:
    Tile(int _x, int _y, char _sign, ObjectType _type)
        : GameObject(_x, _y, _sign, _type) {}

    void set_visable(bool state);
    bool get_visible() const;

    void add_to_switch_vector(Switch_class* s);
    void remove_switch(Switch_class* s);
    bool all_pressed() const;
    void tile_conditions();
};
