#include "Room.h"
#include "Wall.h"
#include "Key.h"
#include "Empty.h"
#include "Door.h"
#include "Torch.h"
#include "Bomb.h"
#include "Riddle.h"
#include "Riddlers.h"
#include "Switch.h"
#include "Tile.h"
#include "Obstacle.h"
#include "Spring.h"
#include "Spring_element.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <algorithm>

namespace {
    constexpr int SCREEN_HEIGHT = 25; // Full console screen (0-24)
}

Room::Room() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            map[i][j] = nullptr;
        }
    }
}

Room::Room(const std::string& filename) {
    bool has_errors = false;

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            map[y][x] = nullptr;
        }
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "\n========================================" << std::endl;
        std::cerr << "ERROR: Cannot open room file: " << filename << std::endl;
        std::cerr << "The file may not exist or is not accessible." << std::endl;
        std::cerr << "Creating empty room as fallback." << std::endl;
        std::cerr << "========================================\n" << std::endl;

        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                map[y][x] = new Empty(x, y, ' ', ObjectType::EMPTY);
            }
        }
        readKey();
        return;
    }

    std::string line;
    int player1_count = 0;
    int player2_count = 0;

    // Read the game board (first 19 lines)
    for (int y = 0; y < HEIGHT; y++) {
        if (!std::getline(file, line)) {
            std::cerr << "\n========================================" << std::endl;
            std::cerr << "WARNING: File " << filename << " ended early at line " << (y + 1) << std::endl;
            std::cerr << "Expected " << HEIGHT << " lines, filling remaining with empty spaces." << std::endl;
            std::cerr << "========================================\n" << std::endl;
            has_errors = true;

            for (; y < HEIGHT; y++) {
                for (int x = 0; x < WIDTH; x++) {
                    if (map[y][x] == nullptr)
                        map[y][x] = new Empty(x, y, ' ', ObjectType::EMPTY);
                }
            }
            break;
        }

        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.length() >= 11) {
            char first_char = line[0];
            if ((first_char == 'K' || first_char == 'S' || first_char == 'D' ||
                 first_char == 'L' || first_char == 'R' || first_char == 'T') &&
                line[5] == ' ') {
                std::cerr << "\n========================================" << std::endl;
                std::cerr << "WARNING: Configuration line detected at line " << (y + 1) << std::endl;
                std::cerr << "Expected game board content, but found config line." << std::endl;
                std::cerr << "Line content: \"" << line << "\"" << std::endl;
                std::cerr << "ATTENTION: Game board is incomplete! Only " << y << " lines of game board found." << std::endl;
                std::cerr << "Expected " << HEIGHT << " lines of game board before configuration." << std::endl;
                std::cerr << "========================================\n" << std::endl;
                has_errors = true;

                for (; y < HEIGHT; y++) {
                    for (int x = 0; x < WIDTH; x++) {
                        if (map[y][x] == nullptr)
                            map[y][x] = new Empty(x, y, ' ', ObjectType::EMPTY);
                    }
                }
                break;
            }
        }

        if (line.length() > static_cast<size_t>(WIDTH)) {
            std::cerr << "\n========================================" << std::endl;
            std::cerr << "WARNING: Line " << (y + 1) << " in " << filename
                      << " is too long (" << line.length() << " chars). "
                      << "Truncating to " << WIDTH << " characters." << std::endl;
            std::cerr << "========================================\n" << std::endl;
            has_errors = true;
        }

        for (int x = 0; x < WIDTH; x++) {
            char c = (static_cast<size_t>(x) < line.length()) ? line[x] : ' ';

            switch (c) {
            case 'W': {
                Wall* w = new Wall(x, y, 'W', ObjectType::WALL);
                w->setColor(Color::BLUE);
                map[y][x] = w;
                break;
            }
            case 'K': {
                Key* k = new Key(x, y, 'K', ObjectType::KEY);
                k->setColor(Color::YELLOW);
                map[y][x] = k;
                keys.push_back(k);
                break;
            }
            case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': {
                Door* d = new Door(x, y, c, ObjectType::DOOR);
                map[y][x] = d;
                doors.push_back(d);
                break;
            }
            case 'B': {
                Bomb* b = new Bomb(x, y, 'B', ObjectType::BOMB);
                b->setRoom(this);
                b->setColor(Color::RED);
                map[y][x] = b;
                break;
            }
            case '?': {
                if (!RIDDLE_AVAILABLE_INDICES.empty()) {
                    size_t pos = randomIndex(RIDDLE_AVAILABLE_INDICES.size());
                    int idx = RIDDLE_AVAILABLE_INDICES[pos];
                    RIDDLE_AVAILABLE_INDICES.erase(
                        RIDDLE_AVAILABLE_INDICES.begin() + static_cast<std::ptrdiff_t>(pos));

                    RiddleData& data = RIDDLES[idx];
                    Riddle* r = new Riddle(
                        x, y, '?', ObjectType::RIDDLE,
                        data.question,
                        data.answers[0],
                        data.answers[1],
                        data.answers[2],
                        data.answers[3],
                        data.correct
                    );
                    r->setColor(Color::MAGENTA);
                    map[y][x] = r;
                } else {
                    std::cerr << "\n========================================" << std::endl;
                    std::cerr << "WARNING: Ran out of riddles for '?' at position ("
                              << y << "," << x << "). Placing empty space." << std::endl;
                    std::cerr << "========================================\n" << std::endl;
                    has_errors = true;
                    map[y][x] = new Empty(x, y, ' ', ObjectType::EMPTY);
                }
                break;
            }
            case '/': {
                Switch_class* sw = new Switch_class(x, y, '/', ObjectType::SWITCH);
                sw->setColor(Color::CYAN);
                switches.push_back(sw);
                map[y][x] = sw;
                break;
            }
            case '\\': {
                Switch_class* sw = new Switch_class(x, y, '\\', ObjectType::SWITCH);
                sw->setColor(Color::CYAN);
                switches.push_back(sw);
                map[y][x] = sw;
                break;
            }
            case '-': {
                Tile* t = new Tile(x, y, '-', ObjectType::TILE);
                tiles.push_back(t);
                map[y][x] = t;
                break;
            }
            case '!': {
                Torch* tr = new Torch(x, y, '!', ObjectType::TORCH);
                tr->setColor(Color::YELLOW);
                map[y][x] = tr;
                break;
            }
            case '*': {
                Obstacle* ob = new Obstacle(x, y, '*', ObjectType::OBSTACLE);
                ob->setColor(Color::GRAY);
                map[y][x] = ob;
                break;
            }
            case '#': {
                SpringElement* sp = new SpringElement(x, y, '#', ObjectType::SPRING_ELEMENT);
                sp->setColor(Color::DIJON);
                map[y][x] = sp;
                break;
            }
            case '$': {
                player1_count++;
                player_1_X = x;
                player_1_Y = y;
                map[y][x] = new Empty(x, y, ' ', ObjectType::EMPTY);
                break;
            }
            case '&': {
                player2_count++;
                player_2_X = x;
                player_2_Y = y;
                map[y][x] = new Empty(x, y, ' ', ObjectType::EMPTY);
                break;
            }
            case 'L':
            case ' ':
            default:
                map[y][x] = new Empty(x, y, ' ', ObjectType::EMPTY);
                break;
            }
        }
    }

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (map[y][x] == nullptr) {
                map[y][x] = new Empty(x, y, ' ', ObjectType::EMPTY);
            }
        }
    }

    if (player1_count == 0) {
        std::cerr << "\n========================================" << std::endl;
        std::cerr << "ERROR: No Player 1 ($) found in game board" << std::endl;
        std::cerr << "Each room must have exactly one $ character" << std::endl;
        std::cerr << "========================================\n" << std::endl;
        has_errors = true;
    } else if (player1_count > 1) {
        std::cerr << "\n========================================" << std::endl;
        std::cerr << "ERROR: Multiple Player 1 ($) positions found: " << player1_count << " instances" << std::endl;
        std::cerr << "Each room must have exactly one $ character" << std::endl;
        std::cerr << "The game will use the last $ position found" << std::endl;
        std::cerr << "========================================\n" << std::endl;
        has_errors = true;
    }

    if (player2_count == 0) {
        std::cerr << "\n========================================" << std::endl;
        std::cerr << "ERROR: No Player 2 (&) found in game board" << std::endl;
        std::cerr << "Each room must have exactly one & character" << std::endl;
        std::cerr << "========================================\n" << std::endl;
        has_errors = true;
    } else if (player2_count > 1) {
        std::cerr << "\n========================================" << std::endl;
        std::cerr << "ERROR: Multiple Player 2 (&) positions found: " << player2_count << " instances" << std::endl;
        std::cerr << "Each room must have exactly one & character" << std::endl;
        std::cerr << "The game will use the last & position found" << std::endl;
        std::cerr << "========================================\n" << std::endl;
        has_errors = true;
    }

    // Parse configuration lines (after line 19)
    int config_line_number = HEIGHT;
    while (std::getline(file, line)) {
        config_line_number++;

        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue;
        }

        if (line.length() < 11) {
            std::cerr << "\n========================================" << std::endl;
            std::cerr << "WARNING: Configuration line " << config_line_number
                      << " is too short (expected format: X#### X####). Skipping." << std::endl;
            std::cerr << "Line content: \"" << line << "\"" << std::endl;
            std::cerr << "========================================\n" << std::endl;
            has_errors = true;
            continue;
        }

        try {
            char source_type = line[0];

            if (line[5] != ' ') {
                std::cerr << "\n========================================" << std::endl;
                std::cerr << "WARNING: Configuration line " << config_line_number
                          << " missing space separator at position 6. Skipping." << std::endl;
                std::cerr << "Line content: \"" << line << "\"" << std::endl;
                std::cerr << "========================================\n" << std::endl;
                has_errors = true;
                continue;
            }

            int source_y = 0, source_x = 0, target_y = 0, target_x = 0;

            try {
                source_y = std::stoi(line.substr(1, 2));
                source_x = std::stoi(line.substr(3, 2));
                target_y = std::stoi(line.substr(7, 2));
                target_x = std::stoi(line.substr(9, 2));
            } catch (const std::exception& e) {
                std::cerr << "\n========================================" << std::endl;
                std::cerr << "ERROR: Failed to parse numbers in configuration line "
                          << config_line_number << std::endl;
                std::cerr << "Line content: \"" << line << "\"" << std::endl;
                std::cerr << "Parse error: " << e.what() << std::endl;
                std::cerr << "========================================\n" << std::endl;
                has_errors = true;
                continue;
            }

            if (source_type == 'L') {
                if (source_y < 0 || source_y >= SCREEN_HEIGHT || source_x < 0 || source_x >= WIDTH) {
                    std::cerr << "\n========================================" << std::endl;
                    std::cerr << "ERROR: Legend source coordinates (" << source_y << "," << source_x
                              << ") out of bounds in line " << config_line_number << std::endl;
                    std::cerr << "========================================\n" << std::endl;
                    has_errors = true;
                    continue;
                }
                if (target_y < 0 || target_y >= SCREEN_HEIGHT || target_x < 0 || target_x >= WIDTH) {
                    std::cerr << "\n========================================" << std::endl;
                    std::cerr << "ERROR: Legend target coordinates (" << target_y << "," << target_x
                              << ") out of bounds in line " << config_line_number << std::endl;
                    std::cerr << "========================================\n" << std::endl;
                    has_errors = true;
                    continue;
                }
            } else if (source_type != 'R') {
                if (source_y < 0 || source_y >= HEIGHT || source_x < 0 || source_x >= WIDTH) {
                    std::cerr << "\n========================================" << std::endl;
                    std::cerr << "ERROR: Source coordinates (" << source_y << "," << source_x
                              << ") out of bounds in line " << config_line_number << std::endl;
                    std::cerr << "========================================\n" << std::endl;
                    has_errors = true;
                    continue;
                }
            }

            if (source_type != 'R' && source_type != 'D' && source_type != 'L') {
                if (target_y < 0 || target_y >= HEIGHT || target_x < 0 || target_x >= WIDTH) {
                    std::cerr << "\n========================================" << std::endl;
                    std::cerr << "ERROR: Target coordinates (" << target_y << "," << target_x
                              << ") out of bounds in line " << config_line_number << std::endl;
                    std::cerr << "========================================\n" << std::endl;
                    has_errors = true;
                    continue;
                }
            }

            switch (source_type) {
            case 'S': {
                if (map[source_y][source_x]->get_object_type() != ObjectType::SWITCH) {
                    std::cerr << "\n========================================" << std::endl;
                    std::cerr << "ERROR: Expected Switch at (" << source_y << "," << source_x
                              << ") in line " << config_line_number << std::endl;
                    std::cerr << "========================================\n" << std::endl;
                    has_errors = true;
                    break;
                }
                if (map[target_y][target_x]->get_object_type() != ObjectType::TILE) {
                    std::cerr << "\n========================================" << std::endl;
                    std::cerr << "ERROR: Expected Tile at (" << target_y << "," << target_x
                              << ") in line " << config_line_number << std::endl;
                    std::cerr << "========================================\n" << std::endl;
                    has_errors = true;
                    break;
                }

                Switch_class* sw = static_cast<Switch_class*>(map[source_y][source_x]);
                Tile* t = static_cast<Tile*>(map[target_y][target_x]);
                sw->set_tile(t);
                t->add_to_switch_vector(sw);
                break;
            }
            case 'K': {
                if (map[source_y][source_x]->get_object_type() != ObjectType::KEY) {
                    std::cerr << "\n========================================" << std::endl;
                    std::cerr << "ERROR: Expected Key at (" << source_y << "," << source_x
                              << ") in line " << config_line_number << std::endl;
                    std::cerr << "========================================\n" << std::endl;
                    has_errors = true;
                    break;
                }
                if (map[target_y][target_x]->get_object_type() != ObjectType::DOOR) {
                    std::cerr << "\n========================================" << std::endl;
                    std::cerr << "ERROR: Expected Door at (" << target_y << "," << target_x
                              << ") in line " << config_line_number << std::endl;
                    std::cerr << "========================================\n" << std::endl;
                    has_errors = true;
                    break;
                }

                Key* k = static_cast<Key*>(map[source_y][source_x]);
                Door* d = static_cast<Door*>(map[target_y][target_x]);
                d->getKeys().push_back(k);
                break;
            }
            case 'R':
                this->set_dark();
                break;

            case 'L':
                L_start_x = target_x;
                L_start_y = target_y;
                break;

            case 'D': {
                if (map[source_y][source_x]->get_object_type() != ObjectType::DOOR) {
                    std::cerr << "\n========================================" << std::endl;
                    std::cerr << "ERROR: Expected Door at (" << source_y << "," << source_x
                              << ") in line " << config_line_number << std::endl;
                    std::cerr << "========================================\n" << std::endl;
                    has_errors = true;
                    break;
                }

                Door* nd = static_cast<Door*>(map[source_y][source_x]);
                int target_room = target_y;

                const int MAX_ROOM = 3;
                if (target_room < 0 || target_room > MAX_ROOM) {
                    std::cerr << "\n========================================" << std::endl;
                    std::cerr << "ERROR: Invalid target room number " << target_room
                              << " in line " << config_line_number << std::endl;
                    std::cerr << "========================================\n" << std::endl;
                    has_errors = true;
                    break;
                }

                nd->set_room_target_number(target_room);
                next_room_doors.insert({ nd, target_room });
                break;
            }
            default:
                std::cerr << "\n========================================" << std::endl;
                std::cerr << "WARNING: Unknown configuration type '" << source_type
                          << "' in line " << config_line_number << std::endl;
                std::cerr << "========================================\n" << std::endl;
                has_errors = true;
                break;
            }
        } catch (const std::exception& e) {
            std::cerr << "\n========================================" << std::endl;
            std::cerr << "ERROR: Exception while processing configuration line "
                      << config_line_number << ": " << e.what() << std::endl;
            std::cerr << "========================================\n" << std::endl;
            has_errors = true;
        }
    }

    file.close();

    try {
        initilize_springs();
    } catch (const std::exception& e) {
        std::cerr << "\n========================================" << std::endl;
        std::cerr << "ERROR: Exception during spring initialization: " << e.what() << std::endl;
        std::cerr << "========================================\n" << std::endl;
        has_errors = true;
    }

    if (has_errors) {
        std::cerr << "\n*** ERRORS DETECTED - Review above ***" << std::endl;
        readKey();
    }
}

