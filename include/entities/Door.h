#pragma once

#include "GameObject.h"
#include <vector>

class Room;
class Key;

class Door : public GameObject {
    Room* targetRoom = nullptr;
    std::vector<Key*> keys;
    int room_target_number = 0;

public:
    Door(int _x, int _y, char _sign, ObjectType _type)
        : GameObject(_x, _y, _sign, _type) {}

    void set_target_room(Room* room) { targetRoom = room; }
    std::vector<Key*>& getKeys() { return keys; }

    bool already_in_keys(Key* key) const;
    bool is_opened() const { return keys.empty(); }
    bool use_key(Key* key);
    void removeKey(Key* key);

    Room* get_target_room() const { return targetRoom; }
    int get_room_target_number() const { return room_target_number; }
    void set_room_target_number(int num) { room_target_number = num; }
};
