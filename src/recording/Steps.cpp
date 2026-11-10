#include "Steps.h"
#include <fstream>

Steps Steps::loadSteps() {
    Steps steps;
    std::ifstream steps_file(stepsFile);
    if (!steps_file.is_open()) {
        return steps;
    }

    steps_file >> steps.randomSeed;
    size_t size = 0;
    steps_file >> size;

    size_t iteration = 0;
    char step = 0;
    while (size > 0 && (steps_file >> iteration >> step)) {
        steps.addStep(iteration, step);
        size--;
    }
    return steps;
}

void Steps::saveSteps() const {
    std::ofstream steps_file(stepsFile);
    steps_file << randomSeed << '\n' << steps.size();
    for (const auto& step : steps) {
        steps_file << '\n' << step.iteration << ' ' << step.ch;
    }
    steps_file << '\n';
}