Room::~Room() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            delete map[y][x];
        }
    }
    for (Spring* sp : springs) {
        delete sp;
    }
}

void Room::Draw() {
    cls();
    if (is_dark) {
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                GameObject* obj = map[y][x];
                gotoxy(x, y);
                if (obj->getSign() == '!')
                    std::cout << '!';
                else if (obj->get_shown())
                    printColored(obj->getSign(), obj->getColor());
                else
                    std::cout << ' ';
            }
        }
        return;
    }

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            gotoxy(x, y);
            GameObject* obj = map[y][x];
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

GameObject* Room::get_object_by_index(int row, int col) {
    return map[row][col];
}

void Room::set_game_keys() {
    if (doors.empty()) return;
    for (auto& k : keys) {
        size_t door_number = randomIndex(doors.size());
        Door* randomDoor = doors[door_number];
        randomDoor->getKeys().push_back(k);
    }
}

void Room::Erase(int _x, int _y) {
    delete map[_y][_x];
    map[_y][_x] = new Empty(_x, _y, ' ', ObjectType::EMPTY);
}

void Room::place_object_at(int _x, int _y, GameObject* obj) {
    if (map[_y][_x] != nullptr && map[_y][_x] != obj) {
        delete map[_y][_x];
    }
    map[_y][_x] = obj;
    obj->set_location(_x, _y);
    gotoxy(_x, _y);
    printColored(obj->getSign(), obj->getColor());
}

