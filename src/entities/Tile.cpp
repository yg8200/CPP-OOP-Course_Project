#include "Tile.h"
#include "Switch.h"
#include "utils.h"
#include <algorithm>
#include <iostream>

void Tile::set_visable(bool state) {
    visable = state;
}

bool Tile::get_visible() const {
    return visable;
}

void Tile::add_to_switch_vector(Switch_class* s) {
    switches.push_back(s);
}

void Tile::remove_switch(Switch_class* s) {
    switches.erase(std::remove(switches.begin(), switches.end(), s), switches.end());
}

bool Tile::all_pressed() const {
    for (const auto& s : switches) {
        if (!s->get_switch_state())
            return false;
    }
    return true;
}

void Tile::tile_conditions() {
    if (all_pressed()) {
        visable = false;
        gotoxy(get_x(), get_y());
        std::cout << ' ';
    } else {
        visable = true;
        gotoxy(get_x(), get_y());
        printColored('-', Color::YELLOW);
    }
}
