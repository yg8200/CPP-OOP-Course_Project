#pragma once

class Menu {
public:
    enum class MenuOption {
        START_NEW_GAME = 1,
        CHANGE_COLOR = 3,
        INSTRUCTIONS = 8,
        EXIT = 9,
        INVALID = -1
    };

    MenuOption displayMainMenu();
    void displayInstructions();

private:
    void clearScreen();
    void displayMenuOptions();
    MenuOption getUserChoice();
};