void Room::move_object(int from_x, int from_y, int to_x, int to_y) {
    GameObject* obj = map[from_y][from_x];
    if (obj == nullptr || (from_x == to_x && from_y == to_y))
        return;

    delete map[to_y][to_x];
    map[to_y][to_x] = obj;
    obj->set_location(to_x, to_y);

    // The source cell is NOT deleted: it still holds the moved object.
    map[from_y][from_x] = new Empty(from_x, from_y, ' ', ObjectType::EMPTY);

    gotoxy(from_x, from_y);
    std::cout << ' ';
    gotoxy(to_x, to_y);
    printColored(obj->getSign(), obj->getColor());
}

bool Room::is_inside(int _x, int _y) const {
    return (_x >= 0 && _x < WIDTH && _y >= 0 && _y < HEIGHT);
}

void Room::hideObject(int _y, int _x) {
    map[_y][_x] = new Empty(_x, _y, ' ', ObjectType::EMPTY);
    gotoxy(_x, _y);
    std::cout << ' ';
    std::cout.flush();
}

void Room::addActiveBomb(Bomb* bomb) {
    activeBombs.push_back(bomb);
}

void Room::updateBombs() {
    for (auto it = activeBombs.begin(); it != activeBombs.end();) {
        Bomb* bomb = *it;
        bomb->tick();

        if (bomb->getCountdown() > 0) {
            gotoxy(bomb->getPosX(), bomb->getPosY());
            std::cout << bomb->getCountdown();
        }

        if (bomb->shouldExplode()) {
            explodeBomb(bomb);
            it = activeBombs.erase(it);
        } else {
            ++it;
        }
    }
}

