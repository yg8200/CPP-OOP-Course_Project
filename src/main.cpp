#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <filesystem>
#include <ctime>
#include <cstdlib>
#include "Game.h"
#include "GameFromKeyboard.h"
#include "GameFromTextFile.h"
#include "Menu.h"
#include "Riddlers.h"
#include "utils.h"

namespace {
// The game loads layouts, screens and riddles by relative file name, so it must run with the
// asset folder as its working directory. Find that folder no matter where the game was started from.
bool enterAssetsDirectory(const char* argv0) {
    namespace fs = std::filesystem;
    std::error_code ec;

    const fs::path cwd = fs::current_path(ec);
    const fs::path exeDir = fs::weakly_canonical(fs::absolute(argv0, ec), ec).parent_path();

    const fs::path candidates[] = {
        cwd, cwd / "assets", cwd / "Adventure_Project" / "assets",
        exeDir, exeDir / "assets", exeDir.parent_path() / "assets",
        exeDir / "Adventure_Project" / "assets"
    };
    for (const fs::path& dir : candidates) {
        if (fs::exists(dir / "menu_options.txt", ec)) {
            fs::current_path(dir, ec);
            return !ec;
        }
    }
    return false;
}
}

int main(int argc, char** argv) {
    if (!enterAssetsDirectory(argv[0])) {
        std::cerr << "Cannot find the game files (menu_options.txt, layout_1.txt, ...).\n"
                     "They must be in an 'assets' folder next to the game." << std::endl;
        return 1;
    }

    bool isLoad = argc > 1 && std::string(argv[1]) == "-load";
    bool isSilent = isLoad && argc > 2 && std::string(argv[2]) == "-silent";
    bool isSave = argc > 1 && std::string(argv[1]) == "-save";

    player_mode mode = player_mode::REGULAR;
    if (isLoad)
        mode = player_mode::PLAYBACK;
    else if (isSave)
        mode = player_mode::RECORDING;

    long long current_time = 0;
    Menu menu;
    bool running = true;

    if (mode != player_mode::PLAYBACK) {
        current_time = std::time(nullptr);
        seedRandom(static_cast<unsigned int>(current_time));
    } else {
        std::ifstream file("adv-world.steps");
        std::string line;
        if (!file.is_open()) {
            std::cerr << "Cannot open adv-world.steps - nothing to play back." << std::endl;
            return 1;
        }
        try {
            if (std::getline(file, line) && !line.empty())
                current_time = std::stoll(line);
        } catch (const std::exception&) {
            std::cerr << "adv-world.steps is corrupted (bad random seed)." << std::endl;
            return 1;
        }
        seedRandom(static_cast<unsigned int>(current_time));
    }

    while (running) {
        cls();
        Menu::MenuOption choice = (mode == player_mode::PLAYBACK)
            ? Menu::MenuOption::START_NEW_GAME
            : menu.displayMainMenu();

        switch (choice) {
        case Menu::MenuOption::START_NEW_GAME: {
            loadRiddlesFromFile("riddlers.txt");
            initRiddlePool();

            Player players[] = {
                Player(Point(5, 2, 0, 0, '$'), "wdxase", mode, isSilent),
                Player(Point(72, 3, 0, 0, '&'), "ilmjko", mode, isSilent)
            };

            std::unique_ptr<Game> game;
            switch (mode) {
            case player_mode::REGULAR:
                game = std::make_unique<Game>(players);
                break;
            case player_mode::RECORDING:
                game = std::make_unique<GameFromKeyboard>(players, current_time);
                break;
            case player_mode::PLAYBACK:
                game = std::make_unique<GameFromTextFile>(players, isSilent);
                break;
            }

            game->setup_players_starting_index();
            game->Start();
            break;
        }
        case Menu::MenuOption::EXIT:
            running = false;
            break;
        case Menu::MenuOption::INSTRUCTIONS:
        case Menu::MenuOption::CHANGE_COLOR:
        case Menu::MenuOption::INVALID:
            break;
        }

        if (mode == player_mode::PLAYBACK)
            break;
    }

    showCursor();
    return 0;
}
