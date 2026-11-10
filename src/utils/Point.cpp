#include "Point.h"
#include "utils.h"
#include <iostream>

void Point::draw(char c) const {
    gotoxy(x, y);
    std::cout << c;
}

void Point::move() {
    if (x + diff_x < 0 || x + diff_x > MAX_X) {
        diff_x = -diff_x;
    }
    if (y + diff_y < 0 || y + diff_y > MAX_Y) {
        diff_y = -diff_y;
    }
    x += diff_x;
    y += diff_y;
}

void Point::setDirection(Direction dir) {
    switch (dir) {
    case Direction::UP:
        diff_x = 0;
        diff_y = -1;
        break;
    case Direction::RIGHT:
        diff_x = 1;
        diff_y = 0;
        break;
    case Direction::DOWN:
        diff_x = 0;
        diff_y = 1;
        break;
    case Direction::LEFT:
        diff_x = -1;
        diff_y = 0;
        break;
    case Direction::STAY:
        diff_x = 0;
        diff_y = 0;
        break;
    }
}

void Point::set_point_cords(int _x, int _y) {
    x = _x;
    y = _y;
}
