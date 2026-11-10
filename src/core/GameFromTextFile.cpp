#include "GameFromTextFile.h"
#include "Player.h"
#include "Steps.h"
#include "utils.h"
#include <iostream>
#include <iterator>

void GameFromTextFile::Start() {
    setSilentMode(silent_playback);
    hideCursor();
    change_room();
    if (!silent_playback)
        print_players();

    Steps steps = Steps::loadSteps();
    results = Results::load_results();

    for (auto& p : players) {
        p.set_results_obj(&results);
    }

    bool need_to_get_next_step = true;
    size_t iteration_order = 0;
    char key = 0;

    while (!need_to_get_next_step || (need_to_get_next_step && steps.next_step_exists())) {
        if (need_to_get_next_step) {
            single_step st = steps.popStep();
            iteration_order = st.iteration;
            key = st.ch;
            need_to_get_next_step = false;
        }

        for (auto& p : players) {
            p.move(iteration);
        }

        if (!silent_playback)
            print_players();

        rooms[current_room].updateBombs();
        int exp = check_bomb_explosion();
        if (exp != -1) {
            event* ev = results.get_event(iteration);
            if (ev == nullptr || ev->res != result_type::BOMB_HIT || ev->type_specific_parameter != exp) {
                setSilentMode(false);
                cls();
                std::cout << "game recording was compromised! - bomb result mismatch" << std::endl;
                break;
            }
        }

        if (iteration == iteration_order) {
            if (key == '~') {
                // End-of-recording marker. If the player quit with ESC+H the recording holds an
                // INTERRUPTED event here; consume it so it is not reported as unread.
                results.get_event(iteration);
            } else {
                for (auto& p : players) {
                    p.handleKeyPressed(key);
                }
            }
            need_to_get_next_step = true;
        }

        size_t changed_room = 0;
        for (int i = 0; i < players_size; i++) {
            Player& p = players[i];
            if (p.get_current_room() != &rooms[current_room] || p.get_moved_room()) {
                changed_room++;
            }
            if (p.check_life_changed()) {
                if (!silent_playback)
                    draw_lifes_scores();
                p.clear_life_flag();
                if (p.getLives() <= 0) {
                    event* ev = results.get_event(iteration + 1);
                    if (ev == nullptr || ev->res != result_type::LOST) {
                        setSilentMode(false);
                        cls();
                        std::cout << "game recording was compromised! - game was supposed to end with lose" << std::endl;
                        readKey();
                        break;
                    }
                    if (!silent_playback) {
                        print_room_by_txtName(losing_screen_layout_txt);
                        sleepMs(2000);
                    }
                    break;
                }
            }

            if (p.check_score_changed()) {
                if (!silent_playback)
                    draw_lifes_scores();
                p.clear_score_flag();
            }
        }

        if (changed_room == std::size(players)) {
            current_room = players[1].get_room_to_move_to();
            if (current_room == 3) {
                event* ev = results.get_event(iteration + 1);
                if (ev == nullptr || ev->res != result_type::FINISHED) {
                    setSilentMode(false);
                    cls();
                    std::cout << "game recording was compromised! - game was supposed to end with win" << std::endl;
                    readKey();
                    break;
                }
                if (!silent_playback) {
                    print_room_by_txtName(winning_screen_layout_txt);
                    sleepMs(2500);
                    gotoxy(12, 23);
                }
                break;
            }
            move_players_room();
        }

        if (!silent_playback)
            sleepMs(25);
        iteration++;
    }

    setSilentMode(false);
    cls();

    if (results.unread_results() != 0) {
        std::cout << "game recording was compromised! - game ended prematurely" << std::endl;
        readKey();
    } else if (silent_playback) {
        std::cout << "Silent playback test passed successfully!" << std::endl;
    }
}

void GameFromTextFile::change_room() {
    set_next_room();
    setup_players_room();
    if (!silent_playback)
        draw_room();
}

void GameFromTextFile::move_players_room() {
    for (auto& p : players) {
        p.set_moved_room(false);
    }
    setup_players_room();
    set_next_room();
    if (!silent_playback)
        draw_room();
    setup_players_starting_index();
}
