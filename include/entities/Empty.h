#pragma once

#include "GameObject.h"

class Empty : public GameObject {
public:
    Empty(int _x, int _y, char _sign, ObjectType _type)
        : GameObject(_x, _y, _sign, _type) {}
};
