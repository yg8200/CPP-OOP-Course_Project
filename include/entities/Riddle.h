#pragma once

#include "GameObject.h"
#include <string>

class Riddle : public GameObject {
    std::string question;
    std::string options[4];
    int correctIndex = 0;
    bool solved = false;

public:
    Riddle(int x, int y, char sign, ObjectType type,
           const std::string& q,
           const std::string& a1,
           const std::string& a2,
           const std::string& a3,
           const std::string& a4,
           int correctIdx);

    bool isSolved() const { return solved; }
    int ask(int playback_answer, int speed);
};
