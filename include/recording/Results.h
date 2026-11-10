#pragma once

#include <map>
#include <cstddef>

enum class result_type {
    NO_EVENT,
    BOMB_HIT,
    MOVED_ROOM,
    RIDDLE_SUCCESS,
    RIDDLE_FAILURE,
    LOST,
    FINISHED,
    INTERRUPTED
};

struct event {
    result_type res = result_type::NO_EVENT;
    int type_specific_parameter = -1;
    bool was_read = false;
};

class Results {
private:
    static constexpr const char* res_filename = "adv-world.result";
    std::map<size_t, event*> events;
    size_t results_read = 0;

public:
    Results() = default;
    ~Results();
    Results(const Results& other);
    Results& operator=(const Results& other);

    static Results load_results();
    void add_event(size_t iteration, result_type result, int res_parameter);
    event* get_event(size_t iteration);
    void save_results() const;
    size_t unread_results() const { return events.size() > results_read ? events.size() - results_read : 0; }
};
