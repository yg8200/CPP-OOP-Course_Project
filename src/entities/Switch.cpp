#include "Switch.h"
#include "Tile.h"
#include "utils.h"

void Switch_class::pressSwitch() {
    if (!pressed) {
        pressed = true;
        set_sign('\\');
        gotoxy(get_x(), get_y());
        printColored('\\', Color::CYAN);
    } else {
        pressed = false;
        set_sign('/');
        gotoxy(get_x(), get_y());
        printColored('/', Color::CYAN);
    }
    if (tile != nullptr) {
        tile->tile_conditions();
    }
}

bool Switch_class::get_switch_state() const {
    return pressed;
}

void Switch_class::set_tile(Tile* t) {
    tile = t;
}

Tile* Switch_class::get_tile() const {
    return tile;
}
