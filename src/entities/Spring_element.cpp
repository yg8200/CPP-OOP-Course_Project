#include "Spring_element.h"
#include "utils.h"

void SpringElement::set_shrinked(bool sh) {
    if (shrinked && !sh) {
        gotoxy(get_x(), get_y());
        printColored(getSign(), getColor());

        shrinked = false;
    } else {
        shrinked = sh;
    }
}
