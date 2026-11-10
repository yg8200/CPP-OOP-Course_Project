#include "Player.h"
#include "Bomb.h"
#include "Door.h"
#include "Key.h"
#include "Riddle.h"
#include "Switch.h"
#include "Tile.h"
#include "Torch.h"
#include "Obstacle.h"
#include "Spring_element.h"
#include "Spring.h"
#include "utils.h"
#include <iostream>
#include <cctype>

void Player::draw() {
    body.draw();
}

void Player::move(size_t iteration) {
    if (moved_room)
        return;

    if (body.getDiff_X() == 0 && body.getDiff_Y() == 0) {
        GameObject* go = current_room->get_object_by_index(body.get_Y(), body.get_X());

        if ((go->get_object_type() == ObjectType::SPRING_ELEMENT) && (acceleration == 1)) {
            SpringElement* spe = static_cast<SpringElement*>(go);
            Spring* spring = spe->get_owning_spring();
            if (spring == nullptr)
                return;
            int shrinked_count = spring->get_shrinked_elements_count(current_room);
            if (shrinked_count == 0)
                return; // nothing is compressed - nothing to release (avoids acceleration == 0 freeze)
            current_dir = spring->get_release_direction();
            body.setDirection(current_dir);
            acceleration = shrinked_count;
            spring_acceleration_cycles = (acceleration * acceleration);
            power = acceleration;
        } else {
            return;
        }
    } else {
        if (spring_acceleration_cycles > 0) {
            spring_acceleration_cycles--;
            if (spring_acceleration_cycles == 0) {
                reset_acceleration();
            }
        }
    }

    Point body_original = body;
    for (int i = acceleration; i > 0 && !moved_room; i--) {
        body_original = body;
        if (!silent_playback)
            body.draw(' ');
        body.move();
        GameObject* obj = current_room->get_object_by_index(body.get_Y(), body.get_X());
        handle_move(iteration, obj, body_original);
    }

    GameObject* prev_go = current_room->get_object_by_index(body_original.get_Y(), body_original.get_X());
    GameObject* current_go = current_room->get_object_by_index(body.get_Y(), body.get_X());

    if (prev_go->get_object_type() == ObjectType::SPRING_ELEMENT &&
        current_go->get_object_type() != ObjectType::SPRING_ELEMENT) {
        SpringElement* prev_spe = static_cast<SpringElement*>(prev_go);
        if (prev_spe->get_owning_spring() != nullptr)
            prev_spe->get_owning_spring()->restore_spring(current_room);
    }

    if (current_room->isDark() && has_torch && !moved_room) {
        if (torch_active)
            clear_torch_radius();

        if (!silent_playback)
            draw_torch_radius();
        torch_active = true;
    }
}

void Player::reset_acceleration() {
    acceleration = 1;
    power = 1;
    spring_acceleration_cycles = 0;
}

void Player::handleKeyPressed(char key_pressed) {
    if (std::tolower(static_cast<unsigned char>(key_pressed)) ==
        std::tolower(static_cast<unsigned char>(keys[NUM_KEYS - 1]))) {
        dispose_item();
        return;
    }

    size_t index = 0;
    for (char k : keys) {
        if (std::tolower(static_cast<unsigned char>(k)) ==
            std::tolower(static_cast<unsigned char>(key_pressed))) {
            Direction dir = static_cast<Direction>(index);
            if (dir != Direction::STAY)
                last_dir = dir;

            current_dir = dir;
            body.setDirection(dir);
            return;
        }
        ++index;
    }
}

