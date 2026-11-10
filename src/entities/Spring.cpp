#include "Spring.h"
#include "Spring_element.h"
#include "Room.h"
#include "utils.h"
#include <iostream>

void Spring::draw() const {
    // Elements are drawn individually as SpringElement instances on the room map
}

int Spring::get_shrinked_elements_count(Room* r) const {
    int count_shrinked = 0;
    switch (direction) {
    case Direction::RIGHT:
    case Direction::DOWN:
        for (int i = start_row; i <= end_row; i++) {
            for (int j = start_col; j <= end_col; j++) {
                SpringElement* spe = static_cast<SpringElement*>(r->get_object_by_index(i, j));
                if (spe->get_shrinked())
                    count_shrinked++;
            }
        }
        break;
    case Direction::UP:
    case Direction::LEFT:
        for (int i = start_row; i >= end_row; i--) {
            for (int j = start_col; j >= end_col; j--) {
                SpringElement* spe = static_cast<SpringElement*>(r->get_object_by_index(i, j));
                if (spe->get_shrinked())
                    count_shrinked++;
            }
        }
        break;
    case Direction::STAY:
        break;
    }

    return count_shrinked;
}

bool Spring::can_pass(Direction player_direction) const {
    return player_direction == direction;
}

Direction Spring::get_release_direction() const {
    switch (direction) {
    case Direction::UP:    return Direction::DOWN;
    case Direction::DOWN:  return Direction::UP;
    case Direction::RIGHT: return Direction::LEFT;
    case Direction::LEFT:  return Direction::RIGHT;
    default:               return Direction::STAY;
    }
}

void Spring::restore_spring(Room* r) const {
    switch (direction) {
    case Direction::RIGHT:
    case Direction::DOWN:
        for (int i = start_row; i <= end_row; i++) {
            for (int j = start_col; j <= end_col; j++) {
                SpringElement* spe = static_cast<SpringElement*>(r->get_object_by_index(i, j));
                if (spe->get_shrinked())
                    spe->set_shrinked(false);
            }
        }
        break;
    case Direction::UP:
    case Direction::LEFT:
        for (int i = start_row; i >= end_row; i--) {
            for (int j = start_col; j >= end_col; j--) {
                SpringElement* spe = static_cast<SpringElement*>(r->get_object_by_index(i, j));
                if (spe->get_shrinked())
                    spe->set_shrinked(false);
            }
        }
        break;
    case Direction::STAY:
        break;
    }
}

void Spring::clean_spring(Room* r) {
    switch (direction) {
    case Direction::RIGHT:
    case Direction::DOWN:
        for (int i = start_row; i <= end_row; i++) {
            for (int j = start_col; j <= end_col; j++) {
                r->Erase(j, i);
                gotoxy(j, i);
                std::cout << ' ';
            }
        }
        break;
    case Direction::UP:
    case Direction::LEFT:
        for (int i = start_row; i >= end_row; i--) {
            for (int j = start_col; j >= end_col; j--) {
                r->Erase(j, i);
                gotoxy(j, i);
                std::cout << ' ';
            }
        }
        break;
    case Direction::STAY:
        break;
    }
}
