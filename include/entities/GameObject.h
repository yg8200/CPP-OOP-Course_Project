#pragma once

#include "ObjectTypes.h"
#include "colors.h"

// Base class of all game objects
class GameObject {
    int x = 0;
    int y = 0;
    char sign = ' ';
    bool shown_in_map = false;
    int light_sources = 0;
    Color color = Color::DEFAULT;
    ObjectType type = ObjectType::EMPTY;

public:
    GameObject(int _x = 0, int _y = 0, char _sign = ' ', ObjectType _type = ObjectType::EMPTY)
        : x(_x), y(_y), sign(_sign), type(_type) {}
    virtual ~GameObject() = default;

    char getSign() const { return sign; }
    int get_x() const { return x; }
    int get_y() const { return y; }
    ObjectType get_object_type() const { return type; }

    void setColor(Color c) { color = c; }
    Color getColor() const { return color; }

    void set_sign(char s) { sign = s; }
    void set_location(int _x, int _y) { x = _x; y = _y; }

    void set_shown(bool sh) { shown_in_map = sh; }
    bool get_shown() const { return shown_in_map; }

    void addLight() {
        light_sources++;
        shown_in_map = true;
    }

    void removeLight() {
        if (light_sources > 0)
            light_sources--;
        if (light_sources == 0)
            shown_in_map = false;
    }
};
