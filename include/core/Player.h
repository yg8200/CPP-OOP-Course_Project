#pragma once

#include <cstring>
#include <cstddef>
#include "Point.h"
#include "Room.h"
#include "Results.h"

enum class player_mode { REGULAR, RECORDING, PLAYBACK };

class Player {
    static constexpr int NUM_KEYS = 6; // Up, Right, Down, Left, Stay, Dispose
    Point body;
    char keys[NUM_KEYS] = {};
    Room* current_room = nullptr;
    GameObject* collected = nullptr;
    Direction last_dir = Direction::STAY;
    Direction current_dir = Direction::STAY;
    bool moved_room = false;
    int move_to = 0;
    int power = 1;
    int start_up_cord_X = 0;
    int start_up_cord_Y = 0;

    // Torch state
    bool has_torch = false;
    Point last_torch_pos;
    bool torch_active = false;
    int torch_radius = 1;

    // Player lives and score
    int lives = 5;
    int score = 0;
    bool life_changed = false;
    bool score_changed = false;

    // Spring physics
    int acceleration = 1;
    int spring_acceleration_cycles = 0;

    player_mode mode = player_mode::REGULAR;
    bool silent_playback = false;
    Results* results = nullptr;

public:
    Player() = default;
    Player(const Point& point, const char(&the_keys)[NUM_KEYS + 1], player_mode _mode, bool _silent_playback)
        : body(point), mode(_mode), silent_playback(_silent_playback) {
        std::memcpy(keys, the_keys, NUM_KEYS * sizeof(keys[0]));
    }

    void draw();
    void move(size_t iteration);

    void set_mode(player_mode _mode) { mode = _mode; }
    void set_results_obj(Results* _results) { results = _results; }

    int getX() const { return body.get_X(); }
    int getY() const { return body.get_Y(); }
    void set_player_room(Room* room) { current_room = room; }
    void setCollected(GameObject* obj) { collected = obj; }
    GameObject* getCollected() const { return collected; }
    Room* get_current_room() const { return current_room; }
    void set_last_direction(Direction d) { last_dir = d; }
    void set_moved_room(bool m) { moved_room = m; }
    bool get_moved_room() const { return moved_room; }
    void set_player_cords(int _x, int _y) { body.set_point_cords(_x, _y); }
    void set_diff_to_0() { body.set_diff(0, 0); }
    Direction get_current_dir() const { return current_dir; }

    int get_start_cord_X() const { return start_up_cord_X; }
    int get_start_cord_Y() const { return start_up_cord_Y; }
    void set_start_cords(int _x, int _y) {
        start_up_cord_X = _x;
        start_up_cord_Y = _y;
    }

    int getLives() const { return lives; }
    int getScore() const { return score; }
    void loseLife(int life_lost) {
        lives = (lives > life_lost) ? lives - life_lost : 0;
        life_changed = true;
    }
    void addScore(int amount) {
        score += amount;
        score_changed = true;
    }
    void remove_score(int amount) {
        if (score > 0) {
            score = (score > amount) ? score - amount : 0;
            score_changed = true;
        }
    }
    bool check_life_changed() const { return life_changed; }
    bool check_score_changed() const { return score_changed; }
    void clear_life_flag() { life_changed = false; }
    void clear_score_flag() { score_changed = false; }
    int get_room_to_move_to() const { return move_to; }

    void set_acceleration(int acc) { acceleration = acc; }
    void set_spring_acceleration_cycles(int cycles) { spring_acceleration_cycles = cycles; }
    int get_accleration() const { return acceleration; }
    int get_spring_accelration_cycles() const { return spring_acceleration_cycles; }
    void reset_acceleration();

    void handleKeyPressed(char key);
    void handle_move(size_t iteration, GameObject* obj, Point body_original);

    bool player_stopped() const {
        return (body.getDiff_X() == 0) && (body.getDiff_Y() == 0);
    }

    void dispose_item();
    bool dispose_conditions(int _drop_x, int _drop_y);
    bool try_other_direction(int& dx, int& dy);

    bool hasTorch() const { return has_torch; }
    void clear_torch_radius();
    void draw_torch_radius();
    int getTorchRadius() const { return torch_radius; }
    void draw_torch_aura_at(int cx, int cy);
    bool is_key_match(const char k) const;
};
