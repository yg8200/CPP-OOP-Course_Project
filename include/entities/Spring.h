#pragma once

#include "Direction.h"

class Room;

class Spring {
private:
    int start_row = -1;
    int end_row = -1;
    int start_col = -1;
    int end_col = -1;
    Direction direction = Direction::STAY;

public:
    Spring() = default;
    Spring(int _start_row, int _end_row, int _start_col, int _end_col, Direction _dir)
        : start_row(_start_row), end_row(_end_row),
          start_col(_start_col), end_col(_end_col), direction(_dir) {}

    void draw() const;
    int get_start_row() const { return start_row; }
    int get_end_row() const { return end_row; }
    int get_start_col() const { return start_col; }
    int get_end_col() const { return end_col; }
    Direction get_dir() const { return direction; }

    int get_shrinked_elements_count(Room* r) const;
    bool can_pass(Direction player_direction) const;
    Direction get_release_direction() const;
    void restore_spring(Room* r) const;
    void clean_spring(Room* r);
};
