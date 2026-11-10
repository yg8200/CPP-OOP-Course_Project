#pragma once

#include "Game.h"
#include "Results.h"

class GameFromKeyboard : public Game {
private:
    Results results;
    size_t iteration = 0;
    long long seed = 1;

public:
    GameFromKeyboard(const Player(&player)[2], long long _seed)
        : Game(player), seed(_seed) {}

    void Start() override;
    int check_bomb_explosion() override;
};
