#include "Results.h"
#include <fstream>

Results::~Results() {
    for (auto& pair : events) {
        delete pair.second;
    }
}

Results::Results(const Results& other) : results_read(other.results_read) {
    for (const auto& pair : other.events) {
        events[pair.first] = new event(*pair.second);
    }
}

Results& Results::operator=(const Results& other) {
    if (this != &other) {
        for (auto& pair : events) {
            delete pair.second;
        }
        events.clear();
        results_read = other.results_read;
        for (const auto& pair : other.events) {
            events[pair.first] = new event(*pair.second);
        }
    }
    return *this;
}

Results Results::load_results() {
    Results res;
    std::ifstream res_file(res_filename);
    if (!res_file.is_open()) {
        return res;
    }

    size_t iteration = 0;
    int res_type = 0;
    int param = 0;
    while (res_file >> iteration >> res_type >> param) {
        res.add_event(iteration, static_cast<result_type>(res_type), param);
    }
    return res;
}

void Results::add_event(size_t iteration, result_type result, int res_parameter) {
    auto it = events.find(iteration);
    if (it != events.end()) {
        delete it->second;
        it->second = new event{ result, res_parameter };
    } else {
        events.insert({ iteration, new event{ result, res_parameter } });
    }
}

void Results::save_results() const {
    std::ofstream results_file(res_filename);
    for (const auto& pair : events) {
        int res = static_cast<int>(pair.second->res);
        results_file << pair.first << " " << res << " " << pair.second->type_specific_parameter << '\n';
    }
}

event* Results::get_event(size_t iteration) {
    auto it = events.find(iteration);
    if (it == events.end())
        return nullptr;

    if (!it->second->was_read) { // count each event once, however often it is looked up
        it->second->was_read = true;
        results_read++;
    }
    return it->second;
}
