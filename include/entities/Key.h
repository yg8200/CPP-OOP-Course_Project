#pragma once

#include "GameObject.h"

class Key : public GameObject {
public:
    Key(int _x, int _y, char _sign, ObjectType _type)
        : GameObject(_x, _y, _sign, _type) {}
};
