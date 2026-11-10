#pragma once

#include "GameObject.h"
#include "Direction.h"
#include <vector>
#include <map>
#include <string>

class Key;
class Door;
class Bomb;
class Switch_class;
class Tile;
class Spring;

class Room {
public:
    static constexpr int WIDTH = 80;
    static constexpr int HEIGHT = 19;

private:
    GameObject* map[HEIGHT][WIDTH];

    std::vector<Key*> keys;
    std::vector<Door*> doors;
    std::vector<Bomb*> activeBombs;
    std::vector<Switch_class*> switches;
    std::vector<Tile*> tiles;
    std::vector<Spring*> springs;

    void remove_tile_from_switches(Tile* t);
    void detach_switch(Switch_class* s);

    bool bomb_exploded = false;
    int exploded_x = -1;
    int exploded_y = -1;
    int exploded_radius = 0;

    bool is_dark = false;

    int player_1_X = 5;
    int player_2_X = 5;
    int player_1_Y = 2;
    int player_2_Y = 2;

    int L_start_x = 0;
    int L_start_y = 22;

    std::map<Door*, int> next_room_doors;

public:
    Room();
    Room(const std::string& filename);

    ~Room();
    Room(const Room& other) = delete;
    Room& operator=(const Room& other) = delete;

    void Draw();
    // NOTE: (row, col) == (y, x)
    GameObject* get_object_by_index(int row, int col);
    const std::vector<Key*>& getKeys() const { return keys; }
    const std::vector<Door*>& getDoors() const { return doors; }

    void set_game_keys();
    void Erase(int _x, int _y);
    void place_object_at(int _x, int _y, GameObject* obj);
    // Moves the object in (from) to (to); (to) must hold an Empty cell.
    void move_object(int from_x, int from_y, int to_x, int to_y);
    bool is_inside(int _x, int _y) const;
    void hideObject(int _y, int _x);

    void addActiveBomb(Bomb* bomb);
    void updateBombs();
    void explodeBomb(Bomb* bomb);
    bool check_bomb_exploded() const { return bomb_exploded; }
    void get_explosion_data(int& x, int& y, int& r) const {
        x = exploded_x;
        y = exploded_y;
        r = exploded_radius;
    }
    void clear_bomb_explosion_flag() { bomb_exploded = false; }

    void set_dark() { is_dark = true; }
    bool isDark() const { return is_dark; }
    bool checkIfTorch(int _y, int _x) const {
        return map[_y][_x]->get_object_type() == ObjectType::TORCH;
    }

    const std::map<Door*, int>& get_next_room_doors() const { return next_room_doors; }

    int get_p1_x() const { return player_1_X; }
    int get_p1_y() const { return player_1_Y; }
    int get_p2_x() const { return player_2_X; }
    int get_p2_y() const { return player_2_Y; }

    int get_L_start_x() const { return L_start_x; }
    int get_L_start_y() const { return L_start_y; }

    int get_spring_size(int row, int col, Direction dir) const;
    void initilize_springs();
    void update_spring_elements(Spring* sp);
};
