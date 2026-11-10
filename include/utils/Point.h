#pragma once

#include <utility>
#include <cmath>
#include "Direction.h"

class Point {
    int x = 0;
    int y = 0;
    int diff_x = 0;
    int diff_y = 0;
    char ch = '*';
    static constexpr int MAX_X = 79;
    static constexpr int MAX_Y = 18;

public:
    Point() = default;
    Point(int x1, int y1, int dirx1, int diry1, char c)
        : x(x1), y(y1), diff_x(dirx1), diff_y(diry1), ch(c) {}

    void draw() const { draw(ch); }
    void draw(char c) const;
    void move();
    void setDirection(Direction dir);

    int getDiff_X() const { return diff_x; }
    int getDiff_Y() const { return diff_y; }
    int get_X() const { return x; }
    int get_Y() const { return y; }

    void set_diff(int _diff_x, int _diff_y) {
        diff_x = _diff_x;
        diff_y = _diff_y;
    }
    void set_point_cords(int _x, int _y);
};
