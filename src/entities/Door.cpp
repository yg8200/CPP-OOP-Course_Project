#include "Door.h"
#include <cstddef>

bool Door::already_in_keys(Key* key) const {
    for (const auto& k : keys) {
        if (k == key)
            return true;
    }
    return false;
}

bool Door::use_key(Key* key) {
    for (const auto& k : keys) {
        if (k == key) {
            removeKey(key);
            return true;
        }
    }
    return false;
}

void Door::removeKey(Key* key) {
    for (std::size_t i = 0; i < keys.size(); i++) {
        if (keys[i] == key) {
            keys.erase(keys.begin() + static_cast<std::ptrdiff_t>(i));
            break;
        }
    }
}
