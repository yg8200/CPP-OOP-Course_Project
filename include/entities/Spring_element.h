#pragma once

#include "GameObject.h"
#include "Direction.h"
#include "Spring.h"

class SpringElement : public GameObject {
private:
    Spring* owning_spring = nullptr;
    bool shrinked = false;

public:
    SpringElement(int _x, int _y, char _sign, ObjectType _type)
        : GameObject(_x, _y, _sign, _type) {}

    void set_owning_spring(Spring* sp) { owning_spring = sp; }
    Spring* get_owning_spring() const { return owning_spring; }
    bool get_shrinked() const { return shrinked; }
    void set_shrinked(bool sh);
};
