#pragma once

#include "GameObject.h"

class Room;

class Bomb : public GameObject {
private:
    Room* current_room = nullptr;
    bool activated = false;
    int countdown = 5;
    int explosionRadius = 2;
    int posX = 0;
    int posY = 0;
    int slowCounter = 0;

public:
    Bomb(int _x, int _y, char _sign, ObjectType _type)
        : GameObject(_x, _y, _sign, _type), posX(_x), posY(_y) {}

    void setRoom(Room* room);
    Room* getRoom() const { return current_room; }

    void activate(int x, int y);
    bool isActivated() const { return activated; }

    void tick();

    int getCountdown() const { return countdown; }
    bool shouldExplode() const { return countdown <= 0 && activated; }
    int getPosX() const { return posX; }
    int getPosY() const { return posY; }
    int getRadius() const { return explosionRadius; }
};
