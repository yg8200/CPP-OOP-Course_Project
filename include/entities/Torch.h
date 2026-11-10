#pragma once

#include "GameObject.h"

class Torch : public GameObject {
public:
    Torch(int _x, int _y, char _sign, ObjectType _type)
        : GameObject(_x, _y, _sign, _type) {}
};