void Room::explodeBomb(Bomb* bomb) {
    int centerX = bomb->getPosX();
    int centerY = bomb->getPosY();
    int radius = bomb->getRadius();

    gotoxy(0, HEIGHT);
    sleepMs(50);

    if (is_inside(centerX, centerY)) {
        GameObject* centerObj = map[centerY][centerX];
        if (centerObj->get_object_type() == ObjectType::BOMB) {
            delete map[centerY][centerX];
            map[centerY][centerX] = new Empty(centerX, centerY, ' ', ObjectType::EMPTY);
            gotoxy(centerX, centerY);
            std::cout << ' ';
        }
    }

    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            int x = centerX + dx;
            int y = centerY + dy;

            if (is_inside(x, y)) {
                GameObject* obj = map[y][x];
                auto t = obj->get_object_type();

                if (t == ObjectType::WALL || t == ObjectType::RIDDLE ||
                    t == ObjectType::SWITCH || t == ObjectType::TILE ||
                    t == ObjectType::DOOR || t == ObjectType::OBSTACLE ||
                    t == ObjectType::SPRING_ELEMENT) {

                    if (t == ObjectType::DOOR) {
                        Door* d = static_cast<Door*>(obj);
                        if (d->get_target_room() != nullptr)
                            continue;
                    }

                    if (t == ObjectType::SPRING_ELEMENT) {
                        SpringElement* sp = static_cast<SpringElement*>(obj);
                        if (sp->get_owning_spring() != nullptr)
                            sp->get_owning_spring()->clean_spring(this);
                        else
                            Erase(x, y);
                        continue;
                    }

                    if (t == ObjectType::TILE) {
                        remove_tile_from_switches(static_cast<Tile*>(obj));
                    }
                    if (t == ObjectType::SWITCH) {
                        detach_switch(static_cast<Switch_class*>(obj));
                    }

                    delete map[y][x];
                    map[y][x] = new Empty(x, y, ' ', ObjectType::EMPTY);

                    gotoxy(x, y);
                    std::cout << ' ';
                }
            }
        }
    }

    gotoxy(0, HEIGHT);
    std::cout << std::string(79, ' ');

    bomb_exploded = true;
    exploded_x = centerX;
    exploded_y = centerY;
    exploded_radius = radius;
}

