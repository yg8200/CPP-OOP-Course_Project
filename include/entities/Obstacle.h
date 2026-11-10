#pragma once

#include "GameObject.h"
#include "Direction.h"

class Room;

class Obstacle : public GameObject {
private:
    static constexpr int power = 1;

public:
    Obstacle(int _x, int _y, char _sign, ObjectType _type)
        : GameObject(_x, _y, _sign, _type) {}

    int move_cost(Direction d, Room* r);
    void move(Direction d, Room* r);
    void draw() const;
};