void Player::handle_move(size_t iteration, GameObject* obj, Point body_original) {
    switch (obj->get_object_type()) {
    case ObjectType::WALL:
        body = body_original;
        body.set_diff(0, 0);
        if (!silent_playback)
            body.draw();
        reset_acceleration();
        break;

    case ObjectType::EMPTY:
        if (!silent_playback)
            body.draw();
        break;

    case ObjectType::KEY:
        if (collected == nullptr) {
            collected = obj;
            current_room->hideObject(body.get_Y(), body.get_X());
        } else {
            body = body_original;
            body.set_diff(0, 0);
            reset_acceleration();
        }
        if (!silent_playback)
            body.draw();
        break;

    case ObjectType::DOOR: {
        Door* door = static_cast<Door*>(obj);
        if (collected != nullptr && collected->get_object_type() == ObjectType::KEY) {
            bool key_used = door->use_key(static_cast<Key*>(collected));
            if (key_used) {
                delete collected;
                collected = nullptr;
            }
        }

        if (door->is_opened()) {
            if (door->get_target_room() != nullptr) {
                if (current_room->isDark() && has_torch)
                    clear_torch_radius();

                current_room = door->get_target_room();
                if (collected != nullptr && collected->get_object_type() == ObjectType::BOMB) {
                    Bomb* b = static_cast<Bomb*>(collected);
                    b->setRoom(current_room);
                }

                moved_room = true;
                move_to = door->get_room_target_number();
                body = body_original;
                if (!silent_playback)
                    body.draw(' ');
                reset_acceleration();
                break;
            } else {
                current_room->Erase(body.get_X(), body.get_Y());
                if (!silent_playback)
                    body.draw();
            }
        } else {
            body = body_original;
            if (!silent_playback)
                body.draw();
            reset_acceleration();
        }
        body.set_diff(0, 0);
        break;
    }

    case ObjectType::BOMB: {
        Bomb* bomb = static_cast<Bomb*>(obj);

        if (bomb->isActivated()) {
            body = body_original;
            body.set_diff(0, 0);
            if (!silent_playback)
                body.draw();
            reset_acceleration();
            break;
        }

        if (collected == nullptr) {
            collected = obj;
            current_room->hideObject(body.get_Y(), body.get_X());
        } else {
            body = body_original;
            body.set_diff(0, 0);
            reset_acceleration();
        }
        if (!silent_playback)
            body.draw();
        break;
    }

    case ObjectType::SPRING_ELEMENT: {
        SpringElement* spe = static_cast<SpringElement*>(obj);
        if (acceleration == 1) {
            Spring* spring = spe->get_owning_spring();
        if (spring != nullptr && !spring->can_pass(current_dir)) {
                body = body_original;
                body.set_diff(0, 0);
            } else {
                spe->set_shrinked(true);
            }
        }

        if (!silent_playback)
            body.draw();
        break;
    }

    case ObjectType::OBSTACLE: {
        Obstacle* obs = static_cast<Obstacle*>(obj);
        int required_power = obs->move_cost(current_dir, current_room);
        if (required_power == 0 || power < required_power) {
            body = body_original;
            body.set_diff(0, 0);
            if (!silent_playback)
                body.draw();
            reset_acceleration();
            break;
        }
        obs->move(current_dir, current_room);
        if (!silent_playback)
            body.draw();
        obs->draw();
        break;
    }

    case ObjectType::RIDDLE: {
        Riddle* r = static_cast<Riddle*>(obj);
        int speed = 800;
        int playback_answer = -1;

        if (mode == player_mode::PLAYBACK) {
            event* e = (results != nullptr) ? results->get_event(iteration) : nullptr;
            playback_answer = (e != nullptr) ? e->type_specific_parameter : 0;
            speed = silent_playback ? 0 : 100;
        }

        int correct = r->ask(playback_answer, speed);

        if (!silent_playback)
            current_room->Draw();

        if (mode == player_mode::RECORDING && results != nullptr) {
            if (correct < 10)
                results->add_event(iteration, result_type::RIDDLE_SUCCESS, correct);
            else
                results->add_event(iteration, result_type::RIDDLE_FAILURE, correct - 10);
        }

        if (correct < 10) {
            current_room->Erase(body.get_X(), body.get_Y());
            if (!silent_playback)
                body.draw();
            addScore(100);
        } else {
            body = body_original;
            if (!silent_playback)
                body.draw();
            reset_acceleration();
            remove_score(50);
            loseLife(1);
        }
        body.set_diff(0, 0);
        break;
    }

    case ObjectType::TORCH: {
        if (collected == nullptr) {
            has_torch = true;
            collected = obj;
            torch_active = false;
            current_room->hideObject(body.get_Y(), body.get_X());
        } else {
            body = body_original;
            body.set_diff(0, 0);
            reset_acceleration();
        }

        if (!silent_playback)
            body.draw();
        break;
    }

    case ObjectType::SWITCH: {
        Switch_class* s = static_cast<Switch_class*>(obj);
        s->pressSwitch();
        body = body_original;
        if (!silent_playback)
            body.draw();
        body.set_diff(0, 0);
        reset_acceleration();
        break;
    }

    case ObjectType::TILE: {
        Tile* t = static_cast<Tile*>(obj);
        if (t->get_visible()) {
            body = body_original;
            body.set_diff(0, 0);
            reset_acceleration();
        }
        if (!silent_playback)
            body.draw();
        break;
    }
    }
}

bool Player::dispose_conditions(int _dropX, int _dropY) {
    if (collected == nullptr || !player_stopped())
        return false;

    if (!current_room->is_inside(_dropX, _dropY))
        return false;

    GameObject* obj_ahead = current_room->get_object_by_index(_dropY, _dropX);

    return obj_ahead->get_object_type() == ObjectType::EMPTY;
}