int Room::get_spring_size(int row, int col, Direction dir) const {
    if (!is_inside(col, row))
        return 0;

    if (map[row][col]->get_object_type() != ObjectType::SPRING_ELEMENT)
        return 0;

    if (dir == Direction::DOWN)
        return 1 + get_spring_size(row + 1, col, dir);
    else
        return 1 + get_spring_size(row, col + 1, dir);
}

void Room::initilize_springs() {
    for (int row = 0; row < HEIGHT; row++) {
        for (int col = 0; col < WIDTH; col++) {
            if (map[row][col]->get_object_type() == ObjectType::SPRING_ELEMENT) {
                SpringElement* spe = static_cast<SpringElement*>(map[row][col]);
                if (spe->get_owning_spring() == nullptr) {
                    Spring* sp = nullptr;
                    int spring_size = 1 + get_spring_size(row, col + 1, Direction::RIGHT);

                    if (spring_size > 1) {
                        if (is_inside(col - 1, row) && map[row][col - 1]->get_object_type() == ObjectType::WALL) {
                            sp = new Spring(row, row, col + (spring_size - 1), col, Direction::LEFT);
                        } else {
                            sp = new Spring(row, row, col, col + (spring_size - 1), Direction::RIGHT);
                        }
                    } else {
                        spring_size = 1 + get_spring_size(row + 1, col, Direction::DOWN);
                        if (spring_size > 1) {
                            if (is_inside(col, row - 1) && map[row - 1][col]->get_object_type() == ObjectType::WALL) {
                                sp = new Spring(row + (spring_size - 1), row, col, col, Direction::UP);
                            } else {
                                sp = new Spring(row, row + (spring_size - 1), col, col, Direction::DOWN);
                            }
                        }
                    }

                    if (sp != nullptr) {
                        springs.push_back(sp);
                        update_spring_elements(sp);
                    }
                }
            }
        }
    }
}

