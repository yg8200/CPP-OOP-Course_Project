#include "Riddle.h"
#include "utils.h"
#include <iostream>

Riddle::Riddle(int x, int y, char sign, ObjectType type,
               const std::string& q,
               const std::string& a1,
               const std::string& a2,
               const std::string& a3,
               const std::string& a4,
               int correctIdx)
    : GameObject(x, y, sign, type),
      question(q),
      correctIndex(correctIdx) {
    options[0] = a1;
    options[1] = a2;
    options[2] = a3;
    options[3] = a4;
}

int Riddle::ask(int playback_answer, int speed) {
    const int boxWidth = 60;
    const int boxHeight = 12;
    const int startX = 10;
    const int startY = 1;

    if (speed > 0) {
        clear_map_box();

        gotoxy(startX, startY);
        std::cout << "+" << std::string(boxWidth - 2, '-') << "+";

        for (int i = 1; i < boxHeight - 1; ++i) {
            gotoxy(startX, startY + i);
            std::cout << "|" << std::string(boxWidth - 2, ' ') << "|";
        }

        gotoxy(startX, startY + boxHeight - 1);
        std::cout << "+" << std::string(boxWidth - 2, '-') << "+";

        gotoxy(startX + (boxWidth / 2) - 3, startY + 1);
        std::cout << "RIDDLE";

        gotoxy(startX + 2, startY + 3);
        std::cout << question;

        for (int i = 0; i < 4; ++i) {
            gotoxy(startX + 4, startY + 5 + i);
            std::cout << (i + 1) << ") " << options[i];
        }

        gotoxy(startX + 2, startY + 10);
        std::cout << "Choose answer (1-4): ";
    }

    int idx = -1;
    if (playback_answer == -1) {
        while (true) {
            int key = readKey();
            if (key < 0) { // input closed (EOF): count as a wrong answer instead of spinning forever
                idx = 0;
                break;
            }
            char c = static_cast<char>(key);
            if (c >= '1' && c <= '4') {
                idx = c - '1';
                std::cout << c;
                break;
            }
        }
    } else {
        idx = playback_answer;
    }

    bool correct = (idx == correctIndex);
    if (correct)
        solved = true;

    if (speed != 0) {
        gotoxy(startX + 2, startY + 13);
        if (correct) {
            std::cout << "Correct! Press any key to continue...";
        } else {
            std::cout << "Wrong answer. Press any key to continue...";
        }
    }

    if (playback_answer == -1) {
        readKey();
    } else {
        sleepMs(static_cast<unsigned int>(speed));
    }

    return correct ? idx : (10 + idx);
}
