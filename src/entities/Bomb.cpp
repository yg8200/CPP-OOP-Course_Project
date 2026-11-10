#include "Bomb.h"

void Bomb::setRoom(Room* room) {
    current_room = room;
}

void Bomb::activate(int x, int y) {
    activated = true;
    posX = x;
    posY = y;
    countdown = 5;
    slowCounter = 0;
}

void Bomb::tick() {
    if (!activated)
        return;

    slowCounter++;
    if (slowCounter >= 3) {
        if (countdown > 0)
            countdown--;
        slowCounter = 0;
    }
}
