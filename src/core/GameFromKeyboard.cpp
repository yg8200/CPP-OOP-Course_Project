#include "GameFromKeyboard.h"
#include "Player.h"
#include "Steps.h"
#include "Results.h"
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

void GameFromKeyboard::Start() {
    hideCursor();
    change_room();
    print_players();

    for (auto& p : players) {
        p.set_results_obj(&results);
    }

    bool gameRunning = true;
    Steps steps;
    steps.setRandomSeed(seed);

    while (gameRunning) {
        for (auto& p : players) {
            p.move(iteration);
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
                    results.add_event(iteration + 1, result_type::INTERRUPTED, 0);
                } else if (key == Keys::ESC) {
                    gotoxy(3, 24);
                    std::cout << "                                                                           ";
                    continue;
                }
            } else {
                for (auto& p : players) {
                    if (p.get_spring_accelration_cycles() == 0) {
                        if (p.is_key_match(key))
                            steps.addStep(iteration, key);
                        p.handleKeyPressed(key);
                    }
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
                    results.add_event(iteration + 1, result_type::LOST, 0);
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
                gameRunning = false;
                results.add_event(iteration + 1, result_type::FINISHED, 0);
                break;
            }
            move_players_room();
        }

        sleepMs(65);
        iteration++;
    }

    steps.addStep(iteration, '~');
    steps.saveSteps();

    results.save_results();
    cls();
}

int GameFromKeyboard::check_bomb_explosion() {
    int players_exploded = Game::check_bomb_explosion();
    if (players_exploded != -1)
        results.add_event(iteration, result_type::BOMB_HIT, players_exploded);
    return players_exploded;
}
