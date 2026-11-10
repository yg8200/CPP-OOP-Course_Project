#pragma once

#include <list>
#include <string>
#include <cstddef>

struct single_step {
    size_t iteration;
    char ch;
};

class Steps {
    long long randomSeed = 0;
    std::list<single_step> steps;
    static constexpr const char* stepsFile = "adv-world.steps";

public:
    static Steps loadSteps();
    void saveSteps() const;

    long long getRandomSeed() const { return randomSeed; }
    void setRandomSeed(long long seed) { randomSeed = seed; }

    void addStep(size_t iteration, char step) {
        steps.push_back({ iteration, step });
    }

    single_step popStep() {
        single_step step = steps.front();
        steps.pop_front();
        return step;
    }

    bool next_step_exists() const { return !steps.empty(); }
};
