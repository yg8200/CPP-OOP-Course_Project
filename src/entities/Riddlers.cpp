#include "Riddlers.h"
#include <fstream>
#include <iostream>

std::vector<RiddleData> RIDDLES;
std::vector<int> RIDDLE_AVAILABLE_INDICES;

namespace {
// getline that also strips the '\r' left by Windows (CRLF) line endings
bool readLine(std::istream& in, std::string& line) {
    if (!std::getline(in, line))
        return false;
    if (!line.empty() && line.back() == '\r')
        line.pop_back();
    return true;
}
}

void initRiddlePool() {
    RIDDLE_AVAILABLE_INDICES.clear();
    for (size_t i = 0; i < RIDDLES.size(); i++) {
        RIDDLE_AVAILABLE_INDICES.push_back(static_cast<int>(i));
    }
}

void loadRiddlesFromFile(const std::string& filename) {
    RIDDLES.clear();

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open riddles file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (true) {
        RiddleData riddle;

        if (!readLine(file, riddle.question))
            break;

        for (int i = 0; i < 4; i++) {
            if (!readLine(file, riddle.answers[i]))
                return;
        }

        if (!readLine(file, line))
            return;
        try {
            riddle.correct = std::stoi(line) - 1;
        } catch (const std::exception&) {
            std::cerr << "Error: bad correct-answer line in riddles file: \"" << line << "\"" << std::endl;
            return;
        }
        if (riddle.correct < 0 || riddle.correct > 3) {
            std::cerr << "Error: correct answer out of range 1-4 in riddles file: \"" << line << "\"" << std::endl;
            return;
        }

        readLine(file, line); // separator line "--------"
        RIDDLES.push_back(riddle);
    }
}
