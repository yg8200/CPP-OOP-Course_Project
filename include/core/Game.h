#pragma once

#include "Player.h"
#include "Room.h"
#include <string>

class Game {
protected:
    static constexpr int room_size = 3;
    static constexpr int players_size = 2;
    static constexpr const char* losing_screen_layout_txt = "losing_screen.txt";
    static constexpr const char* winning_screen_layout_txt = "winning.txt";

    Player players[players_size];
    Room rooms[room_size];
    int current_room = 0;

public:
    Game(const Player(&player)[2])
        : rooms{ std::string("layout_1.txt"), std::string("layout_2.txt"), std::string("layout_3.txt") } {
        players[0] = player[0];
        players[1] = player[1];
    }

    virtual void Start();
    virtual ~Game() = default;

    void draw_room();
    void draw_lifes_scores();
    void setup_players_room();
    void set_next_room();
    virtual void change_room();
    void print_players();
    virtual int check_bomb_explosion();
    virtual void move_players_room();
    void setup_players_starting_index();
};
