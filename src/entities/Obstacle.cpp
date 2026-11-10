#include "Obstacle.h"
#include "Room.h"
#include "utils.h"
#include <iostream>

int Obstacle::move_cost(Direction d, Room* r) {
    int power_required = this->power;
    int next_x = this->get_x();
    int next_y = this->get_y();

    switch (d) {
    case Direction::UP:
        next_y = this->get_y() - 1;
        break;
    case Direction::DOWN:
        next_y = this->get_y() + 1;
        break;
    case Direction::LEFT:
        next_x = this->get_x() - 1;
        break;
    case Direction::RIGHT:
        next_x = this->get_x() + 1;
        break;
    case Direction::STAY:
        return 0;
    }

    if (!r->is_inside(next_x, next_y))
        return 0;

    GameObject* obj = r->get_object_by_index(next_y, next_x);

    switch (obj->get_object_type()) {
    case ObjectType::EMPTY:
        return power_required;
    case ObjectType::OBSTACLE: {
        Obstacle* obs = static_cast<Obstacle*>(obj);
        int next_cost = obs->move_cost(d, r);
        return (next_cost != 0) ? (power_required + next_cost) : 0;
    }
    default:
        return 0;
    }
}

void Obstacle::move(Direction d, Room* r) {
    int next_x = get_x();
    int next_y = get_y();

    switch (d) {
    case Direction::UP:    next_y--; break;
    case Direction::DOWN:  next_y++; break;
    case Direction::LEFT:  next_x--; break;
    case Direction::RIGHT: next_x++; break;
    default: return;
    }

    if (!r->is_inside(next_x, next_y))
        return;

    GameObject* obj = r->get_object_by_index(next_y, next_x);

    // Push the obstacle in front of us first, then re-check that the cell is free.
    if (obj->get_object_type() == ObjectType::OBSTACLE) {
        static_cast<Obstacle*>(obj)->move(d, r);
        obj = r->get_object_by_index(next_y, next_x);
    }
    if (obj->get_object_type() != ObjectType::EMPTY)
        return;

    r->move_object(get_x(), get_y(), next_x, next_y);
}

void Obstacle::draw() const {
    gotoxy(this->get_x(), this->get_y());
    std::cout << this->getSign();
}
