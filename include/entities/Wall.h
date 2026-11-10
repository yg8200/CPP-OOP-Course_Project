#pragma once

#include "GameObject.h"

class Wall : public GameObject {
public:
    Wall(int _x, int _y, char _sign, ObjectType _type)
        : GameObject(_x, _y, _sign, _type) {}
};
