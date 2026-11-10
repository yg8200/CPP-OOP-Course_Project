#include "Game.h"
#include "Player.h"
#include "Door.h"
#include "utils.h"
#include <iostream>
#include <iterator>

enum Keys { ESC = 27 };

namespace {
// Key read while paused. A closed stdin (EOF) acts as "go back to the menu" instead of spinning forever.
int readPauseKey() {
    int key = readGameKey();
    return key < 0 ? 'h' : key;
}
}

void Game::Start() {
    hideCursor();
    change_room();
    print_players();

    bool gameRunning = true;

    while (gameRunning) {
        for (auto& p : players) {
            p.move(0);
        }
        print_players();

        rooms[current_room].updateBombs();
        check_bomb_explosion();

        if (keyAvailable()) {
            char key = static_cast<char>(readGameKey());
            if (key == Keys::ESC) {
                gotoxy(3, 24);
                std::cout << " Game paused, press ESC again to continue or H to go back to the main menu ";

                key = static_cast<char>(readPauseKey());
                while (key != 'h' && key != 'H' && key != Keys::ESC)
                    key = static_cast<char>(readPauseKey());

                if (key == 'H' || key == 'h') {
                    gameRunning = false;
                } else if (key == Keys::ESC) {
                    gotoxy(3, 24);
                    std::cout << "                                                                           ";
                    continue;
                }
            }
            if (!gameRunning) {
                // leaving to the menu - ignore the key that was used to leave
            } else if (key == '6') {
                current_room++;
                if (current_room == 3) {
                    cls();
                    print_room_by_txtName(winning_screen_layout_txt);
                    sleepMs(2000);
                    gotoxy(12, 23);
                    std::cout << " Game ended, Press any key to go to menu ";
                    gameRunning = false;
                    readKey();
                    break;
                }
                move_players_room();
            } else {
                for (auto& p : players) {
                    if (p.get_spring_accelration_cycles() == 0)
                        p.handleKeyPressed(key);
                }
            }
        }

        size_t changed_room = 0;
        for (int i = 0; i < players_size; i++) {
            Player& p = players[i];
            if (p.get_current_room() != &rooms[current_room] || p.get_moved_room()) {
                changed_room++;
            }
            if (p.check_life_changed()) {
                draw_lifes_scores();
                p.clear_life_flag();
                if (p.getLives() <= 0) {
                    print_room_by_txtName(losing_screen_layout_txt);
                    gameRunning = false;
                    readKey();
                    break;
                }
            }

            if (p.check_score_changed()) {
                draw_lifes_scores();
                p.clear_score_flag();
            }
        }

        if (changed_room == std::size(players)) {
            current_room = players[1].get_room_to_move_to();
            if (current_room == 3) {
                print_room_by_txtName(winning_screen_layout_txt);
                sleepMs(2500);
                gotoxy(12, 23);
                std::cout << " Game ended, Press any key to go to menu ";
                gameRunning = false;
                readKey();
                break;
            }
            move_players_room();
        }

        sleepMs(65);
    }
    cls();
}

void Game::draw_room() {
    rooms[current_room].Draw();
    draw_lifes_scores();
}

void Game::draw_lifes_scores() {
    int lives_x_index = rooms[current_room].get_L_start_x();
    int lives_y_index = rooms[current_room].get_L_start_y();

    gotoxy(lives_x_index, lives_y_index);
    std::cout << "Player ($) Lives: " << players[0].getLives();
    gotoxy(lives_x_index + 25, lives_y_index);
    std::cout << "Player (&) Lives: " << players[1].getLives();

    gotoxy(lives_x_index, lives_y_index + 1);
    std::cout << "Player ($) Score: " << players[0].getScore();
    gotoxy(lives_x_index + 25, lives_y_index + 1);
    std::cout << "Player (&) Score: " << players[1].getScore();
}

void Game::setup_players_room() {
    for (auto& p : players) {
        p.set_player_room(&rooms[current_room]);
    }
}

void Game::set_next_room() {
    for (const auto& elem : rooms[current_room].get_next_room_doors()) {
        Door* door = elem.first;
        int room_dest = elem.second;
        if (room_dest >= 3)
            door->set_target_room(&rooms[2]);
        else
            door->set_target_room(&rooms[room_dest]);
    }
}

void Game::change_room() {
    set_next_room();
    setup_players_room();
    draw_room();
}

void Game::print_players() {
    for (auto& p : players) {
        if (!p.get_moved_room())
            p.draw();
    }
}

int Game::check_bomb_explosion() {
    int players_exploded = 0;
    int players_in_room = 0;

    if (rooms[current_room].check_bomb_exploded()) {
        int bx = 0, by = 0, br = 0;
        rooms[current_room].get_explosion_data(bx, by, br);

        for (int i = 0; i < players_size; i++) {
            Player& p = players[i];
            if (p.get_current_room() != &rooms[current_room])
                continue;
            players_in_room++;

            int px = p.getX();
            int py = p.getY();

            int dx = px - bx;
            int dy = py - by;

            if (dx * dx + dy * dy <= br * br) {
                players_exploded += i + 1; // 1 = player ($), 2 = player (&), 3 = both
                p.loseLife(3);
                gotoxy(p.getX(), p.getY());
                std::cout << " ";
                p.set_player_cords(p.get_start_cord_X(), p.get_start_cord_Y());
                p.set_diff_to_0();
            }
        }

        rooms[current_room].clear_bomb_explosion_flag();
    }

    // -1 means "no explosion was evaluated"
    return players_in_room > 0 ? players_exploded : -1;
}

void Game::setup_players_starting_index() {
    players[0].set_start_cords(rooms[current_room].get_p1_x(), rooms[current_room].get_p1_y());
    players[1].set_start_cords(rooms[current_room].get_p2_x(), rooms[current_room].get_p2_y());

    players[0].set_player_cords(rooms[current_room].get_p1_x(), rooms[current_room].get_p1_y());
    players[1].set_player_cords(rooms[current_room].get_p2_x(), rooms[current_room].get_p2_y());
    players[0].set_diff_to_0();
    players[1].set_diff_to_0();
}

void Game::move_players_room() {
    for (auto& p : players) {
        p.set_moved_room(false);
    }
    setup_players_room();
    set_next_room();
    draw_room();
    setup_players_starting_index();
}