bool Player::try_other_direction(int& dx, int& dy) {
    if (last_dir == Direction::UP || last_dir == Direction::DOWN) {
        dx = -1; dy = 0;
        if (dispose_conditions(body.get_X() + dx, body.get_Y() + dy))
            return true;
        dx = 1; dy = 0;
        return dispose_conditions(body.get_X() + dx, body.get_Y() + dy);
    }

    if (last_dir == Direction::LEFT || last_dir == Direction::RIGHT) {
        dx = 0; dy = -1;
        if (dispose_conditions(body.get_X() + dx, body.get_Y() + dy))
            return true;
        dx = 0; dy = 1;
        return dispose_conditions(body.get_X() + dx, body.get_Y() + dy);
    }

    return false;
}

void Player::dispose_item() {
    int dx = 0, dy = 0;

    switch (last_dir) {
    case Direction::RIGHT: dx = 1; break;
    case Direction::LEFT:  dx = -1; break;
    case Direction::UP:    dy = -1; break;
    case Direction::DOWN:  dy = 1; break;
    default: return;
    }

    int dropX = body.get_X() + dx;
    int dropY = body.get_Y() + dy;

    if (!dispose_conditions(dropX, dropY)) {
        if (!try_other_direction(dx, dy))
            return;

        dropX = body.get_X() + dx;
        dropY = body.get_Y() + dy;

        if (!dispose_conditions(dropX, dropY))
            return;
    }

    switch (collected->get_object_type()) {
    case ObjectType::KEY: {
        current_room->place_object_at(dropX, dropY, collected);
        break;
    }
    case ObjectType::BOMB: {
        Bomb* bomb = static_cast<Bomb*>(collected);
        bomb->activate(dropX, dropY);
        current_room->place_object_at(dropX, dropY, collected);
        current_room->addActiveBomb(bomb);

        gotoxy(dropX, dropY);
        std::cout << '5';
        break;
    }
    case ObjectType::TORCH: {
        has_torch = false;
        torch_active = false;
        if (current_room->isDark() && !silent_playback)
            clear_torch_radius();
        current_room->place_object_at(dropX, dropY, collected);
        if (current_room->isDark() && !silent_playback) {
            draw_torch_aura_at(dropX, dropY);
        }
        break;
    }
    default:
        current_room->place_object_at(dropX, dropY, collected);
        gotoxy(dropX, dropY);
        std::cout << collected->getSign();
        std::cout.flush();
        break;
    }
    collected = nullptr;
}

void Player::clear_torch_radius() {
    int px = last_torch_pos.get_X();
    int py = last_torch_pos.get_Y();

    for (int dy = -3; dy <= 3; dy++) {
        for (int dx = -3; dx <= 3; dx++) {
            int x = px + dx;
            int y = py + dy;

            if (current_room->is_inside(x, y)) {
                if (!current_room->checkIfTorch(y, x)) {
                    gotoxy(x, y);
                    GameObject* obj = current_room->get_object_by_index(y, x);
                    obj->removeLight();

                    if (!obj->get_shown()) {
                        gotoxy(x, y);
                        std::cout << ' ';
                    }
                }
            }
        }
    }
}

void Player::draw_torch_radius() {
    int px = body.get_X();
    int py = body.get_Y();

    for (int dy = -3; dy <= 3; dy++) {
        for (int dx = -3; dx <= 3; dx++) {
            int x = px + dx;
            int y = py + dy;

            if (!current_room->is_inside(x, y))
                continue;

            GameObject* obj = current_room->get_object_by_index(y, x);

            gotoxy(x, y);
            if (obj->get_object_type() == ObjectType::TILE) {
                Tile* t = static_cast<Tile*>(obj);
                if (!t->get_visible()) {
                    std::cout << ' ';
                    continue;
                }
            }
            obj->addLight();
            printColored(obj->getSign(), obj->getColor());
        }
    }

    last_torch_pos = body;
}

void Player::draw_torch_aura_at(int cx, int cy) {
    for (int dy = -3; dy <= 3; dy++) {
        for (int dx = -3; dx <= 3; dx++) {
            int x = cx + dx;
            int y = cy + dy;

            if (!current_room->is_inside(x, y))
                continue;

            GameObject* obj = current_room->get_object_by_index(y, x);
            obj->set_shown(true);
            gotoxy(x, y);

            if (obj->get_object_type() == ObjectType::TILE) {
                Tile* t = static_cast<Tile*>(obj);
                if (!t->get_visible()) {
                    std::cout << ' ';
                    continue;
                }
            }

            printColored(obj->getSign(), obj->getColor());
        }
    }
}

bool Player::is_key_match(const char k) const {
    // Must agree with handleKeyPressed(), which is case-insensitive; otherwise a
    // key accepted by the game (e.g. with Caps Lock) would not be recorded.
    const int lower = std::tolower(static_cast<unsigned char>(k));
    for (const auto& key : keys) {
        if (std::tolower(static_cast<unsigned char>(key)) == lower)
            return true;
    }
    return false;
}
