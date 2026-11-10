#pragma once

#include <string>
#include <vector>

class RiddleData {
public:
    std::string question;
    std::string answers[4];
    int correct = 0;
};

extern std::vector<RiddleData> RIDDLES;
extern std::vector<int> RIDDLE_AVAILABLE_INDICES;

void loadRiddlesFromFile(const std::string& filename);
void initRiddlePool();