void Room::update_spring_elements(Spring* sp) {
    switch (sp->get_dir()) {
    case Direction::RIGHT:
    case Direction::DOWN:
        for (int i = sp->get_start_row(); i <= sp->get_end_row(); i++) {
            for (int j = sp->get_start_col(); j <= sp->get_end_col(); j++) {
                SpringElement* spe = static_cast<SpringElement*>(map[i][j]);
                spe->set_owning_spring(sp);
            }
        }
        break;
    case Direction::UP:
    case Direction::LEFT:
        for (int i = sp->get_start_row(); i >= sp->get_end_row(); i--) {
            for (int j = sp->get_start_col(); j >= sp->get_end_col(); j--) {
                SpringElement* spe = static_cast<SpringElement*>(map[i][j]);
                spe->set_owning_spring(sp);
            }
        }
        break;
    case Direction::STAY:
        break;
    }
}

void Room::remove_tile_from_switches(Tile* t) {
    for (auto& s : switches) {
        if (s->get_tile() == t)
            s->set_tile(nullptr);
    }
    tiles.erase(std::remove(tiles.begin(), tiles.end(), t), tiles.end());
}

// A destroyed switch must not stay referenced by the room or by its tile.
void Room::detach_switch(Switch_class* s) {
    if (Tile* t = s->get_tile())
        t->remove_switch(s);
    switches.erase(std::remove(switches.begin(), switches.end(), s), switches.end());
}
