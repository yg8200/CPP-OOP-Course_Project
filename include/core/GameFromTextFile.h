#pragma once

#include "Game.h"
#include "Results.h"

class GameFromTextFile : public Game {
private:
    Results results;
    size_t iteration = 0;
    bool silent_playback = false;

public:
    GameFromTextFile(const Player(&player)[2], bool _silent_playback)
        : Game(player), silent_playback(_silent_playback) {}

    void Start() override;
    void change_room() override;
    void move_players_room() override;
};
